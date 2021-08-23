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
//	CIFXCLODModifier2Encoder.cpp
//
//	DESCRIPTION:
//		Implementation of the CIFXCLODModifier2Encoder.
//		The CIFXCLODModifier2Encoder contains CLOD modifier
//		encoding functionality that is used by the write manager.
//
//*****************************************************************************


#include "CIFXCLODModifier2Encoder.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXCoreCIDs.h"
#include "IFXException.h"
#include "IFXCLODModifier.h"
#include "IFXShapesCIDs.h"
#include "IFXModifierChain.h"

// constructor
CIFXCLODModifier2Encoder::CIFXCLODModifier2Encoder() :
	IFXDEFINEMEMBER(m_pModifier)
{
	m_bInitialized = FALSE;
	m_pBitStream = NULL;
	m_pCoreServices = NULL;
	m_pObject = NULL;
	m_uRefCount = 0;
	m_pName = NULL;
	m_blockType = 0;
	m_putNODB = FALSE;
}

// destructor
CIFXCLODModifier2Encoder::~CIFXCLODModifier2Encoder()
{
	IFXRELEASE( m_pBitStream );
	IFXRELEASE( m_pCoreServices );
	IFXRELEASE( m_pObject );
	IFXDELETE( m_pName );
}


// IFXUnknown
U32 CIFXCLODModifier2Encoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXCLODModifier2Encoder::Release()
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

