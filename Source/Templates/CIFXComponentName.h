TODO:  Replace CIFXComponentName usages with the mixed-case version of your component name.
TODO:  Replace IFXInterfaceName usages with the mixed-case version of your component's main interface name.
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
	@file	CIFXComponentName.h

			This module defines the CIFXComponentName component.
*/


#ifndef CIFXComponentName_H
#define CIFXComponentName_H


//***************************************************************************
//	Includes
//***************************************************************************


#include "IFXInterfaceName.h"


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

/**
	This is the implementation of a component that is used to TODO.

	It supports the following interfaces:  TODO.

	@note	This component's id is named TODO.

			TODO:  Pick one of the following descriptions...
	@note	This component can be instantiated multiple times.
	@note	This component is a @e singleton.  It will only be instantiated
			once.  Attempts to perform additional instantiations will hand
			back references to the existing component's interfaces.  After
			@e all references to it have been released, the component will be
			destroyed just as with standard non-singleton ones.
*/
class CIFXComponentName : public IFXInterfaceName
{
public:

	// Member functions:  IFXUnknown.
	U32		  IFXAPI AddRef();
	U32		  IFXAPI Release();
	IFXRESULT IFXAPI QueryInterface( IFXREFIID interfaceId, void** ppInterface );

	// Member functions:  IFXInterfaceName.
	// TODO:  INSERT CUSTOM METHODS.  DOCUMENT THEM IN ONLY ONE SPOT.
	// IT'S BEST TO DO THIS IN THE INTERFACE HEADER.  OTHERWISE IF IT'S
	// A PROTECTED/PRIVATE METHOD, DO IT IN THE BODY WITH THE
	// IMPLEMENTATION.
	IFXRESULT IFXAPI CustomMethod( 
						U32	 inputValue,
						U32* pOutputValue );

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXComponentName_Factory( IFXREFIID interfaceId, void** ppInterface );

private:

	// Member functions.
			CIFXComponentName();
	virtual	~CIFXComponentName();

	IFXRESULT	RobustConstructor();

	// Member data.
	U32	m_refCount;	///< Number of outstanding references to the component.
};


//***************************************************************************
//	Global function prototypes
//***************************************************************************


//***************************************************************************
//	Global data
//***************************************************************************


#endif
