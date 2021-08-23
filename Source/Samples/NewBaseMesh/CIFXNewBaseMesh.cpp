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

/**
	@file	CIFXNewBaseMesh.cpp

			This module defines the CIFXNewBaseMesh component.
*/


//***************************************************************************
//	Includes
//***************************************************************************


#include "IFXModifierDataPacket.h"
#include "IFXMesh.h"
#include "IFXDidRegistry.h"
#include "IFXSceneGraph.h"
#include "IFXCheckX.h"
#include "IFXAutoRelease.h"
#include "IFXDids.h"

#include "CIFXNewBaseMesh.h"
#include "IFXNewBaseMeshPluginCIDs.h"

//***************************************************************************
//	Defines
//***************************************************************************


//***************************************************************************
//	Constants
//***************************************************************************


//***************************************************************************
//	Enumerations
//***************************************************************************


//***************************************************************************
//	Classes, structures and types
//***************************************************************************


//***************************************************************************
//	Global data
//***************************************************************************


//***************************************************************************
//	Local data
//***************************************************************************
static const IFXGUID* s_scpOutputDIDsBones[] =
{
	&DID_IFXTransform,
	&DID_IFXRenderableGroupBounds,
	&DID_IFXRenderableGroup,
	&DID_IFXCLODController,
	&DID_IFXNeighborMesh,
	&DID_IFXNeighborResController,
	&DID_IFXBonesManager
};


static const IFXGUID* s_scpNeighborMeshDeps[] =
{
	&DID_IFXRenderableGroup
};


static const IFXGUID* s_scpCLODControllerOutputDepDIDs[] =
{
	&DID_IFXRenderableGroup
};

static const U32 s_scuCLODControllerOutputDepAttrs[] =
{
	(1<<7)//IFX_MESHGROUP_CB_NUMMESHES
		| (1<<4)//IFX_MESHGROUP_CB_NORMALS
		| (1<<2)//IFX_MESHGROUP_CB_VERTEXCOUNT
		| (1<<1)//IFX_MESHGROUP_CB_FACECOUNT
};


static const IFXGUID* s_scpNeighborResControllerOutputDepDIDs[] =
{
	&DID_IFXNeighborMesh,
	&DID_IFXCLODController,
};
//***************************************************************************
//	Local function prototypes
//***************************************************************************


//***************************************************************************
//	Public methods
//***************************************************************************


//---------------------------------------------------------------------------
U32 CIFXNewBaseMesh::AddRef()
{
	return ++m_refCount;
}

//---------------------------------------------------------------------------
U32 CIFXNewBaseMesh::Release()
{
	if ( !( --m_refCount ) )
	{
		CIFXModifier::PreDestruct();
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return m_refCount;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXNewBaseMesh::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXNewBaseMesh )
			*ppInterface = (IFXNewBaseMesh*)this;
		else if ( interfaceId == IID_IFXModifier )
			*ppInterface = (IFXModifier*)this;
		else if ( interfaceId == IID_IFXUnknown )
			*ppInterface = (IFXUnknown*)this;
		else if ( interfaceId == IID_IFXMarker )
			*ppInterface = (IFXMarker*)this;
		else if ( interfaceId == IID_IFXMarkerX )
			*ppInterface = (IFXMarkerX*)this;
		else if ( interfaceId == IID_IFXMetaDataX )
			*ppInterface = (IFXMetaDataX*)this;
		else if ( interfaceId == IID_IFXSubject )
			*ppInterface = (IFXSubject*)this;
		else
		{
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( result ) )
			AddRef();
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

//---------------------------------------------------------------------------
void CIFXNewBaseMesh::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
	CIFXMarker::GetEncoderX(CID_IFXNewBaseMeshEncoder, rpEncoderX);
}


IFXRESULT CIFXNewBaseMesh::GetOutputs( IFXGUID**& rpOutOutputs,
										   U32&       rOutNumberOfOutputs,
										   U32*&      rpOutOutputDepAttrs )
{
	IFXRESULT result = IFX_OK;
	if (!m_pCLOD)
		result = IFX_E_NOT_INITIALIZED;
	if (IFXSUCCESS(result))
		result = m_pCLOD->GetOutputs(rpOutOutputs, rOutNumberOfOutputs, rpOutOutputDepAttrs);
	return result;
}

