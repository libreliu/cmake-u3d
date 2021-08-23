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
//  CIFXPrimitiveShape.cpp
//
//  DESCRIPTION
//    Implementation file for the primitives shape classes
//
//  NOTES
//      None.
//
//***************************************************************************
#include "IFXPrimitiveGenerator.h"
#include "IFXCoreCIDs.h"
#include "IFXCOM.h"

extern U32 IFXGetNextRenderableId();

//---------------------------------------------------------------------------
//  CIFXPrimitiveShape
//
//  This is the constructor for the CIFXPrimitiveShape class.  It initializes
//  everything to meaningful defaults and/or NULL
//---------------------------------------------------------------------------
CIFXPrimitiveShape::CIFXPrimitiveShape()
{

  uPrimitiveType=IFX_SPHERE;
  uNormalOrientation=IFXPrimitive::eFrontFacing;

  // arc parameters
  bTopCap=FALSE;
  bBottomCap=FALSE;
  uNumArcSlices=1;
  pArcCoords=NULL;
  pArcNormals=NULL;

  uNumTopCapVerts=0;
  pArcTopCapVerts=NULL;
  pArcTopCapNormals=NULL;

  uNumBottomCapVerts=0;
  pArcBottomCapVerts=NULL;
  pArcBottomCapNormals=NULL;

  // solid parameters
  bSolidCapEnd=FALSE;       // generate ending cap
  bSolidCapStart=FALSE;     // generate starting cap
  uSolidNumSurfaceSlices=1;   // number of slices to generate
  fSolidAngleStart=0.0f;      // in radians
  fSolidAngleEnd=0.0f;      // in radians

  // meshes
  IFXASSERT( 14 == sizeof(m_pAllMeshes)/sizeof(m_pAllMeshes[0]) );
  U32 i;
  for (i = 0; i < sizeof(m_pAllMeshes)/sizeof(m_pAllMeshes[0]); i++ )
  {
    m_pAllMeshes[i] = NULL;
    m_uLastMeshId[i] = 0;
  }
  //  m_pPrimitiveMesh=NULL;    // output for the main mesh
  //  pTopCapMesh=NULL;     // output for top cap mesh
  //  pBottomCapMesh=NULL;    // output for bottom cap mesh
  //  pPrimitiveMeshInverse=NULL; // output for the main mesh inverse
  //  pTopCapMeshInverse=NULL;  // output for top cap mesh inverse
  //  pBottomCapMeshInverse=NULL; // output for bottom cap mesh inverse
  //  pFrontMesh=NULL;
  //  pFrontMeshInverse=NULL;
  //  pBackMesh=NULL;
  //  pBackMeshInverse=NULL;
  //  pSolidCapStartMesh=NULL;
  //  pSolidCapEndMesh=NULL;
  //  pSolidCapStartMeshInverse=NULL;
  //  pSolidCapEndMeshInverse=NULL;

  m_pMeshGroup = NULL;
  m_pNeighborMesh = NULL;

  // primitive arc coordinates
  pArcCoords=NULL;
  pArcNormals=NULL;
  pArcTopCapVerts=NULL;
  pArcTopCapNormals=NULL;
  pArcBottomCapVerts=NULL;
  pArcBottomCapNormals=NULL;

  ePrimitiveOrientation = IFXPrimitive::eUninitialized;

#ifdef PRIMITIVE_GEN_BUILD_DISPLAY_LIST
  // testing data
  uMeshDisplayList=0;
  uTopMeshDisplayList=0;
  uBottomMeshDisplayList=0;
  uStartCapMeshDisplayList=0;
  uEndCapMeshDisplayList=0;
#endif
}

//---------------------------------------------------------------------------
//  ~CIFXPrimitiveShape
//
//  This is the destructor for the ~CIFXPrimitiveShape class.  It releases
//  all immediate data.  IT DOES NOT DELETE THE generated IFXMesh's or
//  IFXMeshGroup!!  This is so you can continue to use the meshes after you
//  have destroyed this object.
//  You must delete these manually or it will leak.
//---------------------------------------------------------------------------
CIFXPrimitiveShape::~CIFXPrimitiveShape()
{
  ClearImmediateData();
  // meshes

  IFXRELEASE(m_pMeshGroup);
  IFXRELEASE(m_pNeighborMesh);

  U32 i;
  for (i = 0; i < sizeof(m_pAllMeshes)/sizeof(m_pAllMeshes[0]); i++ )
  {
    IFXRELEASE( m_pAllMeshes[i] );
  }

  //  IFXRELEASE(m_pPrimitiveMesh);
  //  IFXRELEASE(pTopCapMesh);
  //  IFXRELEASE(pBottomCapMesh);
  //  IFXRELEASE(pPrimitiveMeshInverse);
  //  IFXRELEASE(pTopCapMeshInverse);
  //  IFXRELEASE(pBottomCapMeshInverse);
  //  IFXRELEASE(pFrontMesh);
  //  IFXRELEASE(pFrontMeshInverse);
  //  IFXRELEASE(pBackMesh);
  //  IFXRELEASE(pBackMeshInverse);
  //  IFXRELEASE(pSolidCapEndMesh);
  //  IFXRELEASE(pSolidCapEndMeshInverse);
  //  IFXRELEASE(pSolidCapStartMesh);
  //  IFXRELEASE(pSolidCapStartMeshInverse);
}

//---------------------------------------------------------------------------
//  CIFXPrimitiveShape::ClearImmediateData
//
//  This function releases all immediate data.  IT DOES NOT DELETE THE
//  generated IFXMesh's or IFXMeshGroup!!  This is so you can continue to use
//  the meshes after you have destroyed this object.
//  You must delete these manually or it will leak.
//---------------------------------------------------------------------------
IFXRESULT CIFXPrimitiveShape::ClearImmediateData()
{

  IFXDELETE_ARRAY( pArcCoords );
  IFXDELETE_ARRAY( pArcNormals );
  IFXDELETE_ARRAY( pArcTopCapVerts );
  IFXDELETE_ARRAY( pArcTopCapNormals );
  IFXDELETE_ARRAY( pArcBottomCapVerts );
  IFXDELETE_ARRAY( pArcBottomCapNormals );

  return IFX_OK;
}


