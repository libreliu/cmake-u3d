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

#include "U3DSceneUtilities.h"

#include "IFXPalette.h"
#include "IFXImportingCIDs.h"
#include "IFXMatrix4x4.h"
#include "IFXCheckX.h"
#include "IFXModifierChain.h"
#include "IFXDids.h" // IFX_DID_RENDERABLE

#include <wchar.h>


//*****************************************************************************
//  Defines
//*****************************************************************************


#define _VIEW_DEFAULT_ACCUMULATION_R	0.0
#define _VIEW_DEFAULT_ACCUMULATION_G	0.0
#define _VIEW_DEFAULT_ACCUMULATION_B	0.0
#define _VIEW_DEFAULT_ACCUMULATION_A	1.0
#define _VIEW_DEFAULT_COLOR_R			0.0
#define _VIEW_DEFAULT_COLOR_G			0.0
#define _VIEW_DEFAULT_COLOR_B			0.0
#define _VIEW_DEFAULT_COLOR_A			1.0
#define _VIEW_DEFAULT_DEPTH				1.0
#define _VIEW_DEFAULT_STENCIL			0

#define _VIEW_DEFAULT_POS_X				0.0
#define _VIEW_DEFAULT_POS_Y				0.0
#define _VIEW_DEFAULT_POS_Z				250.0

// this setting guarantees that we will look at the origin.
#define _VIEW_DEFAULT_DIR_X				-( _VIEW_DEFAULT_POS_X )
#define _VIEW_DEFAULT_DIR_Y				-( _VIEW_DEFAULT_POS_Y )
#define _VIEW_DEFAULT_DIR_Z				-( _VIEW_DEFAULT_POS_Z )

#define _VIEW_DEFAULT_UP_BIAS_X			0.0
#define _VIEW_DEFAULT_UP_BIAS_Y			1.0
#define _VIEW_DEFAULT_UP_BIAS_Z			0.0

#define _VIEW_DEFAULT_PROJECTION		34.516f


//------------------------------------------------------------------
/**
Class constructor.
**/
U3DSceneUtilities::U3DSceneUtilities() :
IFXDEFINEMEMBER(m_pSceneGraph),
IFXDEFINEMEMBER(m_pScheduler),
IFXDEFINEMEMBER(m_pCoreServicesMain),
IFXDEFINEMEMBER(m_pCoreServices),
IFXDEFINEMEMBER(m_pMaterialPalette),
IFXDEFINEMEMBER(m_pGeneratorPalette),
IFXDEFINEMEMBER(m_pNodePalette),
IFXDEFINEMEMBER(m_pShaderPalette),
IFXDEFINEMEMBER(m_pTexturePalette),
IFXDEFINEMEMBER(m_pIFXReadBuffer),
IFXDEFINEMEMBER(m_pIFXStdio)
{
	m_bInit = false;

	U32 i;
	for(i=0;i< NUM_OF_LM;i++) {
		m_pIFXLoadManagers[i] = NULL;
	}

	m_LoadManagerCounter = 0;
	m_bEndOfU3DFile = TRUE;
	m_bCancelLoading = TRUE;
	m_nodeFound = FALSE;
}

//------------------------------------------------------------------
/**
Class destructor.
**/
U3DSceneUtilities::~U3DSceneUtilities()
{
	U32 i;
	for(i=0;i< NUM_OF_LM;i++) {
		IFXRELEASE(m_pIFXLoadManagers[i]);
	}

	m_bInit = false;
}

//---------------------------------------------------------------------------
/**
Performs scenegraph initialization.
*/
IFXRESULT U3DSceneUtilities::InitializeScene( U32 profile, F64 units )
{
	IFXRESULT result = IFX_OK;

	if (m_bInit && IFXSUCCESS(result))
	{
		result = IFX_E_ALREADY_INITIALIZED;
		IFXASSERT(0);
	}

	// Initialize the core services
	if ( IFXSUCCESS(result) )
	{
		result = IFXCreateComponent( CID_IFXCoreServices, IID_IFXCoreServices,
			(void**)&m_pCoreServicesMain );

	}

	if ( m_pCoreServicesMain && IFXSUCCESS(result) )
	{
		result = m_pCoreServicesMain->Initialize(profile, units);

		if(IFXSUCCESS(result))
		{
			IFXRELEASE(m_pCoreServices);
			result = m_pCoreServicesMain->GetWeakInterface(&m_pCoreServices);
		}

	}

	// Get hold of the scene graph
	if ( m_pCoreServices && IFXSUCCESS(result) )
		result = m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph,
		(void**)&m_pSceneGraph );

	// Get Scheduler
	if ( m_pCoreServices && IFXSUCCESS(result) )
		result = m_pCoreServices->GetScheduler(IID_IFXScheduler,
		(void**)&m_pScheduler);

	// Gain access to the palettes
	if (m_pSceneGraph)
	{
		if ( IFXSUCCESS(result) )
		{
			result = m_pSceneGraph->GetPalette( IFXSceneGraph::NODE,
				&m_pNodePalette );
		}
		if ( IFXSUCCESS(result) )
		{
			result = m_pSceneGraph->GetPalette( IFXSceneGraph::GENERATOR,
				&m_pGeneratorPalette );
		}
		if ( IFXSUCCESS(result) )
		{
			result = m_pSceneGraph->GetPalette( IFXSceneGraph::SHADER,
				&m_pShaderPalette );
		}
		if ( IFXSUCCESS(result) )
		{
			result = m_pSceneGraph->GetPalette( IFXSceneGraph::MATERIAL,
				&m_pMaterialPalette );
		}
		if ( IFXSUCCESS(result) )
		{
			result = m_pSceneGraph->GetPalette( IFXSceneGraph::TEXTURE,
				&m_pTexturePalette );
		}
	}
	// Flag us as initialized
	if ( IFXSUCCESS(result) )
	{
		m_bInit = true;
	}

	return result;
}

