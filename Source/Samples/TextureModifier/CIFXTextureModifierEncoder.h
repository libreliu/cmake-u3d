//***************************************************************************
//
//  Copyright (c) 2002 - 2006 Intel Corporation
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
	@file	CIFXTextureModifierEncoder.h

			Declaration of the CIFXTextureModifierEncoder.
			The CIFXTextureModifierEncoder contains Texture modifier
			encoding functionality that is used by the write manager.
*/

#ifndef CIFXTextureModifierENCODER_H__
#define CIFXTextureModifierENCODER_H__

// no ordering requirements for include files
#include "IFXBitStreamX.h"
#include "IFXCoreServices.h"
#include "IFXDataBlockQueueX.h"
#include "IFXExtensionEncoderX.h"
#include "IFXString.h"
#include "IFXModifier.h"
#include "IFXAutoRelease.h"

class  CIFXTextureModifierEncoder : virtual public IFXExtensionEncoderX
{
public:
	
	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXTextureModifierEncoder_Factory( IFXREFIID interfaceId, void** ppInterface );

	// IFXUnknown
	U32 IFXAPI 			AddRef ( void );
	U32 IFXAPI 			Release ( void );
	IFXRESULT IFXAPI 	QueryInterface ( IFXREFIID interfaceId, void** ppInterface );

	// IFXEncoderX
	void	IFXAPI		EncodeX( IFXString& rName, IFXDataBlockQueueX& rDataBlockQueue, F64 units = 1.0f );
	void	IFXAPI		InitializeX( IFXCoreServices& rCoreServices );
	void	IFXAPI		SetObjectX( IFXUnknown& rObject );

	void IFXAPI GetExtensionDataX( IFXExtensionData &rExtData );
	void IFXAPI SetBlockTypesX( U32* pBlockTypes, const U32 blockTypeCount );

private:
	// methods
	CIFXTextureModifierEncoder();
	virtual ~CIFXTextureModifierEncoder();

	// members
	BOOL				m_bInitialized;
	IFXBitStreamX*		m_pBitStream;
	IFXCoreServices*	m_pCoreServices;
	IFXUnknown*			m_pObject;
	U32					m_uRefCount;
	IFXString*			m_pName;
	IFXDECLAREMEMBER(IFXModifier,m_pModifier);
	U32					m_blockType;

	IFXString			m_urls[2]; // reserve it for providing URLs info to write manager
};

#endif
