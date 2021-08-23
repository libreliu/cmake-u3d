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
//	CIFXCLODModifier2Decoder.cpp
//

#include "IFXCLODModifier.h"
#include "CIFXCLODModifier2Decoder.h"
#include "IFXCheckX.h"

CIFXCLODModifier2Decoder::CIFXCLODModifier2Decoder()
: IFXDEFINEMEMBER( m_pBitStreamX )
{
	m_contBlockCount = 0;
	m_pContBlocks = NULL;
}

CIFXCLODModifier2Decoder::~CIFXCLODModifier2Decoder()
{
	if( m_pContBlocks )
		delete [] m_pContBlocks;
}

U32 CIFXCLODModifier2Decoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXCLODModifier2Decoder::Release()
{
	if ( 1 == m_uRefCount ) {
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXCLODModifier2Decoder::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
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
void CIFXCLODModifier2Decoder::InitializeX( const IFXLoadConfig &lc )
{
	IFXModifierBaseDecoder::InitializeX(lc);
}

// Provide next block of data to the decoder
void CIFXCLODModifier2Decoder::PutNextBlockX( IFXDataBlockX &rDataBlockX )
{
	if(NULL == m_pDataBlockQueueX) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	m_pDataBlockQueueX->AppendBlockX( rDataBlockX );
	CreateObjectX(rDataBlockX, CID_IFXCLODModifier2);
}

void CIFXCLODModifier2Decoder::TransferX(IFXRESULT &rWarningPartialTransfer)
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

			if( blockType == m_pContBlocks[0] )
				ProcessCLODModifierBlockX(*pDataBlockX);
			else if( blockType == m_pContBlocks[1] )
				ProcessCLODModifierContBlockX(*pDataBlockX);
			else if( blockType == m_pContBlocks[2] )
				ProcessCLODModifierContBlock2X(*pDataBlockX);
			else if( blockType == m_pContBlocks[3] )
				ProcessCLODModifierContBlock3X(*pDataBlockX);
			else
				IFXCHECKX( IFX_E_UNSUPPORTED );
		}
	}

	rWarningPartialTransfer = IFX_OK;
}

void CIFXCLODModifier2Decoder::ProcessCLODModifierBlockX( IFXDataBlockX &rDataBlockX )
{
	// get the required interfaces of the object
	IFXDECLARELOCAL(IFXCLODModifier,pCLODModifier);
	IFXCHECKX(m_pObject->QueryInterface( IID_IFXCLODModifier, (void**)&pCLODModifier ));

	// The following elements are common to all Modifier blocks
	// and are decoded in DecodeCommonElements():
		// 1. ModelName
		// 2. Index for modifier chain

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
	IFXDECLARELOCAL(IFXNameMap, m_pNameMap);
	m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&m_pNameMap);
	IFXCHECKX(m_pNameMap->Map(m_uLoadId, m_ePalette, m_stringObjectName));

	// 2. Index for modifier chain
	m_pBitStreamX->ReadU32X( m_uChainPosition );

	// 3. CLOD Modifier Attributes
	// 0x00000000: Default attributes ( automatic LOD control is disabled )
	// 0x00000001: Automatic level of detail control based on screen space method
	U32 uTemp;
	m_pBitStreamX->ReadU32X( uTemp );
	IFXCHECKX( pCLODModifier->SetCLODScreenSpaceControllerState( (BOOL)uTemp ) );

	// 4. CLOD Screen space bias
	F32 fTemp;
	m_pBitStreamX->ReadF32X( fTemp );
	IFXCHECKX( pCLODModifier->SetLODBias( fTemp ) );

	// 5. CLOD Modifier level
	m_pBitStreamX->ReadF32X( fTemp );
	IFXCHECKX( pCLODModifier->SetCLODLevel( fTemp ) );
}

void CIFXCLODModifier2Decoder::ProcessCLODModifierContBlockX( IFXDataBlockX &rDataBlockX )
{
	IFXRELEASE( m_pBitStreamX );
	IFXCHECKX(IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&m_pBitStreamX ));
	m_pBitStreamX->SetDataBlockX( rDataBlockX );

	// 1. Model Name
	m_pBitStreamX->ReadIFXStringX(m_stringObjectName);
	// 2. Index for modifier chain
	m_pBitStreamX->ReadU32X( m_uChainPosition );

	U32 temp;
	m_pBitStreamX->ReadU32X( temp );
	m_pBitStreamX->ReadU32X( temp );
	m_pBitStreamX->ReadU32X( temp );
}

void CIFXCLODModifier2Decoder::ProcessCLODModifierContBlock2X( IFXDataBlockX &rDataBlockX )
{
	IFXRELEASE( m_pBitStreamX );
	IFXCHECKX(IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&m_pBitStreamX ));
	m_pBitStreamX->SetDataBlockX( rDataBlockX );

	// 1. Model Name
	m_pBitStreamX->ReadIFXStringX(m_stringObjectName);
	// 2. Index for modifier chain
	m_pBitStreamX->ReadU32X( m_uChainPosition );

	F32 temp;
	m_pBitStreamX->ReadF32X( temp );
	m_pBitStreamX->ReadF32X( temp );
	m_pBitStreamX->ReadF32X( temp );
}

void CIFXCLODModifier2Decoder::ProcessCLODModifierContBlock3X( IFXDataBlockX &rDataBlockX )
{
	IFXRELEASE( m_pBitStreamX );
	IFXCHECKX(IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&m_pBitStreamX ));
	m_pBitStreamX->SetDataBlockX( rDataBlockX );

	// 1. Model Name
	m_pBitStreamX->ReadIFXStringX(m_stringObjectName);
	// 2. Index for modifier chain
	m_pBitStreamX->ReadU32X( m_uChainPosition );

	F32 temp;
	U32 tempU;
	m_pBitStreamX->ReadF32X( temp );
	m_pBitStreamX->ReadU32X( tempU );
	m_pBitStreamX->ReadF32X( temp );
}


IFXRESULT IFXAPI_CALLTYPE CIFXCLODModifier2Decoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		CIFXCLODModifier2Decoder *pComponent = new CIFXCLODModifier2Decoder;

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

void CIFXCLODModifier2Decoder::SetContBlockTypes( U32 *blockTypes, U32 typesCount )
{
	IFXASSERT( typesCount == 4 );

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
