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
	@file	CIFXModifierNameEncoder.h

			This module defines the CIFXModifierNameEncoder component.
*/


#ifndef CIFXModifierNameEncoder_H
#define CIFXModifierNameEncoder_H


//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXEncoderX.h"
#include "IFXExtensionEncoderX.h"
#include "IFXBitStreamX.h"
#include "IFXCoreServices.h"
#include "IFXDataBlockQueueX.h"
#include "IFXString.h"
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


//---------------------------------------------------------------------------
/**
	This is the implementation of a component that is used to encode
	CIFXModifierName component.

	It supports the following interfaces: IFXExtensionEncoderX.

	@note	This component's id is named CID_IFXModifierNameEncoder.

	@note	This component can be instantiated multiple times.
*/
class CIFXModifierNameEncoder : virtual public IFXExtensionEncoderX
{
public:
	// IFXUnknown
	U32		  IFXAPI AddRef();
	U32		  IFXAPI Release();
	IFXRESULT IFXAPI QueryInterface( IFXREFIID interfaceId, void** ppInterface );

	// IFXEncoderX
	void	  IFXAPI EncodeX( 
						IFXString& rName, 
						IFXDataBlockQueueX& rDataBlockQueue );
	void	  IFXAPI InitializeX( IFXCoreServices& rCoreServices );
	void	  IFXAPI SetObjectX( IFXUnknown& rObject );

	// IFXExtensionEncoderX
	void	  IFXAPI GetExtensionDataX( IFXExtensionData &rExtData );
	void	  IFXAPI SetBlockTypesX( U32* pBlockTypes, const U32 blockTypeCount );


	// Factory function
	friend IFXRESULT IFXAPI_CALLTYPE CIFXModifierNameEncoder_Factory( 
											IFXREFIID interfaceId, 
											void** ppInterface );

private:
			CIFXModifierNameEncoder();
	virtual	~CIFXModifierNameEncoder();

	IFXRESULT RobustConstructor();

	// IFXUnknown
	U32			m_refCount;	///< Number of outstanding references to the component.

	U32			m_blockType;
	
	// IFXEncoder
	IFXUnknown*	m_pObject;
};


//***************************************************************************
//	Global function prototypes
//***************************************************************************


//***************************************************************************
//	Global data
//***************************************************************************


#endif
