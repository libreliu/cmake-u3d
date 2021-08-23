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
@file	CIFXNURBSModifier.h

This module defines the CIFXNURBSModifier component.
*/

#ifndef CIFXNURBSModifier_H
#define CIFXNURBSModifier_H

//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXNURBSModifier.h"
#include "CIFXModifier.h"
#include "NURBSGenerator.h"
#include "IFXBoundSphereDataElement.h"
#include "IFXNeighborMesh.h"

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
This is the implementation of a component that is used to convert
NURBS to U3D mesh

It supports the following interfaces: IFXNURBSModifier.

@note	This component's id is named CID_IFXNURBSModifier.

@note	This component can be instantiated multiple times.
*/
class CIFXNURBSModifier : private CIFXModifier,
	virtual public  IFXNURBSModifier
{
public:
	// IFXUnknown
	U32		  IFXAPI AddRef();
	U32		  IFXAPI Release();
	IFXRESULT IFXAPI QueryInterface( 
		IFXREFIID interfaceId, 
		void** ppInterface );

	// IFXMarker
	IFXRESULT IFXAPI SetSceneGraph( IFXSceneGraph* pInSceneGraph );
	void	  IFXAPI GetEncoderX ( IFXEncoderX*& rpEncoderX );

	// IFXModifier
	IFXRESULT IFXAPI GetOutputs ( 
		IFXGUID**& rpOutOutputs,
		U32&       rOutNumberOfOutputs,
		U32*&      rpOutOutputDepAttrs );

	IFXRESULT IFXAPI GetDependencies (    
		IFXGUID*   pInOutputDID,
		IFXGUID**& rpOutInputDependencies,
		U32&       ruOutNumberInputDependencies,
		IFXGUID**& rpOutOutputDependcies,
		U32&       rOutNumberOfOutputDependcies,
		U32*&	   rpOutOutputDepAttrs );

	IFXRESULT IFXAPI GenerateOutput ( 
		U32    inOutputDataElementIndex,
		void*& rOutData, 
		BOOL&  rNeedRelease );

	IFXRESULT IFXAPI SetDataPacket ( 
		IFXModifierDataPacket* pInInputDataPacket,
		IFXModifierDataPacket* pInDataPacket );

	IFXRESULT IFXAPI Notify ( 
		IFXModifierMessage eInMessage,
		void*              pMessageContext );

	// IFXNURBSModifier
	virtual NURBSGenerator* IFXAPI GetGenerator();
	virtual void IFXAPI SetGenerator( NURBSGenerator* pGen );
	virtual void IFXAPI Tesselate();

	// Factory function
	friend IFXRESULT IFXAPI_CALLTYPE CIFXNURBSModifier_Factory( 
		IFXREFIID interfaceId, 
		void** ppInterface );

private:
	CIFXNURBSModifier();
	virtual	~CIFXNURBSModifier();

	IFXRESULT RobustConstructor();

	// IFXUnknown
	U32	m_refCount;	///< Number of outstanding references to the component.

	// IFXModifier
	U32 m_meshGroupDataElementIndex;
	U32 m_neighborMeshDataElementIndex;
	U32 m_boundSphereDataElementIndex;

	// IFXNURBSModifier
	NURBSGenerator* m_pGen; ///< NURBSGenerator
	IFXMeshGroup* m_pMeshGroup; ///<output meshgroup
	IFXBoundSphereDataElement* m_pBoundSphereDataElement;
	IFXNeighborMesh* m_pNeighborMesh;
};


//***************************************************************************
//	Global function prototypes
//***************************************************************************


//***************************************************************************
//	Global data
//***************************************************************************


#endif
