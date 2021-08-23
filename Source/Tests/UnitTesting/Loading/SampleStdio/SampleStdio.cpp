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

#ifndef SAMPLESTDIO_H
#include "SampleStdio.h"
#endif

#include "UTHeaders.h"
#include "IFXLoadManager.h"
#include "IFXWriteManager.h"
#include "IFXStdio.h"
#include "IFXReadBuffer.h"
#include "IFXWriteBuffer.h"
#include "IFXDebug.h"

  
IFXRESULT SampleStdio::Run()
{
	IFXRESULT result = IFXCOMInitialize();

	if(IFXSUCCESS(result))      
	{
		LoadWrite();
	}

	if(IFXSUCCESS(result))      
		result = IFXCOMUninitialize();
	
	return result;
}

void SampleStdio::LoadWrite()
{
	IFXRESULT result = IFX_OK;
	IFXString source( getenv("U3D_SOURCE") );

	IFXDECLARELOCAL(IFXScheduler, pScheduler);
	IFXDECLARELOCAL(IFXCoreServices,pCoreServices);
	IFXDECLARELOCAL(IFXCoreServices, pCoreServicesMain);
	IFXDECLARELOCAL(IFXSceneGraph, pSceneGraph);

	IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
	IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);
	IFXDECLARELOCAL(IFXStdio, pIFXStdio1);

	IFXDECLARELOCAL(IFXWriteManager, pIFXWriteManager);
	IFXDECLARELOCAL(IFXWriteBuffer, pIFXWriteBuffer);
	IFXDECLARELOCAL(IFXStdio, pIFXStdio2);

	result = InitializeSceneGraph_CoreService( &pCoreServices, &pCoreServicesMain, &pSceneGraph );
	if(IFXSUCCESS(result))
		result = pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler);
	
	// 1  ======================================================================
	{
		if(IFXSUCCESS(result))
			result = IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager);

		if(IFXSUCCESS(result))
			result = pIFXLoadManager->Initialize(pCoreServices);

		if(IFXSUCCESS(result))
			result = IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer);

		if(IFXSUCCESS(result))
			result = pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio1);

		IFXString src = source + L"Tests/UnitTesting/Loading/SampleStdio/box.u3d";
         
		if(IFXSUCCESS(result)) 
			result = pIFXStdio1->Open( (IFXCHAR*)src.Raw() );

		if(IFXSUCCESS(result)) 
			result = pIFXStdio1->Close();
	}

	// 2 =======================================================================
	{
		if(IFXSUCCESS(result))
			result = IFXCreateComponent(CID_IFXWriteManager, IID_IFXWriteManager, (void**)&pIFXWriteManager);

		if(IFXSUCCESS(result))
			result = pIFXWriteManager->Initialize(pCoreServices);

		IFXASSERT(result == IFX_OK);
			result = IFXCreateComponent(CID_IFXStdioWriteBuffer, IID_IFXWriteBuffer, (void**)&pIFXWriteBuffer);

		if(IFXSUCCESS(result))
			result = pIFXWriteBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio2);

		IFXString src_out = source + L"Tests/UnitTesting/Loading/SampleStdio/box_out.u3d";

		if(IFXSUCCESS(result))
			result = pIFXStdio2->Open( (IFXCHAR*) src_out.Raw() );

		if(IFXSUCCESS(result))
			result = pIFXStdio2->Close();
	}

	IFXASSERT(result == IFX_OK);
}