//---------------------------------------------------------------------------
void U3DSceneUtilities::LoadU3DFileInit(IFXCHAR* FileName,
										BOOL bLoadingAtomic,
										BOOL bRemoteConnection)
{
	if (*m_pIFXLoadManagers)
		m_LoadManagerCounter++;
	else
		m_LoadManagerCounter = 0;

	IFXCHECKX(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager,
		(void**)&m_pIFXLoadManagers[m_LoadManagerCounter]));
	IFXCHECKX(m_pIFXLoadManagers[m_LoadManagerCounter]->Initialize(m_pCoreServices));

	if (bRemoteConnection)
		IFXCHECKX(IFXCreateComponent(CID_IFXInternetReadBuffer, IID_IFXReadBuffer,
		(void**)&m_pIFXReadBuffer));
	else
		IFXCHECKX(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer,
		(void**)&m_pIFXReadBuffer));

	IFXCHECKX(m_pIFXReadBuffer->QueryInterface(IID_IFXStdio,
		(void**)&m_pIFXStdio));
	IFXCHECKX(m_pIFXStdio->Open(FileName));

	IFXCHECKX(m_pIFXLoadManagers[m_LoadManagerCounter]->Load(m_pIFXReadBuffer,
		bLoadingAtomic, "n",
		IFXCOLLISIONPOLICY_PREPENDCOLLIDED));

	IFXRELEASE(m_pIFXReadBuffer);
	IFXRELEASE(m_pIFXStdio);

	m_bEndOfU3DFile = FALSE; ///bLoadingAtomic;
	m_bCancelLoading = FALSE;
}

//---------------------------------------------------------------------------
BOOL U3DSceneUtilities::IsU3DFileLoaded()
{
	return m_bEndOfU3DFile;
}

//---------------------------------------------------------------------------
U64 U3DSceneUtilities::LoadRemainder()
{
	U64 TotalFileSize = 0;
	U64 CurrentFileSize = 0;
	U64 Size;
	U32 i;
	for (i = 0; i <= m_LoadManagerCounter; i++) {
		IFXCHECKX(m_pIFXLoadManagers[i]->GetAggregateFileSize(Size));
		TotalFileSize += Size;

		IFXCHECKX(m_pIFXLoadManagers[i]->GetAggregateFileSizeLoaded(Size));
		CurrentFileSize += Size;
	}
	return TotalFileSize - CurrentFileSize;
}

//---------------------------------------------------------------------------
F32 U3DSceneUtilities::LoadingProgress()
{
	I64 TotalFileSize = 0;
	I64 CurrentFileSize = 0;
	U64 Size;
	U32 i;
	for (i = 0; i <= m_LoadManagerCounter; i++) {
		IFXCHECKX(m_pIFXLoadManagers[m_LoadManagerCounter]->GetAggregateFileSize(Size));
		TotalFileSize += Size;

		IFXCHECKX(m_pIFXLoadManagers[m_LoadManagerCounter]->GetAggregateFileSizeLoaded(Size));
		CurrentFileSize += Size;
	}
	return (F32)CurrentFileSize/(F32)TotalFileSize * 100;
}

//---------------------------------------------------------------------------
void U3DSceneUtilities::CancelLoading()
{
	U32 i;
	m_bCancelLoading = TRUE;
	m_bEndOfU3DFile = TRUE;
	for (i = 0; i < m_LoadManagerCounter; i++) {
		if (m_pIFXLoadManagers[m_LoadManagerCounter])
			IFXCHECKX(m_pIFXLoadManagers[m_LoadManagerCounter]->Cancel());
		IFXRELEASE( m_pIFXLoadManagers[m_LoadManagerCounter] );
	}
}

