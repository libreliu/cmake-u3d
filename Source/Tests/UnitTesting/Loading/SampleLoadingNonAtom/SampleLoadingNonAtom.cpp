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

#ifndef SAMPLELOADINGNONATOM_H
#include "SampleLoadingNonAtom.h"
#endif

#include "UTHeaders.h"
#include "IFXLoadManager.h"
#include "IFXWriteManager.h"
#include "IFXStdio.h"
#include "IFXReadBuffer.h"
#include "IFXWriteBuffer.h"

IFXRESULT SampleLoadingNonAtom::Run()
{
    IFXRESULT result = IFXCOMInitialize();

	if(IFXSUCCESS(result))
	{
		NonAtomic();
		result = IFXCOMUninitialize();
	}

	return result;
}

void SampleLoadingNonAtom::NonAtomic()
{
	IFXRESULT result = IFX_OK;

    IFXCoreServices* pCoreServices     = NULL;
    IFXCoreServices* pCoreServicesMain = NULL;
    IFXSceneGraph*   pSceneGraph       = NULL;

	TEST(IFXSUCCESS(InitializeSceneGraph_CoreService( &pCoreServices, &pCoreServicesMain, &pSceneGraph )), IFXException);

	IFXString src, src_out;
	{
		IFXDECLARELOCAL(IFXScheduler, pScheduler);
		IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
		IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);
		IFXDECLARELOCAL(IFXStdio, pIFXStdio1);

		TEST(IFXSUCCESS(pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler)), IFXException);
        TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager)), IFXException);
        TEST(IFXSUCCESS(pIFXLoadManager->Initialize(pCoreServices)), IFXException);
        TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer)), IFXException);
        TEST(IFXSUCCESS(pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio1)), IFXException);

		src = getenv("U3D_SOURCE");
		src+="Tests/UnitTesting/Loading/SampleLoadingNonAtom/box.u3d";

		TEST(IFXSUCCESS(pIFXStdio1->Open( (IFXCHAR*) src.Raw() )), IFXException);

        result = pIFXLoadManager->Load(pIFXReadBuffer, FALSE);
		IFXASSERT(result == IFX_OK);

         EIFXLoadState eState = IFX_LOAD_STATE_UNLOADED;
         while (eState < IFX_LOAD_STATE_MEDIA_LOADED)
         {
            result = pScheduler->Service();
			IFXASSERT(result == IFX_OK);
            result = pIFXLoadManager->GetAggregateLoadState(eState);
            IFXASSERT(result == IFX_OK);
         };
		TEST(IFXSUCCESS(pIFXStdio1->Close()), IFXException);
	}

	{
         IFXDECLARELOCAL(IFXStdio, pIFXStdio2);
         IFXDECLARELOCAL(IFXWriteManager, pIFXWriteManager);
         IFXDECLARELOCAL(IFXWriteBuffer, pIFXWriteBuffer);
 
         TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXWriteManager, IID_IFXWriteManager, (void**)&pIFXWriteManager)), IFXException);
         TEST(IFXSUCCESS(pIFXWriteManager->Initialize(pCoreServices)), IFXException);
         TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioWriteBuffer, IID_IFXWriteBuffer, (void**)&pIFXWriteBuffer)), IFXException);
         TEST(IFXSUCCESS(pIFXWriteBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio2)), IFXException);
         
         src_out=getenv("U3D_SOURCE");
		 src_out+="Tests/UnitTesting/Loading/SampleLoadingNonAtom/box_non_out.u3d";
         TEST(IFXSUCCESS(pIFXStdio2->Open( (IFXCHAR*) src_out.Raw() )), IFXException);

		 TEST(IFXSUCCESS(pSceneGraph->Mark()), IFXException);
		 TEST(IFXSUCCESS(pIFXWriteManager->Write(pIFXWriteBuffer, IFXEXPORT_EVERYTHING)), IFXException); // ^ IFXEXPORT_FILEREFERENCES
		 TEST(IFXSUCCESS(pIFXStdio2->Close()), IFXException);
	}

    IFXRELEASE(pCoreServices);
    IFXRELEASE(pCoreServicesMain);
    IFXRELEASE(pSceneGraph);
}
