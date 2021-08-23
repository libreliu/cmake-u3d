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
//
//	CIFXTextureDecoder.h
//
//		Declaration of the CIFXTextureDecoder.
//		The CIFXTextureDecoder is used by the CIFXLoadManager to load
//		texture resources into the scene graph. CIFXTextureDecoder
//		exposes an IFXDecoderX interface to the CIFXLoadManager for this
//		purpose.
//
//*****************************************************************************
#ifndef CIFXTextureDecoder_H
#define CIFXTextureDecoder_H

#include "IFXDataBlockQueueX.h"
#include "IFXDecoderX.h"
#include "IFXAutoRelease.h"
#include "IFXSceneGraph.h"
#include "IFXCoreCIDs.h"
#include "IFXSceneGraph.h"
#include "IFXPalette.h"

class CIFXTextureDecoder : virtual public IFXDecoderX
{
public:
	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXTextureDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );

	// IFXUnknown
	virtual U32 IFXAPI  AddRef();
	virtual U32 IFXAPI  Release();
	virtual IFXRESULT IFXAPI  QueryInterface( IFXREFIID interfaceId, void** ppInterface );

	// IFXDecoderX
	virtual void IFXAPI  InitializeX( const IFXLoadConfig &lc );
	virtual void IFXAPI  PutNextBlockX(IFXDataBlockX &rDataBlockX) ;
	virtual void IFXAPI  TransferX(IFXRESULT &rWarningPartialTransfer) ;

private:
	// Member functions.
    CIFXTextureDecoder();
	virtual ~CIFXTextureDecoder();

	// Member data.
	U32 m_uRefCount;
	U32 m_uLoadId;
	BOOL m_bExternal;
	IFXDECLAREMEMBER(IFXDecoderX,m_pDecoderX);
	IFXDECLAREMEMBER(IFXCoreServices,m_pCoreServices);
};

#endif
