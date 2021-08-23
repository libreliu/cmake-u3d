TODO:  Replace IFXModifierName usages with the mixed-case version of your modifier name.
TODO:  Address other TODOs.
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
	@file	CIFXModifierNameEncoder.cpp

			This module defines the CIFXModifierNameEncoder component.
*/


//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXCheckX.h"
#include "IFXModifierName.h"
#include "CIFXModifierNameEncoder.h"
#include "IFXPluginNameCIDs.h"

//***************************************************************************
//	Defines
//***************************************************************************


//***************************************************************************
//	Constants
//***************************************************************************

// TODO: adjast the following constant as necessary
const U32 BLOCK_TYPE_COUNT = 2;

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


//***************************************************************************
//	Local function prototypes
//***************************************************************************


//***************************************************************************
//	Public methods
//***************************************************************************


//---------------------------------------------------------------------------
U32 CIFXModifierNameEncoder::AddRef()
{
	return ++m_refCount;
}

//---------------------------------------------------------------------------
U32 CIFXModifierNameEncoder::Release()
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
IFXRESULT CIFXModifierNameEncoder::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT result	= IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXEncoderX )
		{
			*ppInterface = static_cast<IFXEncoderX*>(this);
		}
		else if ( interfaceId == IID_IFXUnknown )
		{
			*ppInterface = static_cast<IFXUnknown*>(this);
		}
		else if ( interfaceId == IID_IFXExtensionEncoderX )
		{
			*ppInterface = static_cast<IFXExtensionEncoderX*>(this);
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

void CIFXModifierNameEncoder::EncodeX( 
									IFXString& rName, 
									IFXDataBlockQueueX& rDataBlockQueue )
{
	// TODO: Add modifier encoding code here
}

void CIFXModifierNameEncoder::InitializeX( IFXCoreServices& rCoreServices )
{
}

void CIFXModifierNameEncoder::SetObjectX( IFXUnknown& rObject )
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
		IFXRELEASE( m_pObject ); // release the member variable, not the input parameter
		throw;
	}
}

// IFXExtensionEncoderX

void CIFXModifierNameEncoder::GetExtensionDataX( IFXExtensionData &rExtData )
{
	// This CID will be used for writing into a file,
	// so it should be a decoder CID. Later it would be
	// possible to create the decoder for data reading.
	rExtData.m_cid = CID_IFXModifierNameDecoder;

	// TODO: Set extension data if you need to
}

void CIFXModifierNameEncoder::SetBlockTypesX( U32* pBlockTypes, const U32 blockTypeCount )
{
	// we know how many blocks this encoder supports, so we can check if 
	// input data is correct
	IFXCHECKX_RESULT ( blockTypeCount == BLOCK_TYPE_COUNT, IFX_E_INVALID_RANGE );

	// TODO: Add code to set block types
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
CIFXModifierNameEncoder::CIFXModifierNameEncoder() :
	m_refCount( 0 ),
	m_blockType( 0 ),
	m_pObject( NULL )
{
}

//---------------------------------------------------------------------------
/**
	This is the component destructor.  After all references to the component
	are released, it performs any necessary cleanup activities.
*/
CIFXModifierNameEncoder::~CIFXModifierNameEncoder()
{
	IFXRELEASE( m_pObject );
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
IFXRESULT CIFXModifierNameEncoder::RobustConstructor()
{
	return IFX_OK;
}

//***************************************************************************
//	Global functions
//***************************************************************************

//---------------------------------------------------------------------------
/**
	This is the CIFXModifierNameEncoder component factory function.  It's used by
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
IFXRESULT IFXAPI_CALLTYPE CIFXModifierNameEncoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT result = IFX_OK;

	if ( ppInterface )
	{
		// Create the CIFXModifierNameEncoder component.
		CIFXModifierNameEncoder* pComponent = new CIFXModifierNameEncoder;

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
