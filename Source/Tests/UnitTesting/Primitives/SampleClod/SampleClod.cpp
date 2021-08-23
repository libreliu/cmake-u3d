//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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

#ifndef SAMPLECLOD_H
#include "SampleClod.h"
#endif

#include "IFXAuthorCLODMesh.h"
#include "IFXAuthorCLODResource.h"
#include "IFXModifierChain.h"

#include "ClodInitialize.h"

IFXRESULT VerifyRenderableData(IFXAuthorCLODMesh* pAuthorClod, IFXMeshGroup* pMeshGroup);


IFXRESULT SampleClod::Run( U3DSceneUtilities* pSGUtils )
{
	SGNodeHeader   nodeHeader;
	SGNodeHeader nh;
	U32 uResourceID = 0;
	U32 uShaderID = 0;

	TEST(IFXSUCCESS(pSGUtils->InitializeScene()), IFXException);

	// Create camera
	{
		IFXView* pViewNode = NULL;

		wcscpy(nh.Name, L"Camera1");
		wcscpy(nh.ParentName, PALETTE_DEFAULT_GROUP);
		nh.NodeTm.Reset();
		TEST(IFXSUCCESS(pSGUtils->CreateCameraNode(&nh, &pViewNode)), IFXException);
		// Set up the camera based on its type
		pViewNode->SetAttributes( 0 );
		TEST(IFXSUCCESS(pViewNode->SetProjection(5.0)), IFXException);

		IFXRELEASE(pViewNode);
	}

	// Create light
	{
		IFXDECLARELOCAL( IFXPalette, pLightResourcePalette );
		IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );
		IFXLight *pLight = NULL;
		IFXVector4 colorWhite(1.0f, 1.0f, 1.0f);

		wcscpy(nh.Name, L"Light1");
		wcscpy(nh.ParentName, PALETTE_DEFAULT_GROUP);

		// Create a light node with this name
		TEST(IFXSUCCESS(pSGUtils->CreateLightNode(&nh, &pLight)), IFXException);

		// Get the light resource palette
		TEST(IFXSUCCESS(pSGUtils->m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph )), IFXException);
		TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::LIGHT, &pLightResourcePalette )), IFXException);
		U32 uLightResourceId;

		// Look for the light resource in the light resource palette
		IFXLightResource* pLightResource = NULL;
		// create a light resource and add the light resource to the palette
		IFXRESULT rc = pLightResourcePalette->Add( nh.Name, &uLightResourceId );
		if (IFXSUCCESS(rc))
		{
				IFXDECLARELOCAL( IFXUnknown, pUnk );
				IFXDECLARELOCAL( IFXMarker, pMarker );

				TEST(IFXSUCCESS( IFXCreateComponent( CID_IFXLightResource, IID_IFXLightResource, (void**)&pLightResource ) ), IFXException);
				TEST(IFXSUCCESS( pLightResource->QueryInterface( IID_IFXUnknown, (void**)&pUnk ) ), IFXException);

				TEST(IFXSUCCESS(pUnk->QueryInterface( IID_IFXMarker, (void**)&pMarker )), IFXException);
				TEST(IFXSUCCESS(pMarker->SetSceneGraph( pSceneGraph )), IFXException);
				TEST(IFXSUCCESS( pLightResourcePalette->SetResourcePtr( uLightResourceId, pUnk ) ), IFXException);
		}

		// Link the light node to the light resource pallete entry
		TEST(IFXSUCCESS( pLight->SetLightResourceID(uLightResourceId) ), IFXException);

		// Ambient light
		pLightResource->SetType(IFXLightResource::DIRECTIONAL);
		pLightResource->SetColor(colorWhite);

		IFXRELEASE(pLight);
		IFXRELEASE(pLightResource);
	}

	//
	// --- Create a ClodMesh primitive to indicate the picking location
	{
		IFXModifier     *pGenerator = NULL;
		IFXAuthorCLODResource  *pMeshGenerator = NULL;
		IFXAuthorCLODMesh* pAuthorMesh = NULL;
		IFXMeshGroup* pMeshGroup = NULL;

		TEST(IFXSUCCESS(CreatePrimitive("PickClodMesh", CID_IFXAuthorCLODResource, 0,
			&uResourceID, &pGenerator, pSGUtils->m_pGeneratorPalette, pSGUtils->m_pSceneGraph) ), IFXException);

		// Create a model node in the node palette for the sphere
		wcscpy(nodeHeader.Name, L"PickClodMesh");
		wcscpy(nodeHeader.ParentName, PALETTE_DEFAULT_GROUP);
		nodeHeader.NodeTm.Reset();
		nodeHeader.NodeTm.Translate3x4(IFXVector3(0, -5, -200));

		// Create Author Mesh
		TEST( IFXSUCCESS(IFXCreateComponent(CID_IFXAuthorMesh, IID_IFXAuthorCLODMesh,(void**)&pAuthorMesh)),IFXException);
		IFXASSERT(pAuthorMesh);

		// simplest vesrsion
		// TEST(IFXSUCCESS(Init_Simplest_AuthorClod(pAuthorMesh)),IFXException);
		//advanced version
		TEST(IFXSUCCESS(InitAuthorClod(pAuthorMesh)),IFXException);

		//
		TEST(IFXSUCCESS(pGenerator->QueryInterface( IID_IFXAuthorCLODResource, (void**)&pMeshGenerator )), IFXException);
		TEST(IFXSUCCESS(pMeshGenerator->SetAuthorMesh(pAuthorMesh)), IFXException);

		TEST(IFXSUCCESS(pMeshGenerator->GetMeshGroup(&pMeshGroup)), IFXException);

		TEST(IFXSUCCESS(VerifyRenderableData(pAuthorMesh, pMeshGroup)), IFXException);

		IFXRELEASE(pMeshGroup);
		IFXRELEASE(pAuthorMesh);
		IFXRELEASE(pGenerator);
		IFXRELEASE(pMeshGenerator);
	}

	TEST(IFXSUCCESS(pSGUtils->CreateModelNode(&nodeHeader, uResourceID)), IFXException);

	IFXVector4 Diffuse = IFXVector4(1,(F32)0.1,(F32)0.8);
	//Set material resource

	//create new material
	U32 idx = 1;
	IFXDECLARELOCAL( IFXMaterialResource, pMResource );
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXMaterialResource, IID_IFXMaterialResource, (void**)&pMResource)), IFXException);
	IFXVector4 color(1,(F32).1,(F32)0.8);
	pMResource->SetOpacity(0.5f);
	pMResource->SetReflectivity( 0.0f );
	pMResource->SetTransparent(FALSE);
	pMResource->SetDiffuse(color);
	pMResource->SetSceneGraph( pSGUtils->m_pSceneGraph );
	pSGUtils->m_pMaterialPalette->Add( L"NewMaterial", &idx );
	pSGUtils->m_pMaterialPalette->SetResourcePtr( idx, pMResource );

	//create new shader lit texture
	IFXDECLARELOCAL( IFXShaderLitTexture, pShaderLitTexture );
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderLitTexture, IID_IFXShaderLitTexture, (void**)&pShaderLitTexture)), IFXException);
	TEST( IFXSUCCESS( pShaderLitTexture->SetMaterialID( idx )), IFXException);
	pShaderLitTexture->SetFlags( IFXShaderLitTexture::MATERIAL );
	pShaderLitTexture->SetSceneGraph( pSGUtils->m_pSceneGraph );

	//add it to the shader palette
	U32 idxsh = 1;
	TEST(IFXSUCCESS(pSGUtils->m_pShaderPalette->Add(L"NewShader", &idxsh)), IFXException);
	TEST(IFXSUCCESS(pSGUtils->m_pShaderPalette->SetResourcePtr( idxsh, pShaderLitTexture )), IFXException);

	//create shader set
	IFXDECLARELOCAL( IFXShaderList, pShaderList );
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderList, IID_IFXShaderList, (void**)&pShaderList)), IFXException);
	pShaderList->Allocate(1);
	pShaderList->SetShader(0,1);

	// Create a shader modifier
	IFXDECLARELOCAL( IFXShadingModifier, pShadingModifier );

	TEST( IFXSUCCESS( IFXCreateComponent( CID_IFXShadingModifier, IID_IFXShadingModifier,
		(void**)&pShadingModifier ) ), IFXException);
	pShadingModifier->SetSceneGraph( pSGUtils->m_pSceneGraph );
	TEST(IFXSUCCESS(pShadingModifier->SetElementShaderList( 0, pShaderList )), IFXException);

	// Add it to the end of the modifier chain associated with the IFXModel
	IFXDECLARELOCAL( IFXModifierChain, pModChain );
	IFXDECLARELOCAL( IFXModel, pModel );

	TEST( IFXSUCCESS( pSGUtils->GetModelFromName(nodeHeader.Name, &pModel) ), IFXException);
	TEST( IFXSUCCESS( pModel->GetModifierChain(&pModChain) ), IFXException );
	TEST( IFXSUCCESS( pModChain->AddModifier( *pShadingModifier ) ), IFXException );

	return IFX_OK;
}

