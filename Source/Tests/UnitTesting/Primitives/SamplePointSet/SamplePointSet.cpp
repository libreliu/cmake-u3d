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
#ifndef SAMPLE_PointSET_H
#include "SamplePointSet.h"
#endif

#include "IFXAuthorPointSet.h"
#include "IFXAuthorPointSetResource.h"
#include "IFXMeshGroup.h"
#include "IFXOSUtilities.h"

IFXRESULT InitAuthorPointSet(IFXAuthorPointSet* pPointSet);
IFXRESULT VerifyRenderableData(IFXAuthorPointSet* pLineSet, IFXMeshGroup* pMeshGroup);

IFXRESULT SamplePointSet::Run( U3DSceneUtilities* pSGUtils )
{
  SGNodeHeader nh;
  IFXDECLARELOCAL( IFXLight,pLight);
  IFXDECLARELOCAL( IFXModifier, pGenerator );
  IFXDECLARELOCAL( IFXPalette, pShaderPalette);
  U32           uResourceID = 0;
  U32           uCloneID  = 0;
  U32           uShaderID = 0;

  struct SGNodeHeader   nodeHeader;
  IFXVector4 colorWhite(1.0f, 1.0f, 1.0f);

  TEST(IFXSUCCESS(pSGUtils->InitializeScene()), IFXException);

  // locale should be set for Linux and Mac OS X
  TEST(IFXSUCCESS(IFXSetDefaultLocale()), IFXException);

  // korean name
  IFXCHAR w_name[4];
  {
    w_name[0] = 0x5944;
    w_name[1] = 0x7e4a;
    w_name[2] = 0x4f0a;
    w_name[3] = L'\0';
  }

  // Create camera
  {
    IFXDECLARELOCAL(IFXView,pViewNode);

    wcscpy(nh.Name, w_name);
    wcscpy(nh.ParentName, PALETTE_DEFAULT_GROUP);
    nh.NodeTm.Reset();
    TEST(IFXSUCCESS(pSGUtils->CreateCameraNode(&nh, &pViewNode)), IFXException);
    // Set up the camera based on its type
    pViewNode->SetAttributes( 0 );
    TEST(IFXSUCCESS(pViewNode->SetProjection(5.0)), IFXException);
  }

  wcscpy(nh.Name, L"Light1");
  wcscpy(nh.ParentName, PALETTE_DEFAULT_GROUP);
  // Create a light node with this name
  TEST(IFXSUCCESS(pSGUtils->CreateLightNode(&nh, &pLight)), IFXException);

    // Get the light resource palette
    IFXDECLARELOCAL( IFXPalette, pLightResourcePalette );
    IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );
    TEST(IFXSUCCESS(pSGUtils->m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph )), IFXException);
    TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::LIGHT, &pLightResourcePalette )), IFXException);
    U32 uLightResourceId = 0;

    // Look for the light resource in the light resource palette
    IFXDECLARELOCAL( IFXLightResource,pLightResource );
    IFXRESULT rc = pLightResourcePalette->Add( nh.Name, &uLightResourceId );
    if (IFXSUCCESS(rc))
    {
        TEST(IFXSUCCESS( IFXCreateComponent( CID_IFXLightResource, IID_IFXLightResource, (void**)&pLightResource ) ), IFXException);
        IFXDECLARELOCAL( IFXUnknown, pUnk );
        TEST(IFXSUCCESS( pLightResource->QueryInterface( IID_IFXUnknown, (void**)&pUnk ) ), IFXException);

        IFXDECLARELOCAL( IFXMarker, pMarker );
        TEST(IFXSUCCESS(pUnk->QueryInterface( IID_IFXMarker, (void**)&pMarker )), IFXException);
        TEST(IFXSUCCESS(pMarker->SetSceneGraph( pSceneGraph )), IFXException);
        TEST(IFXSUCCESS( pLightResourcePalette->SetResourcePtr( uLightResourceId, pUnk ) ), IFXException);
        IFXRELEASE(pUnk);
    }

    // Link the light node to the light resource pallete entry
    TEST(IFXSUCCESS( pLight->SetLightResourceID(uLightResourceId) ), IFXException);

  pLightResource->SetType(IFXLightResource::DIRECTIONAL);
  pLightResource->SetColor(colorWhite);

  //
  // --- Create a PointSet primitive to indicate the picking location
  //
  TEST(IFXSUCCESS(CreatePrimitive("PickPointSet", CID_IFXAuthorPointSetResource, 0,
    &uResourceID, &pGenerator, pSGUtils->m_pGeneratorPalette, pSGUtils->m_pSceneGraph) ), IFXException);

  // Create a model node in the node palette for the sphere
  wcscpy(nodeHeader.Name, L"PickPointSet");
  wcscpy(nodeHeader.ParentName, PALETTE_DEFAULT_GROUP);
  nodeHeader.NodeTm.Reset();
  nodeHeader.NodeTm.Translate3x4(IFXVector3(0, 0, -1000));

  // Create Author PointSet
  IFXDECLARELOCAL(IFXAuthorPointSet,pAuthorPS );
  TEST( IFXSUCCESS(IFXCreateComponent(CID_IFXAuthorPointSet, IID_IFXAuthorPointSet,(void**)&pAuthorPS)),IFXException);
  IFXASSERT(pAuthorPS);


  IFXDECLARELOCAL(IFXAuthorPointSetResource, pPointSetGenerator);
  // Initialize Author Point Set by some sample data
  TEST(IFXSUCCESS(InitAuthorPointSet(pAuthorPS)),IFXException);

  TEST(IFXSUCCESS(pGenerator->QueryInterface( IID_IFXAuthorPointSetResource, (void**)&pPointSetGenerator )), IFXException);
  TEST(IFXSUCCESS(pPointSetGenerator->SetAuthorPointSet(pAuthorPS)), IFXException);

  TEST(IFXSUCCESS(pSGUtils->CreateModelNode(&nodeHeader, uResourceID)), IFXException);

  IFXVector4 Diffuse = IFXVector4(1,(F32)1,(F32)0.8);
  //Set material resource
  TEST(IFXSUCCESS(pSGUtils->SetMaterialResource(nodeHeader.Name,
    NULL, //&Ambient,
    &Diffuse,
    NULL, //&Emission,
    NULL, //&Specular,
    0.5f,
    0.f,
    FALSE,
    IFXShaderLitTexture::MATERIAL
    )), IFXException);

  // Get the shader palette
  pShaderPalette = pSGUtils->m_pShaderPalette;
  pShaderPalette->AddRef();
  TEST(pShaderPalette != NULL, IFXException);

  IFXDECLARELOCAL( IFXMeshGroup, pMeshGroup);
  TEST(IFXSUCCESS(pPointSetGenerator->GetMeshGroup(&pMeshGroup)), IFXException);
  TEST(IFXSUCCESS(VerifyRenderableData(pAuthorPS, pMeshGroup)), IFXException);

  IFXString src( getenv("U3D_SOURCE") );
  src += L"Tests/UnitTesting/Primitives/SamplePointSet/pointset.u3d";

  // write point set file
  {
    IFXDECLARELOCAL(IFXStdio, pIFXStdio2);
    IFXDECLARELOCAL(IFXWriteManager, pIFXWriteManager);
    IFXDECLARELOCAL(IFXWriteBuffer, pIFXWriteBuffer);

    TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXWriteManager, IID_IFXWriteManager, (void**)&pIFXWriteManager)), IFXException);
    TEST(IFXSUCCESS(pIFXWriteManager->Initialize(pSGUtils->m_pCoreServices)), IFXException);
    TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioWriteBuffer, IID_IFXWriteBuffer, (void**)&pIFXWriteBuffer)), IFXException);
    TEST(IFXSUCCESS(pIFXWriteBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio2)), IFXException);

    TEST(IFXSUCCESS(pIFXStdio2->Open( (IFXCHAR*) src.Raw() )), IFXException);
    TEST(IFXSUCCESS(pSceneGraph->Mark()), IFXException);
    TEST(IFXSUCCESS(pIFXWriteManager->Write(pIFXWriteBuffer, IFXEXPORT_EVERYTHING)), IFXException); // ^ IFXEXPORT_FILEREFERENCES
    TEST(IFXSUCCESS(pIFXStdio2->Close()), IFXException);
  }

  // load point set file
  {
    IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
    IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);
    IFXDECLARELOCAL(IFXStdio, pIFXStdio1);

    TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager)), IFXException);
    TEST(IFXSUCCESS(pIFXLoadManager->Initialize(pSGUtils->m_pCoreServices)), IFXException);
    TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer)), IFXException);
    TEST(IFXSUCCESS(pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio1)), IFXException);

    TEST(IFXSUCCESS(pIFXStdio1->Open( (IFXCHAR*) src.Raw() )), IFXException);
    TEST(IFXSUCCESS(pIFXLoadManager->Load(pIFXReadBuffer, TRUE)), IFXException);
    TEST(IFXSUCCESS(pIFXStdio1->Close()), IFXException);
  }

  // check that unicode view name was written and loaded properly
  {
    IFXDECLARELOCAL( IFXPalette, pPalette );
    IFXString viewName( w_name );
    U32 viewId = 0;

      TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::VIEW, &pPalette )), IFXException);
    TEST(IFXSUCCESS(pPalette->Find( &viewName, &viewId )), IFXException);
  }

  return IFX_OK;
}