// CIFXSpherePrimitive methods

//---------------------------------------------------------------------------
//  CIFXSpherePrimitive::CIFXSpherePrimitive()
//
//  The constructor for the sphere primitive - sets all the default values.
//---------------------------------------------------------------------------
CIFXSpherePrimitive::CIFXSpherePrimitive() {
  fRadius=1.0;
  fArcAngleStart=0.0;
  fArcAngleEnd=IFX_PI;
}

//---------------------------------------------------------------------------
//  CIFXSpherePrimitive::GenerateArc
//
//  The member function generates a vector of points that makes up the primitive
//  arc for rotating.  It allocates the points in the IFXVector3 pointers in
//  the primitive structure to be used by the GeneratePrimitive function.
//---------------------------------------------------------------------------
IFXRESULT CIFXSpherePrimitive::GenerateArc() {
  U32 uIndex;
  F32 fX,fY,fZ;
  F32 fLength;
  F32 fTheta;
  F32 fThetaDelta;
  F32 iNormal;

  iNormal = 1.0f;

  if(uNumArcSlices==0)
    return IFX_E_INVALID_RANGE;
  if(fRadius==0)
    return IFX_E_INVALID_RANGE;
  if(fArcAngleStart>fArcAngleEnd)
    return IFX_E_INVALID_RANGE;

  // this method is being called more than once prior
  // to the destruction of the primitive, thus, we need to delete these
  // arrays here as well as in the destructor.
  ClearImmediateData();

  pArcCoords=new IFXVector3[uNumArcSlices+1];
  pArcNormals=new IFXVector3[uNumArcSlices+1];

  if(pArcCoords==NULL || pArcNormals==NULL)
  {
    IFXDELETE_ARRAY(pArcCoords);
    return IFX_E_OUT_OF_MEMORY;
  }

  fX=0.0f; fY=0.0f; fZ=0.0f; fTheta=0.0f;

  fTheta=fArcAngleStart;
  fThetaDelta=(fArcAngleEnd - fArcAngleStart)/uNumArcSlices;

  if(uNormalOrientation==IFXPrimitive::eFrontFacing)
    iNormal=1.0f;
  else if(uNormalOrientation==IFXPrimitive::eBackFacing)
    iNormal=-1.0f;

  // Add a top cap
  if(bTopCap && fArcAngleStart!=0.0f) {
    if(pArcTopCapVerts!=NULL) {
      delete[] pArcTopCapVerts;
      pArcTopCapVerts=NULL;
    }
    if(pArcTopCapNormals!=NULL) {
      delete[] pArcTopCapNormals;
      pArcTopCapNormals=NULL;
    }

    uNumTopCapVerts=1;
    pArcTopCapVerts=new IFXVector3[2];
    pArcTopCapNormals=new IFXVector3[2];
    if(pArcTopCapVerts==NULL || pArcTopCapVerts==NULL) {
      IFXDELETE_ARRAY(pArcTopCapVerts);
      return IFX_E_OUT_OF_MEMORY;
    }

    fZ=fRadius*((F32)cos(fTheta));
    fY=fRadius*((F32)sin(fTheta));

    pArcTopCapVerts[0].Set(0, 0, fZ);
    pArcTopCapVerts[1].Set(0, fY, fZ);
    pArcTopCapNormals[0].Set(0, 0, iNormal);
    pArcTopCapNormals[1].Set(0, 0, iNormal);
  }

  for(uIndex=0;uIndex<=uNumArcSlices;uIndex++) {
    // calculate x,y coordinates
    /// @todo: replace with sin/cos lookup table
    fZ=fRadius*((F32)cos(fTheta));
    fY=fRadius*((F32)sin(fTheta));
    pArcCoords[uIndex].Set(fX, fY, fZ);
    fTheta=fTheta+fThetaDelta;

    // calculate normals smoothly
    // normalize the point we just generated to get normal
    fLength=(F32)sqrt((F32)(fZ*fZ + fY*fY));
    fZ=fZ/fLength; fY=fY/fLength;
    pArcNormals[uIndex].Set(0, fY*iNormal, fZ*iNormal);
  }
  if(fArcAngleEnd==IFX_PI)
  {
    // garantee zero
    fZ=-fRadius;
    pArcCoords[uIndex-1].Set(0, 0, fZ);
    fLength=(F32)sqrt((F32)(fZ*fZ));
    fZ=fZ/fLength;
    pArcNormals[uIndex-1].Set(0, 0, fZ*iNormal);
  }

  // add a bottom cap
  if(bBottomCap && fArcAngleEnd!=IFX_PI)
  {
    if(pArcBottomCapVerts!=NULL) {
      delete[] pArcBottomCapVerts;
      pArcBottomCapVerts=NULL;
    }
    if(pArcBottomCapNormals!=NULL) {
      delete[] pArcBottomCapNormals;
      pArcBottomCapNormals=NULL;
    }
    uNumBottomCapVerts=1;
    pArcBottomCapVerts=new IFXVector3[2];
    pArcBottomCapNormals=new IFXVector3[2];
    if(pArcBottomCapVerts==NULL || pArcBottomCapNormals==NULL)
    {
      IFXDELETE_ARRAY(pArcBottomCapVerts);
      return IFX_E_OUT_OF_MEMORY;
    }

    pArcBottomCapVerts[0].Set(fX, fY*fRadius, fZ*fRadius);
    pArcBottomCapVerts[1].Set(0, 0, fZ*fRadius);
    pArcBottomCapNormals[0].Set(0, 0, -1*iNormal);
    pArcBottomCapNormals[1].Set(0, 0, -1*iNormal);

  }
  return IFX_OK;
}


// CIFXCylinderPrimitive methods

//---------------------------------------------------------------------------
//  CIFXCylinderPrimitive::CIFXCylinderPrimitive
//
//  The constructor for the cylinder primitive - sets all default values
//---------------------------------------------------------------------------
CIFXCylinderPrimitive::CIFXCylinderPrimitive()
{
  fTopRadius=1.0;
  fBottomRadius=1.0;
  fHeight=1.0;
}