//---------------------------------------------------------------------------
BOOL U3DSceneUtilities::IsLoadingCanceled()
{
	return m_bCancelLoading;
}

//---------------------------------------------------------------------------
void U3DSceneUtilities::ResetSimulationX()
{
	IFXCHECKX_RESULT(m_pScheduler, IFX_E_NOT_INITIALIZED);

	IFXASSERT( m_pScheduler );
	IFXCHECKX( m_pScheduler->ResetSimulation() );
}

//---------------------------------------------------------------------------
void U3DSceneUtilities::SetSimulationStateX(const BOOL state,
											const BOOL autoReset)
{
	IFXCHECKX_RESULT(m_pScheduler, IFX_E_NOT_INITIALIZED );

	IFXASSERT( m_pScheduler );
	IFXCHECKX( m_pScheduler->SetSimulationState( state, autoReset ) );
}

BOOL U3DSceneUtilities::GetSimulationStateX()
{
	BOOL bresult;

	IFXCHECKX_RESULT(m_pScheduler, IFX_E_NOT_INITIALIZED );

	IFXASSERT( m_pScheduler );
	IFXCHECKX( m_pScheduler->GetSimulationState( &bresult ) );

	return bresult;
}

//---------------------------------------------------------------------------
/**
This function is used to get the system time in milliseconds.

@note	System time is defined as the number of milliseconds that have
elapsed since this IFXU3D instance was initialized.

@return	Upon success, the system time in milliseconds is returned.
Otherwise, an IFXException exception is thrown containing one of
the following errors:  IFX_E_NOT_INITIALIZED.
*/
U32 U3DSceneUtilities::GetSystemTimeX()
{
	IFXCHECKX_RESULT(m_bInit, IFX_E_NOT_INITIALIZED );

	U32 time  = 0;

	IFXASSERT( m_pScheduler );
	IFXCHECKX( m_pScheduler->GetSystemTime( &time ) );

	return time;
}

//---------------------------------------------------------------------------
void U3DSceneUtilities::ProcessScheduler()
{
	IFXCHECKX(m_pScheduler->Service());

	if (!m_bEndOfU3DFile) {
		EIFXLoadState loadState;
		U32 i;
		for (i = 0; i <= m_LoadManagerCounter; i++) {
			IFXCHECKX(m_pIFXLoadManagers[m_LoadManagerCounter]->GetAggregateLoadState(loadState));
			if (IFX_LOAD_STATE_MEDIA_LOADED != loadState)
				break;
		}
		if (IFX_LOAD_STATE_MEDIA_LOADED == loadState) {
			m_bEndOfU3DFile = TRUE;
			for (i = 0; i < m_LoadManagerCounter; i++)
				IFXRELEASE( m_pIFXLoadManagers[m_LoadManagerCounter] );
		}
	}
}

//****************************************************************
// Node utilities:  finding, creation
//****************************************************************

