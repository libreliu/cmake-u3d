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
#ifndef InitUninit_H
#include "InitUninit.h"
#endif

#include "UTHeaders.h"
#include "IFXLoadManager.h"
#include "IFXReadBuffer.h"
#include "IFXWriteManager.h"
#include "IFXStdio.h"

IFXRESULT InitUninit::Run()
{
	IFXRESULT result = IFXCOMInitialize();

	if( IFXSUCCESS( result ) )
		result = Load();
	
	if( IFXSUCCESS( result ) )
		result = IFXCOMUninitialize();

	if( IFXSUCCESS( result ) )
		result = IFXCOMInitialize();

	if( IFXSUCCESS( result ) )
	{
		Hello();
		result = IFXCOMUninitialize();
	}

	return result;
}

void InitUninit::Hello()
{
	printf("Hello!!!\n\n");
}

IFXRESULT InitUninit::Load()
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

		IFXString src = getenv("U3D_SOURCE"); 
		src+="Tests/UnitTesting/IFXCOM/InitUninit/box.u3d";
		{
			IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
			IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);
			IFXDECLARELOCAL(IFXStdio, pIFXStdio1);
	        
			TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager)), IFXException);
			TEST(IFXSUCCESS(pIFXLoadManager->Initialize(pCoreServices)), IFXException);
			TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer)), IFXException);
			TEST(IFXSUCCESS(pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio1)), IFXException);
			
			TEST(IFXSUCCESS(pIFXStdio1->Open( (IFXCHAR*) src.Raw() )), IFXException);
			TEST(IFXSUCCESS(pIFXLoadManager->Load(pIFXReadBuffer, TRUE)), IFXException);
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

			IFXString src_out=getenv("U3D_SOURCE");
			src_out+="Tests/UnitTesting/IFXCOM/InitUninit/box_out.u3d";
			TEST(IFXSUCCESS(pIFXStdio2->Open( (IFXCHAR*) src_out.Raw() )), IFXException);
			
			TEST(IFXSUCCESS(pSceneGraph->Mark()), IFXException);
			TEST(IFXSUCCESS(pIFXWriteManager->Write(pIFXWriteBuffer, IFXEXPORT_EVERYTHING)), IFXException); // ^ IFXEXPORT_FILEREFERENCES
			TEST(IFXSUCCESS(pIFXStdio2->Close()), IFXException);
		}
	}

	return result;
}
