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
@file CIFXNURBSModifierEncoder.cpp

This module defines the CIFXNURBSModifierEncoder component.
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXDataTypes.h"
#include "encoder.h"
#include "memfunc.h"
#include "IFXNURBSModifier.h"
#include "IFXIOFunc.h"
#include "IFXCheckX.h"
#include "CIFXNURBSModifierEncoder.h"
#include "IFXMesh.h"
#include "IFXCoreCIDs.h"
#include "IFXNURBSCIDs.h"
#include "IFXMetaDataX.h"
#include "IFXAutoRelease.h"
#include "IFXModifierDataPacket.h"
#include "IFXModifierChain.h"


//***************************************************************************
//  Defines
//***************************************************************************

#define NURBSBLOCK_COUNT 2

//***************************************************************************
//  Constants
//***************************************************************************


//***************************************************************************
//  Enumerations
//***************************************************************************


//***************************************************************************
//  Classes, structures and types
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************


//***************************************************************************
//  Local data
//***************************************************************************


//***************************************************************************
//  Local function prototypes
//***************************************************************************


//***************************************************************************
//  Public methods
//***************************************************************************


//---------------------------------------------------------------------------
U32 CIFXNURBSModifierEncoder::AddRef()
{
	return ++m_refCount;
}

//---------------------------------------------------------------------------
U32 CIFXNURBSModifierEncoder::Release()
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
IFXRESULT CIFXNURBSModifierEncoder::QueryInterface( IFXREFIID interfaceId,
												    void** ppInterface )
{
	IFXRESULT result  = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXEncoderX )
		{
			*ppInterface = ( IFXEncoderX* ) this;
		}
		else if ( interfaceId == IID_IFXUnknown )
		{
			*ppInterface = ( IFXUnknown* ) this;
		}
		else if ( interfaceId == IID_IFXExtensionEncoderX )
		{
			*ppInterface = ( IFXExtensionEncoderX* ) this;
		}
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

// IFXEncoderX

void CIFXNURBSModifierEncoder::EncodeX(
									   IFXString& rName,
									   IFXDataBlockQueueX& rDataBlockQueue,
									   F64 units)
{
	// check for initialization
	IFXCHECKX_RESULT( NULL != m_pObject, IFX_E_CANNOT_FIND );

	IFXDECLARELOCAL( IFXNURBSModifier, pModifier );

	IFXCHECKX( m_pObject->QueryInterface(
		IID_IFXNURBSModifier,
		(void**)&pModifier ) );

	{
		// create a bitstream
		IFXDECLARELOCAL( IFXBitStreamX, pBitStream );

		IFXCHECKX( IFXCreateComponent(
			CID_IFXBitStreamX,
			IID_IFXBitStreamX,
			(void**)&pBitStream ) );


		// 1. Name
		pBitStream->WriteIFXStringX( rName );

		// 2. Chain index
		U32 index = 0;
		IFXCHECKX( pModifier->GetModifierChainIndex( index ) );
		pBitStream->WriteU32X( index );

		U3dMemory memFunc = {NULL, U3DMalloc, U3DFree, U3DRealloc};
		StreamCtx stream = {0, 0, 0, (void*)pBitStream};
		U3dEncoder* pEncoder = initializeEncoder( stream, memFunc, U3DIFXWrite );

		encode( pModifier->GetGenerator(), &memFunc, pEncoder );
		finalizeEncoder( stream, pEncoder );

		IFXDECLARELOCAL( IFXDataBlockX, pDataBlock );

		// Get a data block from the bitstream
		pBitStream->GetDataBlockX( pDataBlock );

		// Set the data block type
		pDataBlock->SetBlockTypeX( m_blockType );

		// Set the data block priority
		pDataBlock->SetPriorityX(0);

		// set metadata
		{
			IFXDECLARELOCAL( IFXMetaDataX, pBlockMD );
			IFXDECLARELOCAL( IFXMetaDataX, pObjectMD );

			IFXCHECKX( pDataBlock->QueryInterface( IID_IFXMetaDataX,			
				(void**)&pBlockMD ) );
			IFXCHECKX( pModifier->QueryInterface( IID_IFXMetaDataX,
				(void**)&pObjectMD ) );
			pBlockMD->AppendX( pObjectMD );

			// Put the data block on the list
			rDataBlockQueue.AppendBlockX( *pDataBlock );
		}
	}
	{
		// create a bitstream
		IFXDECLARELOCAL( IFXBitStreamX, pBitStream );

		IFXCHECKX( IFXCreateComponent(
			CID_IFXBitStreamX,
			IID_IFXBitStreamX,
			(void**)&pBitStream ) );


		// 1. Name
		pBitStream->WriteIFXStringX( rName );

		// 2. Chain index
		U32 index = 0;
		IFXCHECKX( pModifier->GetModifierChainIndex( index ) );
		pBitStream->WriteU32X( index );

		U3dMemory memFunc = {NULL, U3DMalloc, U3DFree, U3DRealloc};
		StreamCtx stream = {0, 0, 0, (void*)pBitStream};
		U3dEncoder* pEncoder = initializeEncoder( stream, memFunc, U3DIFXWrite );

		encodeCont( pModifier->GetGenerator(), &memFunc, pEncoder );
		finalizeEncoder( stream, pEncoder );

		IFXDECLARELOCAL( IFXDataBlockX, pDataBlock );

		// Get a data block from the bitstream
		pBitStream->GetDataBlockX( pDataBlock );

		// Set the data block type
		pDataBlock->SetBlockTypeX( m_blockTypeCont );

		// Set the data block priority
		pDataBlock->SetPriorityX(0);

		// set metadata
		{
			IFXDECLARELOCAL( IFXMetaDataX, pBlockMD );
			IFXDECLARELOCAL( IFXMetaDataX, pObjectMD );

			IFXCHECKX( pDataBlock->QueryInterface( IID_IFXMetaDataX,			
				(void**)&pBlockMD ) );
			IFXCHECKX( pModifier->QueryInterface( IID_IFXMetaDataX,
				(void**)&pObjectMD ) );
			pBlockMD->AppendX( pObjectMD );

			// Put the data block on the list
			rDataBlockQueue.AppendBlockX( *pDataBlock );
		}
	}
}