//------------------------------------------------------------------
/**
Creates a view (camara) node that is able to see the entire
scene graph

@param	pNodeHeader	Information on the node to create.
@param	ppView		A pointer to the new view node.
@param	mps			Boolean that if TRUE indicates to use the 
multiple parents data members of the 
SGNodeHeader parameters.  Otherwise, if 
FALSE it indicates to use the single 
parent data members.

@return	Return status of this method.
**/
IFXRESULT U3DSceneUtilities::CreateCameraNode(  SGNodeHeader * pNodeHeader,
											  IFXView** ppView, BOOL mps )
{
	IFXRESULT result = IFX_OK;
	IFXView*  pView = NULL;
	U32 i;

	if (!m_bInit || !pNodeHeader || !ppView)
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	// Create a view component with an IFXUnknown interface
	if ( IFXSUCCESS(result) )
	{
		result = IFXCreateComponent( CID_IFXView, IID_IFXView,
			(void**)&pView);
	}

	// Initialize the view node
	if ( IFXSUCCESS(result) )
	{
		result = pView->SetSceneGraph( m_pSceneGraph );
	}

	if ( IFXSUCCESS(result) )
	{
		IFXDECLARELOCAL( IFXNode, pIFXParentNode );

		if (!mps) 
		{
			// Try to find its parent
			result = FindNode(pNodeHeader->ParentName, &pIFXParentNode);

			// Give the view node a pointer to its parent node.
			if ( IFXSUCCESS(result) )
				result = pView->AddParent( pIFXParentNode );

			// Set the view node's local transformation matrix
			if ( IFXSUCCESS(result) )
				result = pView->SetMatrix(0, &(pNodeHeader->NodeTm));
		} 
		else 
		{
			for (i = 0; i < pNodeHeader->NumberOfParents && IFXSUCCESS(result); i++) 
			{
				IFXRELEASE(pIFXParentNode);

				result = FindNode(pNodeHeader->ParentNames[i], &pIFXParentNode);

				if ( IFXSUCCESS(result) )
					result = pView->AddParent( pIFXParentNode );

				// Set the group node's local transformation matrix
				if ( IFXSUCCESS(result) )
					result = pView->SetMatrix(i, &(pNodeHeader->NodeTms[i]));
			}
		}
	}

	// Add an entry for the view node to the node palette.
	if ( IFXSUCCESS(result) )
	{
		IFXDECLARELOCAL( IFXPalette, pPalette );
		U32 uViewNodeId;

		IFXCHECKX(m_pSceneGraph->GetPalette( IFXSceneGraph::NODE, &pPalette ));
		IFXCHECKX(pPalette->Add( pNodeHeader->Name, &uViewNodeId ));

		// Point the node palette entry for the view to the view component
		// (created above)
		IFXCHECKX(pPalette->SetResourcePtr( uViewNodeId, pView ));
	}

	if ( IFXSUCCESS(result) )
	{
		IFXDECLARELOCAL( IFXViewResource, pVR );
		IFXDECLARELOCAL( IFXPalette, pPalette );
		U32 uResourceID;

		IFXCHECKX(IFXCreateComponent( CID_IFXViewResource, IID_IFXViewResource, (void**)&pVR ));
		IFXCHECKX(pVR->SetSceneGraph( m_pSceneGraph ));

		IFXCHECKX(m_pSceneGraph->GetPalette( IFXSceneGraph::VIEW, &pPalette ));

		result = pPalette->Find( pNodeHeader->Name, &uResourceID );
		if ( IFX_E_CANNOT_FIND == result  )
		{
			result = pPalette->Add( pNodeHeader->Name, &uResourceID );
		}

		IFXCHECKX(pPalette->SetResourcePtr( uResourceID, pVR ));

		IFXCHECKX(pView->SetViewResourceID( uResourceID ));

		// Point the view to this resource.  This isn't necessarily the view node's
		// parent.  Rather, it indicates what subsection of the scenegraph should
		// be visualized by this camera/view.  You could point a custom camera's
		// "RootNode" at any node in the scenegraph.  The camera will only see those
		// nodes at or below the node in the parent/child hierarchy.
		IFXCHECKX(pVR->SetRootNode( 0, 0 ));
	}

	if( IFXSUCCESS(result) )
		*ppView = pView; // releasing the object is left to the caller

	return result;
}

//---------------------------------------------------------------------------
IFXRESULT U3DSceneUtilities::KeepTextureDecompressed()
{
	IFXRESULT result  = IFX_OK;

	if ( m_pTexturePalette )
	{
		U32 entryCount;

		result = m_pTexturePalette->GetPaletteSize( &entryCount );

		U32 entryId = 0;

		while ( IFXSUCCESS( result ) && entryId < entryCount )
		{
			IFXDECLARELOCAL( IFXTextureObject, pTextureObject );

			result = m_pTexturePalette->GetResourcePtr(
				entryId,
				IID_IFXTextureObject,
				( void** ) &pTextureObject );

			if ( IFXSUCCESS( result ) )
				result = pTextureObject->SetKeepDecompressed( TRUE );

			++entryId;
		}
	}
	else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}

