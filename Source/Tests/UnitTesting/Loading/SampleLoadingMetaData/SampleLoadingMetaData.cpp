//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//***************************************************************************
#ifndef SAMPLELOADINGMETADATA_H
#include "SampleLoadingMetaData.h"
#endif

#include "UTHeaders.h"
#include "IFXLoadManager.h"
#include "IFXWriteManager.h"
#include "IFXStdio.h"
#include "IFXReadBuffer.h"
#include "IFXWriteBuffer.h"
#include "IFXMetaDataX.h"


IFXRESULT SampleLoadingMetaData::Run()
{
    IFXRESULT result = IFXCOMInitialize();

	if(IFXSUCCESS(result))
	{
#ifdef WIN32
		LoadMode(FALSE);
		LoadMode(TRUE);
#else   
		LoadModeLin_stdio1(FALSE);
		// fails on Linux if we will have LoadModeLin_stdio1(FALSE) and LoadModeLin_stdio1(TRUE)
		//LoadModeLin_stdio1(TRUE);
#endif
	}

	if(IFXSUCCESS(result))
		result = IFXCOMUninitialize();

	return result;
}


#ifdef WIN32

void SampleLoadingMetaData::LoadMode( BOOL Mode )
{
	IFXCoreServices* pCoreServices = NULL;
	IFXCoreServices* pCoreServicesMain = NULL;
	IFXSceneGraph* pSceneGraph = NULL;

	TEST(IFXSUCCESS(InitializeSceneGraph_CoreService( &pCoreServices, &pCoreServicesMain, &pSceneGraph )), IFXException);

	IFXString src(getenv("U3D_SOURCE"));
	src+="Tests/UnitTesting/SamplePlayer/SimpleTestNR.u3d";

	EIFXLoadState eState = IFX_LOAD_STATE_UNLOADED;

    //1: load file
    {
	 	IFXDECLARELOCAL(IFXScheduler, pScheduler);
        IFXDECLARELOCAL(IFXStdio, pIFXStdio1);
        IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
        IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);

		TEST(IFXSUCCESS(pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler)), IFXException);

        TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager)), IFXException);
        TEST(IFXSUCCESS(pIFXLoadManager->Initialize(pCoreServices)), IFXException);
        TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer)), IFXException);
        TEST(IFXSUCCESS(pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio1)), IFXException);

        TEST(IFXSUCCESS(pIFXStdio1->Open( (IFXCHAR*) src.Raw() )), IFXException);
		TEST(IFXSUCCESS(pIFXLoadManager->Load(pIFXReadBuffer, Mode)), IFXException);

		while (eState < IFX_LOAD_STATE_MEDIA_LOADED) 
		{
			TEST(IFXSUCCESS(pScheduler->Service()), IFXException);
			TEST(IFXSUCCESS(pIFXLoadManager->GetAggregateLoadState(eState)), IFXException);
		};

        TEST(IFXSUCCESS(pIFXStdio1->Close()), IFXException);
    }

	// 2: save file
	{
		IFXDECLARELOCAL(IFXStdio, pIFXStdio2);
		IFXDECLARELOCAL(IFXWriteManager, pIFXWriteManager);
		IFXDECLARELOCAL(IFXWriteBuffer, pIFXWriteBuffer);

		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXWriteManager, IID_IFXWriteManager, (void**)&pIFXWriteManager)), IFXException);
		TEST(IFXSUCCESS(pIFXWriteManager->Initialize(pCoreServices)), IFXException);
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioWriteBuffer, IID_IFXWriteBuffer, (void**)&pIFXWriteBuffer)), IFXException);
		TEST(IFXSUCCESS(pIFXWriteBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio2)), IFXException);

		src = getenv("U3D_SOURCE");
		src+="Tests/UnitTesting/SamplePlayer/SimpleTestNR_output.u3d";

		TEST(IFXSUCCESS(pIFXStdio2->Open( (IFXCHAR*) src.Raw() )), IFXException);
		TEST(IFXSUCCESS(pSceneGraph->Mark()), IFXException);
		TEST(IFXSUCCESS(pIFXWriteManager->Write(pIFXWriteBuffer, IFXEXPORT_EVERYTHING)), IFXException); // ^ IFXEXPORT_FILEREFERENCES

		TEST(IFXSUCCESS(pIFXStdio2->Close()), IFXException);
	}

	// 3: load saved file
	{
		IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManagerCheck);
		IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBufferCheck);
		IFXDECLARELOCAL(IFXStdio, pIFXStdio3);
	 	IFXDECLARELOCAL(IFXScheduler, pScheduler);

		TEST(IFXSUCCESS(pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler)), IFXException);

		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManagerCheck)), IFXException);
		TEST(IFXSUCCESS(pIFXLoadManagerCheck->Initialize(pCoreServices)), IFXException);

		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBufferCheck)), IFXException);
		TEST(IFXSUCCESS(pIFXReadBufferCheck->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio3)), IFXException);

		TEST(IFXSUCCESS(pIFXStdio3->Open( (IFXCHAR*) src.Raw() )), IFXException);

		TEST(IFXSUCCESS(pIFXLoadManagerCheck->Load(pIFXReadBufferCheck, Mode)), IFXException);

		eState = IFX_LOAD_STATE_UNLOADED;

		while (eState < IFX_LOAD_STATE_MEDIA_LOADED) 
		{
			TEST(IFXSUCCESS(pScheduler->Service()), IFXException);
			TEST(IFXSUCCESS(pIFXLoadManagerCheck->GetAggregateLoadState(eState)), IFXException);
		};

        TEST(IFXSUCCESS(pIFXStdio3->Close()), IFXException);
	}

	// 4: set metadata
    {
         IFXDECLARELOCAL(IFXMetaDataX, pMetaDataX);

         TEST(IFXSUCCESS(pCoreServices->QueryInterface(IID_IFXMetaDataX, (void**)&pMetaDataX)), IFXException);
         
		 U8 str[] = "Value of key N2";
         pMetaDataX->SetStringValueX(IFXString("Key N1"), IFXString("Value of key N1"));
         pMetaDataX->SetBinaryValueX(IFXString("Key N2"), sizeof(str), str);
         pMetaDataX->SetStringValueX(IFXString("Key N3"), IFXString("Value of key N3"));

         IFXString s1, s2, s3;
         U32 i, s;
         U8* p;

         TEST(IFXSUCCESS(pMetaDataX->GetIndex(IFXString("Key N1"), i)), IFXException);
         pMetaDataX->GetStringX(i, s1);

         TEST(IFXSUCCESS(pMetaDataX->GetIndex(IFXString("Key N2"), i)), IFXException);
         pMetaDataX->GetBinarySizeX(i, s);

         p = new U8[s];

         pMetaDataX->GetBinaryX(i, p);
         s2.Assign(p);

         TEST(IFXSUCCESS(pMetaDataX->GetIndex(IFXString("Key N3"), i)), IFXException);

         pMetaDataX->GetStringX(i, s3);

         TEST(s1 == "Value of key N1", IFXException);
         TEST(s2 == "Value of key N2", IFXException);
         TEST(s3 == "Value of key N3", IFXException);

         delete []p;     
    }

	IFXRELEASE(pSceneGraph);  
	IFXRELEASE(pCoreServicesMain);
	IFXRELEASE(pCoreServices);
}

