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
	@file	CIFXModifierName.h

			This module defines the CIFXModifierName component.
*/

#ifndef CIFXModifierName_H
#define CIFXModifierName_H

//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXModifierName.h"
#include "CIFXModifier.h"

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

	It supports the following interfaces: TODO.

	@note	This component's id is named TODO.

			TODO:  Pick one of the following descriptions...
	@note	This component can be instantiated multiple times.
*/
class CIFXModifierName : private CIFXModifier,
				  virtual public  IFXModifierName
{
public:
	// IFXUnknown
	U32		  IFXAPI AddRef();
	U32		  IFXAPI Release();
	IFXRESULT IFXAPI QueryInterface( 
								IFXREFIID interfaceId, 
								void** ppInterface );

	// IFXMarker
	void	  IFXAPI GetEncoderX ( IFXEncoderX*& rpEncoderX );
	IFXRESULT IFXAPI SetSceneGraph( IFXSceneGraph* pInSceneGraph );

	// IFXModifier
	IFXRESULT IFXAPI GetOutputs ( 
								IFXGUID**& rpOutOutputs,
								U32&       rOutNumberOfOutputs,
								U32*&      rpOutOutputDepAttrs );

	IFXRESULT IFXAPI GetDependencies (    
								IFXGUID*   pInOutputDID,
								IFXGUID**& rpOutInputDependencies,
								U32&       ruOutNumberInputDependencies,
								IFXGUID**& rpOutOutputDependcies,
								U32&       rOutNumberOfOutputDependcies,
								U32*&	   rpOutOutputDepAttrs );

	IFXRESULT IFXAPI GenerateOutput ( 
								U32    inOutputDataElementIndex,
								void*& rOutData, 
								BOOL&  rNeedRelease );

	IFXRESULT IFXAPI SetDataPacket ( 
								IFXModifierDataPacket* pInInputDataPacket,
								IFXModifierDataPacket* pInDataPacket );

	IFXRESULT IFXAPI Notify ( 
								IFXModifierMessage eInMessage,
								void*              pMessageContext );

	// Member functions:  IFXModifierName.
	// TODO:  INSERT CUSTOM METHODS.  DOCUMENT THEM IN ONLY ONE SPOT.
	// IT'S BEST TO DO THIS IN THE INTERFACE HEADER.  OTHERWISE IF IT'S
	// A PROTECTED/PRIVATE METHOD, DO IT IN THE BODY WITH THE
	// IMPLEMENTATION.
	IFXRESULT IFXAPI CustomMethod( 
						U32	 inputValue,
						U32* pOutputValue );

	// Factory function
	friend IFXRESULT IFXAPI_CALLTYPE CIFXModifierName_Factory( 
								IFXREFIID interfaceId, 
								void** ppInterface );

private:
			CIFXModifierName();
	virtual	~CIFXModifierName();

	IFXRESULT RobustConstructor();

	// IFXUnknown
	U32	m_refCount;	///< Number of outstanding references to the component.
};


//***************************************************************************
//	Global function prototypes
//***************************************************************************


//***************************************************************************
//	Global data
//***************************************************************************


#endif
