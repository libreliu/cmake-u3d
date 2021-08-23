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
#ifndef INTERNETREADBUFFER_H
#include "InternetReadBuffer.h"
#endif

#include "UTHeaders.h"
#include "IFXLoadManager.h"
#include "IFXStdio.h"
#include "IFXReadBuffer.h"
#include "IFXException.h"

InternetReadBuffer::InternetReadBuffer()
{
	TEST( IFXSUCCESS( IFXCOMInitialize() ), IFXException );
}

InternetReadBuffer::~InternetReadBuffer()
{
	try	
	{
		TEST( IFXSUCCESS( IFXCOMUninitialize() ), IFXException );
	}
	catch (IFXException e) 
	{
		IFXRESULT result = e.GetIFXResult();
	}
}

IFXRESULT InternetReadBuffer::Run()
{
	IFXRESULT result = IFX_OK;

	IFXString http = L"http://nnsu3d1.ccr.corp.intel.com/u3d/metadata.u3d";
	IFXString ftp = L"ftp://nnsu3d1.ccr.corp.intel.com/u3d/metadata.u3d";

	result = Load( http );

	if( IFXSUCCESS(result))
		result = Load( ftp );

	return result;
}

IFXRESULT InternetReadBuffer::Load( IFXString& rPath )
{
	IFXRESULT result = IFX_OK;

    IFXCoreServices* pCoreServices     = NULL;
    IFXCoreServices* pCoreServicesMain = NULL;

	result = InitializeSceneGraph_CoreService( &pCoreServices, &pCoreServicesMain );

	if( IFXSUCCESS(result))
	{
		IFXDECLARELOCAL(IFXScheduler, pScheduler);
		IFXDECLARELOCAL(IFXLoadManager, pLoadManager);
		IFXDECLARELOCAL(IFXReadBuffer, pReadBuffer);
		IFXDECLARELOCAL(IFXStdio, pStdio);

		TEST(IFXSUCCESS(pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler)), IFXException);
        TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pLoadManager)), IFXException);
        TEST(IFXSUCCESS(pLoadManager->Initialize(pCoreServices)), IFXException);
        TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXInternetReadBuffer, IID_IFXReadBuffer, (void**)&pReadBuffer)), IFXException);
        TEST(IFXSUCCESS(pReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pStdio)), IFXException);

		TEST(IFXSUCCESS(pStdio->Open( (IFXCHAR*)rPath.Raw() )), IFXException);

        result = pLoadManager->Load(pReadBuffer, FALSE);
		IFXASSERT(result == IFX_OK);

		EIFXLoadState eState = IFX_LOAD_STATE_UNLOADED;
		while (eState < IFX_LOAD_STATE_MEDIA_LOADED)
		{
			result = pScheduler->Service();
			IFXASSERT(result == IFX_OK);
			result = pLoadManager->GetAggregateLoadState(eState);
			IFXASSERT(result == IFX_OK);
		};

		TEST(IFXSUCCESS(pStdio->Close()), IFXException);
	}

    IFXRELEASE(pCoreServices);
    IFXRELEASE(pCoreServicesMain);

	return result;
}

