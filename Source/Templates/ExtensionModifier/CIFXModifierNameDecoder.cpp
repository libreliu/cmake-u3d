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
	@file	CIFXModifierNameDecoder.cpp

			This module defines the CIFXModifierNameDecoder component.
*/


//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXPluginNameCIDs.h"
#include "CIFXModifierNameDecoder.h"
#include "IFXModifierName.h"
#include "IFXCheckX.h"

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


//***************************************************************************
//	Local function prototypes
//***************************************************************************


//***************************************************************************
//	Public methods
//***************************************************************************


//---------------------------------------------------------------------------
U32 CIFXModifierNameDecoder::AddRef()
{
	return ++m_refCount;
}

//---------------------------------------------------------------------------
U32 CIFXModifierNameDecoder::Release()
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
IFXRESULT CIFXModifierNameDecoder::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT	result	= IFX_OK;

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
void CIFXModifierNameDecoder::InitializeX(
									IFXCoreServices &rCoreServices, 
									U32 loadId, 
									BOOL isExternal, 
									IFXSceneGraph::EIFXPalette ePalette)
{
	CIFXModifierBaseDecoder::InitializeX( rCoreServices, loadId, isExternal, ePalette );
}

// Provide next block of data to the decoder
void CIFXModifierNameDecoder::PutNextBlockX( IFXDataBlockX &rDataBlockX )
{
	if(NULL == m_pDataBlockQueueX) 
	{
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	m_pDataBlockQueueX->AppendBlockX( rDataBlockX );
	CIFXModifierBaseDecoder::CreateObjectX( rDataBlockX, CID_IFXModifierName );
}

void CIFXModifierNameDecoder::TransferX( IFXRESULT &rWarningPartialTransfer )
{
	if(NULL == m_pCoreServices || NULL == m_pObject) 
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

		if ( pDataBlockX )
		{
			// Process the data block
			ProcessModifierNameBlockX( *pDataBlockX );
		}
	}

	rWarningPartialTransfer = IFX_OK;
}

void CIFXModifierNameDecoder::SetContBlockTypes( U32 *blockTypes, U32 typesCount )
{
	// TODO: Add continuation block types code handling here
}

//***************************************************************************
//	Protected methods
//***************************************************************************


//***************************************************************************
//	Private methods
//***************************************************************************

void CIFXModifierNameDecoder::ProcessModifierNameBlockX( IFXDataBlockX &rDataBlockX )
{
	// TODO: Add modifier block processing code here
}

//---------------------------------------------------------------------------
/**
	This is the component constructor.  It doesn't do anything significant.

	@note	Significant state initialization is done by the RobustConstructor
			method if it's available or an interface specific Initialize
			method if one's supported.
*/
CIFXModifierNameDecoder::CIFXModifierNameDecoder() :
	m_refCount( 0 )
{
}

//---------------------------------------------------------------------------
/**
	This is the component destructor.  After all references to the component
	are released, it performs any necessary cleanup activities.
*/
CIFXModifierNameDecoder::~CIFXModifierNameDecoder()
{
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
IFXRESULT CIFXModifierNameDecoder::RobustConstructor()
{
	return IFX_OK;
}


//***************************************************************************
//	Global functions
//***************************************************************************

//---------------------------------------------------------------------------
/**
	This is the CIFXModifierNameDecoder component factory function.  It's used by
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
IFXRESULT IFXAPI_CALLTYPE CIFXModifierNameDecoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT result = IFX_OK;

	if ( ppInterface )
	{
		// Create the CIFXModifierNameDecoder component.
		CIFXModifierNameDecoder* pComponent = new CIFXModifierNameDecoder;

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
