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
	@file	CIFXModifierName.cpp

			This module defines the CIFXModifierName component.
*/


//***************************************************************************
//	Includes
//***************************************************************************

#include "CIFXModifierName.h"
#include "IFXModifierDataPacket.h"

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

// TODO:  Customize these example input and output DID lists.

static const IFXGUID* s_pOutputDIDs[] =
{
	&DID_IFXOutputDidName1
};

static const IFXGUID* s_pInputDIDs[] =
{
	&DID_IFXInputDidName1
};

//***************************************************************************
//	Local function prototypes
//***************************************************************************


//***************************************************************************
//	Public methods
//***************************************************************************


//---------------------------------------------------------------------------
U32 CIFXModifierName::AddRef()
{
	return ++m_refCount;
}

//---------------------------------------------------------------------------
U32 CIFXModifierName::Release()
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
IFXRESULT CIFXModifierName::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXModifierName )
		{
			*ppInterface = static_cast<IFXModifierName*>(this);
		}
		else if ( interfaceId == IID_IFXUnknown )
		{
			*ppInterface = static_cast<IFXUnknown*>(this);
		}
		else if ( interfaceId == IID_IFXMarker )
		{
			*ppInterface = static_cast<IFXMarker*>(this);
		}
		else if ( interfaceId == IID_IFXMarkerX )
		{
			*ppInterface = static_cast<IFXMarkerX*>(this);
		}
		else if ( interfaceId == IID_IFXModifier)
		{
			*ppInterface = static_cast<IFXModifier*>(this);
		}
		else if ( interfaceId == IID_IFXSubject )
		{
			*ppInterface = static_cast<IFXSubject*>(this);
		}
		else if ( interfaceId == IID_IFXMetaDataX )
		{
			*ppInterface = static_cast<IFXMetaDataX*>(this);
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

//---------------------------------------------------------------------------
// IFXMarkerX
void CIFXModifierName::GetEncoderX( IFXEncoderX*& rpEncoderX )
{
	CIFXMarker::GetEncoderX( CID_IFXModifierNameEncoder, rpEncoderX );
}


//---------------------------------------------------------------------------
// IFXModifier

IFXRESULT CIFXModifierName::GetOutputs( 
										IFXGUID**& rpOutOutputs,
										U32&       rOutNumberOfOutputs,
										U32*&      rpOutOutputDepAttrs )
{
	rOutNumberOfOutputs = sizeof( s_pOutputDIDs ) / sizeof(IFXGUID*);
	rpOutOutputs = (IFXGUID**)&s_pOutputDIDs;
	rpOutOutputDepAttrs = NULL;

	return IFX_OK;
}


IFXRESULT CIFXModifierName::GetDependencies(
										IFXGUID*   pInOutputDID,
										IFXGUID**& rpOutInputDependencies,
										U32&       rOutNumberInputDependencies,
										IFXGUID**& rpOutOutputDependcies,
										U32&       rOutNumberOfOutputDependcies,
										U32*&      rpOutOutputDepAttrs )
{
	// TODO: Add code to return dependencies
}


IFXRESULT CIFXModifierName::GenerateOutput( 
										U32    inOutputDataElementIndex,
										void*& rOutData, 
										BOOL&  rNeedRelease )
{
	// TODO: Add code to generate output
}


IFXRESULT CIFXModifierName::SetDataPacket(
										IFXModifierDataPacket* pInInputDataPacket,
										IFXModifierDataPacket* pInDataPacket )
{
	// TODO: Add code to set data packet
}

IFXRESULT CIFXModifierName::Notify( 
										IFXModifierMessage eInMessage,
										void*              messageContext )

{
	return IFX_OK;
}


//---------------------------------------------------------------------------
// TODO:  INSERT CUSTOM METHODS.  DOCUMENT THEM IN ONLY ONE SPOT.  IT'S BEST
// TO DO THIS IN INTERFACE HEADER.  OTHERWISE IF IT'S A PROTECTED/PRIVATE
// METHOD, DO IT IN THE BODY WITH THE IMPLEMENTATION.
IFXRESULT CIFXModifierName::CustomMethod( U32	inputValue,
										  U32*	pOutputValue )
{
	return IFX_E_UNSUPPORTED;
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

	@note	Significant state initialization is done by an interface specific
			Initialize method if one's supported.
*/
CIFXModifierName::CIFXModifierName() :
	m_refCount( 0 )
{
}

//---------------------------------------------------------------------------
/**
	This is the component destructor.  After all references to the component
	are released, it performs any necessary cleanup activities.
*/
CIFXModifierName::~CIFXModifierName()
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
IFXRESULT CIFXModifierName::RobustConstructor()
{
	return IFX_OK;
}

//***************************************************************************
//	Global functions
//***************************************************************************


//---------------------------------------------------------------------------
/**
	This is the CIFXModifierName component factory function.  It's used by
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
IFXRESULT IFXAPI_CALLTYPE CIFXModifierName_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		// Create the CIFXModifierName component.
		CIFXModifierName	*pComponent	= new CIFXModifierName;

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