IFXRESULT VerifyRenderableData(IFXAuthorCLODMesh* pAuthorClod, IFXMeshGroup* pMeshGroup)
{
  IFXRESULT ir = IFX_OK;
  U32 j,k;

  if( !pMeshGroup || !pAuthorClod )
  { 
	  ir = IFX_FALSE; 
	  return ir;
  }

  U32 numGroup = pMeshGroup->GetNumMeshes();
  //if( numGroup != pLineSet->GetLineSetDesc()->m_uNumMaterials )
  //  { ir = IFX_FALSE; return ir; }

  // checking the sum of number of Faces in meshgroup
  // it should be equal to number of lines at author line set
  U32 numFacesLS  = 0;

  IFXTRACE_GENERIC(L"[AuthorCLOD Test] AuthorClodMesh with %i faces, %i positons, %i normales, %i materials \n",
	  pAuthorClod->GetMeshDesc()->NumFaces,
	  pAuthorClod->GetMeshDesc()->NumPositions,
	  pAuthorClod->GetMeshDesc()->NumNormals,
	  pAuthorClod->GetMeshDesc()->NumMaterials);

  IFXTRACE_GENERIC(L"[AuthorCLOD Test] Mesh Group with %i meshes\n",
            numGroup);
  for ( j = 0; j < numGroup; j++ )
  {
	IFXMesh* pMesh = NULL;
    IFXInterleavedData* pData = NULL;
    IFXVector3Iter vPosIter;
    IFXVector3Iter vNormIter;
    IFXU32Iter vColorIter;

    pMeshGroup->GetMesh(j,pMesh);

    U32 n = pMesh->GetNumFaces();
    IFXTRACE_GENERIC(L"[AuthorCLOD Test] Mesh at Group %i Faces %i Positions %i\n",
            j, n, pMesh->GetNumVertices());

    ir = pMesh->GetInterleavedVertexData(pData);
    U32 uVectorNum = 0;
    pData->GetVectorIter(uVectorNum++, vPosIter);
    pData->GetVectorIter(uVectorNum++, vNormIter);
    pData->GetVectorIter(uVectorNum, vColorIter);

    U32 stride = vPosIter.GetStride();
    IFXTRACE_GENERIC(L"[Mesh] Strides:  Vert %d Norm %d Color %d\n",
        vPosIter.GetStride(), vNormIter.GetStride(), vColorIter.GetStride());

    for ( k = 0; k < pMesh->GetNumVertices(); k++) 
	{
        IFXVector3* pVec = vPosIter.Get();
		IFXVector3* pNorm = vNormIter.Get();
		U32* color = vColorIter.Get();

		IFXTRACE_GENERIC(L"[Point at Mesh] i %d at %x Vertex: X %f Y %f Z %f\n",
			k, pVec, pVec->X(), pVec->Y(), pVec->Z());

		IFXTRACE_GENERIC(L"[Normal at Point] at %x Vertex: X %f Y %f Z %f\n",
			pNorm, pNorm->X(), pNorm->Y(), pNorm->Z());

		IFXTRACE_GENERIC(L"[Diff Color at Point] %x\n", *color);

		pVec  = vPosIter.Next();
		pNorm = vNormIter.Next();
		color = vColorIter.Next();
    }

    numFacesLS = numFacesLS + n;

	IFXRELEASE( pMesh );
	IFXRELEASE( pData );
  }

  IFXTRACE_GENERIC(L"[AuthorCLOD Test] AuthorClodMesh:Faces %i\n",
	  pAuthorClod->GetMeshDesc()->NumFaces );

  if (numFacesLS !=  pAuthorClod->GetMeshDesc()->NumFaces) 
  {
    ir = IFX_FALSE;
    return ir;
  }

  // AuthorLineSet
  float dist  = 0.0f;
  float dist2 = 0.0f;
  IFXVector3 point1, point2, point3;
  for( j = 0; j < (int)numFacesLS; j++ ) 
  {
	IFXAuthorFace Face;
	ir = pAuthorClod->GetPositionFace(j,&Face);
    IFXASSERT(ir==IFX_OK);

    ir = pAuthorClod->GetPosition(Face.VertexA(), &point1);
    IFXASSERT(ir==IFX_OK);
	ir = pAuthorClod->GetPosition(Face.VertexB(), &point2);
    IFXASSERT(ir==IFX_OK);
	ir = pAuthorClod->GetPosition(Face.VertexC(), &point3);
    IFXASSERT(ir==IFX_OK);
    dist = dist + distance(point1, point2);
	dist = dist + distance(point1, point3);
	dist = dist + distance(point3, point2);
  }

  //  Renderable distance
  for ( j = 0; j < numGroup; j++ )
  {
    IFXDECLARELOCAL( IFXMesh, pMesh );

    pMeshGroup->GetMesh(j,pMesh);
    numFacesLS = pMesh->GetNumFaces();

	IFXFaceIter faceIter;
    ir = pMesh->GetFaceIter(faceIter);
    IFXVector3Iter vertIter;
    IFXVector3Iter normIter;
    ir = pMesh->GetPositionIter(vertIter);
    IFXASSERT(ir==IFX_OK);
    ir = pMesh->GetNormalIter(normIter);
    IFXASSERT(ir==IFX_OK);
    IFXTRACE_GENERIC(L"[LineSet Test] Mesh %i\n", j);
    //IFX_MESH_DIFFUSE_COLOR

    //IFX_MESH_SPECULAR_COLOR

    for( k = 0; k< (int)numFacesLS; k++ ) {

      IFXFace* pFace = faceIter.Index(k);
      IFXVector3 *pv1 = vertIter.Index(pFace->VertexA());
      IFXVector3 *pv2 = vertIter.Index(pFace->VertexB());
	  IFXVector3 *pv3 = vertIter.Index(pFace->VertexC());
            dist2 = dist2 + distance(*pv1, *pv2);
			dist2 = dist2 + distance(*pv1, *pv3);
			dist2 = dist2 + distance(*pv3, *pv2);

      IFXTRACE_GENERIC(L"[AuthorCLOD Test] Face Positions %i  %i\n",
            pFace->VertexA(), pFace->VertexB(), pFace->VertexC());
      IFXTRACE_GENERIC(L"[AuthorCLOD Test] Position A: %f  %f  %f\n",
        pv1->X(), pv1->Y(), pv1->Z());
      IFXTRACE_GENERIC(L"[AuthorCLOD Test] Position B: %f  %f  %f\n",
        pv2->X(), pv2->Y(), pv2->Z());
	  IFXTRACE_GENERIC(L"[AuthorCLOD Test] Position C: %f  %f  %f\n",
        pv3->X(), pv3->Y(), pv3->Z());

      // normals
      pv1 = normIter.Index(pFace->VertexA());
      pv2 = normIter.Index(pFace->VertexB());
	  pv3 = normIter.Index(pFace->VertexC());
      IFXTRACE_GENERIC(L"[AuthorCLOD Test] Face Normals %i  %i %i\n",
            pFace->VertexA(), pFace->VertexB(), pFace->VertexC());

      IFXTRACE_GENERIC(L"[AuthorCLOD Test] Normal A: %f  %f  %f\n",
        pv1->X(), pv1->Y(), pv1->Z());

      IFXTRACE_GENERIC(L"[AuthorCLOD Test] Normal B: %f  %f  %f\n",
        pv2->X(), pv2->Y(), pv2->Z());

	  IFXTRACE_GENERIC(L"[AuthorCLOD Test] Normal B: %f  %f  %f\n",
        pv3->X(), pv3->Y(), pv3->Z());


    }// k
  }// j
  IFXTRACE_GENERIC(L"[AuthorCLOD Test Distances] At AuthorCLOD %f; At Renderable AuthorCLOD (MeshGroup)  %f \n",
					dist, dist2);

  if ( fabs(dist - dist2 ) >= 1E-5){
    ir = IFX_FALSE;
  }/**/
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
