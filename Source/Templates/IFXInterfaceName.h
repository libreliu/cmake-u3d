TODO:  Replace IFXInterfaceName usages with the mixed-case version of your interface name.
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
	@file	IFXInterfaceName.h

			This header defines the IFXInterfaceName interface and
			the associated interface identifier.

	@note	The IFXInterfaceName interface is supported by the TODO
			component.  Its component id is named TODO.
*/


#ifndef IFXInterfaceName_H
#define IFXInterfaceName_H


//***************************************************************************
//	Includes
//***************************************************************************


#include "IFXUnknown.h"


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
//	Interfaces and interface identifiers
//***************************************************************************


//---------------------------------------------------------------------------
/**
	This interface is used to TODO.

	@note	The associated IID is named IID_IFXInterfaceName.
*/
class IFXInterfaceName : public IFXUnknown
{
public:

	/**
		TODO:  INSERT CUSTOM METHOD PROTOTYPES AND DESCRIPTIONS.
		DOCUMENT THEM IN ONLY ONE SPOT.  IT'S BEST TO DO THIS IN THE
		INTERFACE HEADER HERE TO REDUCE DUPLICATION.

		@param	inputValue		An input value.
		@param	pOutputValue	A pointer to the output value that is
								initialize upon success.

		@return	Upon success, IFX_OK is returned.  Otherwise, one of the
				following failures are returned:  TODO.
	*/
	virtual	IFXRESULT IFXAPI CustomMethod( 
								U32	 inputValue,
								U32* pOutputValue ) = 0;
};

/**
	TODO: REPLACE FOLLOWING GUID WITH ACTUAL GUID OF THIS INTERFACE. USE 
	SPECIAL TOOLS TO GENERATE UNIQUE GUID (EX. MS GUIDGEN)

	This IID identifies the IFXInterfaceName interface.

	@note	The GUID string is {TODO}. 
			FOR EXAMPLE {B5869C62-5545-11D5-8736-00008652D936}.
*/
IFXDEFINE_GUID(IID_IFXInterfaceName,
0xB5869C62, 0x5545, 0x11D5, 0x87, 0x36, 0x00, 0x00, 0x86, 0x52, 0xD9, 0x36);


//***************************************************************************
//	Failure return codes
//***************************************************************************


/**
	TODO:  Insert module/component/interface specific return code description.
*/
//#define IFX_E_????	MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_????, 0x0000 )


#endif
