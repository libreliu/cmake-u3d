//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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
	@file	CIFXTextureModifierDecoder.h

			Declaration of the CIFXTextureModifierDecoder.
*/

#ifndef CIFXTextureModifierDecoder_H
#define CIFXTextureModifierDecoder_H

#include "IFXExtensionDecoderX.h"
#include "IFXSceneGraph.h"
#include "IFXCoreCIDs.h"
#include "IFXModifierBaseDecoder.h"

class CIFXTextureModifierDecoder : virtual public IFXExtensionDecoderX, public IFXModifierBaseDecoder
{
public:
	// IFXUnknown
	U32 IFXAPI  AddRef();
	U32 IFXAPI  Release();
	IFXRESULT IFXAPI  QueryInterface( IFXREFIID interfaceId, void** ppInterface );

	// IFXDecoderX
	void IFXAPI  InitializeX( const IFXLoadConfig &pLC);
	void IFXAPI  PutNextBlockX(IFXDataBlockX &rDataBlockX) ;
	void IFXAPI  TransferX(IFXRESULT &rWarningPartialTransfer) ;

	// IFXExtensionDecoderX
	void IFXAPI  SetContBlockTypes( U32 *blockTypes, U32 typesCount );

private:
	// Member functions.
	CIFXTextureModifierDecoder();
	virtual ~CIFXTextureModifierDecoder();

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXTextureModifierDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );

	void ProcessTextureModifierBlockX( IFXDataBlockX &rDataBlockX );

	U32 m_blockType;
};

#endif
