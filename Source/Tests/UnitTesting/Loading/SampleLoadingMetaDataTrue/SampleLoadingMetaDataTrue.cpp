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
#ifndef SAMPLELOADINGMETADATATRUE_H
#include "SampleLoadingMetaDataTrue.h"
#endif

#include "UTHeaders.h"
#include "IFXLoadManager.h"
#include "IFXWriteManager.h"
#include "IFXStdio.h"
#include "IFXReadBuffer.h"
#include "IFXWriteBuffer.h"
#include "IFXMetaDataX.h"

IFXRESULT SampleLoadingMetaDataTrue::Run()
{
    IFXRESULT result = IFXCOMInitialize();

	if( IFXSUCCESS( result ) )
	{
		LoadModeLin_stdio1(TRUE);
		LoadModeLin_stdio1(FALSE);
	}

	if( IFXSUCCESS( result ) )
    	result = IFXCOMUninitialize();

	return result;
}


void SampleLoadingMetaDataTrue::LoadModeLin_stdio1(BOOL Mode)
{
	IFXRESULT result = IFX_OK;
	IFXString u3d_src( getenv("U3D_SOURCE") );

	IFXCoreServices* pCoreServicesMain = NULL;
	IFXCoreServices* pCoreServices     = NULL;

	if( IFXSUCCESS( result ) )
		TEST(IFXSUCCESS( result = IFXCreateComponent( CID_IFXCoreServices, IID_IFXCoreServices, (void**)&pCoreServicesMain ) ), IFXException);

	if( IFXSUCCESS( result ) )
		TEST(IFXSUCCESS( result = pCoreServicesMain->Initialize( 0 ) ), IFXException);

	if(IFXSUCCESS(result)) 
	{
		TEST(IFXSUCCESS( result = pCoreServicesMain->GetWeakInterface(&pCoreServices) ), IFXException);
		pCoreServices->SetBaseURL(u3d_src);
	}

	if( IFXSUCCESS( result ) )
	{
		{
			IFXDECLARELOCAL(IFXStdio, pIFXStdio1);
			IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
			IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);

			IFXString src = u3d_src + L"Tests/UnitTesting/SamplePlayer/SimpleTestNR_output.u3d";

			TEST(IFXSUCCESS( result = IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager) ), IFXException);

			if( IFXSUCCESS( result ) )
				TEST(IFXSUCCESS( result = pIFXLoadManager->Initialize(pCoreServices) ), IFXException);

			if( IFXSUCCESS( result ) )
				TEST(IFXSUCCESS( result = IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer) ), IFXException);

			if( IFXSUCCESS( result ) )
				TEST(IFXSUCCESS( result = pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio1) ), IFXException);

			if( IFXSUCCESS( result ) )
				TEST(IFXSUCCESS( result = pIFXStdio1->Open( (IFXCHAR*) src.Raw() ) ), IFXException);

			if( IFXSUCCESS( result ) )
				TEST(IFXSUCCESS( result = pIFXLoadManager->Load(pIFXReadBuffer, TRUE) ), IFXException);
				/*result = pIFXLoadManager->Load(pIFXReadBuffer, FALSE);

			/// @todo investigate issue with non atomic loading - it hangs

			if( IFXSUCCESS( result ) )
			{
				IFXScheduler* pScheduler = NULL;
				result = pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler);

				if( IFXSUCCESS( result ) )
				{
					EIFXLoadState eState = IFX_LOAD_STATE_UNLOADED;
					while (eState < IFX_LOAD_STATE_MEDIA_LOADED)
					{
						TEST(IFXSUCCESS(pScheduler->Service()), IFXException);
						TEST(IFXSUCCESS(pIFXLoadManager->GetAggregateLoadState(eState)), IFXException);
					};
				}

				IFXRELEASE(pScheduler);
			}*/

			if( IFXSUCCESS( result ) )
				TEST(IFXSUCCESS( result = pIFXStdio1->Close() ), IFXException);
		}

		if( IFXSUCCESS( result ) )
		{
			IFXString s1, s2, s3;   
			IFXMetaDataX* pMetaDataX = NULL;

			TEST(IFXSUCCESS( result = pCoreServices->QueryInterface(IID_IFXMetaDataX, (void**)&pMetaDataX) ), IFXException);

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
				delete [] p;
			}

			IFXRELEASE(pMetaDataX);
		}
	}

	IFXRELEASE(pCoreServices);
	IFXRELEASE(pCoreServicesMain);
}



