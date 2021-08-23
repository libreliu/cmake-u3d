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


#ifndef SAMPLE_LINESET_H
#include "SampleLineSet.h"
#endif

#include "IFXAuthorLineSet.h"
#include "IFXAuthorLineSetResource.h"
#include "LineSetInitialize.h"


IFXRESULT SampleLineSet::Run( U3DSceneUtilities* pSGUtils )
{
	SGNodeHeader nh;

	IFXDECLARELOCAL(IFXLight,pLight);
	IFXDECLARELOCAL(IFXModifier, pGenerator);
	IFXDECLARELOCAL(IFXShaderLitTexture, pShader);
	IFXPalette* pShaderPalette = NULL;

	U32           uResourceID = 0;
	U32           uCloneID  = 0;

	IFXVector3        translateVector(10.0f, 10.0f, 10.0f);
	struct SGNodeHeader   nodeHeader;
	U32           uShaderID = 0;
	IFXVector4 colorWhite(1.0f, 1.0f, 1.0f);

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
	IFXDECLARELOCAL( IFXLightResource, pLightResource);
	// create a light resource and add the light resource to the palette
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
	}

	TEST(IFXSUCCESS( pLight->SetLightResourceID(uLightResourceId) ), IFXException);
	pLightResource->SetType(IFXLightResource::DIRECTIONAL);
	pLightResource->SetColor(colorWhite);

	// --- Create a LineSet primitive to indicate the picking location
	TEST(IFXSUCCESS(CreatePrimitive("PickLineSet", CID_IFXAuthorLineSetResource, 0,
		&uResourceID, &pGenerator, pSGUtils->m_pGeneratorPalette, pSGUtils->m_pSceneGraph) ), IFXException);

	// Create a model node in the node palette for the sphere
	wcscpy(nodeHeader.Name, L"PickLineSet");
	wcscpy(nodeHeader.ParentName, PALETTE_DEFAULT_GROUP);
	nodeHeader.NodeTm.Reset();
	nodeHeader.NodeTm.Translate3x4(IFXVector3(0, 0, -400));

	// Create Author LineSet
	IFXDECLARELOCAL( IFXAuthorLineSet,pAuthorLS);
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXAuthorLineSet, IID_IFXAuthorLineSet,(void**)&pAuthorLS)),IFXException);
	IFXASSERT(pAuthorLS);

	IFXDECLARELOCAL( IFXAuthorLineSetResource, pLineSetGenerator);

	// 
	//TEST(IFXSUCCESS(InitAuthorLineSet(pAuthorLS)),IFXException);
	//TEST(IFXSUCCESS(Init_simplest_AuthorLineSet(pAuthorLS)),IFXException);
	//TEST(IFXSUCCESS(InitGridAuthorLineSet(pAuthorLS)),IFXException);

	TEST(IFXSUCCESS(InitGrid256AuthorLineSet(pAuthorLS)),IFXException);

	TEST(IFXSUCCESS(pGenerator->QueryInterface( IID_IFXAuthorLineSetResource, (void**)&pLineSetGenerator )), IFXException);
	TEST(IFXSUCCESS(pLineSetGenerator->SetAuthorLineSet(pAuthorLS)), IFXException);

	TEST(IFXSUCCESS(pSGUtils->CreateModelNode(&nodeHeader, uResourceID)), IFXException);

	IFXVector4 Diffuse = IFXVector4(1,(F32)0.1,(F32)0.8);
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

	// Access the shader and turn on line rendering for the default
	// shader so we can see the bounding sphere.
	IFXDECLARELOCAL( IFXShaderLitTexture, pShaderLitText);
	// Get the shader palette
	pShaderPalette = pSGUtils->m_pShaderPalette;
	IFXRESULT GetResourcePtr(U32 uIndex, IFXREFIID interfaceId, void** ppObject);
	pShaderPalette->GetResourcePtr(0, IID_IFXShaderLitTexture, (void **)&pShaderLitText);
	pShaderLitText->SetChannels(0);

	TEST(pShaderPalette != NULL, IFXException);

	// Get the Shader on this model
	TEST(IFXSUCCESS(pShaderPalette->Find(L"", &uShaderID)), IFXException);
	TEST(IFXSUCCESS(pShaderPalette->GetResourcePtr(uShaderID, IID_IFXShaderLitTexture, (void **)&pShader)), IFXException);
	pShader->SetDrawLines(TRUE);
	pShader->SetLightingEnabled(TRUE);

	IFXDECLARELOCAL( IFXMeshGroup, pMeshGroup);
	TEST(IFXSUCCESS(pLineSetGenerator->GetMeshGroup(&pMeshGroup)), IFXException);

	TEST(IFXSUCCESS(VerifyRenderableData(pAuthorLS, pMeshGroup)), IFXException);

	pAuthorLS->Deallocate();

	return IFX_OK;
}