#else

void SampleLoadingMetaData::LoadModeLin_stdio1(BOOL Mode)
{
	IFXRESULT result = IFX_OK;

	IFXCoreServices* pCoreServicesMain = NULL;
	IFXCoreServices* pCoreServices     = NULL;
	IFXScheduler* pScheduler           = NULL;
	IFXMetaDataX* pMetaDataX           = NULL;

	if( IFXSUCCESS( result ) )    
		result = IFXCreateComponent( CID_IFXCoreServices, IID_IFXCoreServices, (void**)&pCoreServicesMain );

	if( IFXSUCCESS( result ) )
	{
		result = pCoreServicesMain->Initialize( 0 );      

		if(IFXSUCCESS(result)) 
			result = pCoreServicesMain->GetWeakInterface(&pCoreServices);

		if( IFXSUCCESS( result ) )    
			result = pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler);
	}

	IFXString src;

	src = getenv("U3D_SOURCE");
	src+="Tests/UnitTesting/SamplePlayer/SimpleTestNR.u3d";

	EIFXLoadState eState = IFX_LOAD_STATE_UNLOADED;

	IFXString s1, s2, s3;

     //1...
     {
         IFXDECLARELOCAL(IFXStdio, pIFXStdio1);
         IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
         IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);

		 result = IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager);

		 if( IFXSUCCESS( result ) )
			result = pIFXLoadManager->Initialize(pCoreServices);

		 if( IFXSUCCESS( result ) )
			result = IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer);

		 if( IFXSUCCESS( result ) )
			result = pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio1);

		 if( IFXSUCCESS( result ) )
			result = pIFXStdio1->Open( (IFXCHAR*) src.Raw() );

		 if( IFXSUCCESS( result ) )
            result = pIFXLoadManager->Load(pIFXReadBuffer, Mode);

		 if( IFXSUCCESS( result ) ) 
		 {      
             while (eState < IFX_LOAD_STATE_MEDIA_LOADED) 
			 {
                 result = pScheduler->Service();
                 result = pIFXLoadManager->GetAggregateLoadState(eState);
             };
		 }

		 if( IFXSUCCESS( result ) )
			result = pIFXStdio1->Close();

		 result = pCoreServices->QueryInterface(IID_IFXMetaDataX, (void**)&pMetaDataX);

		 if( IFXSUCCESS( result ) ) 
		 {
			 U8 str[] = "Value of key N2";

			 pMetaDataX->SetStringValueX(IFXString("Key N1"), IFXString("Value of key N1"));
			 pMetaDataX->SetBinaryValueX(IFXString("Key N2"), sizeof(str), str);
			 pMetaDataX->SetStringValueX(IFXString("Key N3"), IFXString("Value of key N3"));

			 U32 i, s;
			 U8* p;

			 TEST(IFXSUCCESS(pMetaDataX->GetIndex(IFXString("Key N1"), i)), IFXException);

			 pMetaDataX->GetStringX(i, s1);

			 TEST(IFXSUCCESS(pMetaDataX->GetIndex(IFXString("Key N2"), i)), IFXException);

			 pMetaDataX->GetBinarySizeX(i, s);

			 p = new U8[s];

			 pMetaDataX->GetBinaryX(i, p);

			 s2.Assign(p);

			 TEST(IFXSUCCESS(pMetaDataX->GetIndex(IFXString("Key N3"), i)), IFXException);

			 pMetaDataX->GetStringX(i, s3);

			 TEST(s1 == "Value of key N1", IFXException);
			 TEST(s2 == "Value of key N2", IFXException);
			 TEST(s3 == "Value of key N3", IFXException);

			 delete []p;
		 }
     }

	IFXRELEASE(pCoreServicesMain);
	IFXRELEASE(pCoreServices);
	IFXRELEASE(pScheduler);  
	IFXRELEASE(pMetaDataX);
}

#endif
