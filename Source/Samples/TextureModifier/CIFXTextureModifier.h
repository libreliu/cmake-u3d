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
	@file	CIFXTextureModifier.h

			This module defines the CIFXTextureModifier component.
*/


#ifndef CIFXTextureModifier_H
#define CIFXTextureModifier_H


//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXTextureModifier.h"
#include "CIFXModifier.h"

#include "IFXModifierDataPacket.h"
#include "IFXTextureObject.h"

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
	This is the implementation of a component that is used to modify texture
	resource.

	It supports the following interface: IFXTextureModifier.
	It inherits the following functionality: CIFXModifier.

	@note	This is the example of an extension modifier.

	@note	This component's id is named CID_IFXTextureModifier.

	@note	This component can be instantiated multiple times.
*/
class CIFXTextureModifier : private CIFXModifier,
							virtual public IFXTextureModifier
{
public:
	// IFXUnknown
	U32		  IFXAPI AddRef(void);
	U32		  IFXAPI Release(void);
	IFXRESULT IFXAPI QueryInterface( IFXREFIID riid, void **ppv);

	// IFXMarkerX interface...
	void IFXAPI	GetEncoderX(IFXEncoderX*& rpEncoderX);

	// IFXModifier
	IFXRESULT IFXAPI GetOutputs( 
							IFXGUID**& rpOutOutputs,
							U32&	   rOutNumberOfOutputs,
							U32*&	   rpOutOutputDepAttrs );

	IFXRESULT IFXAPI GetDependencies( 
							IFXGUID*   pInOutputDID,
							IFXGUID**& rppOutInputDependencies,
							U32&       rOutNumberInputDependencies,
							IFXGUID**& rppOutOutputDependencies,
							U32&       rOutNumberOfOutputDependencies,
							U32*&      rpOutOutputDepAttrs );

	IFXRESULT IFXAPI GenerateOutput( 
							U32 inOutputDataElementIndex, 
							void*& rpOutData, 
							BOOL& rNeedRelease );

	IFXRESULT IFXAPI SetDataPacket( 
							IFXModifierDataPacket* pInInputDataPacket, 
							IFXModifierDataPacket* pInDataPacket );

	IFXRESULT IFXAPI Notify( 
							IFXModifierMessage eInMessage, 
							void* pMessageContext );

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXTextureModifier_Factory( 
											IFXREFIID interfaceId, 
											void** ppInterface );

private:
	// Member functions.
			CIFXTextureModifier();
	virtual	~CIFXTextureModifier();

	// Member data.
	U32 m_refCount;
	STextureOutputInfo m_ImageInfo;

	// IFXModifier
	static const IFXDID* m_scpOutputDIDs[];
	static const IFXDID* m_scpInputDIDs[];
	U32 m_textureDataElementIndex;
};

//***************************************************************************
//	Global function prototypes
//***************************************************************************


//***************************************************************************
//	Global data
//***************************************************************************


#endif