//------------------------------------------------------------------
/**
Create a group node somewhere in the scenegraph.

@param	pNodeHeader	Information on the node to create.

@return	Return status of this method.
**/
IFXRESULT U3DSceneUtilities::CreateGroupNode(SGNodeHeader * pNodeHeader, BOOL mps)
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL( IFXNode, pNode );
	U32 i;

	if (!m_bInit || !pNodeHeader )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	// Create a model
	if ( IFXSUCCESS(result) )
	{
		result = IFXCreateComponent( CID_IFXGroup, IID_IFXNode,
			(void**)&pNode);
	}

	// Initialize the model node
	if ( IFXSUCCESS(result) )
	{
		result = pNode->SetSceneGraph( m_pSceneGraph );
	}

	if ( IFXSUCCESS(result) )
	{
		IFXDECLARELOCAL( IFXNode, pIFXParentNode );

		if (!mps) 
		{
			// Try to find its parent
			result = FindNode(pNodeHeader->ParentName, &pIFXParentNode);

			// Give the group node a pointer to its parent node.
			if ( IFXSUCCESS(result) )
				result = pNode->AddParent( pIFXParentNode );

			// Set the group node's local transformation matrix
			if ( IFXSUCCESS(result) )
				result = pNode->SetMatrix(0, &(pNodeHeader->NodeTm));
		} 
		else 
		{
			for (i = 0; i < pNodeHeader->NumberOfParents && IFXSUCCESS(result); i++) 
			{
				IFXRELEASE(pIFXParentNode);

				result = FindNode(pNodeHeader->ParentNames[i], &pIFXParentNode);

				if ( IFXSUCCESS(result) )
					result = pNode->AddParent( pIFXParentNode );

				// Set the group node's local transformation matrix
				if ( IFXSUCCESS(result) )
					result = pNode->SetMatrix(i, &(pNodeHeader->NodeTms[i]));
			}
		}
	}

	// Add an entry for the group node to the node palette.
	if ( IFXSUCCESS(result) )
	{
		IFXDECLARELOCAL( IFXPalette, pPalette );
		U32 uNodeId;

		IFXCHECKX(m_pSceneGraph->GetPalette( IFXSceneGraph::NODE, &pPalette ));
		IFXCHECKX(pPalette->Add( pNodeHeader->Name, &uNodeId ));

		// Point the node palette entry for the group to the group component
		// (created above)
		IFXCHECKX(pPalette->SetResourcePtr( uNodeId, pNode ));
	}

	return result;
}

//------------------------------------------------------------------
/**
Create a light node and hook it into the scene graph.

@param	pNodeHeader	Information on the node to create.
@param	ppLight		A pointer to the light resource, *not* the
light node that uses this resource.

@return	Return status of this method.
**/
IFXRESULT U3DSceneUtilities::CreateLightNode( SGNodeHeader* pNodeHeader,
											 IFXLight** ppLight, BOOL mps)
{
	IFXRESULT result = IFX_OK;
	IFXLight* pLight = NULL; // releasing the object is left to the caller
	U32 i;

	if (!m_bInit || !pNodeHeader || !ppLight)
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	// Create a light component
	if ( IFXSUCCESS(result) )
	{
		result = IFXCreateComponent( CID_IFXLight, IID_IFXLight,
			(void**)&pLight);
	}

	// Initialize the light node
	if ( IFXSUCCESS(result) )
	{
		result = pLight->SetSceneGraph( m_pSceneGraph );
	}

	if ( IFXSUCCESS(result) )
	{
		IFXDECLARELOCAL( IFXNode, pIFXParentNode );

		if (!mps) 
		{
			// Try to find its parent
			result = FindNode(pNodeHeader->ParentName, &pIFXParentNode);

			// Give the light node a pointer to its parent node.
			if ( IFXSUCCESS(result) )
				result = pLight->AddParent( pIFXParentNode );

			// Set the light node's local transformation matrix
			if ( IFXSUCCESS(result) )
				result = pLight->SetMatrix(0, &(pNodeHeader->NodeTm));
		} 
		else 
		{
			for (i = 0; i < pNodeHeader->NumberOfParents && IFXSUCCESS(result); i++) 
			{
				IFXRELEASE(pIFXParentNode);

				result = FindNode(pNodeHeader->ParentNames[i], &pIFXParentNode);

				if ( IFXSUCCESS(result) )
					result = pLight->AddParent( pIFXParentNode );

				// Set the group node's local transformation matrix
				if ( IFXSUCCESS(result) )
					result = pLight->SetMatrix(i, &(pNodeHeader->NodeTms[i]));
			}
		}
	}

	// Add an entry for the light node to the node palette.
	if ( IFXSUCCESS(result) )
	{
		IFXDECLARELOCAL( IFXPalette, pPalette );
		U32 nodeId = 0;

		result = m_pSceneGraph->GetPalette( IFXSceneGraph::NODE, &pPalette );
	
		if ( IFXSUCCESS(result) )
			result = pPalette->Add( pNodeHeader->Name, &nodeId );

		if ( IFX_W_ALREADY_EXISTS != result )
		{
			// Point the node palette entry for the light to the light component
			// (created above)
			if ( IFXSUCCESS(result) )
				result = pPalette->SetResourcePtr( nodeId, pLight );

			if( IFXSUCCESS(result) )
				*ppLight = pLight;
		}
		else
		{
			// return warning because name mapping is not properly handled here
			pLight->Prune(); // remove parents
			pLight->SetSceneGraph(NULL);
			IFXRELEASE( pLight );
		}
	}

	return result;
}