void CIFXNURBSModifierEncoder::InitializeX( IFXCoreServices& rCoreServices )
{
}

void CIFXNURBSModifierEncoder::SetObjectX( IFXUnknown& rObject )
{
	try
	{
		// set the object
		IFXRELEASE( m_pObject );
		m_pObject = &rObject;
		m_pObject->AddRef();
	}
	catch ( ... )
	{
		// release the member variable, not the input parameter
		IFXRELEASE( m_pObject );
		throw;
	}
}

// IFXEncoderExtX

void CIFXNURBSModifierEncoder::GetExtensionDataX( IFXExtensionData &rExtData )
{
	// This CID will be used for writing into a file,
	// so it should be a decoder CID. Later it would be
	// possible to create the decoder for data reading.
	rExtData.m_cid = CID_IFXNURBSModifierDecoder;

	rExtData.m_extensionName.Assign( L"NURBS modifier" );
	rExtData.m_vendorName.Assign( L"Intel Corporation" );
	rExtData.m_modifierType = IFXModifierChain::RESOURCE;
	rExtData.m_continuationBlockTypesCount = 1;
}

void CIFXNURBSModifierEncoder::SetBlockTypesX( U32* pBlockTypes,
											   const U32 blockTypeCount )
{
	// we know that this encoder supports only declaration block (without any
	// continuation blocks), so we can check if input data is correct
	if ( blockTypeCount > NURBSBLOCK_COUNT )
		throw IFXException( IFX_E_INVALID_RANGE );

	m_blockType = pBlockTypes[0];
	m_blockTypeCont = pBlockTypes[1];
}


//***************************************************************************
//  Protected methods
//***************************************************************************


//***************************************************************************
//  Private methods
//***************************************************************************


//---------------------------------------------------------------------------
/**
This is the component constructor.  It doesn't do anything significant.

@note Significant state initialization is done by the RobustConstructor
method if it's available or an interface specific Initialize
method if one's supported.
*/
CIFXNURBSModifierEncoder::CIFXNURBSModifierEncoder() :
m_refCount( 0 ),
m_blockType( 0 ),
m_blockTypeCont( 0 ),
m_pObject( NULL )
{
}

//---------------------------------------------------------------------------
/**
This is the component destructor.  After all references to the component
are released, it performs any necessary cleanup activities.
*/
CIFXNURBSModifierEncoder::~CIFXNURBSModifierEncoder()
{
	IFXRELEASE( m_pObject );
}

//***************************************************************************
//  Global functions
//***************************************************************************

//---------------------------------------------------------------------------
/**
This is the CIFXNURBSModifierEncoder component factory function.  It's used by
IFXCreateComponent to create an instance of the component and hand back
a pointer to a supported interface that's requested.

@note This component can be instaniated multiple times.

@param  interfaceId Reference to the interface id that's supported by the
component that will be handed back.
@param  ppInterface Pointer to the interface pointer that will be
initialized upon successful creation of the
component.

@return Upon success, IFX_OK is returned.  Otherwise, one of the
following failures are returned:  IFX_E_COMPONENT,
IFX_E_INVALID_POINTER, IFX_E_OUT_OF_MEMORY or
IFX_E_UNSUPPORTED.
*/
IFXRESULT IFXAPI_CALLTYPE CIFXNURBSModifierEncoder_Factory(
	IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT result = IFX_OK;

	if ( ppInterface )
	{
		// Create the CIFXNURBSModifierEncoder component.
		CIFXNURBSModifierEncoder* pComponent = new CIFXNURBSModifierEncoder;

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
//  Local functions
//***************************************************************************
