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

//
//	CIFXTextureModifierDecoder.cpp
//

#include "CIFXTextureModifierDecoder.h"
#include "IFXCheckX.h"
#include "IFXTextureModifierPluginCIDs.h"

CIFXTextureModifierDecoder::CIFXTextureModifierDecoder()
{
}

CIFXTextureModifierDecoder::~CIFXTextureModifierDecoder()
{
}

U32 CIFXTextureModifierDecoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXTextureModifierDecoder::Release()
{
	if ( 1 == m_uRefCount ) {
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXTextureModifierDecoder::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		if ( interfaceId == IID_IFXDecoderX )
		{
			*ppInterface = ( IFXDecoderX* ) this;
			this->AddRef();
		}
		else if ( interfaceId == IID_IFXUnknown ) 
		{
			*ppInterface = ( IFXUnknown* ) this;
			this->AddRef();
		} 
		else if ( interfaceId == IID_IFXExtensionDecoderX ) 
		{
			*ppInterface = ( IFXExtensionDecoderX* ) this;
			this->AddRef();
		} 
		else
		{
			*ppInterface = NULL;
			rc = IFX_E_UNSUPPORTED;
		}
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	return rc;
}

// Initialize and get a reference to the core services object
void CIFXTextureModifierDecoder::InitializeX( const IFXLoadConfig &lc )
{
	IFXModifierBaseDecoder::InitializeX(lc);
}

// Provide next block of data to the decoder
void CIFXTextureModifierDecoder::PutNextBlockX( IFXDataBlockX &rDataBlockX )
{
	if(NULL == m_pDataBlockQueueX) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	m_pDataBlockQueueX->AppendBlockX( rDataBlockX );
	CreateObjectX(rDataBlockX, CID_IFXTextureModifier);
}

void CIFXTextureModifierDecoder::TransferX(IFXRESULT &rWarningPartialTransfer)
{
	if(NULL == m_pCoreServices || NULL == m_pObject) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	// For each data block in the list
	BOOL bDone = FALSE;
	while ( IFX_OK == bDone) {
		// Get the next data block
		IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
		m_pDataBlockQueueX->GetNextBlockX( pDataBlockX, bDone);

		if(pDataBlockX) 
		{
			U32 blockType;
			pDataBlockX->GetBlockTypeX( blockType );

			if( blockType == m_blockType )
				ProcessTextureModifierBlockX(*pDataBlockX);
			else
				IFXCHECKX( IFX_E_UNSUPPORTED );
		}
	}

	rWarningPartialTransfer = IFX_OK;
}

void CIFXTextureModifierDecoder::ProcessTextureModifierBlockX( IFXDataBlockX &rDataBlockX )
{
	// This version of Texture Modifier has only hardcoded functionality, so
	// it doesn't write/read any data except some common values like name and 
	// chain index. These elements are common to all Modifier blocks and are
	// decoded in DecodeCommonElements(), so we can skip them since we decode
	// nothing after them. In case if any information specific to Texture Mod
	// are added then uncomment the following block and add specific data
	// decoding after it.

/*
	// set metadata
	IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
	IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
	rDataBlockX.QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
	m_pObject->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
	pObjectMD->AppendX(pBlockMD);

	// Create the bitstream for reading from the data block
	{
		IFXRELEASE( m_pBitStreamX );
		IFXCHECKX(IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&m_pBitStreamX ));
		m_pBitStreamX->SetDataBlockX( rDataBlockX );
	}

	// 1. Model Name
	m_pBitStreamX->ReadIFXStringX(m_stringObjectName);
	IFXDECLARELOCAL(IFXNameMap, pNameMap);
	m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap);
	IFXCHECKX(pNameMap->Map(m_uLoadId, m_ePalette, m_stringObjectName));

	// 2. Index for modifier chain
	m_pBitStreamX->ReadU32X( m_uChainPosition );*/
}

IFXRESULT IFXAPI_CALLTYPE CIFXTextureModifierDecoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		CIFXTextureModifierDecoder *pComponent = new CIFXTextureModifierDecoder;

		if ( pComponent ) {
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			rc = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		} else {
			rc = IFX_E_OUT_OF_MEMORY;
		}
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}

void CIFXTextureModifierDecoder::SetContBlockTypes( U32 *blockTypes, U32 typesCount )
{
	IFXASSERT( typesCount == 1 );

	m_blockType = blockTypes[0];
}