//------------------------------------------------------------------
/**
Create a model node and attach it to the scene graph.

@param	pNodeHeader	Information about the node to create.
@param	ResourceId	The generator palette ID of the
IFXAuthorCLODResource to associate with this
scenegraph node.

@return	Return status of this method.
**/
IFXRESULT U3DSceneUtilities::CreateModelNode( SGNodeHeader * pNodeHeader,
											 U32 ResourceId, BOOL mps)
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL( IFXModel, pModel );
	U32 i;

	if (!m_bInit || !pNodeHeader )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	// Create a model
	if ( IFXSUCCESS(result) )
	{
		result = IFXCreateComponent( CID_IFXModel, IID_IFXModel,
			(void**)&pModel);
	}

	// Initialize the model node
	if ( IFXSUCCESS(result) )
	{
		result = pModel->SetSceneGraph( m_pSceneGraph );
	}

	if ( IFXSUCCESS(result) )
	{
		IFXDECLARELOCAL( IFXNode, pIFXParentNode );

		if (!mps) 
		{
			// Try to find its parent
			result = FindNode(pNodeHeader->ParentName, &pIFXParentNode);

			// Give the model node a pointer to its parent node.
			if ( IFXSUCCESS(result) )
				result = pModel->AddParent( pIFXParentNode );

			// Set the model node's local transformation matrix
			if ( IFXSUCCESS(result) )
				result = pModel->SetMatrix(0, &(pNodeHeader->NodeTm));
		} 
		else 
		{
			for (i = 0; i < pNodeHeader->NumberOfParents && IFXSUCCESS(result); i++) 
			{
				IFXRELEASE(pIFXParentNode);

				result = FindNode(pNodeHeader->ParentNames[i], &pIFXParentNode);

				if ( IFXSUCCESS(result) )
					result = pModel->AddParent( pIFXParentNode );

				// Set the group node's local transformation matrix
				if ( IFXSUCCESS(result) )
					result = pModel->SetMatrix(i, &(pNodeHeader->NodeTms[i]));
			}
		}
	}

	// Add an entry for the model node to the node palette.
	if ( IFXSUCCESS(result) )
	{
		IFXDECLARELOCAL( IFXPalette, pPalette );
		U32 uNodeId;

		IFXCHECKX(m_pSceneGraph->GetPalette( IFXSceneGraph::NODE, &pPalette ));
		IFXCHECKX(pPalette->Add( pNodeHeader->Name, &uNodeId ));

		// Point the node palette entry for the model to the model component
		// (created above)
		IFXCHECKX(pPalette->SetResourcePtr( uNodeId, pModel ));
	}

	// Record the generator palette ID of the generator in
	// the model component
	// Sets up the style used by the generator.
	if ( IFXSUCCESS(result) )
	{
		result = pModel->SetResourceIndex(ResourceId);
	}

	return result;
}

//------------------------------------------------------------------
/**
Return the IFXNode associated with a given node name.  Note
that node names are case sensitive, so be careful when adding 
nodes to a palette.

@param	pName	The case sensitive name of the node to look for.
@param	ppNode	The returned IFX Node from the node palette.

@return Return status of this method.
**/
IFXRESULT U3DSceneUtilities::FindNode(const IFXCHAR * pName,
									  IFXNode ** ppNode)
{
	IFXRESULT result = IFX_OK;
	IFXString Name(pName);
	U32     NodeId = 0;
	IFXUnknown* pUnknown = NULL;


	if (!m_bInit || !pName || !ppNode)
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	if ( pName && m_pNodePalette && IFXSUCCESS(result) )
	{
		if(wcscmp(pName, L"") == 0)
		{
			// If the name is null, look for the world node (palette ID 0).
			NodeId = 0;
		}
		else
		{
			// Else look for this name in the node palette.
			result = m_pNodePalette->Find(&Name, &NodeId);
		}
	}

	// Get the component pointed to by this palette entry
	if ( m_pNodePalette && IFXSUCCESS(result) )
	{
		result = m_pNodePalette->GetResourcePtr(NodeId, &pUnknown);
	}

	// Get the IFXNode interface of the component
	if ( pUnknown && IFXSUCCESS(result) )
	{
		result = pUnknown->QueryInterface(IID_IFXNode, (void**) ppNode);
	}

	// Clean up
	IFXRELEASE(pUnknown);

	return result;
}


