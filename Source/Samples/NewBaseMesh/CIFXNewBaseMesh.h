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
	@file	CIFXNewBaseMesh.h

			This module defines the CIFXNewBaseMesh component.
*/


#ifndef CIFXNEWBASEMESH_H
#define CIFXNEWBASEMESH_H


//***************************************************************************
//	Includes
//***************************************************************************

#include "CIFXModifier.h"
#include "IFXNewBaseMesh.h"
#include <malloc.h>

#include "IFXModifierDataPacket.h"

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

	It supports the following interface: IFXNewBaseMesh.
	It inherits the following functionality: CIFXModifier.

	@note	This is base mesh compression format modifier.

	@note	This component's id is named CID_IFXNewBaseMesh.

	@note	This component can be instantiated multiple times
*/
class CIFXNewBaseMesh : private CIFXModifier,
							virtual public IFXNewBaseMesh
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
	friend IFXRESULT IFXAPI_CALLTYPE CIFXNewBaseMesh_Factory( 
											IFXREFIID interfaceId, 
											void** ppInterface );

	// IFXNewBaseMesh functions
	void IFXAPI GetCLODResource( IFXAuthorCLODResource** ppCLOD )
	{
		*ppCLOD = m_pCLOD;
		IFXADDREF( m_pCLOD );
	}

	void IFXAPI SetCLODResource( IFXAuthorCLODResource* pCLOD )
	{
		m_pCLOD = pCLOD;
		IFXADDREF( m_pCLOD );
	}


private:
	// Member functions.
			CIFXNewBaseMesh();
	virtual	~CIFXNewBaseMesh();

	// Member data.
	U32 m_refCount;

	// IFXModifier
	static const IFXDID* m_scpOutputDIDs[];
	static const IFXDID* m_scpInputDIDs[];
	U32 m_uMeshGroupDataElementIndex;
	U32 m_transformDataElementIndex;
	IFXAuthorCLODResource* m_pCLOD;
};

//***************************************************************************
//	Global function prototypes
//***************************************************************************


//***************************************************************************
//	Global data
//***************************************************************************


#endif
