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
	@file	IFXNewBaseMesh.h

			This header defines the IFXNewBaseMesh interface and
			the associated interface identifier.

	@note	The IFXNewBaseMesh interface is supported by the 
			CIFXNewBaseMesh component.
*/


#ifndef IFXNEWBASEMESH_H
#define IFXNEWBASEMESH_H


//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXAuthorCLODResource.h"


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

	@note	The associated IID is named IID_IFXNewBaseMesh.
*/
class IFXNewBaseMesh : virtual public IFXModifier
{
public:
	virtual void IFXAPI GetCLODResource( IFXAuthorCLODResource** ppCLOD ) = 0;
	virtual void IFXAPI SetCLODResource( IFXAuthorCLODResource* pCLOD ) = 0;

};

/**
	This IID identifies the IFXNewBaseMesh interface.

	@note	The GUID string is {4a3644eb-ce57-4e49-863b-dce4e55013f9}. 
*/
// 4a3644eb-ce57-4e49-863b-dce4e55013f9 
IFXDEFINE_GUID(IID_IFXNewBaseMesh,
0x4a3644eb, 0xce57, 0x4e49, 0x86, 0x3b, 0xdc, 0xe4, 0xe5, 0x50, 0x13, 0xf9);


#endif
