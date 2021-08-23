//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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

#ifndef SAMPLEMULTIPLELOADING_H
#include "SampleMultipleLoading.h"
#endif

#include "UTHeaders.h"
#include "IFXLoadManager.h"
#include "IFXStdio.h"
#include "IFXReadBuffer.h"

IFXRESULT SampleMultipleLoading::Run()
{
  IFXCOMInitialize();

	LoadMode(TRUE);

  IFXCOMUninitialize();

  return IFX_OK;
}

void SampleMultipleLoading::LoadMode( BOOL Mode )
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL(IFXCoreServices,pCoreServices);
	IFXDECLARELOCAL(IFXCoreServices, pCoreServicesMain);
	IFXDECLARELOCAL(IFXSceneGraph, pSceneGraph);
	TEST(IFXSUCCESS(InitializeSceneGraph_CoreService( &pCoreServices, &pCoreServicesMain, &pSceneGraph )), IFXException);

	IFXString src, src_next;
	src = getenv("U3D_SOURCE");
	src+="Tests/UnitTesting/SamplePlayer/SimpleTestNR.u3d";
	src_next = getenv("U3D_SOURCE");
	src_next+="Tests/UnitTesting/Modifiers/SampleSkeleton/simpleBox.u3d";

	{
		IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
		IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManagerCheck);
		IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);
		IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBufferCheck);
		IFXDECLARELOCAL(IFXStdio, pIFXStdio1);
		IFXDECLARELOCAL(IFXStdio, pIFXStdio2);
		IFXDECLARELOCAL(IFXScheduler, pScheduler);
	
		TEST(IFXSUCCESS(pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler)), IFXException);
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager)), IFXException);
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManagerCheck)), IFXException);
		TEST(IFXSUCCESS(pIFXLoadManager->Initialize(pCoreServices)), IFXException);
		TEST(IFXSUCCESS(pIFXLoadManagerCheck->Initialize(pCoreServices)), IFXException);
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer)), IFXException);
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBufferCheck)), IFXException);
		
		TEST(IFXSUCCESS(pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio1)), IFXException);
		TEST(IFXSUCCESS(pIFXReadBufferCheck->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio2)), IFXException);
	
		   
		 TEST(IFXSUCCESS(pIFXStdio1->Open( (IFXCHAR*) src.Raw() )), IFXException);
		 TEST(IFXSUCCESS(pIFXStdio2->Open( (IFXCHAR*) src_next.Raw() )), IFXException);
	
		 TEST(IFXSUCCESS(pIFXLoadManager->Load(pIFXReadBuffer, Mode)), IFXException);
				 TEST(IFXSUCCESS(pIFXLoadManagerCheck->Load(pIFXReadBufferCheck, Mode)), IFXException);
		 
		EIFXLoadState eState1 = IFX_LOAD_STATE_UNLOADED, eState2 = IFX_LOAD_STATE_UNLOADED;
	
		 while ((eState1 < IFX_LOAD_STATE_MEDIA_LOADED) || (eState2 < IFX_LOAD_STATE_MEDIA_LOADED)) {
			 TEST(IFXSUCCESS(pScheduler->Service()), IFXException);
			 TEST(IFXSUCCESS(pIFXLoadManager->GetAggregateLoadState(eState1)), IFXException);
			 TEST(IFXSUCCESS(pIFXLoadManagerCheck->GetAggregateLoadState(eState2)), IFXException);
		 };
			   
		 LoadPrint( src);
		 LoadPrint( src_next);   
	
		 TEST(IFXSUCCESS(pIFXStdio2->Close()), IFXException);
		 TEST(IFXSUCCESS(pIFXStdio1->Close()), IFXException);
	}

} //__end_of_LoadMode_

void SampleMultipleLoading::LoadPrint(IFXString str )
{
      const IFXCHAR* pstr=NULL;
      U32 uDestSize = 0;
      pstr = str.Raw();
      char fname[128];

      U32 ssss = wcstombs(NULL, pstr, 0);
      wcstombs(fname, pstr, ssss );
      fname[ssss] = 0;
      printf( "\n LOAD ==== %s ====\n", fname );
}

