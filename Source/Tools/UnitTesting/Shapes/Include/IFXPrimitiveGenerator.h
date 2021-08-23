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
	@file	IFXPrimitiveGen.h
		    Interface header file for the primitives generator
*/

#ifndef __IFXPRIMITIVEGEN_INTERFACE_H__
#define __IFXPRIMITIVEGEN_INTERFACE_H__


#include "IFXUnknown.h"
#include "IFXVector3.h"
#include "IFXMesh.h"
#include "IFXCoreServices.h"
#include "IFXPrimitive.h"
#include "IFXNeighborMesh.h"

const float PRIMITIVE_TEXTURECOORDINATE_START = 0.0001f;
const float PRIMITIVE_TEXTURECOORDINATE_END = 0.9999f;

//---------------------------------------------------------------------------
//  IFXPrimitiveShapes (enum)
//
//  All the enumerated Shapes that the primitive generator can make
//---------------------------------------------------------------------------
enum IFXPrimitiveShapes
{
  IFX_BOX,
  IFX_CONE,
  IFX_CYLINDER,
  IFX_CUSTOM,
  IFX_SPHERE,
  IFX_PLANE
};

// Definition of PI
const float IFX_PI=3.1415926535897932384626433832795f;

//---------------------------------------------------------------------------
//  CIFXPrimitiveShape
//
//  All the generic parameters that can be specified when creating a primitive
//  shape.  These are applicable to all primitives
//---------------------------------------------------------------------------
class CIFXPrimitiveShape 
{
public:
  // Functions
  CIFXPrimitiveShape();
  virtual ~CIFXPrimitiveShape();
  IFXRESULT ClearImmediateData();
  virtual IFXRESULT GenerateArc(){return IFX_E_UNSUPPORTED;}
  virtual IFXRESULT GenerateMesh() {return IFX_E_UNSUPPORTED;}

  // generic data members
  U32       uPrimitiveType;
  U8        uNormalOrientation;   // facing: inward, outward, both or neither.

  // arc parameters
  BOOL    bTopCap;
  BOOL    bBottomCap;
  U32     uNumArcSlices;
  IFXVector3* pArcCoords;
  IFXVector3* pArcNormals;

  U32     uNumTopCapVerts;
  U32     uNumBottomCapVerts;
  IFXVector3* pArcTopCapVerts;
  IFXVector3* pArcTopCapNormals;
  IFXVector3* pArcBottomCapVerts;
  IFXVector3* pArcBottomCapNormals;


  // solid parameters
  BOOL  bSolidCapEnd;     // generate an ending cap
  BOOL  bSolidCapStart;     // generate a starting cap
  BOOL  bTextureCoordinates;  // generate texture coordinates
  U32   uSolidNumSurfaceSlices; // number of slices to generate
  F32   fSolidAngleStart;   // in radians
  F32   fSolidAngleEnd;     // in radians

#define m_pPrimitiveMesh    m_pAllMeshes[0]
//  IFXMesh*  m_pPrimitiveMesh;   // output for the primitive Generator
#define pFrontMesh        m_pAllMeshes[1]
//  IFXMesh*  pFrontMesh;
#define pSolidCapEndMesh    m_pAllMeshes[2]
//  IFXMesh*  pSolidCapEndMesh;
#define pBackMesh       m_pAllMeshes[3]
//  IFXMesh*  pBackMesh;
#define pSolidCapStartMesh    m_pAllMeshes[4]
//  IFXMesh*  pSolidCapStartMesh;
#define pTopCapMesh       m_pAllMeshes[5]
//  IFXMesh*  pTopCapMesh;      // output for top cap mesh
#define pBottomCapMesh      m_pAllMeshes[6]
//  IFXMesh*  pBottomCapMesh;     // output for bottom cap mesh
#define pPrimitiveMeshInverse m_pAllMeshes[7]
//  IFXMesh*  pPrimitiveMeshInverse;  // output for the primitive Generator
#define pFrontMeshInverse   m_pAllMeshes[8]
//  IFXMesh*  pFrontMeshInverse;
#define pSolidCapEndMeshInverse m_pAllMeshes[9]
//  IFXMesh*  pSolidCapEndMeshInverse;
#define pBackMeshInverse    m_pAllMeshes[10]
//  IFXMesh*  pBackMeshInverse;
#define pSolidCapStartMeshInverse m_pAllMeshes[11]
//  IFXMesh*  pSolidCapStartMeshInverse;
#define pTopCapMeshInverse    m_pAllMeshes[12]
//  IFXMesh*  pTopCapMeshInverse;   // output for top cap mesh
#define pBottomCapMeshInverse m_pAllMeshes[13]
//  IFXMesh*  pBottomCapMeshInverse;  // output for bottom cap mesh