IFXRESULT VerifyRenderableData(IFXAuthorPointSet* pPointSet, IFXMeshGroup* pMeshGroup)
{
  IFXRESULT ir  = IFX_OK;
  int j,k;

  if( !pMeshGroup || !pPointSet){ ir = IFX_FALSE; return ir;}

  int numGroup = pMeshGroup->GetNumMeshes();
  if( numGroup != pPointSet->GetPointSetDesc()->m_numMaterials )
  { ir = IFX_FALSE; return ir; }

  // checking the sum of number of lines in meshgroup
  // it should be equal to number of lines at author line set
  IFXMesh* pMesh = NULL;
  U32 numPoints = 0;
  for ( j = 0; j < numGroup; j++ ){

    pMeshGroup->GetMesh(j,pMesh);
    int n = pMesh->GetNumVertices();
    IFXTRACE_GENERIC(L"[PointSet Test] Mesh Group %i Positions %i\n",
        j, n);

    IFXInterleavedData* pData = 0;
    IFXVector3Iter vPosIter;
    IFXVector3Iter vNormIter;
    IFXU32Iter vColorIter;

    ir = pMesh->GetInterleavedVertexData(pData);
    U32 uVectorNum = 0;
    pData->GetVectorIter(uVectorNum++, vPosIter);
    IFXVector3* pVec = vPosIter.Get();

    pData->GetVectorIter(uVectorNum++, vNormIter);
    IFXVector3* pNorm = vNormIter.Get();

    pData->GetVectorIter(uVectorNum, vColorIter);
    U32* color = vColorIter.Get();

    int stride = vPosIter.GetStride();
    IFXTRACE_GENERIC(L"[Mesh] Strides:  Vert %d Norm %d Color %d\n",
      vPosIter.GetStride(), vNormIter.GetStride(), vColorIter.GetStride());

    for ( k= 0; k<n; k++) {
      IFXTRACE_GENERIC(L"[Point at Mesh] Vertex: X %f Y %f Z %f\n",
      pVec->X(), pVec->Y(), pVec->Z());
      IFXTRACE_GENERIC(L"[Normal at Point] Vertex: X %f Y %f Z %f\n",
      pNorm->X(), pNorm->Y(), pNorm->Z());

      IFXTRACE_GENERIC(L"[Diff Color at Point] %x\n", *color);

      pVec  = vPosIter.Next();
      pNorm = vNormIter.Next();
      color = vColorIter.Next();
    }

    numPoints = numPoints + n;
    pData->Release();
    pMesh->Release();
  }

  IFXTRACE_GENERIC(L"[PointSet Test] Author Point Set: Points %i \n",
      pPointSet->GetPointSetDesc()->m_numPositions);
  if (numPoints != pPointSet->GetPointSetDesc()->m_numPositions) {
    ir = IFX_FALSE;
    return ir;
  }

  return ir;
}