//------------------------------------------------------------------
/**
Get the IFXAuthorCLODResource associated with a named model 
node.

@param	pModelName	The name of the node for which we want
the generator.
@param	ppModel		The returned generator.

@return	Return status of this method.
**/
IFXRESULT U3DSceneUtilities::GetACLODResourceFromName(const IFXCHAR *pModelName,
													  IFXAuthorCLODResource **ppACLODResource)
{
	IFXRESULT       result = IFX_OK;
	IFXPalette        *pGeneratorPalette = NULL;
	IFXModel        *pModel = NULL;
	IFXAuthorCLODResource *pACLODResource = NULL;
	U32           generatorID = 0;

	if (!m_bInit || !pModelName || !ppACLODResource)
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}


	// Get the generator palette
	if ( IFXSUCCESS(result) )
	{
		pGeneratorPalette = m_pGeneratorPalette;

		if (!pGeneratorPalette)
			result = IFX_E_NOT_INITIALIZED;
	}

	if ( IFXSUCCESS(result) )
	{
		result = GetModelFromName(pModelName, &pModel);
	}

	if ( pModel && IFXSUCCESS(result) )
	{
		generatorID = pModel->GetResourceIndex();
	}

	if ( pGeneratorPalette && IFXSUCCESS(result) )
	{
		result = pGeneratorPalette->GetResourcePtr(generatorID,
			IID_IFXAuthorCLODResource,
			(void **)&pACLODResource);
	}

	if ( ppACLODResource && pACLODResource && IFXSUCCESS(result) )
	{
		// Freeing pACLODResource is left to the caller
		*ppACLODResource = pACLODResource;
	}

	IFXRELEASE(pModel);

	return result;
}

//------------------------------------------------------------------
/**
Get the IFXModel node given only its name.

@param	pModelName	The name of the node for which we want
the model mode.
@param	ppModel		The returned model node.

@return	Return status of this method.
**/
IFXRESULT U3DSceneUtilities::GetModelFromName(  const IFXCHAR *pModelName,
											  IFXModel **ppModel)
{
	IFXRESULT       result = IFX_OK;
	IFXPalette        *pNodePalette = NULL;
	IFXNode         *pNode = NULL;
	IFXModel        *pModel = NULL;
	U32           modelID = 0;

	if (!m_bInit || !pModelName || !ppModel)
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	// Get the node palette
	if ( IFXSUCCESS(result) )
	{
		pNodePalette = m_pNodePalette;

		if (!pNodePalette)
			result = IFX_E_NOT_INITIALIZED;
	}

	// Get our model node
	if ( pNodePalette && IFXSUCCESS(result) )
	{
		result = pNodePalette->Find(pModelName, &modelID);
	}

	if ( pNodePalette && IFXSUCCESS(result) )
	{
		result = pNodePalette->GetResourcePtr(modelID, IID_IFXNode, (void **)&pNode);
	}

	// Get the model's resource
	if ( pNode && IFXSUCCESS(result) )
	{
		result = pNode->QueryInterface( IID_IFXModel, (void**)&pModel );
	}

	if ( ppModel && pModel && IFXSUCCESS(result) )
	{
		// Freeing pModel is left to the caller
		*ppModel = pModel;
	}

	IFXRELEASE(pNode);

	return result;
}


