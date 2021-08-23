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
//  CIFXPrimitiveGen.h
//
//  DESCRIPTION
//    Interface header file for the primitives generator class
//
//  NOTES
//      None.
//
//***************************************************************************
#ifndef __CIFXPRIMITIVEGENERATOR_INTERFACE_H__
#define __CIFXPRIMITIVEGENERATOR_INTERFACE_H__

#include "IFXPrimitiveGenerator.h"



class CIFXPrimitiveGenerator : virtual public IFXPrimitiveGenerator
{
public:
  // IFXUnknown methods...
  U32 IFXAPI	AddRef(void);
  U32 IFXAPI	Release(void);
  IFXRESULT IFXAPI	QueryInterface(IFXREFIID interfaceId, void** ppInterface);

  // Factory function.
  friend IFXRESULT IFXAPI_CALLTYPE CIFXPrimitiveGenerator_Factory( IFXREFIID interfaceId, void** ppInterface );

  // IFXPrimitiveGenerator methods...
  IFXRESULT GeneratePrimitiveFromArc(CIFXPrimitiveShape* pPrimitive);
  IFXRESULT GeneratePrimitive(CIFXPrimitiveShape* pPrimitive);

private:
  // make constructor/destructor private to enforce use of
  // factory and Release methods....
  CIFXPrimitiveGenerator();
  virtual ~CIFXPrimitiveGenerator();

  // store the member variables here...
  U32   m_uRefCount;
  IFXRESULT GeneratePrimitiveArc(CIFXPrimitiveShape* pPrimitive);
  IFXRESULT GenerateMeshGroup(CIFXPrimitiveShape* pPrimitiveShape);
  IFXRESULT MeshBuilder(CIFXPrimitiveShape* pPrimitive);

  IFXRESULT MeshBuilder(U32 uNumSourceArcVertices, U32 uNumSurfaceSlices,
            IFXVector3* pArcVertices, IFXVector3* pArcNormals,
            IFXMesh** pDestMesh, CIFXPrimitiveShape* pPrimitiveShape);

  IFXRESULT CapBuilder(F32 fTheta, bool bStart, IFXMesh** pDestMesh,
            CIFXPrimitiveShape* pPrimitiveShape);

  void SetAMesh( CIFXPrimitiveShape *pPrimitiveShape, U32 uMeshIndex, U32 *puIndex );
};


#endif // #define __CIFXPRIMITIVEGENERATOR_INTERFACE_H__
