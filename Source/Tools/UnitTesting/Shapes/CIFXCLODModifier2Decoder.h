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
	@file	CIFXCLODModifier2Decoder.h

			Declaration of the CIFXCLODModifier2Decoder.
*/

#ifndef CIFXCLODModifier2Decoder_H
#define CIFXCLODModifier2Decoder_H

#include "IFXExtensionDecoderX.h"
#include "IFXSceneGraph.h"
#include "IFXCoreCIDs.h"
#include "IFXShapesCIDs.h"
#include "IFXModifierBaseDecoder.h"
#include "IFXBitStreamX.h"

class CIFXCLODModifier2Decoder : virtual public IFXExtensionDecoderX, public IFXModifierBaseDecoder
{
public:
	// IFXUnknown
	U32 IFXAPI  AddRef();
	U32 IFXAPI  Release();
	IFXRESULT IFXAPI  QueryInterface( IFXREFIID interfaceId, void** ppInterface );

	// IFXExtensionDecoderX
	void IFXAPI  InitializeX( const IFXLoadConfig &lc );
	void IFXAPI  PutNextBlockX(IFXDataBlockX &rDataBlockX) ;
	void IFXAPI  TransferX(IFXRESULT &rWarningPartialTransfer) ;

	void IFXAPI  SetContBlockTypes( U32 *blockTypes, U32 typesCount );

private:
	// Member functions.
	CIFXCLODModifier2Decoder();
	virtual ~CIFXCLODModifier2Decoder();

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXCLODModifier2Decoder_Factory( IFXREFIID interfaceId, void** ppInterface );

	void ProcessCLODModifierBlockX( IFXDataBlockX &rDataBlockX );
	void ProcessCLODModifierContBlockX( IFXDataBlockX &rDataBlockX );
	void ProcessCLODModifierContBlock2X( IFXDataBlockX &rDataBlockX );
	void ProcessCLODModifierContBlock3X( IFXDataBlockX &rDataBlockX );

	IFXDECLAREMEMBER( IFXBitStreamX, m_pBitStreamX );
	U32* m_pContBlocks;
	U32 m_contBlockCount;
};

#endif
