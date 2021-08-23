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
	@file	CIFXNewBaseMeshEncoder.h

			Declaration of the CIFXNewBaseMeshEncoder.
*/

#ifndef CIFXNEWBASEMESHENCODER_H
#define CIFXNEWBASEMESHENCODER_H

// no ordering requirements for include files
#include "IFXCoreServices.h"
#include "IFXDataBlockQueueX.h"
#include "IFXExtensionEncoderX.h"
#include "IFXModifier.h"
#include "IFXAutoRelease.h"
#include "IFXAuthorCLODResource.h"
#include "IFXNewBaseMesh.h"

class  CIFXNewBaseMeshEncoder : virtual public IFXExtensionEncoderX
{
public:
	
	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXNewBaseMeshEncoder_Factory
		( IFXREFIID interfaceId, void** ppInterface );

	// IFXUnknown
	U32 IFXAPI 			AddRef ( void );
	U32 IFXAPI 			Release ( void );
	IFXRESULT IFXAPI 	QueryInterface ( IFXREFIID interfaceId, 
		void** ppInterface );

	// IFXEncoderX
	void	IFXAPI		EncodeX( IFXString& rName, 
		IFXDataBlockQueueX& rDataBlockQueue, F64 units = 1.0f );
	void	IFXAPI		InitializeX( IFXCoreServices& rCoreServices );
	void	IFXAPI		SetObjectX( IFXUnknown& rObject );

	void IFXAPI GetExtensionDataX( IFXExtensionData &rExtData );
	void IFXAPI SetBlockTypesX( U32* pBlockTypes, const U32 blockTypeCount );

private:
	CIFXNewBaseMeshEncoder();
	virtual ~CIFXNewBaseMeshEncoder();

	/// Make the static geometry block
	void MakeAuthorCLODStaticGeometryBlockX(IFXString &rName, 
		IFXDataBlockQueueX &rDataBlockQueue, IFXMeshMap &rMeshMap);

	U32 m_uRefCount; // Reference count for IFXUnknown
	U32 m_uPriorityIncrement;
	U32 m_uPriorityCurrent;

	F64 m_unitScale;

	// members
	BOOL				m_bInitialized;
	IFXUnknown*			m_pObject;
	U3dString			m_pName;
	U32					m_blockType;
	IFXCoreServices*	m_pCoreServices;

	IFXString			m_urls[2];

	IFXDECLAREMEMBER(IFXModifier,m_pModifier);
	IFXDECLAREMEMBER(IFXAuthorCLODResource,m_pAuthorCLODResource);
	IFXDECLAREMEMBER(IFXAuthorCLODMesh,m_pOriginalAuthorCLODMesh);
};

#endif
