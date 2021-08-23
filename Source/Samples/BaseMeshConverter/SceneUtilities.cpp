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

#define IFX_INIT_GUID

#include "IFXCheckX.h"
#include "IFXPalette.h"
#include "IFXAuthorCLODResource.h"
#include "IFXModel.h"
#include "IFXModifierChain.h"

#include "IFXLoadManager.h"
#include "IFXWriteManager.h"
#include "IFXStdio.h"

#include "IFXCoreCIDs.h"
#include "IFXImportingCIDs.h"
#include "IFXExportingCIDs.h"

#include "IFXNewBaseMeshPluginCIDs.h"
#include "IFXNewBaseMesh.h"

#include "SceneUtilities.h"

const U32 EXTENSIBLE_PROFILE = 2;


SceneUtilities::SceneUtilities() :
	IFXDEFINEMEMBER(m_pCoreServicesMain),
	IFXDEFINEMEMBER(m_pCoreServices),
	IFXDEFINEMEMBER(m_pSceneGraph)
{
	IFXCHECKX( IFXCreateComponent( 
							CID_IFXCoreServices, 
							IID_IFXCoreServices,
							(void**)&m_pCoreServicesMain ) );

	IFXCHECKX( m_pCoreServicesMain->Initialize(EXTENSIBLE_PROFILE) );

	IFXRELEASE(m_pCoreServices);
	IFXCHECKX( m_pCoreServicesMain->GetWeakInterface(&m_pCoreServices) );

	// Get hold of the scene graph
	IFXCHECKX( m_pCoreServices->GetSceneGraph( 
									IID_IFXSceneGraph,
									(void**)&m_pSceneGraph ) );
}


SceneUtilities::~SceneUtilities()
{
}


void SceneUtilities::WriteU3DFileX( IFXCHAR* pFileName )
{
	IFXDECLARELOCAL( IFXWriteManager, pWriteManager );
	IFXDECLARELOCAL( IFXWriteBuffer,  pWriteBuffer );

	IFXCHECKX_RESULT( NULL != pFileName, IFX_E_INVALID_POINTER );

	IFXCHECKX( IFXCreateComponent( 
						CID_IFXWriteManager, 
						IID_IFXWriteManager,
						(void**)&pWriteManager ) );

	IFXCHECKX( pWriteManager->Initialize( m_pCoreServices ) );

	IFXCHECKX( IFXCreateComponent( 
						CID_IFXStdioWriteBuffer,
						IID_IFXWriteBuffer,
						(void**)&pWriteBuffer ) );

	{
		IFXDECLARELOCAL( IFXStdio, pStdio );
		IFXExportOptions options = IFXEXPORT_EVERYTHING;//IFXEXPORT_GEOMETRY;

		IFXCHECKX( pWriteBuffer->QueryInterface( 
									IID_IFXStdio,
									(void**)&pStdio ) );

		IFXCHECKX( pStdio->Open( pFileName ) );

		IFXCHECKX( m_pSceneGraph->Mark() );

		IFXCHECKX( pWriteManager->Write( pWriteBuffer, options ) );

		IFXCHECKX( pStdio->Close() );
	}
}


void SceneUtilities::LoadU3DFileX( IFXCHAR* pFileName )
{
	IFXDECLARELOCAL( IFXLoadManager, pLoadManager );
	IFXDECLARELOCAL( IFXReadBuffer,	pReadBuffer );

	IFXCHECKX_RESULT( NULL != pFileName, IFX_E_INVALID_POINTER );

	IFXCHECKX( IFXCreateComponent( 
							CID_IFXLoadManager,
							IID_IFXLoadManager,
							(void**)&pLoadManager ) );

	IFXCHECKX( pLoadManager->Initialize( m_pCoreServices ) );

	IFXCHECKX( IFXCreateComponent( 
							CID_IFXStdioReadBuffer,
							IID_IFXReadBuffer,
							(void**)&pReadBuffer ) );

	{
		IFXDECLARELOCAL( IFXStdio, pStdio );
		IFXCHECKX( pReadBuffer->QueryInterface( 
										IID_IFXStdio,
										(void**)&pStdio ) );

		IFXCHECKX( pStdio->Open( pFileName ) );
		IFXCHECKX( pLoadManager->Load( pReadBuffer, TRUE ) );
		IFXCHECKX( pStdio->Close() );
	}
}