float distance(IFXVector3 &pnt1, IFXVector3 &pnt2 ) {

  float dist =0.0f;

  dist = sqrt(  ( pnt1.X() - pnt2.X() )*( pnt1.X() - pnt2.X() ) +
          ( pnt1.Y() - pnt2.Y() )*( pnt1.Y() - pnt2.Y() ) +
          ( pnt1.Z() - pnt2.Z() )*( pnt1.Z() - pnt2.Z() )
        );

  return dist;
};


IFXRESULT InitAuthorPointSet(IFXAuthorPointSet* pPointSet)
{

#define NUMPOINTS       18
#define NUMPOS          18
#define NUMNORM         18
#define NUMDIFFCOLORS   5
#define NUMSPECCOLORS   5
#define NUMTEXCOORD     0
#define NUMMATERIAL     2

#define NUMTEXTURES     0
#define NUMSHADING      1

#define START_TEX_COORD  0.1f
#define SHIFT_TEX_COORD  0.13f

#define EPS 1.E-5

  IFXRESULT ir = IFX_OK;

  if (!pPointSet) return IFX_E_INVALID_POINTER;

  IFXAuthorPointSetDesc desc;

    desc.m_numPoints     = NUMPOINTS;
    desc.m_numPositions    = NUMPOS;
    desc.m_numNormals        = NUMNORM;
    desc.m_numDiffuseColors  = NUMDIFFCOLORS;
    desc.m_numSpecularColors = NUMSPECCOLORS;
    desc.m_numTexCoords    = NUMTEXCOORD;
    desc.m_numMaterials    = NUMMATERIAL;


    pPointSet->Allocate(&desc);
    pPointSet->SetPointSetDesc(&desc);
    U32 i;
    int num = 0;
    int numpos = pPointSet->GetMaxPointSetDesc()->m_numPositions;


    // 1 - POSITIONS
    // fill Points as square polyPoints
  IFXVector3  buff;
    IFXVector3* pVector = &buff;
  float a = 2.0f;
  pVector->X() = 0.0f + a;  pVector->Y() = 0.0f + a;  pVector->Z() = a;  pPointSet->SetPosition(0,pVector);
    pVector->X() = 0.0f + a;  pVector->Y() = 0.0f + a;  pVector->Z() = a;  pPointSet->SetPosition(0,pVector);
    pVector->X() = 10.0f + a; pVector->Y() = 0.0f + a;  pVector->Z() = a;    pPointSet->SetPosition(1,pVector);
    pVector->X() = 10.0f + a; pVector->Y() = -10.0f + a;  pVector->Z() = a;  pPointSet->SetPosition(2,pVector);
    pVector->X() = 0.0f + a;  pVector->Y() = -10.0f + a;  pVector->Z() = a;  pPointSet->SetPosition(3,pVector);
    pVector->X() = -10.0f + a;  pVector->Y() = -10.0f + a;  pVector->Z() = a ;  pPointSet->SetPosition(4,pVector);
    pVector->X() = -10.0f + a;  pVector->Y() = 0.0f + a;  pVector->Z() =  a;  pPointSet->SetPosition(5,pVector);
    pVector->X() = -10.0f + a;  pVector->Y() = 10.0f + a; pVector->Z() = a;  pPointSet->SetPosition(6,pVector);
    pVector->X() = 0.0f + a;  pVector->Y() = 10.0f + a; pVector->Z() = a;  pPointSet->SetPosition(7,pVector);
    pVector->X() = 10.0f + a; pVector->Y() = 10.0f + a; pVector->Z() = a;  pPointSet->SetPosition(8,pVector);
    float b = 13.3f;
    pVector->X() = 0.0f + b ; pVector->Y() = 0.0f+ b;   pVector->Z() = 10.0f + a;   pPointSet->SetPosition(9,pVector);
    pVector->X() = 10.0f + b; pVector->Y() = 0.0f+ b;   pVector->Z() = 10.0f + a;   pPointSet->SetPosition(10,pVector);
    pVector->X() = 10.0f + b; pVector->Y() = -10.0f+ b; pVector->Z() = 10.0f + a; pPointSet->SetPosition(11,pVector);
    pVector->X() = 0.0f + b;  pVector->Y() = -10.0f+ b; pVector->Z() = 10.0f + a; pPointSet->SetPosition(12,pVector);
    pVector->X() = -10.0f + b;  pVector->Y() = -10.0f+ b; pVector->Z() = 10.0f + a; pPointSet->SetPosition(13,pVector);
    pVector->X() = -10.0f + b;  pVector->Y() = 0.0f+ b;   pVector->Z() = 10.0f + a; pPointSet->SetPosition(14,pVector);
    pVector->X() = -10.0f + b;  pVector->Y() = 10.0f+ b;  pVector->Z() = 10.0f + a; pPointSet->SetPosition(15,pVector);
    pVector->X() = 0.0f + b;  pVector->Y() = 10.0f+ b;  pVector->Z() = 10.0f + a; pPointSet->SetPosition(16,pVector);
    pVector->X() = 10.0f + b; pVector->Y() = 10.0f+ b;  pVector->Z() = 10.0f + a; pPointSet->SetPosition(17,pVector);

    // 1.A - points
    U32 point;
    point  = 0; pPointSet->SetPositionPoint(0, &point);
    point  = 1; pPointSet->SetPositionPoint(1, &point);
    point  = 2; pPointSet->SetPositionPoint(2, &point);
    point  = 3; pPointSet->SetPositionPoint(3, &point);
    point  = 4; pPointSet->SetPositionPoint(4, &point);
    point  = 5; pPointSet->SetPositionPoint(5, &point);
    point  = 6; pPointSet->SetPositionPoint(6, &point);
    point  = 7; pPointSet->SetPositionPoint(7, &point);
    point  = 8; pPointSet->SetPositionPoint(8, &point);
    point  = 9; pPointSet->SetPositionPoint(9, &point);
    point  = 10; pPointSet->SetPositionPoint(10, &point);
    point  = 11; pPointSet->SetPositionPoint(11, &point);
    point  = 12; pPointSet->SetPositionPoint(12, &point);
    point  = 13; pPointSet->SetPositionPoint(13, &point);
    point  = 14; pPointSet->SetPositionPoint(14, &point);
    point  = 15; pPointSet->SetPositionPoint(15, &point);
    point  = 16; pPointSet->SetPositionPoint(16, &point);
    point  = 17; pPointSet->SetPositionPoint(17, &point);

 //   // 3 - DIFFUSE
 //   U32 diff_color = 0x000000FF;
 //   for (i=0; i < pPointSet->GetPointSetDesc()->m_numDiffuseColors; i++) {
 //     pPointSet->SetDiffuseColor( i, &diff_color );
 //     //diff_color = diff_color<<8;
 //   }


    // 2 - NORMALS
  pVector->X() = 0.0f;  pVector->Y() = 0.5f;  pVector->Z() = 0.5f;  pPointSet->SetNormal(0,pVector);
  pVector->X() = 0.0f;  pVector->Y() = 0.2f;  pVector->Z() = 0.8f;  pPointSet->SetNormal(1,pVector);
  pVector->X() = 0.0f;  pVector->Y() = 0.01f; pVector->Z() = 0.99f; pPointSet->SetNormal(2,pVector);
  pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;  pPointSet->SetNormal(3,pVector);
  pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;  pPointSet->SetNormal(4,pVector);
  pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;  pPointSet->SetNormal(5,pVector);
  pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pPointSet->SetNormal(6,pVector);
  pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pPointSet->SetNormal(7,pVector);
  pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pPointSet->SetNormal(8,pVector);
  pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pPointSet->SetNormal(9,pVector);
  pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pPointSet->SetNormal(10,pVector);
  pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pPointSet->SetNormal(11,pVector);
  pVector->X() = 0.0f;  pVector->Y() = 0.5f;  pVector->Z() = 0.5f;  pPointSet->SetNormal(12,pVector);
  pVector->X() = 0.0f;  pVector->Y() = 0.2f;  pVector->Z() = 0.8f;  pPointSet->SetNormal(13,pVector);
  pVector->X() = 0.0f;  pVector->Y() = 0.01f; pVector->Z() = 0.99f; pPointSet->SetNormal(14,pVector);
  pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;  pPointSet->SetNormal(15,pVector);
  pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;  pPointSet->SetNormal(16,pVector);
  pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;  pPointSet->SetNormal(17,pVector);

    U32 p = 1;
    for (i=0; i< pPointSet->GetPointSetDesc()->m_numPoints; i++) {
      pPointSet->SetNormalPoint(i,&p);
    }


    // 3 - DIFFUSE
  IFXVector4 diff_color( 0.0f, 0.0f, 0.0f, 1.0f );
    for (i=0; i < pPointSet->GetPointSetDesc()->m_numDiffuseColors; i++) {
      pPointSet->SetDiffuseColor( i, &diff_color );
    }
    //  DIFFUSE POINTS
    p = 0;  // for simplicity
    for (i=0; i < pPointSet->GetPointSetDesc()->m_numPoints; i++) {
      pPointSet->SetDiffusePoint(i, &p);
    }

    // 4 - SPECULAR
  IFXVector4 spec_color( 0.0f, 1.0f, 0.0f, 1.0f );
    for (i=0; i < pPointSet->GetPointSetDesc()->m_numSpecularColors; i++) {
      pPointSet->SetSpecularColor( i, &spec_color );
    }

    //  SPECULAR LINE
    p = 0;  // for simplicity
    for (i=0; i < pPointSet->GetPointSetDesc()->m_numPoints; i++) {
      pPointSet->SetSpecularPoint(i, &p);
    }

    // 5 - TEXTURECOORDS
    F32 tcoord;
    for (i=0; i < pPointSet->GetPointSetDesc()->m_numTexCoords; i++) {
      tcoord = (float)START_TEX_COORD;
      pPointSet->SetTexCoord( i, new IFXVector4( tcoord+i, tcoord+1+i, tcoord+2+i, tcoord+3+i ) );
    }

        // 6 - MATERIALS
    IFXAuthorMaterial material;
      material.m_uNumTextureLayers  = 0;
      material.m_uOriginalMaterialID  = 1;
      material.m_uDiffuseColors   = TRUE;
      material.m_uSpecularColors    = FALSE;
      material.m_uNormals       = TRUE;
    IFXAuthorMaterial material_1;
      material_1.m_uNumTextureLayers  = 0;
      material_1.m_uOriginalMaterialID = 2;
      material_1.m_uDiffuseColors   = FALSE;
      material_1.m_uSpecularColors  = TRUE;
      material_1.m_uNormals     = TRUE;

    for (i=0; i<material.m_uNumTextureLayers; i++){
      material.m_uTexCoordDimensions[i] = 0;
    }

    for (i=0; i < pPointSet->GetPointSetDesc()->m_numMaterials; i++) {

      if( i==0 )
        pPointSet->SetMaterial( i, &material );
      else
        pPointSet->SetMaterial( i, &material_1 );

    }

    // 7 - Point MATERIALS
  for (i=0; i < pPointSet->GetPointSetDesc()->m_numPoints; i++) {

    pPointSet->SetPointMaterial(i,0);

    }

  return ir;
}// InitAuthorPointSet()
