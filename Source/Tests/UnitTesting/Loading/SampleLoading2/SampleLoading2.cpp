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
#ifndef SAMPLELOADING_H
#include "SampleLoading2.h"
#endif

#include "UTHeaders.h"
#include "IFXLoadManager.h"
#include "IFXReadBuffer.h"
#include "IFXWriteManager.h"
#include "IFXStdio.h"

IFXRESULT SampleLoading2::Run()
{
	IFXRESULT result = IFX_OK;

	if( IFXSUCCESS( result ) )
		result = Load();
	
	if( IFXSUCCESS( result ) )
		Hello();

	return result;
}

void SampleLoading2::Hello()
{
	printf("Hello!!!\n\n");
}

IFXRESULT SampleLoading2::Load()
{
	IFXRESULT result = IFX_OK;

	IFXDECLARELOCAL( IFXCoreServices, pCoreServices );
	IFXDECLARELOCAL( IFXCoreServices, pCoreServicesMain );

	if( IFXSUCCESS( result ) )
	{
		IFXDECLARELOCAL(IFXScheduler, pScheduler);
		IFXDECLARELOCAL(IFXSceneGraph, pSceneGraph);

		// Initialize the core services
		if( IFXSUCCESS( result ) )
		{
			result = IFXCreateComponent( 
							CID_IFXCoreServices, 
							IID_IFXCoreServices,
							(void**)&pCoreServicesMain );
		}

		if( IFXSUCCESS( result ) )
		{
			result = pCoreServicesMain->Initialize( 0 );

			if(IFXSUCCESS(result))
			{
				IFXRELEASE(pCoreServices);
				result = pCoreServicesMain->GetWeakInterface(&pCoreServices);
			}
		}

		if ( IFXSUCCESS(result) )
			result = pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph );

		//result = InitializeSceneGraph_CoreService( &pCoreServices, &pCoreServicesMain, &pSceneGraph );
		TEST(IFXSUCCESS(pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler)), IFXException);

		IFXString path(getenv("U3D_SOURCE"));
		path+="Tests/UnitTesting/Loading/SampleLoading2/";

		IFXString name(L"box");
		
		IFXString file_in = name + (L".u3d");
		IFXString file_out = name + (L"_out,u3d");

		{
			IFXString file_path = path + file_in;
			IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
			IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);
			IFXDECLARELOCAL(IFXStdio, pIFXStdio1);
	        
			TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager)), IFXException);
			TEST(IFXSUCCESS(pIFXLoadManager->Initialize(pCoreServices)), IFXException);
			TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer)), IFXException);
			TEST(IFXSUCCESS(pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio1)), IFXException);
			
			TEST(IFXSUCCESS(pIFXStdio1->Open( (IFXCHAR*) file_path.Raw() )), IFXException);
			TEST(IFXSUCCESS(pIFXLoadManager->Load(pIFXReadBuffer, TRUE)), IFXException);
			TEST(IFXSUCCESS(pIFXStdio1->Close()), IFXException);
		}

		{
			IFXString file_path = path + file_out;
			IFXDECLARELOCAL(IFXStdio, pIFXStdio2);
			IFXDECLARELOCAL(IFXWriteManager, pIFXWriteManager);
			IFXDECLARELOCAL(IFXWriteBuffer, pIFXWriteBuffer);

			TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXWriteManager, IID_IFXWriteManager, (void**)&pIFXWriteManager)), IFXException);
			TEST(IFXSUCCESS(pIFXWriteManager->Initialize(pCoreServices)), IFXException);
			TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioWriteBuffer, IID_IFXWriteBuffer, (void**)&pIFXWriteBuffer)), IFXException);

			TEST(IFXSUCCESS(pIFXWriteBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio2)), IFXException);

			TEST(IFXSUCCESS(pIFXStdio2->Open( (IFXCHAR*) file_path.Raw() )), IFXException);
			
			TEST(IFXSUCCESS(pSceneGraph->Mark()), IFXException);
			TEST(IFXSUCCESS(pIFXWriteManager->Write(pIFXWriteBuffer, IFXEXPORT_EVERYTHING)), IFXException); // ^ IFXEXPORT_FILEREFERENCES
			TEST(IFXSUCCESS(pIFXStdio2->Close()), IFXException);
		}
	}

	return result;
}
