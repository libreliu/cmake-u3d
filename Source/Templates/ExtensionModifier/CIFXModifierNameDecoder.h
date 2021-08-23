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
	@file	CIFXModifierNameDecoder.h

			This module defines the CIFXModifierNameDecoder component.
*/


#ifndef CIFXModifierNameDecoder_H
#define CIFXModifierNameDecoder_H


//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXDecoderX.h"
#include "IFXSceneGraph.h"
#include "CIFXModifierBaseDecoder.h"

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
	This is the implementation of a component that is used to decode 
	CIFXModifierName. component

	It supports the following interfaces: IFXDecoderX.

	@note	This component's id is named TODO.

	@note	This component can be instantiated multiple times.
*/
class CIFXModifierNameDecoder : virtual public IFXExtensionDecoderX,
								protected CIFXModifierBaseDecoder
{
public:

	// IFXUnknown
	U32		  IFXAPI AddRef();
	U32		  IFXAPI Release();
	IFXRESULT IFXAPI QueryInterface( 
						IFXREFIID interfaceId, 
						void** ppInterface );

	// IFXDecoderX
	void IFXAPI InitializeX(
						IFXCoreServices &rCoreServices, 
						U32 loadId, 
						BOOL isExternal, 
						IFXSceneGraph::EIFXPalette ePalette);
	void IFXAPI PutNextBlockX( IFXDataBlockX &rDataBlockX );
	void IFXAPI TransferX( IFXRESULT &rWarningPartialTransfer );

	// IFXExtensionDecoderX
	void IFXAPI SetContBlockTypes( U32 *blockTypes, U32 typesCount );

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXModifierNameDecoder_Factory( 
											IFXREFIID interfaceId, 
											void** ppInterface );

private:
	// Member functions.
			CIFXModifierNameDecoder();
	virtual	~CIFXModifierNameDecoder();

	IFXRESULT RobustConstructor();
	void ProcessModifierNameBlockX( IFXDataBlockX &rDataBlockX );
};


//***************************************************************************
//	Global function prototypes
//***************************************************************************


//***************************************************************************
//	Global data
//***************************************************************************


#endif