  IFXMesh*  m_pAllMeshes[14];
  U32     m_uLastMeshId[14];

  IFXMeshGroup*  m_pMeshGroup;    // mesh group output
  IFXNeighborMesh* m_pNeighborMesh;

  IFXPrimitive::EIPrimitiveOrientation ePrimitiveOrientation;
};

//---------------------------------------------------------------------------
//  CIFXSpherePrimitive
//
//  All these parameters are specific to creating a sphere
//---------------------------------------------------------------------------
class CIFXSpherePrimitive: public CIFXPrimitiveShape
{
public:
  CIFXSpherePrimitive();
  IFXRESULT GenerateArc();
  F32   fRadius;
  F32   fArcAngleStart;
  F32   fArcAngleEnd;
};

//---------------------------------------------------------------------------
//  CIFXCylinderPrimitive
//
//  All these parameters are specific to creating a cylinder
//---------------------------------------------------------------------------
class CIFXCylinderPrimitive: public CIFXPrimitiveShape
{
public:
  CIFXCylinderPrimitive();
  IFXRESULT GenerateArc();

  F32 fTopRadius;
  F32 fBottomRadius;
  F32 fHeight;
};

//---------------------------------------------------------------------------
//  CIFXConePrimitive
//
//  All these parameters are specific to creating a cone
//---------------------------------------------------------------------------
class CIFXConePrimitive: public CIFXPrimitiveShape
{
public:
  CIFXConePrimitive();
  IFXRESULT GenerateArc();
  F32 fRadius;
  F32 fHeight;
};

//---------------------------------------------------------------------------
//  CIFXPlanePrimitive
//
//  All these parameters are specific to creating a plane
//---------------------------------------------------------------------------
class CIFXPlanePrimitive: public CIFXPrimitiveShape
{
public:
  CIFXPlanePrimitive();
  IFXRESULT GenerateMesh();
  IFXRESULT GenerateMesh(IFXMesh** pDestinationMesh);
  F32 fLength;
  F32 fWidth;
  U32 uLengthVertices;
  U32 uWidthVertices;
};

//---------------------------------------------------------------------------
//  CIFXBoxPrimitive
//
//  All these parameters are specific to creating a box
//---------------------------------------------------------------------------
class CIFXBoxPrimitive: public CIFXPrimitiveShape
{
public:
  CIFXBoxPrimitive();
  IFXRESULT GenerateMesh();

  BOOL bFront;
  BOOL bBack;
  BOOL bLeft;
  BOOL bRight;
  F32 fHeight;
  F32 fWidth;
  F32 fLength;
  U32 uHeightVertices;
  U32 uWidthVertices;
  U32 uLengthVertices;
};


// {D9039C01-A75D-11d3-A0E9-00A0C9A0F93B}
IFXDEFINE_GUID(IID_IFXPrimitiveGenerator,
0xd9039c01, 0xa75d, 0x11d3, 0xa0, 0xe9, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

//---------------------------------------------------------------------------
//  IFXPrimitiveGenerator
//
//  Interface description for the Primitive generator component
//---------------------------------------------------------------------------
class IFXPrimitiveGenerator : virtual public IFXUnknown
{
public:
  virtual IFXRESULT GeneratePrimitiveFromArc(CIFXPrimitiveShape* pPrimitive)=0;
  virtual IFXRESULT GeneratePrimitive(CIFXPrimitiveShape* pPrimitive)=0;
};

#endif