IFXRESULT CIFXNewBaseMesh::GetDependencies( IFXGUID*   pInOutputDID,
												IFXGUID**& rppOutInputDependencies,
												U32&       rOutNumberInputDependencies,
												IFXGUID**& rppOutOutputDependencies,
												U32&       rOutNumberOfOutputDependencies,
												U32*&      rpOutOutputDepAttrs )
{
	IFXRESULT result = IFX_OK;
	if (!m_pCLOD)
		result = IFX_E_NOT_INITIALIZED;
	if (IFXSUCCESS(result))
		result = m_pCLOD->GetDependencies(pInOutputDID, rppOutInputDependencies, rOutNumberInputDependencies,
			rppOutOutputDependencies, rOutNumberOfOutputDependencies, rpOutOutputDepAttrs);
	return result;
}

IFXRESULT CIFXNewBaseMesh::GenerateOutput( U32 inOutputDataElementIndex, void*& rpOutData, BOOL& rNeedRelease )
{
	IFXRESULT result = IFX_OK;
	
	if (!m_pCLOD)
		result = IFX_E_NOT_INITIALIZED;
	if (IFXSUCCESS(result)) {
		result = m_pCLOD->GenerateOutput(inOutputDataElementIndex, rpOutData, rNeedRelease);
	}

	return result;
}

IFXRESULT CIFXNewBaseMesh::SetDataPacket(
	IFXModifierDataPacket* pInInputDataPacket,
	IFXModifierDataPacket* pInDataPacket )
{
	IFXRESULT result = IFX_OK;
	if (pInDataPacket && pInInputDataPacket) {

		if (m_pCLOD) {
			result = m_pCLOD->SetDataPacket(pInInputDataPacket, pInDataPacket);
		}

		IFXRELEASE(m_pModifierDataPacket);
		IFXRELEASE(m_pInputDataPacket);

		if( IFXSUCCESS(result) )
		{
			m_pInputDataPacket = pInInputDataPacket;
			IFXADDREF(m_pInputDataPacket);
			m_pModifierDataPacket = pInDataPacket;
			IFXADDREF(m_pModifierDataPacket);
		}

		if( IFXSUCCESS(result) )
			result = pInDataPacket->GetDataElementIndex(
			DID_IFXTransform,
			m_transformDataElementIndex );

		if( IFXSUCCESS(result) )
			result = pInDataPacket->GetDataElementIndex(
			DID_IFXRenderableGroup,
			m_uMeshGroupDataElementIndex );
	}
	return result;
}

IFXRESULT CIFXNewBaseMesh::Notify( IFXModifierMessage eInMessage, void* pMessageContext )
{
	return IFX_OK;
}

//***************************************************************************
//	Protected methods
//***************************************************************************


//***************************************************************************
//	Private methods
//***************************************************************************


//---------------------------------------------------------------------------
/**
	This is the component constructor.  It doesn't do anything significant.

	@note	Significant state initialization is done by the RobustConstructor
			method if it's available or an interface specific Initialize
			method if one's supported.
*/
CIFXNewBaseMesh::CIFXNewBaseMesh() :
	m_refCount( 0 )
{
	m_uMeshGroupDataElementIndex = (U32)-1;
	m_transformDataElementIndex = (U32)-1;
	m_pCLOD = NULL;
}

//---------------------------------------------------------------------------
/**
	This is the component destructor.  After all references to the component
	are released, it performs any necessary cleanup activities.
*/
CIFXNewBaseMesh::~CIFXNewBaseMesh()
{
	if (m_pCLOD)
		m_pCLOD->Release();
}


//***************************************************************************
//	Global functions
//***************************************************************************

//---------------------------------------------------------------------------
/**
	This is the CIFXNewBaseMesh component factory function.  It's used by
	IFXCreateComponent to create an instance of the component and hand back
	a pointer to a supported interface that's requested.

	@note	This component can be instaniated multiple times.

	@param	interfaceId	Reference to the interface id that's supported by the
						component that will be handed back.
	@param	ppInterface	Pointer to the interface pointer that will be
						initialized upon successful creation of the
						component.

	@return	Upon success, IFX_OK is returned.  Otherwise, one of the
			following failures are returned:  IFX_E_COMPONENT,
			IFX_E_INVALID_POINTER, IFX_E_OUT_OF_MEMORY or
			IFX_E_UNSUPPORTED.
*/
IFXRESULT IFXAPI_CALLTYPE CIFXNewBaseMesh_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		// Create the CIFXNewBaseMesh component.
		CIFXNewBaseMesh	*pComponent	= new CIFXNewBaseMesh;

		if ( pComponent )
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			if ( IFXSUCCESS( result ) )
				result = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the RobustConstructor or QI fails,
			// this will cause the component to be destroyed.
			pComponent->Release();
		}
		else
			result = IFX_E_OUT_OF_MEMORY;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


//***************************************************************************
//	Local functions
//***************************************************************************