//---------------------------------------------------------------------------
//  CIFXCylinderPrimitive::GenerateArc
//
//  The member function generates a vector of points that makes up the primitive
//  arc for rotating.  It allocates the points in the IFXVector3 pointers in
//  the primitive structure to be used by the GeneratePrimitive function.
//---------------------------------------------------------------------------
IFXRESULT CIFXCylinderPrimitive::GenerateArc()
{
  U32 uIndex;
  F32 fY, fZ;
  F32 fPointDeltaZ;
  F32 fPointDeltaY;
  F32 fNormalOrientation;

  fNormalOrientation = 1.0f;

  if(uNumArcSlices==0) {
    return IFX_E_INVALID_RANGE;
  }

  if(uNormalOrientation==IFXPrimitive::eFrontFacing)
    fNormalOrientation=1.0f;
  else if(uNormalOrientation==IFXPrimitive::eBackFacing)
    fNormalOrientation=-1.0f;

  // this method is being called more than once prior
  // to the destruction of the primitive, thus, we need to delete these
  // arrays here as well as in the destructor.
  ClearImmediateData();

  pArcCoords=new IFXVector3[uNumArcSlices+1];
  pArcNormals=new IFXVector3[uNumArcSlices+1];

  if(pArcCoords==NULL || pArcNormals==NULL) {
    IFXDELETE_ARRAY(pArcCoords);
    return IFX_E_OUT_OF_MEMORY;
  }

  // build top cap (if specified)
  if(bTopCap) {
    if(pArcTopCapVerts!=NULL) {
      delete[] pArcTopCapVerts;
      pArcTopCapVerts=NULL;
    }
    if(pArcTopCapNormals!=NULL) {
      delete[] pArcTopCapNormals;
      pArcTopCapNormals=NULL;
    }

    uNumTopCapVerts=1;
    pArcTopCapVerts=new IFXVector3[2];
    pArcTopCapNormals=new IFXVector3[2];
    if(pArcTopCapVerts==NULL || pArcTopCapVerts==NULL) {
      IFXDELETE_ARRAY(pArcTopCapVerts);
      return IFX_E_OUT_OF_MEMORY;
    }

    pArcTopCapVerts[0].Set(0.0f, 0.0f, (fHeight/2));
    if(fTopRadius==0)
      fTopRadius=0.0001f;
    pArcTopCapVerts[1].Set(0.0f, fTopRadius, (fHeight/2));
    pArcTopCapNormals[0].Set(0, 0, 1.0f*fNormalOrientation);
    pArcTopCapNormals[1].Set(0, 0, 1.0f*fNormalOrientation);
  }

  if(fHeight != 0.0f)
  {
    // build walls
    fZ=fHeight/2;
    fPointDeltaZ=fHeight/uNumArcSlices;
    fPointDeltaY=(fTopRadius-fBottomRadius)/uNumArcSlices;
    IFXVector3 vNormal, v3SideVector;

    if(uNormalOrientation==IFXPrimitive::eFrontFacing)
      v3SideVector.Set(0, fTopRadius-fBottomRadius, fNormalOrientation*fHeight);
    else
      v3SideVector.Set(0, fBottomRadius-fTopRadius, fNormalOrientation*fHeight);
    IFXVector3 v3Reference(1,0,0);
    vNormal.CrossProduct(v3SideVector, v3Reference);
    vNormal.Normalize();

    fY=fTopRadius;

    for(uIndex=0; uIndex<=uNumArcSlices; uIndex++) {
      pArcCoords[uIndex].Set(0.0f, fY, fZ);
      pArcNormals[uIndex].Set(0, vNormal.Y(), vNormal.Z());
      fZ=fZ-fPointDeltaZ;
      fY=fY-fPointDeltaY;
    }
  }

  // build bottom cap (if specified)
  if(bBottomCap)
  {
    // allocate the vector arrays
    if( pArcBottomCapVerts )
    {
      delete[] pArcBottomCapVerts;
      pArcBottomCapVerts=NULL;
    }
    if( pArcBottomCapNormals )
    {
      delete[] pArcBottomCapNormals;
      pArcBottomCapNormals=NULL;
    }
    uNumBottomCapVerts=1;
    pArcBottomCapVerts=new IFXVector3[2];
    pArcBottomCapNormals=new IFXVector3[2];
    if(pArcBottomCapVerts==NULL || pArcBottomCapNormals==NULL)
    {
      IFXDELETE_ARRAY(pArcBottomCapVerts);
      return IFX_E_OUT_OF_MEMORY;
    }

    if(fBottomRadius==0)
      fBottomRadius=0.0001f;
    pArcBottomCapVerts[1].Set(0, fBottomRadius, -1.0f*(fHeight/2));
    pArcBottomCapVerts[0].Set(0, 0, -1.0f*(fHeight/2));
    pArcBottomCapNormals[0].Set(0, 0, -1.0f*fNormalOrientation);
    pArcBottomCapNormals[1].Set(0, 0, -1.0f*fNormalOrientation);

  }

  return IFX_OK;
}


// CIFXConePrimitive methods

//---------------------------------------------------------------------------
//  CIFXConePrimitive::CIFXConePrimitive
//
//  The constructor for the cone primitive - sets all default values
//---------------------------------------------------------------------------
CIFXConePrimitive::CIFXConePrimitive() {
  fRadius=1.0;
  fHeight=1.0;
}

