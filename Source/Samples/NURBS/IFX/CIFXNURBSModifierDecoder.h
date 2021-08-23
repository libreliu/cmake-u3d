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
@file CIFXNURBSModifierDecoder.h

This module defines the CIFXNURBSModifierDecoder component.
*/


#ifndef CIFXNURBSModifierDecoder_H
#define CIFXNURBSModifierDecoder_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXDecoderX.h"
#include "IFXSceneGraph.h"
#include "IFXModifierBaseDecoder.h"
#include "IFXBitStreamX.h"
#include "IFXExtensionDecoderX.h"

//***************************************************************************
//  Defines
//***************************************************************************


//***************************************************************************
//  Constants
//***************************************************************************


//***************************************************************************
//  Enumerations
//***************************************************************************


//***************************************************************************
//  Classes, structures and types
//***************************************************************************


//---------------------------------------------------------------------------
/**
This is the implementation of a component that is used to decode
NURBS.

It supports the following interfaces: IFXDecoderX.

@note This component's id is named CID_IFXNURBSModifierDecoder.

@note This component can be instantiated multiple times.
*/
class CIFXNURBSModifierDecoder : virtual public IFXExtensionDecoderX,
	public IFXModifierBaseDecoder
{
public:

	// IFXUnknown
	virtual U32     IFXAPI AddRef();
	virtual U32     IFXAPI Release();
	virtual IFXRESULT IFXAPI QueryInterface(
		IFXREFIID interfaceId,
		void** ppInterface );

	// IFXExtensionDecoderX
	virtual void    IFXAPI InitializeX( const IFXLoadConfig &lc );
	virtual void    IFXAPI PutNextBlockX( IFXDataBlockX &rDataBlockX );
	virtual void    IFXAPI TransferX( IFXRESULT &rWarningPartialTransfer );
	virtual void	IFXAPI SetContBlockTypes( U32 *blockTypes, U32 typesCount );

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXNURBSModifierDecoder_Factory(
		IFXREFIID interfaceId,
		void** ppInterface );

private:
	// Member functions.
	CIFXNURBSModifierDecoder();
	virtual ~CIFXNURBSModifierDecoder();

	void ProcessNURBSModifierBlockX( IFXDataBlockX &rDataBlockX, U32 blockNum );

	IFXBitStreamX* m_pBitStreamX;
	U32* m_pContBlocks;
	U32 m_contBlockCount;
};


//***************************************************************************
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************


#endif
