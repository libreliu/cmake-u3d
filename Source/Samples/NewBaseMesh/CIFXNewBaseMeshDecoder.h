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
	@file	CIFXNewBaseMeshDecoder.h

			Declaration of the CIFXNewBaseMeshDecoder.
*/

#ifndef CIFXNEWBASEMESHDECODER_H
#define CIFXNEWBASEMESHDECODER_H

#include "IFXCoreServices.h"
#include "IFXDataBlockQueueX.h"
#include "IFXModifier.h"
#include "IFXAutoRelease.h"
#include "IFXAuthorCLODResource.h"

#include "IFXExtensionDecoderX.h"
#include "IFXCoreCIDs.h"
#include "IFXModifierBaseDecoder.h"

class CIFXNewBaseMeshDecoder : virtual public IFXExtensionDecoderX, public IFXModifierBaseDecoder
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

	// IFXModifierBaseDecoder
	void IFXAPI  CreateObjectX(IFXDataBlockX &rDataBlockX, IFXREFCID cid);
private:
	// Member functions.
	CIFXNewBaseMeshDecoder();
	virtual ~CIFXNewBaseMeshDecoder();

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXNewBaseMeshDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );

	void ProcessNewBaseMeshBlockX( IFXDataBlockX &rDataBlockX );

	F64 m_unitScale;

	U32 m_blockType;
	IFXDECLAREMEMBER(IFXAuthorCLODResource, m_pAuthorCLODResource);
	IFXDECLAREMEMBER(IFXAuthorCLODMesh,m_pReconstructedAuthorCLODMesh);
};

#endif
