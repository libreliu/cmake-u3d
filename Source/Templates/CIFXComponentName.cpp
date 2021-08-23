TODO:  Replace CIFXComponentName usages with the mixed-case version of your component name.
TODO:  Replace IFXInterfaceName usages with the mixed-case version of your component's main interface name.
TODO:  If the component is to be a singleton address TODO SINGLETON usages.
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
	@file	CIFXComponentName.cpp

			This module defines the CIFXComponentName component.
*/


//***************************************************************************
//	Includes
//***************************************************************************


#include "CIFXComponentName.h"


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


// TODO SINGLETON:  ENABLE THIS IF THE COMPONENT IS A SINGLETON.  OTHERWISE, REMOVE IT.
#if 0
/**
	Pointer to the static singleton component instance.

	@note	This pointer will be NULL if the singleton doesn't exist.  It
			will also become NULL if the last reference to the component is
			released and it's destroyed.
*/
static CIFXComponentName	*gs_pSingleton	= NULL;
#endif


//***************************************************************************
//	Local function prototypes
//***************************************************************************


//***************************************************************************
//	Public methods
//***************************************************************************


//---------------------------------------------------------------------------
U32 CIFXComponentName::AddRef()
{
	return ++m_refCount;
}

//---------------------------------------------------------------------------
U32 CIFXComponentName::Release()
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
IFXRESULT CIFXComponentName::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXInterfaceName )
		{
			*ppInterface = ( IFXInterfaceName* ) this;
		}
		else if ( interfaceId == IID_IFXUnknown )
		{
			*ppInterface = ( IFXUnknown* ) this;
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
// TODO:  INSERT CUSTOM METHODS.  DOCUMENT THEM IN ONLY ONE SPOT.  IT'S BEST
// TO DO THIS IN INTERFACE HEADER.  OTHERWISE IF IT'S A PROTECTED/PRIVATE
// METHOD, DO IT IN THE BODY WITH THE IMPLEMENTATION.
IFXRESULT CIFXComponentName::CustomMethod( U32	inputValue,
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

	@note	Significant state initialization is done by the RobustConstructor
			method if it's available or an interface specific Initialize
			method if one's supported.
*/
CIFXComponentName::CIFXComponentName() :
	m_refCount( 0 )
{
// TODO SINGLETON:  ENABLE THIS IF THE COMPONENT IS A SINGLETON.  OTHERWISE, REMOVE IT.
#if 0
	// Set the module global singleton pointer.
	IFXASSERT( !gs_pSingleton );
	gs_pSingleton	= this;
#endif
}

//---------------------------------------------------------------------------
/**
	This is the component destructor.  After all references to the component
	are released, it performs any necessary cleanup activities.
*/
CIFXComponentName::~CIFXComponentName()
{
// TODO SINGLETON:  ENABLE THIS IF THE COMPONENT IS A SINGLETON.  OTHERWISE, REMOVE IT.
#if 0
	// Clear the module global singleton pointer.
	IFXASSERT( gs_pSingleton );
	gs_pSingleton	= NULL;
#endif
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
IFXRESULT CIFXComponentName::RobustConstructor()
{
	return IFX_OK;
}


//***************************************************************************
//	Global functions
//***************************************************************************


// TODO SINGLETON:  SELECT AND KEEP THE DESIRED FACTORY FUNCTION.  REMOVE THE UNNEEDED ONE.
#if 1
//---------------------------------------------------------------------------
/**
	This is the CIFXComponentName component factory function.  It's used by
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
IFXRESULT IFXAPI_CALLTYPE CIFXComponentName_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		// Create the CIFXComponentName component.
		CIFXComponentName	*pComponent	= new CIFXComponentName;

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
#else
//---------------------------------------------------------------------------
/**
	This is the CIFXComponentName singleton component factory function.  It's
	used indirectly by IFXCreateComponent to create an instance of the
	component and hand back a pointer to a supported interface that's
	requested.

	@note	This component is a @e singleton.  It will only be instantiated
			once.  Attempts to perform additional instantiations will hand
			back references to the existing component's interfaces.

	@note	After @e all references to it have been released, the component
			will be destroyed just as with standard non-singleton ones.

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
IFXRESULT IFXAPI_CALLTYPE CIFXComponentName_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		// Does the singleton component already exist?
		if ( gs_pSingleton )
		{
			// It does exist, so just QI for the requested interface.
			result = gs_pSingleton->QueryInterface( interfaceId, ppInterface );
		}
		else
		{
			// It doesn't exist, so try to create it.  Note:  The component
			// class sets up gs_pSingleton upon construction and NULLs it
			// upon destruction.
			CIFXComponentName	*pComponent	= new CIFXComponentName;

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
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}
#endif


//***************************************************************************
//	Local functions
//***************************************************************************
