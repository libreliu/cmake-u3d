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
	@file	CIFXNURBSModifier.cpp

			This module defines the CIFXNURBSModifier component.
*/


//***************************************************************************
//	Includes
//***************************************************************************

#include "CIFXNURBSModifier.h"
#include "IFXModifierDataPacket.h"
#include "IFXMesh.h"
#include "IFXDidRegistry.h"
#include "IFXSceneGraph.h"
#include "IFXCheckX.h"
#include "IFXAutoRelease.h"
#include "IFXNURBSCIDs.h"
#include "IFXCoreCIDs.h"
#include "IFXDids.h"

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

static const IFXGUID* s_pOutputDIDs[] =
{
	&DID_IFXRenderableGroup,
	&DID_IFXNeighborMesh,
	&DID_IFXRenderableGroupBounds
};

static const IFXGUID* s_scpNeighborMeshDeps[] =
{
  &DID_IFXRenderableGroup
};

//***************************************************************************
//	Local function prototypes
//***************************************************************************


//***************************************************************************
//	Public methods
//***************************************************************************


//---------------------------------------------------------------------------
U32 IFXAPI CIFXNURBSModifier::AddRef()
{
	return ++m_refCount;
}

//---------------------------------------------------------------------------
U32 IFXAPI CIFXNURBSModifier::Release()
{
	if ( !( --m_refCount ) )
	{
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return m_refCount;
}

//---------------------------------------------------------------------------
IFXRESULT IFXAPI CIFXNURBSModifier::QueryInterface( IFXREFIID interfaceId,
												    void** ppInterface )
{
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXNURBSModifier )
			*ppInterface = static_cast<IFXNURBSModifier*>(this);
		else if ( interfaceId == IID_IFXUnknown )
			*ppInterface = static_cast<IFXUnknown*>(this);
		else if ( interfaceId == IID_IFXMarker )
			*ppInterface = static_cast<IFXMarker*>(this);
		else if ( interfaceId == IID_IFXMarkerX )
			*ppInterface = static_cast<IFXMarkerX*>(this);
		else if ( interfaceId == IID_IFXModifier)
			*ppInterface = static_cast<IFXModifier*>(this);
		else if ( interfaceId == IID_IFXSubject )
			*ppInterface = static_cast<IFXSubject*>(this);
		else if ( interfaceId == IID_IFXMetaDataX )
			*ppInterface = static_cast<IFXMetaDataX*>(this);
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
// IFXMarker
IFXRESULT CIFXNURBSModifier::SetSceneGraph( IFXSceneGraph* pInSceneGraph )
{
	IFXRESULT result = IFX_OK;

	if (!m_pBoundSphereDataElement)
		result = IFXCreateComponent(CID_IFXBoundSphereDataElement,
		IID_IFXBoundSphereDataElement, (void**) &m_pBoundSphereDataElement);
	if ( IFXSUCCESS(result) )
		result = CIFXMarker::SetSceneGraph(pInSceneGraph);

	return result;
}
// IFXMarkerX
void IFXAPI CIFXNURBSModifier::GetEncoderX( IFXEncoderX*& rpEncoderX )
{
	CIFXMarker::GetEncoderX( CID_IFXNURBSModifierEncoder, rpEncoderX );
}

// IFXModifier
IFXRESULT IFXAPI CIFXNURBSModifier::GetOutputs( 
										IFXGUID**& rpOutOutputs,
										U32&       rOutNumberOfOutputs,
										U32*&      rpOutOutputDepAttrs )
{
	rOutNumberOfOutputs = sizeof( s_pOutputDIDs ) / sizeof(IFXGUID*);
	rpOutOutputs = (IFXGUID**)&s_pOutputDIDs;
	rpOutOutputDepAttrs = NULL;

	return IFX_OK;
}


IFXRESULT IFXAPI CIFXNURBSModifier::GetDependencies(
										IFXGUID*   pInOutputDID,
										IFXGUID**& rpOutInputDependencies,
										U32&       rOutNumberInputDependencies,
										IFXGUID**& rpOutOutputDependencies,
										U32&     rOutNumberOfOutputDependencies,
										U32*&      rpOutOutputDepAttrs )
{
	if (pInOutputDID == &DID_IFXNeighborMesh)
	{
		rOutNumberInputDependencies = 0;
		rpOutInputDependencies = NULL;
		rpOutOutputDependencies = (IFXGUID**)s_scpNeighborMeshDeps;
		rOutNumberOfOutputDependencies = sizeof(s_scpNeighborMeshDeps)/
			sizeof(IFXGUID*);
		rpOutOutputDepAttrs = NULL;
	}
	else
	{
		rOutNumberInputDependencies = 0;
		rpOutInputDependencies = NULL;
		rOutNumberOfOutputDependencies = 0;
		rpOutOutputDependencies = NULL;
		rpOutOutputDepAttrs = NULL;
	}
	return IFX_OK;
}


IFXRESULT IFXAPI CIFXNURBSModifier::GenerateOutput( 
										U32    inOutputDataElementIndex,
										void*& rOutData, 
										BOOL&  rNeedRelease )
{
	IFXRESULT result = IFX_OK;

	if ( inOutputDataElementIndex == m_meshGroupDataElementIndex )
	{
		IFXMeshGroup* pInputMeshGroup = NULL;
		m_pInputDataPacket->GetDataElement( m_meshGroupDataElementIndex,
			IID_IFXMeshGroup,
			(void**)&pInputMeshGroup );
		// input mesh group isn't needed if NURBSModifier
		// is just a generator. Otherwise it should be
		// merged with m_pMeshGroup
		if( !m_pMeshGroup )
		{
			IFXCreateComponent( CID_IFXMeshGroup,
				IID_IFXMeshGroup, (void**) &m_pMeshGroup );
			Tesselate();
		}
		if( m_pMeshGroup )
			m_pMeshGroup->QueryInterface( IID_IFXUnknown, &rOutData );

		rNeedRelease = TRUE;
		IFXRELEASE( pInputMeshGroup );
	}
	else if ( inOutputDataElementIndex == m_boundSphereDataElementIndex )
	{
		if( !m_pMeshGroup )
		{
			IFXCreateComponent( CID_IFXMeshGroup,
				IID_IFXMeshGroup, (void**) &m_pMeshGroup );
			Tesselate();
		}
		m_pBoundSphereDataElement->Bound() = m_pMeshGroup->CalcBoundingSphere();
		m_pBoundSphereDataElement->QueryInterface( IID_IFXUnknown, &rOutData );
		rNeedRelease = TRUE;
	}
  else if ( inOutputDataElementIndex == m_neighborMeshDataElementIndex )
  {
    if ( m_pNeighborMesh == NULL )
      IFXCreateComponent(CID_IFXNeighborMesh,
      IID_IFXNeighborMesh,
      (void**)&(m_pNeighborMesh));
    if (!m_pNeighborMesh)
      result = IFX_E_OUT_OF_MEMORY;
    if (IFXSUCCESS(result))
      result = m_pNeighborMesh->Allocate(*(m_pMeshGroup));
    if (IFXSUCCESS(result))
      result = m_pNeighborMesh->Build(*(m_pMeshGroup), NULL);
    if (IFXSUCCESS(result))
      result = m_pNeighborMesh->QueryInterface( IID_IFXUnknown, &rOutData );
    if (IFXSUCCESS(result))
      rNeedRelease = TRUE;
  }

	return result;
}


IFXRESULT IFXAPI CIFXNURBSModifier::SetDataPacket(
									IFXModifierDataPacket* pInInputDataPacket,
									IFXModifierDataPacket* pInDataPacket )
{
	IFXRESULT result = IFX_OK;

	IFXRELEASE( m_pModifierDataPacket );
	IFXRELEASE( m_pInputDataPacket );

	if ( pInDataPacket && pInInputDataPacket )
	{
		pInDataPacket->AddRef();
		m_pModifierDataPacket = pInDataPacket;
		pInInputDataPacket->AddRef();
		m_pInputDataPacket = pInInputDataPacket;
	} 
	else
		result = IFX_E_INVALID_POINTER;

	if( IFXSUCCESS(result) )
		result = pInDataPacket->GetDataElementIndex(
									DID_IFXRenderableGroup,
									m_meshGroupDataElementIndex );
	if( IFXSUCCESS(result) )
		m_pBoundSphereDataElement->RenderableIndex() =
		m_meshGroupDataElementIndex;

    if( IFXSUCCESS(result) )
      result = pInDataPacket->GetDataElementIndex(
      DID_IFXRenderableGroupBounds,
      m_boundSphereDataElementIndex );

	if( IFXSUCCESS(result) )
		result = pInDataPacket->GetDataElementIndex(
		DID_IFXNeighborMesh,
		m_neighborMeshDataElementIndex );

	return result;
}

IFXRESULT IFXAPI CIFXNURBSModifier::Notify( 
										IFXModifierMessage eInMessage,
										void*              messageContext )

{
	return IFX_OK;
}

// IFXNURBSModifier

NURBSGenerator* CIFXNURBSModifier::GetGenerator()
{
	if( !m_pGen )
		m_pGen = new NURBSGenerator;
	return m_pGen;
}

void CIFXNURBSModifier::SetGenerator( NURBSGenerator* pGen )
{
	if( m_pGen )
		delete m_pGen;
	m_pGen = pGen;
}

void CIFXNURBSModifier::Tesselate()
{
	//1. read m_pGen properties

	//2. tesselate NURBS

	//3. generate IFXMeshGroup
	//Below the SAMPLE meshgroup is generated
	if( m_pMeshGroup )
	{
		IFXRESULT ifxResult;
		U32 i;

		// mesh variables
		IFXMesh*  pDestinationMesh=NULL;
		IFXVector3Iter v3iVertex;
		IFXVector3Iter v3iNormal;
		IFXFaceIter fiFace;

		IFXCreateComponent(CID_IFXMesh, IID_IFXMesh, (void**)&pDestinationMesh);
		if(pDestinationMesh==NULL)
			return;

		//create simple mesh
		IFXVertexAttributes vertexAttributes;
		ifxResult=pDestinationMesh->Allocate(vertexAttributes,
			8,//number of vertices
			12);//number of faces
		if(ifxResult!=IFX_OK)
			return;

		// set up iterators
		pDestinationMesh->GetPositionIter(v3iVertex);
		pDestinationMesh->GetNormalIter(v3iNormal);
		pDestinationMesh->GetFaceIter(fiFace);

		//vertices
		v3iVertex.Next()->Set( -10, -10, -10 );//0
		v3iNormal.Next()->Set( 0, 0, 1 );
		v3iVertex.Next()->Set( 10, -10, -10 );//1
		v3iNormal.Next()->Set( 0, 0, 1 );
		v3iVertex.Next()->Set( 10, 10, -10 );//2
		v3iNormal.Next()->Set( 0, 0, 1 );
		v3iVertex.Next()->Set( -10, 10, -10 );//3
		v3iNormal.Next()->Set( 0, 0, 1 );

		v3iVertex.Next()->Set( -10, -10, 10 );//4
		v3iNormal.Next()->Set( 0, 0, -1 );
		v3iVertex.Next()->Set( 10, -10, 10 );//5
		v3iNormal.Next()->Set( 0, 0, -1 );
		v3iVertex.Next()->Set( 10, 10, 10 );//6
		v3iNormal.Next()->Set( 0, 0, -1 );
		v3iVertex.Next()->Set( -10, 10, 10 );//7
		v3iNormal.Next()->Set( 0, 0, -1 );

		//faces
		fiFace.Next()->Set(2,1,0);
		fiFace.Next()->Set(3,2,0);
		fiFace.Next()->Set(4,5,6);
		fiFace.Next()->Set(4,6,7);
		fiFace.Next()->Set(0,4,3);
		fiFace.Next()->Set(3,4,7);
		fiFace.Next()->Set(1,2,5);
		fiFace.Next()->Set(5,2,6);
		fiFace.Next()->Set(0,1,4);
		fiFace.Next()->Set(4,1,5);
		fiFace.Next()->Set(2,3,6);
		fiFace.Next()->Set(6,3,7);

        m_pMeshGroup->Allocate( 1 );//in case all the geometry is in one mesh
		m_pMeshGroup->SetMesh( 0, pDestinationMesh );
		IFXRELEASE( pDestinationMesh );
	}
}

//*************************
//	Protected methods
//***************************************************************************


//***************************************************************************
//	Private methods
//***************************************************************************


//---------------------------------------------------------------------------
/**
	This is the component constructor.  It doesn't do anything significant.

	@note	Significant state initialization is done by an interface specific
			Initialize method if one's supported.
*/
CIFXNURBSModifier::CIFXNURBSModifier() :
	m_refCount( 0 ),
	m_meshGroupDataElementIndex( (U32)-1 ),
	m_neighborMeshDataElementIndex( (U32)-1 ),
	m_boundSphereDataElementIndex( (U32)-1 ),
	m_pGen( NULL ),
	m_pMeshGroup( NULL ),
	m_pNeighborMesh( NULL ),
	m_pBoundSphereDataElement( NULL )
{
}

//---------------------------------------------------------------------------
/**
	This is the component destructor.  After all references to the component
	are released, it performs any necessary cleanup activities.
*/
CIFXNURBSModifier::~CIFXNURBSModifier()
{
	if( m_pGen ) delete m_pGen;
	if( m_pMeshGroup ) IFXRELEASE( m_pMeshGroup );
	if( m_pBoundSphereDataElement ) IFXRELEASE( m_pBoundSphereDataElement );
}

//---------------------------------------------------------------------------
/**
	This is the component's constructor that can return a result code.  In
	it, allocation of other components or memory can be performed.  It's
	called by the factory function only, immediately after the component has
	been created and has been AddRef'd.

	@note	If nothing meaningful happens in this method, it can be	removed
	from this class (as well as its usage from the factory function).

	@return	Upon success, IFX_OK is returned.  Otherwise, one of the
			following are returned:  TODO.
*/
IFXRESULT CIFXNURBSModifier::RobustConstructor()
{
	return IFX_OK;
}

//***************************************************************************
//	Global functions
//***************************************************************************


//---------------------------------------------------------------------------
/**
	This is the CIFXNURBSModifier component factory function.  It's used by
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
IFXRESULT IFXAPI_CALLTYPE CIFXNURBSModifier_Factory( IFXREFIID interfaceId,
													 void** ppInterface )
{
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		// Create the CIFXNURBSModifier component.
		CIFXNURBSModifier	*pComponent	= new CIFXNURBSModifier;

		if ( pComponent )
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Call the component's robust constructor that can return a
			// result code to indicate success or failure.
			result = pComponent->RobustConstructor();

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
