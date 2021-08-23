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
//  CIFXBoxGenerator.h
//
//  DESCRIPTION
//
//  NOTES
//
//***************************************************************************

#ifndef __CIFXBOXGENERATOR_H__
#define __CIFXBOXGENERATOR_H__

#include "IFXBoxGenerator.h"
#include "IFXPrimitiveGenerator.h"
#include "CIFXModifier.h"
#include "IFXBoundSphereDataElement.h"
#include "IFXCoreCIDs.h"
#include "IFXArray.h"

class CIFXBoxGenerator : virtual public CIFXModifier,
                         virtual public  IFXBoxGenerator
{
            CIFXBoxGenerator();
  virtual  ~CIFXBoxGenerator();
  friend
  IFXRESULT IFXAPI_CALLTYPE CIFXBoxGenerator_Factory(IFXREFIID iid, void** ppv);

public:
  // IFXUnknown
  U32       IFXAPI	AddRef();
  U32       IFXAPI	Release();
  IFXRESULT IFXAPI	QueryInterface(IFXREFIID riid, void **ppv);

  // IFXMarker
  IFXRESULT IFXAPI	InitializeObject( );

  // IFXModifier
  IFXRESULT IFXAPI	GetOutputs( IFXGUID**& rpOutOutputs,
								U32&     rOutNumberOfOutputs,
								U32*&    rpOutOutputDepAttrs );
  IFXRESULT IFXAPI	GetDependencies(IFXGUID*   pInOutputDID,
									IFXGUID**& rppOutInputDependencies,
									U32&     rOutNumberInputDependencies,
									IFXGUID**& rppOutOutputDependencies,
									U32&     rOutNumberOfOutputDependencies,
									U32*&    rpOutOutputDepAttrs );

  IFXRESULT IFXAPI	GenerateOutput( U32  inOutputDataElementIndex,
									void*& rpOutData, BOOL& rNeedRelease );

  IFXRESULT IFXAPI	SetDataPacket( IFXModifierDataPacket* pInInputDataPacket,
									IFXModifierDataPacket* pInDataPacket );
  IFXRESULT IFXAPI	Notify( IFXModifierMessage eInMessage,
							void*         pMessageContext );

  // IFXMarkerX
  void    IFXAPI	GetEncoderX(IFXEncoderX*& rpEncoderX);

  // IFXPrimitive
  IFXRESULT IFXAPI	SetFacing(EIPrimitiveFacing eFacing);
  IFXRESULT IFXAPI	GetFacing(EIPrimitiveFacing* pFacing);

  EIPrimitiveOrientation
        IFXAPI	GetOrientation(void) {return m_box.ePrimitiveOrientation;};
  void  IFXAPI	  SetOrientation(EIPrimitiveOrientation eInOrientation)
        {m_box.ePrimitiveOrientation = eInOrientation;};

  const IFXVector4& GetBoundingSphere() { return m_pBoundSphereDataElement->Bound(); }
  IFXRESULT IFXAPI	SetBoundingSphere(const IFXVector4& vInBoundingSphere);

  const IFXMatrix4x4& IFXAPI GetTransform() { return m_transform[0]; }
  IFXRESULT IFXAPI	SetTransform(const IFXMatrix4x4& tInTransform);
  IFXRESULT IFXAPI	InvalidateTransform();

  // IFXBoxGenerator
   BOOL   GetTop (void);
   IFXRESULT  SetTop (BOOL bInTop);

   BOOL   GetBottom (void);
   IFXRESULT  SetBottom (BOOL bInBottom);

   BOOL   GetFront (void);
   IFXRESULT  SetFront (BOOL bInFront);

   BOOL   GetBack (void);
   IFXRESULT  SetBack (BOOL bInBack);

   BOOL   GetLeft (void);
   IFXRESULT  SetLeft (BOOL bInLeft);

   BOOL   GetRight (void);
   IFXRESULT  SetRight (BOOL bInRight);

   F32    GetHeight (void);
   IFXRESULT  SetHeight (F32 fInHeight);

   F32    GetWidth (void);
   IFXRESULT  SetWidth (F32 fInWidth);

   F32    GetLength (void);
   IFXRESULT  SetLength (F32 fInLength);

   U32    GetNumHeightVertices (void);
   IFXRESULT  SetNumHeightVertices (U32 iInNumHeightVertices);

   U32    GetNumWidthVertices (void);
   IFXRESULT  SetNumWidthVertices (U32 iInNumWidthVertices);

   U32    GetNumLengthVertices (void);
   IFXRESULT  SetNumLengthVertices (U32 iInNumLengthVertices);

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

  // IFXBoxGenerator
  IFXPrimitiveGenerator*     m_pPrimitiveGenerator;
  CIFXBoxPrimitive           m_box;
  IFXArray<IFXMatrix4x4>     m_transform;
  IFXBoundSphereDataElement* m_pBoundSphereDataElement;
  IFXMeshGroupPtr            m_DegenerateMeshGroup;
};

#endif
