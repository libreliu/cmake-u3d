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
#ifndef SAMPLELOADINGFILEREF_H
#include "SampleLoadingFileRef.h"
#endif

#include "UTHeaders.h"
#include "IFXLoadManager.h"
#include "IFXWriteManager.h"
#include "IFXStdio.h"
#include "IFXReadBuffer.h"
#include "IFXWriteBuffer.h"
#include "IFXWriteManager.h"
#include "IFXMetaDataX.h"

IFXRESULT SampleLoadingFileRef::Run()
{
    IFXRESULT result = IFXCOMInitialize();

	if(IFXSUCCESS(result))
	{
		LoadMode(TRUE);
		LoadMode(FALSE);
	}
	
	if(IFXSUCCESS(result))
		result = IFXCOMUninitialize();

    return result;
}

void SampleLoadingFileRef::LoadMode( BOOL Mode )
{
	IFXRESULT result = IFX_OK;
	IFXString u3d_src( getenv("U3D_SOURCE") );
	IFXString dst = u3d_src + "Tests/UnitTesting/SamplePlayer/SimpleTestNR_output.u3d";

	IFXDECLARELOCAL(IFXCoreServices,pCoreServices);
	IFXDECLARELOCAL(IFXCoreServices, pCoreServicesMain);
	IFXDECLARELOCAL(IFXSceneGraph, pSceneGraph);

	TEST(IFXSUCCESS(InitializeSceneGraph_CoreService( &pCoreServices, &pCoreServicesMain, &pSceneGraph )), IFXException);

	// load file
	{
		IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
		IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);
		IFXDECLARELOCAL(IFXScheduler, pScheduler);
		IFXDECLARELOCAL(IFXStdio, pIFXStdio1);

		TEST(IFXSUCCESS(pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler)), IFXException);
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager)), IFXException);
		TEST(IFXSUCCESS(pIFXLoadManager->Initialize(pCoreServices)), IFXException);
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer)), IFXException);
		TEST(IFXSUCCESS(pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio1)), IFXException);

		pCoreServices->SetBaseURL(u3d_src);
		IFXString src = u3d_src + L"Tests/UnitTesting/SamplePlayer/SimpleTestNR.u3d";

		TEST(IFXSUCCESS(pIFXStdio1->Open( (IFXCHAR*) src.Raw() )), IFXException);
		TEST(IFXSUCCESS(pIFXLoadManager->Load(pIFXReadBuffer, Mode)), IFXException);

		EIFXLoadState eState = IFX_LOAD_STATE_UNLOADED;
		while (eState < IFX_LOAD_STATE_MEDIA_LOADED) 
		{
			TEST(IFXSUCCESS(pScheduler->Service()), IFXException);
			TEST(IFXSUCCESS(pIFXLoadManager->GetAggregateLoadState(eState)), IFXException);
		};

		TEST(IFXSUCCESS(pIFXStdio1->Close()), IFXException);
	}

	// write file
	{
		IFXDECLARELOCAL(IFXMetaDataX, pMetaDataX);

		TEST(IFXSUCCESS(pCoreServices->QueryInterface(IID_IFXMetaDataX, (void**)&pMetaDataX)), IFXException);
		U8 str[] = "Value of key N2";
		pMetaDataX->SetStringValueX(IFXString("Key N1"), IFXString("Value of key N1"));
		pMetaDataX->SetBinaryValueX(IFXString("Key N2"), sizeof(str), str);
		pMetaDataX->SetStringValueX(IFXString("Key N3"), IFXString("Value of key N3"));

		IFXDECLARELOCAL(IFXWriteManager, pIFXWriteManager);
		IFXDECLARELOCAL(IFXWriteBuffer, pIFXWriteBuffer);
		IFXDECLARELOCAL(IFXStdio, pIFXStdio2);
		IFXDECLARELOCAL(IFXFileReference, pIFXFileRef);
		IFXDECLARELOCAL(IFXPalette, pIFXFRPalette);
		IFXDECLARELOCAL(IFXMarker, pMarker);

		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXWriteManager, IID_IFXWriteManager, (void**)&pIFXWriteManager)), IFXException);
		TEST(IFXSUCCESS(pIFXWriteManager->Initialize(pCoreServices)), IFXException);
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioWriteBuffer, IID_IFXWriteBuffer, (void**)&pIFXWriteBuffer)), IFXException);
		TEST(IFXSUCCESS(pIFXWriteBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio2)), IFXException);

		TEST(IFXSUCCESS(pIFXStdio2->Open( (IFXCHAR*) dst.Raw() )), IFXException);

		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXFileReference, IID_IFXFileReference, (void**)&pIFXFileRef)), IFXException);
		TEST(IFXSUCCESS(pCoreServices->GetFileReferencePalette( & pIFXFRPalette )), IFXException);
		U32 index = 0;
		TEST(IFXSUCCESS(pIFXFRPalette->Add( L"file ref", &index )), IFXException);
		IFXArray<IFXString> url;
		/*  dst = getenv("U3D_SOURCE");
		char dir[1024];
		char dr[3];
		_splitpath( src, dr, dir, NULL, NULL );
		dir[0] = '/';
		char ref[1024] = "file://";
		dr[1] = 0;
		strcat( ref, dr );
		strcat( ref, dir );
		strcat( ref, "Tests\\UnitTesting\\Modifiers\\SampleSkeleton\\simpleBox.u3d" );*/
		url.CreateNewElement() = IFXString( L"Tests/UnitTesting/Modifiers/SampleSkeleton/simpleBox.u3d" );
		pIFXFileRef->SetFileURLs( url );

		TEST(IFXSUCCESS(pIFXFileRef->QueryInterface(IID_IFXMarker, (void**)&pMarker)), IFXException);
		// assign scenegraph
		TEST(IFXSUCCESS(pMarker->SetSceneGraph(pSceneGraph)), IFXException);
		pIFXFileRef->SetScopeName("something");
		pIFXFileRef->SetObjectFilters( NULL );
		pIFXFileRef->SetCollisionPolicy(IFXCOLLISIONPOLICY_PREPENDALL);
		pIFXFileRef->SetWorldAlias(PALETTE_DEFAULT_GROUP);

		TEST(IFXSUCCESS(pIFXFRPalette->SetResourcePtr( index, pIFXFileRef )), IFXException);

		TEST(IFXSUCCESS(pSceneGraph->Mark()), IFXException);
		TEST(IFXSUCCESS(pIFXWriteManager->Write(pIFXWriteBuffer, IFXEXPORT_EVERYTHING)), IFXException); // ^ IFXEXPORT_FILEREFERENCES
		TEST(IFXSUCCESS(pIFXStdio2->Close()), IFXException);
	}

	//load saved file
	{
		IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManagerCheck);
		IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBufferCheck);
		IFXDECLARELOCAL(IFXScheduler, pScheduler);
		IFXDECLARELOCAL(IFXStdio, pIFXStdio3);

		TEST(IFXSUCCESS(pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler)), IFXException);

		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManagerCheck)), IFXException);
		TEST(IFXSUCCESS(pIFXLoadManagerCheck->Initialize(pCoreServices)), IFXException);
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBufferCheck)), IFXException);
		TEST(IFXSUCCESS(pIFXReadBufferCheck->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio3)), IFXException);

		TEST(IFXSUCCESS(pIFXStdio3->Open( (IFXCHAR*) dst.Raw() )), IFXException);

		EIFXLoadState eState = IFX_LOAD_STATE_UNLOADED;

		TEST(IFXSUCCESS(result = pIFXLoadManagerCheck->Load(pIFXReadBufferCheck, Mode)), IFXException);
		TEST(IFXSUCCESS(result = pIFXLoadManagerCheck->GetAggregateLoadState(eState)), IFXException);

		while ( (eState < IFX_LOAD_STATE_MEDIA_LOADED) && (IFX_LOAD_STATE_ERR != eState) ) 
		{
			TEST(IFXSUCCESS(result = pScheduler->Service()), IFXException);
			TEST(IFXSUCCESS(result = pIFXLoadManagerCheck->GetAggregateLoadState(eState)), IFXException);
		};
		
		TEST(IFX_LOAD_STATE_MEDIA_LOADED == eState, IFXException);

		TEST(IFXSUCCESS(pIFXStdio3->Close()), IFXException);
	}

	{
		IFXDECLARELOCAL(IFXMetaDataX, pMetaDataX);

		TEST(IFXSUCCESS(pCoreServices->QueryInterface(IID_IFXMetaDataX, (void**)&pMetaDataX)), IFXException);

		IFXString s1, s2, s3;
		U32 i, s;

		TEST(IFXSUCCESS(pMetaDataX->GetIndex(IFXString("Key N1"), i)), IFXException);
		pMetaDataX->GetStringX(i, s1);
		TEST(IFXSUCCESS(pMetaDataX->GetIndex(IFXString("Key N2"), i)), IFXException);
		pMetaDataX->GetBinarySizeX(i, s);

		U8* p = new U8[s];
		TEST(p != NULL, IFXException);

		if( NULL != p )
		{
			pMetaDataX->GetBinaryX(i, p);
			s2.Assign(p);
			TEST(IFXSUCCESS(pMetaDataX->GetIndex(IFXString("Key N3"), i)), IFXException);
			pMetaDataX->GetStringX(i, s3);
			TEST(s1 == "Value of key N1", IFXException);
			TEST(s2 == "Value of key N2", IFXException);
			TEST(s3 == "Value of key N3", IFXException);
		}

		IFXDELETE_ARRAY( p );
	}
}