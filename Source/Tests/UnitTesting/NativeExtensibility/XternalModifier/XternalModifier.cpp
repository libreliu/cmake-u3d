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

#include "UTHeaders.h"

#ifndef XternalModifier_H
#include "XternalModifier.h"
#endif

#include "IFXSphereGenerator.h"
#include "IFXModifierChain.h"
#include "IFXShadingModifier.h"

#include "IFXStdio.h"
#include "IFXTextureObject.h"
#include "IFXWriteManager.h"
#include "IFXLoadManager.h"
#include "IFXWriteBuffer.h"
#include "IFXReadBuffer.h"

#include "IFXCLODModifier.h"

#include "IFXNameMap.h"

IFXRESULT XternalModifier::Run()
{
	U32 uResourceID = 0;

	IFXRESULT result = IFXCOMInitialize();

	if( IFXSUCCESS(result) )
	{
		IFXDECLARELOCAL(IFXCoreServices, pCoreServices);
		IFXDECLARELOCAL(IFXCoreServices, pCoreServicesMain);
		IFXDECLARELOCAL(IFXSceneGraph, pSceneGraph);
		TEST(IFXSUCCESS(InitializeSceneGraph_CoreService( &pCoreServices, &pCoreServicesMain, &pSceneGraph, IFXPROFILE_EXTENSIBLE )), IFXException);

		// load source file
		{
			IFXString src;
			src = getenv("U3D_SOURCE");
			src += L"Tests/UnitTesting/Modifiers/SampleSkeleton/simpleBox.u3d";

			IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
			IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);
			IFXDECLARELOCAL(IFXStdio, pIFXStdio);

			//load CLOD with bones from U3D file
			TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager)), IFXException);
			TEST(IFXSUCCESS(pIFXLoadManager->Initialize(pCoreServices)), IFXException);
			TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer)), IFXException);
			TEST(IFXSUCCESS(pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio)), IFXException);
			
			TEST(IFXSUCCESS(pIFXStdio->Open( (IFXCHAR*) src.Raw() )), IFXException);
			TEST(IFXSUCCESS(pIFXLoadManager->Load(pIFXReadBuffer, TRUE)), IFXException);
			TEST(IFXSUCCESS(pIFXStdio->Close()), IFXException);
		}

		// create extension modifier (CLOD modifier #2)
		{
			IFXDECLARELOCAL(IFXModifierChain,pModifierChain);
			IFXString sName("Box01");

			IFXDECLARELOCAL(IFXUnknown,pObject);
			TEST(IFXSUCCESS(IFXCreateComponent( CID_IFXCLODModifier2, IID_IFXUnknown, (void**)&pObject )), IFXException);
			IFXDECLARELOCAL(IFXMarker,pMarker );
			TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXMarker, (void**)&pMarker )), IFXException);
			TEST(IFXSUCCESS(pMarker->SetSceneGraph( pSceneGraph )), IFXException);
			IFXDECLARELOCAL( IFXModifier, pMod );
			TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXModifier, (void**)&pMod)), IFXException);
			IFXDECLARELOCAL( IFXCLODModifier, pCLODMod );
			TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXCLODModifier, (void**)&pCLODMod)), IFXException);

			{
				U32 uResourceID;

				IFXDECLARELOCAL(IFXPalette,pSGPalette);
				TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::NODE, &pSGPalette )), IFXException);

				U32 iResultPaletteFind = pSGPalette->Find( &sName, &uResourceID );
				if ( IFX_E_CANNOT_FIND == iResultPaletteFind  )
				{
					TEST(IFXSUCCESS(pSGPalette->Add( &sName, &uResourceID )), IFXException);
				}

				// If this is a chain object
				{
					// get the modifier at the head of the chain
					IFXDECLARELOCAL(IFXModifier,pHeadModifier);
					U32 iResultGetResource = pSGPalette->GetResourcePtr(uResourceID,IID_IFXModifier,(void**)&pHeadModifier);

					// Get the modifier chain from the head modifier
					TEST(IFXSUCCESS(pHeadModifier->GetModifierChain(&pModifierChain)), IFXException);

					// Put the object (the new modifier) into the modifier chain
					IFXDECLARELOCAL(IFXModifier,pObjectAsModifier);
					TEST(IFXSUCCESS(pMod->QueryInterface( IID_IFXModifier, (void**)&pObjectAsModifier )), IFXException);
					TEST(IFXSUCCESS(pModifierChain->AddModifier( *pObjectAsModifier, IFXModifierChain::END_OF_MODCHAIN, FALSE )), IFXException);

					U32 uModCount = 0;
					TEST(IFXSUCCESS( pModifierChain->GetModifierCount( uModCount )) , IFXException);

					pCLODMod->SetCLODScreenSpaceControllerState(FALSE);
					pCLODMod->SetCLODLevel(0.25f);
				}
			}
		}

		IFXString name("CLODMod2_output.u3d");

		// save to file with extension modifier
		{
			IFXDECLARELOCAL(IFXWriteManager, pIFXWriteManager);
			IFXDECLARELOCAL(IFXWriteBuffer, pIFXWriteBuffer);
			IFXDECLARELOCAL(IFXStdio, pIFXStdio);

			IFXCHAR *name1 = (IFXCHAR*) name.Raw();
			TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXWriteManager, IID_IFXWriteManager, (void**)&pIFXWriteManager)), IFXException);
			TEST(IFXSUCCESS(pIFXWriteManager->Initialize(pCoreServices)), IFXException);
			TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioWriteBuffer, IID_IFXWriteBuffer, (void**)&pIFXWriteBuffer)), IFXException);
			TEST(IFXSUCCESS(pIFXWriteBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio)), IFXException);
			TEST(IFXSUCCESS(pIFXStdio->Open( name1 ) ), IFXException);
			TEST(IFXSUCCESS(pSceneGraph->Mark()), IFXException);
			TEST(IFXSUCCESS(pIFXWriteManager->Write(pIFXWriteBuffer, IFXEXPORT_EVERYTHING)), IFXException); // ^ IFXEXPORT_FILEREFERENCES
			TEST(IFXSUCCESS(pIFXStdio->Close()), IFXException);
		}

		// load file with extension modifier
		{
			IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
			IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);
			IFXDECLARELOCAL(IFXStdio, pIFXStdio);

			//load CLOD with bones from U3D file
			TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager)), IFXException);
			TEST(IFXSUCCESS(pIFXLoadManager->Initialize(pCoreServices)), IFXException);
			TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer)), IFXException);
			TEST(IFXSUCCESS(pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio)), IFXException);
			
			TEST(IFXSUCCESS(pIFXStdio->Open( (IFXCHAR*) name.Raw() )), IFXException);
			TEST(IFXSUCCESS(pIFXLoadManager->Load(pIFXReadBuffer, TRUE)), IFXException);
			TEST(IFXSUCCESS(pIFXStdio->Close()), IFXException);
		}
	}

	if( IFXSUCCESS( result ) )
		result = IFXCOMUninitialize();

	return result;
}