/** 
	Exchange original IFXAuthorCLODResource as geometry resource generator with 
	new base mesh generator for all objects in generator palette. 
*/

void SceneUtilities::ConvertToCompressedBaseMeshX()
{
	U32 size = 0;
	IFXDECLARELOCAL( IFXPalette, pModelPalette );

	IFXCHECKX( m_pSceneGraph->GetPalette( IFXSceneGraph::NODE, &pModelPalette ));
	IFXCHECKX( pModelPalette->GetPaletteSize( &size ) );

	U32 i;
	for( i = 1; i < size; ++i ) // skip default node
	{
		IFXDECLARELOCAL( IFXModel, pModel );

		// do it only for models
		if( IFXFAILURE( pModelPalette->GetResourcePtr(
							i, 
							IID_IFXModel,
							(void**) &pModel )))
			continue;

		IFXDECLARELOCAL( IFXPalette, pGeneratorPalette );
		IFXDECLARELOCAL( IFXAuthorCLODResource, pAuthorCLODResource );
		U32 generatorIndex = pModel->GetResourceIndex();

		IFXCHECKX( m_pSceneGraph->GetPalette( IFXSceneGraph::GENERATOR, &pGeneratorPalette ));

		// Get the IFXAuthorCLODResource interface if possible
		// if not do nothing since only IFXAuthorCLODResource can be interganged 
		// with new base mesh
		if( IFXSUCCESS( pGeneratorPalette->GetResourcePtr(
									generatorIndex, 
									IID_IFXAuthorCLODResource,
									(void**) &pAuthorCLODResource )))
		{
			IFXDECLARELOCAL( IFXNewBaseMesh, pCompressedBaseMesh );
			IFXDECLARELOCAL( IFXModifierChain, pAuthorCLODResourceModChain );
			IFXDECLARELOCAL( IFXModifierChain, pCompressedBaseMeshModChain );

			IFXCHECKX( 
				pAuthorCLODResource->GetModifierChain( 
										&pAuthorCLODResourceModChain ));

			// Create Compressed Base Mesh generator
			IFXCHECKX( IFXCreateComponent( 
							CID_IFXNewBaseMesh, 
							IID_IFXNewBaseMesh, 
							(void**)&pCompressedBaseMesh ));

			IFXCHECKX( pCompressedBaseMesh->SetSceneGraph( m_pSceneGraph ));
			pCompressedBaseMesh->SetCLODResource( pAuthorCLODResource );

			// Add NewBaseMesh to the generator palette instead of 
			// AuthorCLODResource and create mod chain. 
			IFXCHECKX( pGeneratorPalette->SetResourcePtr( 
												generatorIndex, 
												pCompressedBaseMesh ));

			// Renew resource observers since old were detached by 
			// SetResourcePtr call.
			//IFXCHECKX( pModel->SetResourceIndex( generatorIndex ));

			// copy modifiers
			U32 modCount;

			IFXCHECKX( 
				pCompressedBaseMesh->GetModifierChain( 
										&pCompressedBaseMeshModChain ));
			IFXCHECKX( 
				pAuthorCLODResourceModChain->GetModifierCount( modCount ));

			U32 j;
			for( j = 1; j < modCount; ++j ) // skip generator
			{
				IFXDECLARELOCAL( IFXModifier, pModifier );

				IFXCHECKX( pAuthorCLODResourceModChain->GetModifier( 
															j, pModifier ));
				IFXCHECKX( pCompressedBaseMeshModChain->AddModifier( 
															*pModifier ));
			}
		}
	}
}