//---------------------------------------------------------------------------
//  CIFXConePrimitive::GenerateArc
//
//  The member function generates a vector of points that makes up the primitive
//  arc for rotating.  It allocates the points in the IFXVector3 pointers in
//  the primitive structure to be used by the GeneratePrimitive function.
//---------------------------------------------------------------------------
IFXRESULT CIFXConePrimitive::GenerateArc()
{
  F32 fY,fZ;
  F32 fLength;
  F32 fPointDeltaZ, fPointDeltaY;
  F32 fNormalZ, fNormalY;
  IFXVector3  vp;

  F32 fNormalOrientation;
  U32 uIndex;

  fNormalOrientation = 1.0f;

  // Initialize variables
  if(uNormalOrientation==IFXPrimitive::eFrontFacing)
    fNormalOrientation=1.0f;
  else if(uNormalOrientation==IFXPrimitive::eBackFacing)
    fNormalOrientation=-1.0f;

  // this method is being called more than once prior
  // to the destruction of the primitive, thus, we need to delete these
  // arrays here as well as in the destructor.
  ClearImmediateData();

  // allocate the vector arrays
  pArcCoords=new IFXVector3[uNumArcSlices+1];
  pArcNormals=new IFXVector3[uNumArcSlices+1];

  if(pArcCoords==NULL || pArcNormals==NULL)
  {
    IFXDELETE_ARRAY(pArcCoords);
    return IFX_E_OUT_OF_MEMORY;
  }

  // no top caps on cones
  uNumTopCapVerts=0;

  // build walls of the cone
  fZ=(fHeight)/2;
  fY=0;
  fPointDeltaZ=fHeight/uNumArcSlices;
  fPointDeltaY=fRadius/uNumArcSlices;

  // calculate the normal for a side
  vp.Set(0, fY-(fY+fPointDeltaY), fZ-(fZ-fPointDeltaZ));
  if(uNormalOrientation==0) {
    fNormalZ=1.0f;
    fNormalY=vp.Z()*(-1/vp.Y());
  } else {
    fNormalZ=-1.0f;
    fNormalY=vp.Z()*(1/vp.Y());
  }
  fLength=(F32)sqrt(fNormalZ*fNormalZ+fNormalY*fNormalY);
  fNormalZ=fNormalZ/fLength;
  fNormalY=fNormalY/fLength;

  for(uIndex=0; uIndex<=uNumArcSlices; uIndex++) {
    pArcCoords[uIndex].Set(0, fY, fZ);
    pArcNormals[uIndex].Set(0, fNormalY, fNormalZ);
    fZ=fZ-fPointDeltaZ;
    fY=fY+fPointDeltaY;
  }
  pArcNormals[0].Set(0.0f, 0.0f, 1.0f);

  // build bottom cap (if specified)
  if(bBottomCap)
  {
    if( pArcBottomCapVerts )
    {
      delete[] pArcBottomCapVerts;
      pArcBottomCapVerts=NULL;
    }
    if( pArcBottomCapNormals )
    {
      delete[] pArcBottomCapNormals;
      pArcBottomCapNormals=NULL;
    }
    uNumBottomCapVerts=1;
    pArcBottomCapVerts=new IFXVector3[2];
    pArcBottomCapNormals=new IFXVector3[2];
    if(pArcBottomCapVerts==NULL || pArcBottomCapNormals==NULL) {
      IFXDELETE_ARRAY(pArcBottomCapVerts);
      return IFX_E_OUT_OF_MEMORY;
    }

    pArcBottomCapVerts[1].Set(0, fRadius, -1.0f*(fHeight/2));
    pArcBottomCapVerts[0].Set(0, 0, -1.0f*(fHeight/2));
    pArcBottomCapNormals[0].Set(0, 0, -1.0f*fNormalOrientation);
    pArcBottomCapNormals[1].Set(0, 0, -1.0f*fNormalOrientation);
  }

  return IFX_OK;
}

CIFXPlanePrimitive::CIFXPlanePrimitive()
{
  uNormalOrientation = IFXPrimitive::eBothFacing; // default planes to bothfacing
}


//---------------------------------------------------------------------------
//  CIFXPlanePrimitive::GenerateArc
//
//  The member function generates the actual mesh specified.  Stored in the
//  main and inverse meshes.
//---------------------------------------------------------------------------
IFXRESULT CIFXPlanePrimitive::GenerateMesh()
{
  if(uNormalOrientation==IFXPrimitive::eFrontFacing)
    GenerateMesh(&m_pPrimitiveMesh);
  else
    GenerateMesh(&pPrimitiveMeshInverse);

  return IFX_OK;
}