IFXRESULT VerifyRenderableData(IFXAuthorLineSet* pLineSet, IFXMeshGroup* pMeshGroup)
{
	IFXRESULT ir  = IFX_OK;
	int j,k;

	if( !pMeshGroup || !pLineSet){ ir = IFX_FALSE; return ir;}

	int numGroup = pMeshGroup->GetNumMeshes();
	if( numGroup != pLineSet->GetLineSetDesc()->m_numMaterials )
	{ ir = IFX_FALSE; return ir; }

	// checking the sum of number of lines in meshgroup
	// it should be equal to number of lines at author line set
	IFXMesh* pMesh = NULL;
	U32 numLinesLS  = 0;

	IFXTRACE_GENERIC(L"[LineSet Test] AuthorLineSet with %i lines, %i positons, %i normales, %i materials \n", 
		pLineSet->GetLineSetDesc()->m_numLines,
		pLineSet->GetLineSetDesc()->m_numPositions,
		pLineSet->GetLineSetDesc()->m_numNormals,
		pLineSet->GetLineSetDesc()->m_numMaterials);


	IFXTRACE_GENERIC(L"[LineSet Test] Mesh Group with %i meshes\n",	numGroup);

	IFXVector3 vec, norm;
	U32  col;
	IFXVector3* pVec  = &vec;
	IFXVector3* pNorm = &norm;
	U32* color = &col;
	for ( j = 0; j < numGroup; j++ ) {

		pMeshGroup->GetMesh(j,pMesh);
		int n = pMesh->GetNumLines();
		IFXTRACE_GENERIC(L"[LineSet Test] Mesh at Group %i Lines %i Positions %i\n",
			j, n, pMesh->GetNumVertices());

		IFXInterleavedData* pData = 0;
		IFXVector3Iter vPosIter;
		IFXVector3Iter vNormIter;
		IFXU32Iter     vColorIter;

		ir = pMesh->GetInterleavedVertexData(pData);
		IFXASSERT( ir == IFX_OK);
		U32 uVectorNum = 0;
		pData->GetVectorIter(uVectorNum++, vPosIter);
		pData->GetVectorIter(uVectorNum++, vNormIter);
		pData->GetVectorIter(uVectorNum, vColorIter);

		int stride = vPosIter.GetStride();
		IFXTRACE_GENERIC(L"[Mesh] Strides:  Vert %d Norm %d Color %d\n",
			vPosIter.GetStride(), vNormIter.GetStride(), vColorIter.GetStride());
			
		U32 NumVertices = pMesh->GetNumVertices();
		for ( U32 k= 0; k < NumVertices; k++) {

			pVec  = vPosIter.Get();
			pNorm = vNormIter.Get();
			color = vColorIter.Get();

			IFXTRACE_GENERIC(L"[Point at Mesh] i %d at %x Vertex: X %f Y %f Z %f\n",
				k, pVec, pVec->X(), pVec->Y(), pVec->Z());
			IFXTRACE_GENERIC(L"[Normal at Point] at %x Vertex: X %f Y %f Z %f\n",
				pNorm, pNorm->X(), pNorm->Y(), pNorm->Z());

			IFXTRACE_GENERIC(L"[Diff Color at Point] %x\n", *color);

			pVec  = vPosIter.Next();
			pNorm = vNormIter.Next();
			color = vColorIter.Next();
		}

		numLinesLS = numLinesLS + n;

		pData->Release();
		pMesh->Release();
	}

	IFXTRACE_GENERIC(L"[LineSet Test] Author Line Set: Lines %i \n",
		pLineSet->GetLineSetDesc()->m_numLines);
	if (numLinesLS != pLineSet->GetLineSetDesc()->m_numLines) {
		ir = IFX_FALSE;
		return ir;
	}

	// AuthorLineSet
	float dist  = 0.0f;
	float dist2 = 0.0f;
	IFXVector3 point1, point2;
	for( j = 0; j < (int)numLinesLS; j++ ) {

		IFXU32Line Line;
		ir = pLineSet->GetPositionLine(j,&Line);
		IFXASSERT(ir==IFX_OK);

		ir = pLineSet->GetPosition(Line.VertexA(), &point1);
		IFXASSERT(ir==IFX_OK);
		ir = pLineSet->GetPosition(Line.VertexB(), &point2);
		IFXASSERT(ir==IFX_OK);
		dist = dist + distance(point1, point2);
	}

	//  Renderable distance
	IFXVector3 v1, v2;
	IFXVector3 *pv1 = &v1;
	IFXVector3 *pv2 = &v2;
	for ( j = 0; j < numGroup; j++ ){

		pMeshGroup->GetMesh(j,pMesh);
		numLinesLS = pMesh->GetNumLines();

		IFXLineIter lineIter;
		ir = pMesh->GetLineIter(lineIter);
		IFXASSERT( ir == IFX_OK );
		IFXVector3Iter vertIter;
		IFXVector3Iter normIter;
		ir = pMesh->GetPositionIter(vertIter);
		IFXASSERT(ir==IFX_OK);
		ir = pMesh->GetNormalIter(normIter);
		IFXASSERT(ir==IFX_OK);
		IFXTRACE_GENERIC(L"[LineSet Test] Mesh %i\n", j);

		for( k = 0; k< (int)numLinesLS; k++ ) {

			IFXLine* pLine = lineIter.Index(k);
			pv1 = vertIter.Index(pLine->VertexA());
			pv2 = vertIter.Index(pLine->VertexB());
			dist2 = dist2 + distance(*pv1, *pv2);
			IFXTRACE_GENERIC(L"[LineSet Test] Line Positions %i  %i\n",
				pLine->VertexA(), pLine->VertexB());
			IFXTRACE_GENERIC(L"[LineSet Test] Position A: %f  %f  %f\n",
				pv1->X(), pv1->Y(), pv1->Z());
			IFXTRACE_GENERIC(L"[LineSet Test] Position B: %f  %f  %f\n",
				pv2->X(), pv2->Y(), pv2->Z());

			// normals
			pv1 = normIter.Index(pLine->VertexA());
			pv2 = normIter.Index(pLine->VertexB());
			IFXTRACE_GENERIC(L"[LineSet Test] Line Normals %i  %i\n",
				pLine->VertexA(), pLine->VertexB());
			IFXTRACE_GENERIC(L"[LineSet Test] Normal A: %f  %f  %f\n",
				pv1->X(), pv1->Y(), pv1->Z());
			IFXTRACE_GENERIC(L"[LineSet Test] Normal B: %f  %f  %f\n",
				pv2->X(), pv2->Y(), pv2->Z());

		}// k

		pMesh->Release();
	}// j

	IFXTRACE_GENERIC(L"[LineSet Test Distances] At AuthorLineSet %f; At Renderable LineSet (MeshGroup)  %f \n",
		dist, dist2);

	if ( fabs(dist - dist2 ) >= 1E-5){
		ir = IFX_FALSE;
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


