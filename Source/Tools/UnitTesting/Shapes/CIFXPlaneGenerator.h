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
//
//  CIFXPlaneGenerator.h
//
//  DESCRIPTION
//
//  NOTES
//
//***************************************************************************

#ifndef __CIFXPLANEGENERATOR_H__
#define __CIFXPLANEGENERATOR_H__

#include "IFXPlaneGenerator.h"
#include "IFXPrimitiveGenerator.h"
#include "CIFXModifier.h"
#include "IFXBoundSphereDataElement.h"
#include "IFXCoreCIDs.h"
#include "IFXArray.h"


class CIFXPlaneGenerator : private CIFXModifier,
                   virtual public   IFXPlaneGenerator
{
            CIFXPlaneGenerator();
  virtual  ~CIFXPlaneGenerator();
  friend
  IFXRESULT IFXAPI_CALLTYPE CIFXPlaneGenerator_Factory(IFXREFIID iid, void** ppv);

public:
  // IFXUnknown
  U32           IFXAPI	AddRef();
  U32           IFXAPI	Release();
  IFXRESULT     IFXAPI	QueryInterface(IFXREFIID riid, void **ppv);

  // IFXMarker
  IFXRESULT     IFXAPI	InitializeObject( );

  // IFXMarkerX
  void    IFXAPI	GetEncoderX(IFXEncoderX*& rpEncoderX);

  // IFXPrimitive interface...
  IFXRESULT IFXAPI	SetFacing(EIPrimitiveFacing eFacing);
  IFXRESULT IFXAPI	GetFacing(EIPrimitiveFacing* pFacing);

  EIPrimitiveOrientation
        IFXAPI	GetOrientation(void) {return m_plane.ePrimitiveOrientation;};
  void    IFXAPI	SetOrientation(EIPrimitiveOrientation eInOrientation)
        {m_plane.ePrimitiveOrientation = eInOrientation;};

  // IFXPlaneGenerator interface...
  F32     GetLength (void);
  IFXRESULT SetLength (F32 fInLength);

  F32     GetWidth (void);
  IFXRESULT SetWidth  (F32 fInWidth);

  U32     GetLengthVertices (void);
  IFXRESULT SetLengthVertices (U32 iInLengthVertices);

  U32     GetWidthVertices (void);
  IFXRESULT SetWidthVertices (U32 iInWidthVertices);

  const IFXVector4& GetBoundingSphere() { return m_pBoundSphereDataElement->Bound(); }
  IFXRESULT IFXAPI	SetBoundingSphere(const IFXVector4& vInBoundingSphere);

  const IFXMatrix4x4& IFXAPI GetTransform() { return m_transform[0]; }
  IFXRESULT IFXAPI	SetTransform(const IFXMatrix4x4& tInTransform);
  IFXRESULT IFXAPI	InvalidateTransform();


  // IFXModifier
  IFXRESULT IFXAPI	GetOutputs( IFXGUID**& rpOutOutputs,
								U32&       rOutNumberOfOutputs,
								U32*&    rpOutOutputDepAttrs );
  IFXRESULT IFXAPI	GetDependencies(    IFXGUID*   pInOutputDID,
										IFXGUID**& rppOutInputDependencies,
										U32&       rOutNumberInputDependencies,
										IFXGUID**& rppOutOutputDependencies,
										U32&       rOutNumberOfOutputDependencies,
										U32*&    rpOutOutputDepAttrs );

  IFXRESULT IFXAPI	GenerateOutput( U32    inOutputDataElementIndex,
									void*& rpOutData, BOOL& rNeedRelease );

  IFXRESULT IFXAPI	SetDataPacket( IFXModifierDataPacket* pInInputDataPacket,
									IFXModifierDataPacket* pInDataPacket );
  IFXRESULT IFXAPI	Notify( IFXModifierMessage eInMessage,
                    void*               pMessageContext );
  IFXRESULT IFXAPI	VerifyInvalidation( IFXGUID*   pInDependentDID,
										void*      pInDependentValue,
										IFXGUID*   pInDependencyDID,
										void*      pInDependencyValue );

private:
  // IFXUnknown
  U32 m_uRefCount;

  // IFXModifier
  static const IFXGUID* m_scpOutputDIDs[];
  static const IFXGUID* m_scpNeighborMeshDeps[];
  U32 m_uTransformDataElementIndex;
  U32 m_uMeshGroupDataElementIndex;
  U32 m_uNeighborMeshDataElementIndex;
  U32 m_uBoundSphereDataElementIndex;

  // IFXPlaneGenerator
  IFXPrimitiveGenerator*     m_pPrimitiveGenerator;
  CIFXPlanePrimitive         m_plane;
  IFXArray<IFXMatrix4x4>     m_transform;
  IFXBoundSphereDataElement* m_pBoundSphereDataElement;
};


#endif