//---------------------------------------------------------------------------
IFXRESULT U3DSceneUtilities::SetMaterialResource(const IFXCHAR *pModelName,
												 IFXVector4 *Ambient,
												 IFXVector4 *Diffuse,
												 IFXVector4 *Emission,
												 IFXVector4 *Specular,
												 F32 Opacity,
												 F32 Reflectivity,
												 BOOL Transparent,
												 U32 ShaderFlags)
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL(IFXModifierChain, pModifierChain);
	IFXDECLARELOCAL(IFXModel, pModel);

	result = GetModelFromName(pModelName, &pModel);

	U32 materialID;
	U32 shaderID;

	{
		IFXDECLARELOCAL(IFXMaterialResource, pMaterialResource);
		if (IFXSUCCESS(result))
			result = IFXCreateComponent(CID_IFXMaterialResource, IID_IFXMaterialResource, (void**)&pMaterialResource);
		if (IFXSUCCESS(result))
		{
			pMaterialResource->SetSceneGraph( m_pSceneGraph );
			m_pMaterialPalette->Add( pModelName, &materialID );
			m_pMaterialPalette->SetResourcePtr( materialID, pMaterialResource );
		}

		if (IFXSUCCESS(result) && NULL != Ambient)
			result = pMaterialResource->SetAmbient(*Ambient);
		if (IFXSUCCESS(result) && NULL != Diffuse)
			result = pMaterialResource->SetDiffuse(*Diffuse);
		if (IFXSUCCESS(result) && NULL != Emission)
			result = pMaterialResource->SetEmission(*Emission);
		if (IFXSUCCESS(result) && NULL != Specular)
			result = pMaterialResource->SetSpecular(*Specular);

		if (IFXSUCCESS(result))
			result = pMaterialResource->SetOpacity(Opacity);
		if (IFXSUCCESS(result))
			result = pMaterialResource->SetReflectivity(Reflectivity);
		if (IFXSUCCESS(result))
			result = pMaterialResource->SetTransparent(Transparent);
	}

	//create new shader lit texture
	IFXDECLARELOCAL(IFXShaderLitTexture, pShaderLitTexture);
	if (IFXSUCCESS(result))
		result = IFXCreateComponent( CID_IFXShaderLitTexture, IID_IFXShaderLitTexture, (void**)&pShaderLitTexture );
	if (IFXSUCCESS(result))
	{
		pShaderLitTexture->SetMaterialID( materialID );
		pShaderLitTexture->SetSceneGraph( m_pSceneGraph );
		//add it to the shader palette
		m_pShaderPalette->Add( pModelName, &shaderID);
		m_pShaderPalette->SetResourcePtr( shaderID, pShaderLitTexture );
	}

	//create shader set
	IFXDECLARELOCAL(IFXShaderList, pShaderList);
	if (IFXSUCCESS(result))
		IFXCreateComponent(CID_IFXShaderList, IID_IFXShaderList, (void**)&pShaderList);
	if (IFXSUCCESS(result))
		pShaderList->Allocate(1);
	if (IFXSUCCESS(result))
		pShaderList->SetShader(0,shaderID);

	// Create a shader modifier
	{
		U32 i, renderableGroupIndex = 0;
		IFXDECLARELOCAL( IFXShadingModifier, pShadingModifier );
		IFXDECLARELOCAL( IFXMeshGroup, pMeshGroup );
		IFXDECLARELOCAL(IFXModifierDataPacket, pDataPacket);

		if (IFXSUCCESS(result))
			result = pModel->GetModifierChain( &pModifierChain );
		if ( IFXSUCCESS( result ) )
			result = IFXCreateComponent( CID_IFXShadingModifier, IID_IFXShadingModifier, (void**)&pShadingModifier );
		if ( IFXSUCCESS( result ) )
			result = pModifierChain->GetDataPacket( pDataPacket );
		if ( IFXSUCCESS( result ) )
			result = pDataPacket->GetDataElementIndex( DID_IFXRenderableGroup, renderableGroupIndex );
		if ( IFXSUCCESS( result ) )
			result = pDataPacket->GetDataElement( renderableGroupIndex, IID_IFXMeshGroup, (void**)&pMeshGroup );
		if ( IFXSUCCESS( result ) )
		{
			pShadingModifier->SetSceneGraph( m_pSceneGraph );
			for( i = 0; i < pMeshGroup->GetNumElements() && IFXSUCCESS( result ); i++ )
				result = pShadingModifier->SetElementShaderList( i, pShaderList );
		}
		if ( IFXSUCCESS( result ) )
			pModifierChain->AddModifier( *pShadingModifier );
	}
	return result;
}

IFXRESULT U3DSceneUtilities::FindNextModel(U32* currentID, IFXModel** pModel)
{
	IFXRESULT result = IFX_OK;
	U32 lastNodeID = 0;
	U32 nodeID;
	BOOL notFound = FALSE;

	if (!m_pNodePalette)
		result = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(result))
		result = m_pNodePalette->Last(&lastNodeID);
		
	do {
		if (IFXSUCCESS(result))
		{
			if (!m_nodeFound)
			{
				result = m_pNodePalette->First(&nodeID);
				m_nodeFound = TRUE;
			}
			else
				result = m_pNodePalette->Next(&nodeID);
		}
		if (IFXSUCCESS(result))
		{
			*currentID = nodeID;
			if (nodeID == lastNodeID)
				m_nodeFound = FALSE;
		}

		if (IFXSUCCESS(result))
		{
			result = m_pNodePalette->GetResourcePtr( nodeID, IID_IFXModel, (void**)pModel );
			if (result == IFX_E_UNSUPPORTED && nodeID < lastNodeID)
			{
				notFound = TRUE;
				result = IFX_OK;
			}
			else
				notFound = FALSE;
		}
	} while (notFound);

	return result;
}

IFXRESULT U3DSceneUtilities::GetMeshGroup(IFXModel* pModel, IFXMeshGroup** pMeshGroup)
{
	IFXRESULT result = IFX_OK;
	U32 index;

	if (NULL == pModel)
		result = IFX_E_NOT_INITIALIZED;

	IFXDECLARELOCAL(IFXModifierChain, pModifierChain);
	IFXDECLARELOCAL(IFXModifierDataPacket, pDataPacket);

	if (IFXSUCCESS(result))
		result = pModel->GetModifierChain(&pModifierChain);
	if (IFXSUCCESS(result))
		result = pModifierChain->GetDataPacket(pDataPacket);
	if (IFXSUCCESS(result))
		result = pDataPacket->GetDataElementIndex(DID_IFXRenderableGroup, index);
	if (IFXSUCCESS(result))
		result = pDataPacket->GetDataElement(index, IID_IFXMeshGroup, (void**)pMeshGroup);

	return result;
}

