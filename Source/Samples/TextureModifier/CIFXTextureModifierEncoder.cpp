//***************************************************************************
//
//  Copyright (c) 2002 - 2006 Intel Corporation
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
//
//	CIFXTextureModifierEncoder.cpp
//
//	DESCRIPTION:
//		Implementation of the CIFXTextureModifierEncoder.
//		The CIFXTextureModifierEncoder contains Texture modifier
//		encoding functionality that is used by the write manager.
//
//*****************************************************************************


#include "CIFXTextureModifierEncoder.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXCoreCIDs.h"
#include "IFXException.h"
#include "IFXModifierChain.h"
#include "IFXTextureModifierPluginCIDs.h"

// constructor
CIFXTextureModifierEncoder::CIFXTextureModifierEncoder() :
	IFXDEFINEMEMBER(m_pModifier)
{
	m_bInitialized = FALSE;
	m_pBitStream = NULL;
	m_pCoreServices = NULL;
	m_pObject = NULL;
	m_uRefCount = 0;
	m_pName = NULL;
	m_blockType = 0;
}

// destructor
CIFXTextureModifierEncoder::~CIFXTextureModifierEncoder()
{
	IFXRELEASE( m_pBitStream );
	IFXRELEASE( m_pCoreServices );
	IFXRELEASE( m_pObject );
	IFXDELETE( m_pName );
}


// IFXUnknown
U32 CIFXTextureModifierEncoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXTextureModifierEncoder::Release()
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

IFXRESULT CIFXTextureModifierEncoder::QueryInterface( IFXREFIID interfaceId,
												  void**	ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXEncoderX )
		{
			*ppInterface = ( IFXEncoderX* ) this;
			this->AddRef();
		}
		else if ( interfaceId == IID_IFXExtensionEncoderX )
		{
			*ppInterface = ( IFXExtensionEncoderX* ) this;
			this->AddRef();
		}
		else if ( interfaceId == IID_IFXUnknown )
		{
			*ppInterface = ( IFXUnknown* ) this;
			this->AddRef();
		}
		else
		{
			*ppInterface = NULL;
			rc = IFX_E_UNSUPPORTED;
		}
	}
	else
		rc = IFX_E_INVALID_POINTER;

	IFXRETURN(rc);
}


// IFXEncoderX

void CIFXTextureModifierEncoder::GetExtensionDataX( IFXExtensionData &rExtData )
{
	// This CID will be used for writing into a file,
	// so it should be a decoder CID. Later it would be
	// possible to create the decoder for data reading.
	rExtData.m_cid = CID_IFXTextureModifierDecoder;

	IFXString extName( L"Texture Modifier" );
	IFXString vendorName( L"XYZ Corporation" );
	IFXString extInfo( L"Sample Extension Modifier" );
	m_urls[0] = L"http://www.xyz.com";
	m_urls[1] = L"http://www.xyz.ru";
	rExtData.m_extensionName.Assign( &extName );
	rExtData.m_vendorName.Assign( &vendorName );
	rExtData.m_continuationBlockTypesCount = 0;
	rExtData.m_extensionURLCount = 2;
	rExtData.m_extensionURLs = m_urls;
	rExtData.m_extensionInfo.Assign( &extInfo );
	rExtData.m_modifierType = IFXModifierChain::NODE;
}

void CIFXTextureModifierEncoder::SetBlockTypesX( U32* pBlockTypes, const U32 blockTypeCount )
{
	// we know how many blocks this encoder supports, so we can check if input data is correct
	if ( blockTypeCount != 1 )
		throw IFXException( IFX_E_INVALID_RANGE );
	
	m_blockType  = pBlockTypes[0];
}

void CIFXTextureModifierEncoder::EncodeX( IFXString& rName, IFXDataBlockQueueX& rDataBlockQueue, F64 units )
{
	IFXDECLARELOCAL(IFXDataBlockX, pDataBlock);

	try
	{
		{
			// check for initialization
			if ( FALSE == m_bInitialized )
				throw IFXException( IFX_E_NOT_INITIALIZED );
			if ( NULL == m_pObject )
				throw IFXException( IFX_E_CANNOT_FIND );

			// The following elements are common to all Modifier blocks
			// and are encoded in EncodeCommonElements():
				// 1. ModelGenerator Name
				// 2. Index for modifier chain

			IFXDELETE( m_pName );
			m_pName = new IFXString( rName );
			if ( NULL == m_pName )
				IFXCHECKX( IFX_E_OUT_OF_MEMORY );

			// 1. Name
			m_pBitStream->WriteIFXStringX( *m_pName );

			// 2. Chain index
			U32 uTemp = 0;
			IFXCHECKX( m_pModifier->GetModifierChainIndex( uTemp ) );
			m_pBitStream->WriteU32X( uTemp );

			//-------------------------------------------------------------------------
			//-------------------------------------------------------------------------

			// Get a data block from the bitstream
			m_pBitStream->GetDataBlockX(pDataBlock);

			// Set the data block type
			pDataBlock->SetBlockTypeX( m_blockType );

			// Set the data block priority
			pDataBlock->SetPriorityX(0);

			// set metadata
			IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
			IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
			pDataBlock->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
			m_pModifier->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
			pBlockMD->AppendX(pObjectMD);

			// Put the data block on the list
			rDataBlockQueue.AppendBlockX(*pDataBlock);

			// clean up
			IFXRELEASE( pDataBlock );
		}
	}
	catch ( ... )
	{
		IFXRELEASE( pDataBlock );

		throw;
	}
}

void CIFXTextureModifierEncoder::InitializeX( IFXCoreServices& rCoreServices )
{
	try
	{
		// latch onto the core services object passed in
		IFXRELEASE( m_pCoreServices )
		m_pCoreServices = &rCoreServices;
		m_pCoreServices->AddRef();

		// create a bitstream
		IFXRELEASE( m_pBitStream );
		IFXCHECKX( IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamX,
									   (void**)&m_pBitStream ) );

		m_bInitialized = TRUE;
	}
	catch ( ... )
	{
		IFXRELEASE( m_pCoreServices );
		IFXRELEASE( m_pBitStream );

		throw;
	}
}

void CIFXTextureModifierEncoder::SetObjectX( IFXUnknown& rObject )
{
	IFXModifier* pModifier= NULL;

	try
	{
		// set the object
		IFXRELEASE( m_pObject );
		m_pObject = &rObject;
		m_pObject->AddRef();

		m_pObject->QueryInterface( IID_IFXModifier, (void**)&pModifier );

		pModifier->AddRef();
		IFXRELEASE( m_pModifier );
		m_pModifier = pModifier;

		IFXRELEASE( pModifier );
	}
	catch ( ... )
	{
		IFXRELEASE( m_pObject ); // release the member variable, not the input parameter
		IFXRELEASE( pModifier );

		throw;
	}
}


// Factory friend
IFXRESULT IFXAPI_CALLTYPE CIFXTextureModifierEncoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;


	if ( ppInterface )
	{
		// Create the CIFXLoadManager component.
		CIFXTextureModifierEncoder *pComponent = new CIFXTextureModifierEncoder;

		if ( pComponent )
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			rc = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		}

		else
			rc = IFX_E_OUT_OF_MEMORY;
	}

	else
		rc = IFX_E_INVALID_POINTER;


	IFXRETURN( rc );
}
