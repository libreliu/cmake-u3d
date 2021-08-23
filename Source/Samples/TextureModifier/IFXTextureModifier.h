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
	@file	IFXTextureModifier.h

			This header defines the IFXTextureModifier interface and
			the associated interface identifier.

	@note	The IFXTextureModifier interface is supported by the 
			CIFXTextureModifier component.
*/


#ifndef IFXTextureModifier_H
#define IFXTextureModifier_H


//***************************************************************************
//	Includes
//***************************************************************************


#include "IFXModifier.h"


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
	This interface is used to modify texture resource.

	@note	The associated IID is named IID_IFXTextureModifier.
*/
class IFXTextureModifier : virtual public IFXModifier
{
public:
};

/**
	This IID identifies the IFXTextureModifier interface.

	@note	The GUID string is {EB936B61-E8A8-40f0-B78B-29045898A2C1}. 
*/
IFXDEFINE_GUID(IID_IFXTextureModifier,
0xeb936b61, 0xe8a8, 0x40f0, 0xb7, 0x8b, 0x29, 0x4, 0x58, 0x98, 0xa2, 0xc1);


#endif
