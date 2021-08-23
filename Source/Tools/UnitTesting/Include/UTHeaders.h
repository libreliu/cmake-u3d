//***************************************************************************
//
//  Copyright (c) 2004 - 2006 Intel Corporation
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
#ifndef UTHEADERS_H
#define UTHEADERS_H

#define IFX_INIT_GUID
#include "IFXCOM.h"
#include "IFXImportingCIDs.h"
#include "IFXExportingCIDs.h"
#include "IFXRenderingCIDs.h"
#include "IFXSchedulingCIDs.h"
#include "IFXCoreCIDs.h"
#include "U3DSceneUtilities.h"
#include "IFXWriteManager.h"
#include "IFXWriteBuffer.h"
#include "IFXPickObject.h"
#include "IFXImageCodec.h"
#include "IFXAutoRelease.h"
#include "IFXShapesCIDs.h"
#include "IFXString.h"
#include "IFXShadingModifier.h"
#include "IFXSphereGenerator.h"
#include "IFXTaskCallback.h"

#include <wchar.h>

/**
	Create a primitive and add it to the generator palette

	@param    const char * pName        The name of the primitive in
										the model palette
	@param    IFXREFCID rComponentId     The component ID (CID) of the primitive to create
	@param    U32 uPrimitivePriority    The streaming priority of the data
										associated with this primitive.
	@param    U32 *pModelID				The model palette ID of the new primitive
	@param    IFXModifier *ppPrimitive  A pointer to the new primitive

	@return IFXRESULT     Return status of this method.
*/
IFXINLINE IFXRESULT CreatePrimitive( 
								const char *pName,
								IFXREFCID rComponentId,
								U32 uPrimitivePriority,
								U32 *pModelID,
								IFXModifier **ppPrimitive,
								IFXPalette* pGeneratorPalette,
								IFXSceneGraph* pSceneGraph )
{
	IFXRESULT     result = IFX_OK;
	IFXModifier*  pGenerator  = NULL;
	IFXUnknown*   pUnknown = NULL;

	if (!pSceneGraph || !pGeneratorPalette || !pName || !pModelID || !ppPrimitive)
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	if (IFXSUCCESS(result))
	{
		// Create a new generator:
		// Freeing pGenerator is left to the caller
		result = IFXCreateComponent(
							rComponentId,
							IID_IFXModifier,
							(void**)&pGenerator);
	}

	// Initialize the generator through the marker interface:
	if (IFXSUCCESS(result))
		result = pGenerator->SetSceneGraph( pSceneGraph ) ;

	if (IFXSUCCESS(result))
		pGenerator->SetPriority( uPrimitivePriority ) ;

	// Get the IFXUnknown interface from the primitive we created.
	if (IFXSUCCESS(result) )
		result = pGenerator->QueryInterface(IID_IFXUnknown, (void**)&pUnknown);

	// Make an entry for this resource in the generator palette
	if (IFXSUCCESS(result) )
	{
		IFXString NodeName( pName );
		result = pGeneratorPalette->Add( &NodeName, pModelID );
	}

	// And point the the palette entry to the actual resource
	if ( IFXSUCCESS(result) )
		result = pGeneratorPalette->SetResourcePtr( *pModelID, pUnknown );

	if ( IFXSUCCESS(result) )
		*ppPrimitive = pGenerator;

	IFXRELEASE(pUnknown);

	return result;
}


IFXINLINE IFXRESULT InitializeSceneGraph_CoreService(
										IFXCoreServices** pCoreServices, 
										IFXCoreServices** pCoreServicesMain, 
										IFXSceneGraph** pSceneGraph = NULL,
										U32 profile = 0,
										F64 scalingFactor = 1.0 )
{
	IFXRESULT result = IFX_OK;

	if( NULL == pCoreServices || NULL == pCoreServicesMain )
		result = IFX_E_INVALID_POINTER;

	if ( IFXSUCCESS(result) )
		result = IFXCreateComponent( CID_IFXCoreServices, IID_IFXCoreServices,(void**)pCoreServicesMain );

	if ( IFXSUCCESS(result) )
	{
		result = (*pCoreServicesMain)->Initialize(profile, scalingFactor );

	 	if(IFXSUCCESS(result))
		{
			IFXRELEASE(*pCoreServices);
			result = (*pCoreServicesMain)->GetWeakInterface(pCoreServices);
		}
	}

	if ( ( NULL != pSceneGraph ) && IFXSUCCESS(result) )
		result = (*pCoreServices)->GetSceneGraph( IID_IFXSceneGraph, (void**)pSceneGraph );
	
	return result;
}

#endif