//---------------------------------------------------------------------------
//  CIFXPlanePrimitive::GenerateArc
//
//  The member function generates the actual mesh specified.  Stored in the
//  main and inverse meshes.
//---------------------------------------------------------------------------
IFXRESULT CIFXPlanePrimitive::GenerateMesh(IFXMesh** pMesh)
{
  // booleans to make life nice
  bool bCounterClockWise=TRUE;

  // counting variables
  U32 uFinalMeshVerticeCount;
  U32 uFinalMeshFaceCount;
  U32 uIndex;
  U32 i,j;
  U32 uCurrentVertexIndex;
  U32 uPreviousRowIndex;

  // pointers

  // vector walking variables
  IFXVector3  pVertex;
  IFXVector3  pNormal;
  F32     fThetaLength;
  F32     fThetaWidth;
  F32     fLengthStart;
  F32     fWidthStart;


  // mesh variables
  IFXMesh*  pDestinationMesh=NULL;
  IFXVector3Iter v3iVertex;
  IFXVector3Iter v3iNormal;
  IFXVector2Iter v2iTexCoord;
  IFXFaceIter fiFace;
  IFXVector3* pvVertex;
  IFXVector3* pvNormal;
  IFXVector2* pvTexCoord;
  IFXFace*  pfFace;

  F32     fUDelta;
  F32     fVDelta;
  F32     fUCoord;
  F32     fVCoord;
  IFXRESULT ifxResult;


  // check parameters
  if(uLengthVertices<2 || uWidthVertices<2)
    return IFX_E_INVALID_RANGE;

  // create the IFXMesh with the correct size
  IFXCreateComponent(CID_IFXMesh, IID_IFXMesh, (void**)&pDestinationMesh);
  if(pDestinationMesh==NULL)
    return IFX_E_OUT_OF_MEMORY;

  uFinalMeshVerticeCount=uLengthVertices*uWidthVertices;
  uFinalMeshFaceCount=((uLengthVertices-1)*(uWidthVertices-1))*2;

  IFXVertexAttributes vertexAttributes;
  ifxResult=pDestinationMesh->Allocate(vertexAttributes,
    uFinalMeshVerticeCount,
    uFinalMeshFaceCount);
  if(ifxResult!=IFX_OK)
    return ifxResult;

  // set up iterators
  pDestinationMesh->GetPositionIter(v3iVertex);
  pDestinationMesh->GetNormalIter(v3iNormal);
  pDestinationMesh->GetTexCoordIter(v2iTexCoord);
  pDestinationMesh->GetFaceIter(fiFace);

  // calculate mesh steppings
  fThetaLength=fLength/(uLengthVertices-1);
  fThetaWidth=fWidth/(uWidthVertices-1);
  fLengthStart=-(fLength/2);
  fWidthStart=-(fWidth/2);
  fUDelta = 1.0f/((F32)(uWidthVertices-1));
  fVDelta = 1.0f/((F32)(uLengthVertices-1));

  uPreviousRowIndex=0;
  uCurrentVertexIndex=0;

  pVertex.X()=-fWidthStart;
  pVertex.Y()=fLengthStart;

  pVertex.Z()=0.0f;
  pNormal.X()=0.0f;
  pNormal.Y()=0.0f;
  pNormal.Z()=0.0f;

  if(uNormalOrientation==IFXPrimitive::eBackFacing) {
    pNormal.Z()=1.0f;
    bCounterClockWise=TRUE;
    fUCoord = PRIMITIVE_TEXTURECOORDINATE_END;
    fVCoord = PRIMITIVE_TEXTURECOORDINATE_START;
  }
  else
  {
    pNormal.Z()=-1.0f;
    bCounterClockWise=FALSE;
    fUCoord = PRIMITIVE_TEXTURECOORDINATE_START;
    fVCoord = PRIMITIVE_TEXTURECOORDINATE_START;
  }

  fVCoord = PRIMITIVE_TEXTURECOORDINATE_START;

  // add the first row of vertices/normals to the mesh
  for(uIndex=0; uIndex<uWidthVertices; uIndex++) {
    pvVertex=v3iVertex.Next();
    pvNormal=v3iNormal.Next();
    pvTexCoord=v2iTexCoord.Next();

    pvVertex->Set(pVertex.X(), pVertex.Y(), pVertex.Z());
    pvNormal->Set(pNormal.X(), pNormal.Y(), pNormal.Z());
    pvTexCoord->Set(fUCoord, fVCoord);
    pVertex.X()-=fThetaWidth;
    uCurrentVertexIndex++;

    if(TRUE==bCounterClockWise) {
      fUCoord-=fUDelta;
      if(fUCoord < PRIMITIVE_TEXTURECOORDINATE_START)
        fUCoord = PRIMITIVE_TEXTURECOORDINATE_START;
    } else {
      fUCoord+=fUDelta;
      if(fUCoord > PRIMITIVE_TEXTURECOORDINATE_END)
        fUCoord = PRIMITIVE_TEXTURECOORDINATE_END;
    }
  }

  for(i=0; i<(uLengthVertices-1); i++) {
    // reset for next width row
    pVertex.Y()+=fThetaLength;
    pVertex.X()=-fWidthStart;

    if(TRUE==bCounterClockWise)
      fUCoord=PRIMITIVE_TEXTURECOORDINATE_END;
    else
      fUCoord=PRIMITIVE_TEXTURECOORDINATE_START;

    fVCoord+=fVDelta;
    if(fVCoord > PRIMITIVE_TEXTURECOORDINATE_END)
      fVCoord = PRIMITIVE_TEXTURECOORDINATE_END;

    // put the first vertex of the row down
    pvVertex=v3iVertex.Next();
    pvNormal=v3iNormal.Next();
    pvTexCoord=v2iTexCoord.Next();

    pvVertex->Set(pVertex.X(), pVertex.Y(), pVertex.Z());
    pvNormal->Set(pNormal.X(), pNormal.Y(), pNormal.Z());
    pvTexCoord->Set(fUCoord, fVCoord);

    uCurrentVertexIndex++;

    for(j=0; j<(uWidthVertices-1); j++)
    {
      // set new coordinate
      pVertex.X()-=fThetaWidth;

      if(TRUE==bCounterClockWise) {
        fUCoord-=fUDelta;
        if(fUCoord < PRIMITIVE_TEXTURECOORDINATE_START)
          fUCoord = PRIMITIVE_TEXTURECOORDINATE_START;
      } else {
        fUCoord+=fUDelta;
        if(fUCoord > PRIMITIVE_TEXTURECOORDINATE_END)
          fUCoord = PRIMITIVE_TEXTURECOORDINATE_END;
      }

      // add the new vertex to the mesh
      pvVertex=v3iVertex.Next();
      pvNormal=v3iNormal.Next();
      pvTexCoord=v2iTexCoord.Next();

      pvVertex->Set(pVertex.X(), pVertex.Y(), pVertex.Z());
      pvNormal->Set(pNormal.X(), pNormal.Y(), pNormal.Z());
      pvTexCoord->Set(fUCoord, fVCoord);

      // add the two faces
      pfFace=fiFace.Next();
      if(bCounterClockWise)
        pfFace->Set(uCurrentVertexIndex, uPreviousRowIndex+1, uPreviousRowIndex);
      else
        pfFace->Set(uCurrentVertexIndex, uPreviousRowIndex, uPreviousRowIndex+1);

      pfFace=fiFace.Next();
      if(bCounterClockWise)
        pfFace->Set(uCurrentVertexIndex-1, uCurrentVertexIndex, uPreviousRowIndex);
      else
        pfFace->Set(uCurrentVertexIndex-1, uPreviousRowIndex, uCurrentVertexIndex);

      uCurrentVertexIndex++;
      uPreviousRowIndex++;
    }
    uPreviousRowIndex++;
  }


  *pMesh=pDestinationMesh;

  return IFX_OK;
}


// CIFXBoxPrimitive methods


//---------------------------------------------------------------------------
//  CIFXBoxPrimitive constructor
//
//  Default constructor for the box primitive class
//---------------------------------------------------------------------------
CIFXBoxPrimitive::CIFXBoxPrimitive()
{
  bTopCap=TRUE;
  bBottomCap=TRUE;
  bFront=TRUE;
  bBack=TRUE;
  bLeft=TRUE;
  bRight=TRUE;
  fHeight=1.0f;
  fWidth=1.0f;
  fLength=1.0f;
  uHeightVertices=2;
  uWidthVertices=2;
  uLengthVertices=2;
}


