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
@file CIFXNURBSModifierDecoder.cpp

This module defines the CIFXNURBSModifierDecoder component.
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXDataTypes.h"
#include "decoder.h"
#include "memfunc.h"
#include "IFXIOFunc.h"
#include "IFXCoreCIDs.h"
#include "IFXNURBSCIDs.h"
#include "CIFXNURBSModifierDecoder.h"
#include "IFXNURBSModifier.h"
#include "IFXCheckX.h"

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
U32 CIFXNURBSModifierDecoder::AddRef()
{
	return ++m_uRefCount;
}

//---------------------------------------------------------------------------
U32 CIFXNURBSModifierDecoder::Release()
{
	if ( !( --m_uRefCount ) )
	{
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return m_uRefCount;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXNURBSModifierDecoder::QueryInterface( IFXREFIID interfaceId,
												   void** ppInterface )
{
	IFXRESULT result  = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXDecoderX )
		{
			*ppInterface = static_cast<IFXDecoderX*>(this);
		}
		else if ( interfaceId == IID_IFXUnknown )
		{
			*ppInterface = static_cast<IFXUnknown*>(this);
		}
		else if ( interfaceId == IID_IFXExtensionDecoderX ) 
		{
			*ppInterface = ( IFXExtensionDecoderX* ) this;
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

// Initialize and get a reference to the core services object
void CIFXNURBSModifierDecoder::InitializeX( const IFXLoadConfig &lc )
{
	IFXModifierBaseDecoder::InitializeX(lc);
}

// Provide next block of data to the decoder
void CIFXNURBSModifierDecoder::PutNextBlockX( IFXDataBlockX &rDataBlockX )
{
	if(NULL == m_pDataBlockQueueX)
	{
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	m_pDataBlockQueueX->AppendBlockX( rDataBlockX );
	IFXModifierBaseDecoder::CreateObjectX( rDataBlockX, CID_IFXNURBSModifier );
}

void CIFXNURBSModifierDecoder::TransferX( IFXRESULT &rWarningPartialTransfer )
{
	if(NULL == m_pObject)
	{
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	// For each data block in the list
	BOOL isDone = FALSE;
	while ( IFX_OK == isDone )
	{
		// Get the next data block
		IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
		m_pDataBlockQueueX->GetNextBlockX( pDataBlockX, isDone );
		IFXASSERT(pDataBlockX);

        U32 blockType = 0;
		pDataBlockX->GetBlockTypeX( blockType );

		// Process the data block
		if( blockType == m_pContBlocks[0] )
			ProcessNURBSModifierBlockX( *pDataBlockX, 0 );
		else if( blockType == m_pContBlocks[1] )
			ProcessNURBSModifierBlockX( *pDataBlockX, 1 );
		else
			IFXCHECKX( IFX_E_UNSUPPORTED );
	}

	rWarningPartialTransfer = IFX_OK;
}

void CIFXNURBSModifierDecoder::SetContBlockTypes( U32 *blockTypes, U32 typesCount )
{
	IFXASSERT( typesCount == NURBSBLOCK_COUNT );

	if( typesCount > 0 )
	{
		m_pContBlocks = new U32[typesCount];

		U32 i;
		for( i = 0; i < typesCount; i++ )
		{
			m_pContBlocks[i] = blockTypes[i];
		}
	}
}

//***************************************************************************
//  Protected methods
//***************************************************************************


//***************************************************************************
//  Private methods
//***************************************************************************

void CIFXNURBSModifierDecoder::ProcessNURBSModifierBlockX( IFXDataBlockX
													&rDataBlockX, U32 blockNum )
{
	IFXDECLARELOCAL( IFXNURBSModifier, pModifier );
	IFXDECLARELOCAL( IFXMetaDataX, pBlockMD );
	IFXDECLARELOCAL( IFXMetaDataX, pObjectMD );
	IFXDECLARELOCAL( IFXNameMap, m_pNameMap );

	// get the required interfaces of the object
	IFXCHECKX(m_pObject->QueryInterface( IID_IFXNURBSModifier,
		(void**)&pModifier ));

	// set metadata
	IFXCHECKX( rDataBlockX.QueryInterface( IID_IFXMetaDataX, (void**)&pBlockMD ));
	IFXCHECKX( m_pObject->QueryInterface( IID_IFXMetaDataX, (void**)&pObjectMD ));
	pObjectMD->AppendX( pBlockMD );

	// Create the bitstream for reading from the data block
	{
		IFXRELEASE( m_pBitStreamX );
		IFXCHECKX( IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamX,
			(void**)&m_pBitStreamX ) );
		m_pBitStreamX->SetDataBlockX( rDataBlockX );
	}

	// 1. Model Name
	m_pBitStreamX->ReadIFXStringX( m_stringObjectName );
	m_pCoreServices->GetNameMap( IID_IFXNameMap, (void**)&m_pNameMap );
	IFXCHECKX( m_pNameMap->Map( m_uLoadId, m_ePalette, m_stringObjectName ) );

	// 2. Index for modifier chain
	m_pBitStreamX->ReadU32X( m_uChainPosition );


	U3dMemory memFunc = {NULL, U3DMalloc, U3DFree, U3DRealloc};
	U32 cnt;
	rDataBlockX.GetSizeX( cnt );
	cnt -= m_stringObjectName.LengthU8()+1;//pure block size;
	StreamCtx stream = {0, cnt, 0, (void*)m_pBitStreamX};
	U3dDecoder* pDecoder = initializeDecoder( stream, memFunc, U3DRead );
	if( blockNum == 0 )//declaration block
		decode( pModifier->GetGenerator(), &memFunc, pDecoder );
	else if( blockNum == 1 )//continuation block
		decodeCont( pModifier->GetGenerator(), &memFunc, pDecoder );
	else
		IFXCHECKX( IFX_E_UNSUPPORTED );
	finalizeDecoder( stream, pDecoder, memFunc );
}

//---------------------------------------------------------------------------
/**
This is the component constructor.  It doesn't do anything significant.

@note Significant state initialization is done by the RobustConstructor
method if it's available or an interface specific Initialize
method if one's supported.
*/
CIFXNURBSModifierDecoder::CIFXNURBSModifierDecoder()
{
	m_uRefCount = 0;
	m_pBitStreamX = NULL;
	m_pContBlocks = NULL;
	m_contBlockCount = 0;
}

//---------------------------------------------------------------------------
/**
This is the component destructor.  After all references to the component
are released, it performs any necessary cleanup activities.
*/
CIFXNURBSModifierDecoder::~CIFXNURBSModifierDecoder()
{
	if( m_pBitStreamX ) IFXRELEASE( m_pBitStreamX );
	if( m_pContBlocks ) delete [] m_pContBlocks;
}

//***************************************************************************
//  Global functions
//***************************************************************************

//---------------------------------------------------------------------------
/**
This is the CIFXNURBSModifierDecoder component factory function.  It's used by
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
IFXRESULT IFXAPI_CALLTYPE CIFXNURBSModifierDecoder_Factory(
	IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT result = IFX_OK;

	if ( ppInterface )
	{
		// Create the CIFXNURBSModifierDecoder component.
		CIFXNURBSModifierDecoder* pComponent = new CIFXNURBSModifierDecoder;

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
