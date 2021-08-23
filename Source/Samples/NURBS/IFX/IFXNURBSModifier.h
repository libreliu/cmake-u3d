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
	@file	IFXNURBSModifier.h

			This header defines the IFXNURBSModifier interface and
			the associated interface identifier.

	@note	The IFXNURBSModifier interface is supported by the 
			CIFXNURBSModifier component. Its component id is named 
			CID_IFXNURBSModifier.
*/


#ifndef IFXNURBSModifier_H
#define IFXNURBSModifier_H


//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXModifier.h"
#include "IFXMeshGroup.h"
#include "NURBSGenerator.h"

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


/**
	This interface is used to define NURBS Modifier generated output.

	@note	The associated IID is named IID_IFXNURBSModifier.
*/
class IFXNURBSModifier : virtual public IFXModifier
{
	public:
		/**	
			This function returns NURBS generator
		*/
		virtual NURBSGenerator* IFXAPI GetGenerator() = 0;
		/**	
			This function sets NURBS generator

			@param	pGen NURBSGenerator
		*/
		virtual void IFXAPI SetGenerator( NURBSGenerator* pGen ) = 0;
		/**	
			This function converts NURBS to U3D mesh group

			@param	pMeshGroup mesh group
		*/
		virtual void IFXAPI Tesselate() = 0;
};

/**
	This IID identifies the IFXNURBSModifier interface.

	@note	The GUID string is {6F5A8CE0-7EA2-4402-B867-14141836EDB4}. 
*/
IFXDEFINE_GUID(IID_IFXNURBSModifier,
0x6f5a8ce0, 0x7ea2, 0x4402, 0xb8, 0x67, 0x14, 0x14, 0x18, 0x36, 0xed, 0xb4);

#endif