//---------------------------------------------------------------------------
//  CIFXBoxPrimitive::GenerateArc
//
//  The member function generates the actual mesh specified.  Stored in the
//  main and inverse meshes.
//---------------------------------------------------------------------------
IFXRESULT CIFXBoxPrimitive::GenerateMesh()
{
  U32 uIndex;
  bool  bBothFacing=FALSE;

  // calculation variables
  F32 fcosTheta;
  F32 fsinTheta;

  // mesh variables
  IFXVector3Iter v3iVertex;
  IFXVector3Iter v3iNormal;
  IFXVector3* pvVertex;
  IFXVector3* pvNormal;

  if(uNormalOrientation==IFXPrimitive::eBothFacing)
    bBothFacing=TRUE;

  // build the box walls from the plane primitive

  CIFXPlanePrimitive* pPlane = new CIFXPlanePrimitive();
  if (pPlane == NULL)
    return IFX_E_OUT_OF_MEMORY;

  pPlane->uPrimitiveType=IFX_PLANE;
  pPlane->uNormalOrientation=uNormalOrientation;

  // build front and back meshes
  pPlane->fLength=fWidth;
  pPlane->fWidth=fHeight;
  pPlane->uLengthVertices=uWidthVertices;
  pPlane->uWidthVertices=uHeightVertices;

  if(uNormalOrientation==IFXPrimitive::eFrontFacing || bBothFacing) {
    pPlane->uNormalOrientation=IFXPrimitive::eFrontFacing;
    //  build front mesh
    if(bFront)
    {
      pPlane->GenerateMesh(&pFrontMesh);
    }

    // build back mesh
    if(bBack)
    {
      pPlane->GenerateMesh(&pBackMesh);
    }

  }
  if(uNormalOrientation==IFXPrimitive::eBackFacing || bBothFacing)
  {
    pPlane->uNormalOrientation=IFXPrimitive::eBackFacing;
    //  build front mesh
    if(bFront)
    {
      pPlane->GenerateMesh(&pFrontMeshInverse);
    }

    // build back mesh
    if(bBack)
    {
      pPlane->GenerateMesh(&pBackMeshInverse);
    }
  }

  // build left and right sides
  pPlane->fLength=fWidth;
  pPlane->fWidth=fLength;
  pPlane->uLengthVertices=uWidthVertices;
  pPlane->uWidthVertices=uLengthVertices;

  if(uNormalOrientation==IFXPrimitive::eFrontFacing || bBothFacing)
  {
    pPlane->uNormalOrientation=IFXPrimitive::eFrontFacing;
    // build left mesh
    if(bLeft)
    {
      pPlane->GenerateMesh(&pSolidCapStartMesh);
    }

    // build right mesh
    if(bRight)
    {
      pPlane->GenerateMesh(&pSolidCapEndMesh);
    }

  }
  if(uNormalOrientation==IFXPrimitive::eBackFacing || bBothFacing)
  {
    pPlane->uNormalOrientation=IFXPrimitive::eBackFacing;
    // build left mesh
    if(bLeft)
    {
      pPlane->GenerateMesh(&pSolidCapStartMeshInverse);
    }

    // build right mesh
    if(bRight)
    {
      pPlane->GenerateMesh(&pSolidCapEndMeshInverse);
      pPlane->uNormalOrientation=IFXPrimitive::eBackFacing;
    }

  }


  // build the top and bottom meshes
  pPlane->fLength=fLength;
  pPlane->fWidth=fHeight;
  pPlane->uLengthVertices=uLengthVertices;
  pPlane->uWidthVertices=uHeightVertices;
  if(uNormalOrientation==IFXPrimitive::eFrontFacing || bBothFacing)
  {
    pPlane->uNormalOrientation=IFXPrimitive::eFrontFacing;
    // build top mesh (+X)
    if(bTopCap)
    {
      pPlane->GenerateMesh(&pTopCapMesh);
    }

    // build bottom mesh (-X)
    if(bBottomCap)
    {
      pPlane->GenerateMesh(&pBottomCapMesh);
    }
  }
  if(uNormalOrientation==IFXPrimitive::eBackFacing || bBothFacing) {
    pPlane->uNormalOrientation=IFXPrimitive::eBackFacing;
    // build top mesh (+x)
    if(bTopCap)
    {
      pPlane->GenerateMesh(&pTopCapMeshInverse);
    }

    // build bottom mesh (-x)
    if(bBottomCap)
    {
      pPlane->GenerateMesh(&pBottomCapMeshInverse);
    }
  }

  if(bBothFacing)
    uNormalOrientation=IFXPrimitive::eBothFacing;

  // --- rotate/translate the box walls ---
  // set up iterators
  // outside, front
  if ( pFrontMesh!=NULL )
  {
    pFrontMesh->GetPositionIter(v3iVertex);
    pFrontMesh->GetNormalIter(v3iNormal);

    for(uIndex=0; uIndex<(uHeightVertices*uWidthVertices); uIndex++) {
      pvVertex=v3iVertex.Next();
      pvNormal=v3iNormal.Next();

      // translate on -z axis
      pvVertex->Set(pvVertex->X(), pvVertex->Y(), pvVertex->Z()-fLength/2);
    }
  }

  // inside, front
  if ( pFrontMeshInverse!=NULL )
  {
    pFrontMeshInverse->GetPositionIter(v3iVertex);
    pFrontMeshInverse->GetNormalIter(v3iNormal);

    for(uIndex=0; uIndex<(uHeightVertices*uWidthVertices); uIndex++)
    {
      pvVertex=v3iVertex.Next();
      pvNormal=v3iNormal.Next();

      // translate on -z axis
      pvVertex->Set(pvVertex->X(), pvVertex->Y(), pvVertex->Z()-fLength/2);
    }
  }


  // outside, back
  if ( pBackMesh!=NULL )
  {
    pBackMesh->GetPositionIter(v3iVertex);
    pBackMesh->GetNormalIter(v3iNormal);

    U32 nVertices = uHeightVertices*uWidthVertices;
    fsinTheta=0;  // rotate 180
    fcosTheta=-1; // rotate 180
    for(uIndex=0; uIndex<nVertices; uIndex++)
    {
      pvVertex=v3iVertex.Next();
      pvNormal=v3iNormal.Next();

      // rotate on y axis
      pvVertex->Set(pvVertex->Z()*fsinTheta+pvVertex->X()*fcosTheta,
        pvVertex->Y(),
        pvVertex->Z()*fcosTheta-pvVertex->X()*fsinTheta);
      pvNormal->Set(pvNormal->Z()*fsinTheta+pvNormal->X()*fcosTheta,
        pvNormal->Y(),
        pvNormal->Z()*fcosTheta-pvNormal->X()*fsinTheta);

      // translate +z axis
      pvVertex->Set(pvVertex->X(), pvVertex->Y(), pvVertex->Z()+fLength/2);
    }
  }

  // inside, back
  if ( pBackMeshInverse!=NULL )
  {
    pBackMeshInverse->GetPositionIter(v3iVertex);
    pBackMeshInverse->GetNormalIter(v3iNormal);

    U32 nVertices = uHeightVertices*uWidthVertices;
    fsinTheta=0;  // rotate 180
    fcosTheta=-1; // rotate 180
    for(uIndex=0; uIndex<nVertices; uIndex++)
    {
      pvVertex=v3iVertex.Next();
      pvNormal=v3iNormal.Next();

      // rotate on y axis
      pvVertex->Set(pvVertex->Z()*fsinTheta+pvVertex->X()*fcosTheta,
        pvVertex->Y(),
        pvVertex->Z()*fcosTheta-pvVertex->X()*fsinTheta);
      pvNormal->Set(pvNormal->Z()*fsinTheta+pvNormal->X()*fcosTheta,
        pvNormal->Y(),
        pvNormal->Z()*fcosTheta-pvNormal->X()*fsinTheta);

      // translate +z axis
      pvVertex->Set(pvVertex->X(), pvVertex->Y(), pvVertex->Z()+fLength/2);
    }
  }

  // outside, bottom
  if ( pBottomCapMesh!=NULL )
  {
    pBottomCapMesh->GetPositionIter(v3iVertex);
    pBottomCapMesh->GetNormalIter(v3iNormal);

    U32 nVertices = uLengthVertices*uHeightVertices;
    for(uIndex=0; uIndex<nVertices; uIndex++) {
      pvVertex=v3iVertex.Next();
      pvNormal=v3iNormal.Next();

      fsinTheta=-1; // 270 rotation
      fcosTheta=0;  // 270 rotation
      // rotate on x axis
      pvVertex->Set(pvVertex->X(),
        pvVertex->Y()*fcosTheta-pvVertex->Z()*fsinTheta,
        pvVertex->Y()*fsinTheta+pvVertex->Z()*fcosTheta);
      pvNormal->Set(pvNormal->X(),
        pvNormal->Y()*fcosTheta-pvNormal->Z()*fsinTheta,
        pvNormal->Y()*fsinTheta+pvNormal->Z()*fcosTheta);

      fsinTheta=0;  // rotate 180
      fcosTheta=-1; // rotate 180
      // rotate on y axis
      pvVertex->Set(pvVertex->Z()*fsinTheta+pvVertex->X()*fcosTheta,
        pvVertex->Y(),
        pvVertex->Z()*fcosTheta-pvVertex->X()*fsinTheta);
      pvNormal->Set(pvNormal->Z()*fsinTheta+pvNormal->X()*fcosTheta,
        pvNormal->Y(),
        pvNormal->Z()*fcosTheta-pvNormal->X()*fsinTheta);

      // translate on -y axis
      pvVertex->Set(pvVertex->X(), pvVertex->Y()-fWidth/2, pvVertex->Z());
    }
  }

  // inside, bottom
  if ( pBottomCapMeshInverse!=NULL )
  {
    pBottomCapMeshInverse->GetPositionIter(v3iVertex);
    pBottomCapMeshInverse->GetNormalIter(v3iNormal);

    U32 nVertices = uLengthVertices*uHeightVertices;
    for(uIndex=0; uIndex<nVertices; uIndex++)
    {
      pvVertex=v3iVertex.Next();
      pvNormal=v3iNormal.Next();

      fsinTheta=-1; // 270 rotation
      fcosTheta=0;  // 270 rotation
      // rotate on x axis
      pvVertex->Set(pvVertex->X(),
        pvVertex->Y()*fcosTheta-pvVertex->Z()*fsinTheta,
        pvVertex->Y()*fsinTheta+pvVertex->Z()*fcosTheta);
      pvNormal->Set(pvNormal->X(),
        pvNormal->Y()*fcosTheta-pvNormal->Z()*fsinTheta,
        pvNormal->Y()*fsinTheta+pvNormal->Z()*fcosTheta);

      fsinTheta=0;  // rotate 180
      fcosTheta=-1; // rotate 180
      // rotate on y axis
      pvVertex->Set(pvVertex->Z()*fsinTheta+pvVertex->X()*fcosTheta,
        pvVertex->Y(),
        pvVertex->Z()*fcosTheta-pvVertex->X()*fsinTheta);
      pvNormal->Set(pvNormal->Z()*fsinTheta+pvNormal->X()*fcosTheta,
        pvNormal->Y(),
        pvNormal->Z()*fcosTheta-pvNormal->X()*fsinTheta);

      // translate on -y axis
      pvVertex->Set(pvVertex->X(), pvVertex->Y()-fWidth/2, pvVertex->Z());
    }
  }

  // outside, top
  if ( pTopCapMesh!=NULL )
  {
    pTopCapMesh->GetPositionIter(v3iVertex);
    pTopCapMesh->GetNormalIter(v3iNormal);

    U32 nVertices = uLengthVertices*uHeightVertices;
    fsinTheta=1;  // rotate 90
    fcosTheta=0;  // rotate 90
    for(uIndex=0; uIndex<nVertices; uIndex++)
    {
      pvVertex=v3iVertex.Next();
      pvNormal=v3iNormal.Next();

      // rotate on x axis
      pvVertex->Set(pvVertex->X(),
        pvVertex->Y()*fcosTheta-pvVertex->Z()*fsinTheta,
        pvVertex->Y()*fsinTheta+pvVertex->Z()*fcosTheta);
      pvNormal->Set(pvNormal->X(),
        pvNormal->Y()*fcosTheta-pvNormal->Z()*fsinTheta,
        pvNormal->Y()*fsinTheta+pvNormal->Z()*fcosTheta);

      // translate on +y axis
      pvVertex->Set(pvVertex->X(), pvVertex->Y()+fWidth/2, pvVertex->Z());
    }
  }

  // inside, top
  if ( pTopCapMeshInverse!=NULL )
  {
    pTopCapMeshInverse->GetPositionIter(v3iVertex);
    pTopCapMeshInverse->GetNormalIter(v3iNormal);

    U32 nVertices = uLengthVertices*uHeightVertices;
    fsinTheta=1;  // rotate 90
    fcosTheta=0;  // rotate 90
    for(uIndex=0; uIndex<nVertices; uIndex++)
    {
      pvVertex=v3iVertex.Next();
      pvNormal=v3iNormal.Next();

      // rotate on x axis
      pvVertex->Set(pvVertex->X(),
        pvVertex->Y()*fcosTheta-pvVertex->Z()*fsinTheta,
        pvVertex->Y()*fsinTheta+pvVertex->Z()*fcosTheta);
      pvNormal->Set(pvNormal->X(),
        pvNormal->Y()*fcosTheta-pvNormal->Z()*fsinTheta,
        pvNormal->Y()*fsinTheta+pvNormal->Z()*fcosTheta);

      // translate on +y axis
      pvVertex->Set(pvVertex->X(), pvVertex->Y()+fWidth/2, pvVertex->Z());
    }
  }

  // outside, left
  if ( pSolidCapStartMesh!=NULL )
  {
    pSolidCapStartMesh->GetPositionIter(v3iVertex);
    pSolidCapStartMesh->GetNormalIter(v3iNormal);

    fsinTheta=1;  // rotate 90
    fcosTheta=0;  // rotate 90
    U32 nVertices = uLengthVertices*uWidthVertices;
    for(uIndex=0; uIndex<nVertices; uIndex++)
    {
      pvVertex=v3iVertex.Next();
      pvNormal=v3iNormal.Next();

      // rotate on y axis
      pvVertex->Set(pvVertex->Z()*fsinTheta+pvVertex->X()*fcosTheta,
        pvVertex->Y(),
        pvVertex->Z()*fcosTheta-pvVertex->X()*fsinTheta);
      pvNormal->Set(pvNormal->Z()*fsinTheta+pvNormal->X()*fcosTheta,
        pvNormal->Y(),
        pvNormal->Z()*fcosTheta-pvNormal->X()*fsinTheta);

      // translate on -x axis
      pvVertex->Set(pvVertex->X()-fHeight/2, pvVertex->Y(), pvVertex->Z());
    }
  }

  // inside, left
  if ( pSolidCapStartMeshInverse!=NULL )
  {
    pSolidCapStartMeshInverse->GetPositionIter(v3iVertex);
    pSolidCapStartMeshInverse->GetNormalIter(v3iNormal);

    fsinTheta=1;  // rotate 90
    fcosTheta=0;  // rotate 90
    U32 nVertices = uLengthVertices*uWidthVertices;
    for(uIndex=0; uIndex<nVertices; uIndex++)
    {
      pvVertex=v3iVertex.Next();
      pvNormal=v3iNormal.Next();

      // rotate on y axis
      pvVertex->Set(pvVertex->Z()*fsinTheta+pvVertex->X()*fcosTheta,
        pvVertex->Y(),
        pvVertex->Z()*fcosTheta-pvVertex->X()*fsinTheta);
      pvNormal->Set(pvNormal->Z()*fsinTheta+pvNormal->X()*fcosTheta,
        pvNormal->Y(),
        pvNormal->Z()*fcosTheta-pvNormal->X()*fsinTheta);

      // translate on -x axis
      pvVertex->Set(pvVertex->X()-fHeight/2, pvVertex->Y(), pvVertex->Z());
    }
  }

  //  outside, right
  if ( pSolidCapEndMesh!=NULL )
  {
    pSolidCapEndMesh->GetPositionIter(v3iVertex);
    pSolidCapEndMesh->GetNormalIter(v3iNormal);

    fsinTheta=-1; // 270 rotation
    fcosTheta=0;  // 270 rotation;
    U32 nVertices = uLengthVertices*uWidthVertices;
    for(uIndex=0; uIndex<nVertices; uIndex++)
    {
      pvVertex=v3iVertex.Next();
      pvNormal=v3iNormal.Next();

      // rotate on y axis
      pvVertex->Set(pvVertex->Z()*fsinTheta+pvVertex->X()*fcosTheta,
        pvVertex->Y(),
        pvVertex->Z()*fcosTheta-pvVertex->X()*fsinTheta);
      pvNormal->Set(pvNormal->Z()*fsinTheta+pvNormal->X()*fcosTheta,
        pvNormal->Y(),
        pvNormal->Z()*fcosTheta-pvNormal->X()*fsinTheta);

      // translate down +x axis
      pvVertex->Set(pvVertex->X()+fHeight/2, pvVertex->Y(), pvVertex->Z());
    }
  }

  //  inside, right
  if ( pSolidCapEndMeshInverse!=NULL )
  {
    pSolidCapEndMeshInverse->GetPositionIter(v3iVertex);
    pSolidCapEndMeshInverse->GetNormalIter(v3iNormal);

    fsinTheta=-1; // 270 rotation
    fcosTheta=0;  // 270 rotation;
    U32 nVertices = uLengthVertices*uWidthVertices;
    for(uIndex=0; uIndex<nVertices; uIndex++)
    {
      pvVertex=v3iVertex.Next();
      pvNormal=v3iNormal.Next();

      // rotate on y axis
      pvVertex->Set(pvVertex->Z()*fsinTheta+pvVertex->X()*fcosTheta,
        pvVertex->Y(),
        pvVertex->Z()*fcosTheta-pvVertex->X()*fsinTheta);
      pvNormal->Set(pvNormal->Z()*fsinTheta+pvNormal->X()*fcosTheta,
        pvNormal->Y(),
        pvNormal->Z()*fcosTheta-pvNormal->X()*fsinTheta);

      // translate down +x axis
      pvVertex->Set(pvVertex->X()+fHeight/2, pvVertex->Y(), pvVertex->Z());
    }
  }

  IFXDELETE( pPlane );

  return IFX_OK;
}