IFXRESULT CIFXCLODModifier2Encoder::QueryInterface( IFXREFIID interfaceId,
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

void CIFXCLODModifier2Encoder::GetExtensionDataX( IFXExtensionData &rExtData )
{
	// This CID will be used for writing into a file,
	// so it should be a decoder CID. Later it would be
	// possible to create the decoder for data reading.
	rExtData.m_cid = CID_IFXCLODModifier2Decoder;

	IFXString extName( L"CLOD Modifier 2" );
	IFXString vendorName( L"XYZ Corporation" );
	IFXString extInfo( L"Insert information here" );
	m_urls[0] = L"http://www.xyz.com";
	m_urls[1] = L"http://www.xyz.ru";
	rExtData.m_extensionName.Assign( &extName );
	rExtData.m_vendorName.Assign( &vendorName );
	rExtData.m_continuationBlockTypesCount = 3;
	rExtData.m_extensionURLCount = 2;
	rExtData.m_extensionURLs = m_urls;
	rExtData.m_extensionInfo.Assign( &extInfo );
	rExtData.m_modifierType = IFXModifierChain::NODE;
}

void CIFXCLODModifier2Encoder::SetBlockTypesX( U32* pBlockTypes, const U32 blockTypeCount )
{
	// we know how many blocks this encoder supports, so we can check if input data is correct
	if ( blockTypeCount != 4 )
		throw IFXException( IFX_E_INVALID_RANGE );
	
	m_blockType  = pBlockTypes[0];
	m_blockType1 = pBlockTypes[1];
	m_blockType2 = pBlockTypes[2];
	m_blockType3 = pBlockTypes[3];
}

void CIFXCLODModifier2Encoder::EncodeX( IFXString& rName, IFXDataBlockQueueX& rDataBlockQueue, F64 units )
{
	IFXCLODModifier* pCLODModifier = NULL;

	try
	{
		{
			// check for initialization
			if ( FALSE == m_bInitialized )
				throw IFXException( IFX_E_NOT_INITIALIZED );
			if ( NULL == m_pObject )
				throw IFXException( IFX_E_CANNOT_FIND );

			IFXCHECKX( m_pObject->QueryInterface( IID_IFXCLODModifier, (void**)&pCLODModifier ) );

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

			// 3. CLOD Modifier Attributes
			// 0x00000000: Default attributes ( automatic LOD control is disabled )
			// 0x00000001: Automatic level of detail control based on screen space method
			BOOL bTemp;
			IFXCHECKX( pCLODModifier->GetCLODScreenSpaceControllerState( &bTemp ) );
			m_pBitStream->WriteU32X( (U32)bTemp );

			// 4. CLOD Screen space bias
			F32 fTemp;
			IFXCHECKX( pCLODModifier->GetLODBias( &fTemp ) );
			m_pBitStream->WriteF32X( fTemp );

			// 5. CLOD Modifier level
			IFXCHECKX( pCLODModifier->GetCLODLevel( &fTemp ) );
			m_pBitStream->WriteF32X( fTemp );

			//-------------------------------------------------------------------------
			// Done with CLODModifier specific parameters.
			//-------------------------------------------------------------------------
			IFXDECLARELOCAL(IFXDataBlockX, pDataBlock);

			// Get a data block from the bitstream
			m_pBitStream->GetDataBlockX(pDataBlock);

			// Set the data block type
			//pDataBlock->SetBlockTypeX(BlockType_ModifierCLODU3D);
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
			IFXRELEASE( pCLODModifier );
		}

		{
			// Create a bitstream
			IFXDECLARELOCAL(IFXBitStreamX, pBitStream);
			IFXCHECKX(IFXCreateComponent(CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pBitStream));

			pBitStream->WriteIFXStringX( *m_pName );

			// 2. Chain index
			U32 uTemp = 0;
			IFXCHECKX( m_pModifier->GetModifierChainIndex( uTemp ) );
			pBitStream->WriteU32X( uTemp );

			pBitStream->WriteF32X( 13.6f );
			pBitStream->WriteF32X( 18.6f );
			pBitStream->WriteF32X( 69.6f );

			//-------------------------------------------------------------------------
			// Done with CLODModifier specific parameters.
			//-------------------------------------------------------------------------
			IFXDECLARELOCAL(IFXDataBlockX, pDataBlock);

			// Get a data block from the bitstream
			pBitStream->GetDataBlockX(pDataBlock);

			// Set the data block type
			//pDataBlock->SetBlockTypeX(BlockType_ModifierCLODU3D);
			pDataBlock->SetBlockTypeX( m_blockType2 );

			// Set the data block priority
			pDataBlock->SetPriorityX(100);

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

		{
			// Create a bitstream
			IFXDECLARELOCAL(IFXBitStreamX, pBitStream);
			IFXCHECKX(IFXCreateComponent(CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pBitStream));

			pBitStream->WriteIFXStringX( *m_pName );

			// 2. Chain index
			U32 uTemp = 0;
			IFXCHECKX( m_pModifier->GetModifierChainIndex( uTemp ) );
			pBitStream->WriteU32X( uTemp );

			pBitStream->WriteU32X( 13 );
			pBitStream->WriteU32X( 18 );
			pBitStream->WriteU32X( 69 );

			//-------------------------------------------------------------------------
			// Done with CLODModifier specific parameters.
			//-------------------------------------------------------------------------
			IFXDECLARELOCAL(IFXDataBlockX, pDataBlock);

			// Get a data block from the bitstream
			pBitStream->GetDataBlockX(pDataBlock);

			// Set the data block type
			//pDataBlock->SetBlockTypeX(BlockType_ModifierCLODU3D);
			pDataBlock->SetBlockTypeX( m_blockType1 );

			// Set the data block priority
			pDataBlock->SetPriorityX(100);

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

		{
			// Create a bitstream
			IFXDECLARELOCAL(IFXBitStreamX, pBitStream);
			IFXCHECKX(IFXCreateComponent(CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pBitStream));

			pBitStream->WriteIFXStringX( *m_pName );

			// 2. Chain index
			U32 uTemp = 0;
			IFXCHECKX( m_pModifier->GetModifierChainIndex( uTemp ) );
			pBitStream->WriteU32X( uTemp );

			pBitStream->WriteF32X( 13.5f );
			pBitStream->WriteF32X( 18.5f );
			pBitStream->WriteF32X( 69.5f );

			//-------------------------------------------------------------------------
			// Done with CLODModifier specific parameters.
			//-------------------------------------------------------------------------
			IFXDECLARELOCAL(IFXDataBlockX, pDataBlock);

			// Get a data block from the bitstream
			pBitStream->GetDataBlockX(pDataBlock);

			// Set the data block type
			//pDataBlock->SetBlockTypeX(BlockType_ModifierCLODU3D);
			pDataBlock->SetBlockTypeX( m_blockType2 );

			// Set the data block priority
			pDataBlock->SetPriorityX(100);

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


		{
			// Create a bitstream
			IFXDECLARELOCAL(IFXBitStreamX, pBitStream);
			IFXCHECKX(IFXCreateComponent(CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pBitStream));

			pBitStream->WriteIFXStringX( *m_pName );

			// 2. Chain index
			U32 uTemp = 0;
			IFXCHECKX( m_pModifier->GetModifierChainIndex( uTemp ) );
			pBitStream->WriteU32X( uTemp );

			pBitStream->WriteF32X( 13.6f );
			pBitStream->WriteU32X( 18 );
			pBitStream->WriteF32X( 69.6f );

			//-------------------------------------------------------------------------
			// Done with CLODModifier specific parameters.
			//-------------------------------------------------------------------------
			IFXDECLARELOCAL(IFXDataBlockX, pDataBlock);

			// Get a data block from the bitstream
			pBitStream->GetDataBlockX(pDataBlock);

			// Set the data block type
			//pDataBlock->SetBlockTypeX(BlockType_ModifierCLODU3D);
			pDataBlock->SetBlockTypeX( m_blockType3 );

			// Set the data block priority
			pDataBlock->SetPriorityX(100);

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


		{
			// Create a bitstream
			IFXDECLARELOCAL(IFXBitStreamX, pBitStream);
			IFXCHECKX(IFXCreateComponent(CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pBitStream));

			pBitStream->WriteIFXStringX( *m_pName );

			// 2. Chain index
			U32 uTemp = 0;
			IFXCHECKX( m_pModifier->GetModifierChainIndex( uTemp ) );
			pBitStream->WriteU32X( uTemp );

			pBitStream->WriteU32X( 13 );
			pBitStream->WriteU32X( 18 );
			pBitStream->WriteU32X( 69 );

			//-------------------------------------------------------------------------
			// Done with CLODModifier specific parameters.
			//-------------------------------------------------------------------------
			IFXDECLARELOCAL(IFXDataBlockX, pDataBlock);

			// Get a data block from the bitstream
			pBitStream->GetDataBlockX(pDataBlock);

			// Set the data block type
			//pDataBlock->SetBlockTypeX(BlockType_ModifierCLODU3D);
			pDataBlock->SetBlockTypeX( m_blockType1 );

			// Set the data block priority
			pDataBlock->SetPriorityX(100);

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
//		IFXRELEASE( pDataBlock );
		IFXRELEASE( pCLODModifier );

		throw;
	}
}

void CIFXCLODModifier2Encoder::InitializeX( IFXCoreServices& rCoreServices )
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

void CIFXCLODModifier2Encoder::SetObjectX( IFXUnknown& rObject )
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
IFXRESULT IFXAPI_CALLTYPE CIFXCLODModifier2Encoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;


	if ( ppInterface )
	{
		// Create the CIFXLoadManager component.
		CIFXCLODModifier2Encoder *pComponent = new CIFXCLODModifier2Encoder;

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
