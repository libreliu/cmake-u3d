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

#ifndef SAMPLETEXTUREOBJECT_H
#include "SampleTextureObject.h"
#endif

#include "IFXAuthorCLODMesh.h"
#include "IFXAuthorCLODResource.h"
#include "ClodInitialize.h"
#include "IFXModifierChain.h"

IFXRESULT VerifyRenderableData(IFXAuthorCLODMesh* pAuthorClod, IFXMeshGroup* pMeshGroup);


void SampleTextureObject::AddTexture( U3DSceneUtilities* pSGUtils, char* pName, U32* pBuffer, U32 layer, U32 idx )
{
	IFXDECLARELOCAL(IFXShaderLitTexture,pShaderLitTexture2);
	if( layer ){
		pSGUtils->m_pShaderPalette->GetResourcePtr( idx, IID_IFXShaderLitTexture, (void**)&pShaderLitTexture2 );
	}
	else {
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderLitTexture, IID_IFXShaderLitTexture, (void**)&pShaderLitTexture2)), IFXException);
	}
	pShaderLitTexture2->SetSceneGraph( pSGUtils->m_pSceneGraph );
	IFXDECLARELOCAL( IFXTextureObject, pTextureObject);
	IFXDECLARELOCAL( IFXUnknown, pUnknown );
	STextureSourceInfo sImageInfo;
	U32 textureId;
	IFXString textureName = pName;
	IFXRESULT result = IFX_OK;


	result = IFXCreateComponent( CID_IFXTextureObject, IID_IFXTextureObject,
		(void**)&pTextureObject );

	// Initialize the texture object
	if( pTextureObject && IFXSUCCESS(result) )
		result = pTextureObject->SetSceneGraph( pSGUtils->m_pSceneGraph );

	// Place the bitmap into the IFXTextureObject
	if( pTextureObject && IFXSUCCESS(result) )
	{

		// Populate the fields in STextureSourceInfo with the properties of
		// the texture
		sImageInfo.m_name = textureName;
		sImageInfo.m_height = 2;
		sImageInfo.m_width = 2;
		sImageInfo.m_size= (sImageInfo.m_height * sImageInfo.m_width * 3);
		sImageInfo.m_imageType = IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGB24;
		sImageInfo.m_pCodecCID = NULL;

		IFXRESULT result = IFX_OK;
		// Store the texture data and its properties in the pTextureObject
		result = pTextureObject->SetAnimated(FALSE);

		if(IFXSUCCESS(result))
			result = pTextureObject->SetRawImage( &sImageInfo, pBuffer );

		// tell the source not to throw away its source image
		if(IFXSUCCESS(result))
			result = pTextureObject->SetKeepCompressed( FALSE );

		// tell the source not to throw away its Raw image
		if(IFXSUCCESS(result))
			result = pTextureObject->SetKeepDecompressed( TRUE );

		if(IFXSUCCESS(result))
			result = pTextureObject->SetMagFilterMode( IFX_NEAREST );
	}

	// Get the IFXUnknown interface off of the IFXTextureObject
	if( pTextureObject && IFXSUCCESS(result) )
		result = pTextureObject->QueryInterface( IID_IFXUnknown, (void**)&pUnknown );

	// Add an entry for the texture to the texture palette
	if( pSGUtils->m_pTexturePalette && IFXSUCCESS(result) )
		result = pSGUtils->m_pTexturePalette->Add( textureName.Raw(), &textureId );

	// Point the texture palette entry to the texture object we created above.
	if( pSGUtils->m_pTexturePalette && IFXSUCCESS(result) )
		result = pSGUtils->m_pTexturePalette->SetResourcePtr( textureId, pUnknown );

	pShaderLitTexture2->SetChannels( layer+1 );
	pShaderLitTexture2->SetTextureID( layer, textureId );

	if( layer == 0 )
	{
		U32 idxsh = 0;
		IFXCHAR pLName[256];
		mbstowcs( pLName, pName, 255 );
		TEST(IFXSUCCESS(pSGUtils->m_pShaderPalette->Add(pLName, &idxsh)), IFXException);
		TEST(IFXSUCCESS(pSGUtils->m_pShaderPalette->SetResourcePtr( idxsh, pShaderLitTexture2 )), IFXException);
	}

}
IFXRESULT SampleTextureObject::Run( U3DSceneUtilities* pSGUtils )
{
	IFXDECLARELOCAL(IFXModifier, pPlane);
	U32 uID = 0;
	SGNodeHeader nh;
	IFXDECLARELOCAL(IFXView,pViewNode);
	IFXRenderClear renderClear;
	IFXDECLARELOCAL(IFXLight,pLight);
	IFXDECLARELOCAL(IFXModifier, pGenerator);
	U32           uResourceID = 0;
	U32           uCloneID  = 0;
	//IFXDECLARELOCAL(IFXShaderLitTexture,pSelectShader);
	IFXDECLARELOCAL(IFXMaterialResource, pSelectMaterial);
	IFXPalette *pNodePalette = NULL;
	IFXVector3        translateVector(10.0f, 10.0f, 10.0f);
	struct SGNodeHeader   nodeHeader;
	IFXPalette *pShaderPalette = NULL;
	
	U32           uShaderID = 0;
	IFXDECLARELOCAL(IFXShaderLitTexture,pShader);
	
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
	IFXDECLARELOCAL(IFXLightResource,pLightResource);
	
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
		//IFXRELEASE(pUnk);
	}
	//IFXRELEASE(pLightResource);

	// Link the light node to the light resource pallete entry
	TEST(IFXSUCCESS( pLight->SetLightResourceID(uLightResourceId) ), IFXException);
	// Ambient light
	pLight->GetLightResource()->SetType(IFXLightResource::DIRECTIONAL);
	pLight->GetLightResource()->SetColor(colorWhite);
	//IFXRELEASE(pLight);

	pNodePalette = pSGUtils->m_pNodePalette;
	TEST( pNodePalette != NULL, IFXException );

	TEST(IFXSUCCESS(CreatePrimitive("PickClodMesh", CID_IFXAuthorCLODResource, 0,
		&uResourceID, &pGenerator, pSGUtils->m_pGeneratorPalette, pSGUtils->m_pSceneGraph) ), IFXException);

	wcscpy(nodeHeader.Name, L"PickClodMesh");
	wcscpy(nodeHeader.ParentName, PALETTE_DEFAULT_GROUP);
	nodeHeader.NodeTm.Reset();
	nodeHeader.NodeTm.Translate3x4(IFXVector3(0, 0, -400));

	// Create Author Mesh
	IFXDECLARELOCAL(IFXAuthorCLODMesh, pAuthorMesh);
	TEST( IFXSUCCESS(IFXCreateComponent(CID_IFXAuthorMesh, IID_IFXAuthorCLODMesh,(void**)&pAuthorMesh)),IFXException);
	IFXASSERT(pAuthorMesh);

	IFXDECLARELOCAL(IFXAuthorCLODResource,pMeshGenerator );
	{
		//create new material
		IFXDECLARELOCAL(IFXMaterialResource, pMResource );
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXMaterialResource, IID_IFXMaterialResource, (void**)&pMResource)), IFXException);
		IFXVector4 color(1,(F32)1,(F32).3,1);
		pMResource->SetSpecular(color);
		pMResource->SetDiffuse(color);
		pMResource->SetSceneGraph( pSGUtils->m_pSceneGraph );
		U32 idx = 1;
		pSGUtils->m_pMaterialPalette->Add( L"NewMaterial", &idx );
		pSGUtils->m_pMaterialPalette->SetResourcePtr( idx, pMResource );
	}
	{
		//create new material
		IFXDECLARELOCAL(IFXMaterialResource, pMResource );
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXMaterialResource, IID_IFXMaterialResource, (void**)&pMResource)), IFXException);
		IFXVector4 color((F32).3,(F32)1,(F32)1,1);
		pMResource->SetSpecular(color);
		pMResource->SetDiffuse(color);
		pMResource->SetSceneGraph( pSGUtils->m_pSceneGraph );
		U32 idx = 2;
		pSGUtils->m_pMaterialPalette->Add( L"AnotherMaterial", &idx );
		pSGUtils->m_pMaterialPalette->SetResourcePtr( idx, pMResource );
	}

	{
		//create new shader lit texture
		IFXDECLARELOCAL(IFXShaderLitTexture, pShaderLitTexture);
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderLitTexture, IID_IFXShaderLitTexture, (void**)&pShaderLitTexture)), IFXException);
		TEST( IFXSUCCESS( pShaderLitTexture->SetMaterialID( 1 )), IFXException);
		pShaderLitTexture->SetSceneGraph( pSGUtils->m_pSceneGraph );
		U32 idxsh = 1;
		TEST(IFXSUCCESS(pSGUtils->m_pShaderPalette->Add(L"NewShader", &idxsh)), IFXException);
		TEST(IFXSUCCESS(pSGUtils->m_pShaderPalette->SetResourcePtr( idxsh, pShaderLitTexture )), IFXException);
	}

	{
		//create new shader lit texture
		IFXDECLARELOCAL(IFXShaderLitTexture, pShaderLitTexture);
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderLitTexture, IID_IFXShaderLitTexture, (void**)&pShaderLitTexture)), IFXException);
		TEST( IFXSUCCESS( pShaderLitTexture->SetMaterialID( 1 )), IFXException);
		pShaderLitTexture->SetSceneGraph( pSGUtils->m_pSceneGraph );
		U32 idxsh = 2;
		TEST(IFXSUCCESS(pSGUtils->m_pShaderPalette->Add(L"AnotherShader", &idxsh)), IFXException);
		TEST(IFXSUCCESS(pSGUtils->m_pShaderPalette->SetResourcePtr( idxsh, pShaderLitTexture )), IFXException);
	}

	U32 pBuffer[3];
	pBuffer[2] = 0x33FFFF33; pBuffer[1] = 0x66FF3366; pBuffer[0] = 0xFFFF3366;
	AddTexture( pSGUtils, "Test", pBuffer, 0 );
	U32 pBuffer2[3];
	pBuffer2[2] = 0x399F8833; pBuffer2[1] = 0x6998DDD6; pBuffer2[0] = 0xFF88FFD6;
	AddTexture( pSGUtils, "Test2", pBuffer2, 0 );
	U32 pBuffer3[3];
	pBuffer3[2] = 0x0000FF00; pBuffer2[1] = 0x00FF0000; pBuffer2[0] = 0xFF0000FF;
	AddTexture( pSGUtils, "Test3", pBuffer3, 1, 2 );


	TEST(IFXSUCCESS(InitAuthorClod(pAuthorMesh)),IFXException);

	TEST(IFXSUCCESS(pGenerator->QueryInterface( IID_IFXAuthorCLODResource, (void**)&pMeshGenerator )), IFXException);
	TEST(IFXSUCCESS(pMeshGenerator->SetAuthorMesh(pAuthorMesh)), IFXException);

	TEST(IFXSUCCESS(pSGUtils->CreateModelNode(&nodeHeader, uResourceID)), IFXException);

	// Get the shader palette
        pShaderPalette = pSGUtils->m_pShaderPalette;
	TEST(pShaderPalette != NULL, IFXException);

	IFXDECLARELOCAL(IFXMeshGroup, pMeshGroup);
	TEST(IFXSUCCESS(pMeshGenerator->GetMeshGroup(&pMeshGroup)), IFXException);

	TEST(IFXSUCCESS(VerifyRenderableData(pAuthorMesh, pMeshGroup)), IFXException);

	//create shader set
	IFXDECLARELOCAL(IFXShaderList,pShaderList);
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderList, IID_IFXShaderList, (void**)&pShaderList)), IFXException);
	pShaderList->Allocate(1);
	pShaderList->SetShader(0,1);
	//create shader set
	IFXDECLARELOCAL(IFXShaderList,pShaderList2);
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderList, IID_IFXShaderList, (void**)&pShaderList2)), IFXException);
	pShaderList2->Allocate(1);
	pShaderList2->SetShader(0,2);
	//create shader set
	IFXDECLARELOCAL(IFXShaderList,pShaderList3);
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderList, IID_IFXShaderList, (void**)&pShaderList3)), IFXException);
	pShaderList3->Allocate(1);
	pShaderList3->SetShader(0,3);

	// Create a shader modifier
	IFXDECLARELOCAL(IFXShadingModifier,pShadingModifier);
	TEST( IFXSUCCESS( IFXCreateComponent( CID_IFXShadingModifier, IID_IFXShadingModifier, (void**)&pShadingModifier ) ), IFXException);
	pShadingModifier->SetSceneGraph( pSGUtils->m_pSceneGraph );
	TEST(IFXSUCCESS(pShadingModifier->SetElementShaderList( 0, pShaderList )), IFXException);
	TEST(IFXSUCCESS(pShadingModifier->SetElementShaderList( 1, pShaderList2 )), IFXException);
	TEST(IFXSUCCESS(pShadingModifier->SetElementShaderList( 2, pShaderList3 )), IFXException);
	// Add it to the end of the modifier chain associated with the IFXModel
	IFXDECLARELOCAL(IFXModifierChain,pModChain);
	IFXDECLARELOCAL(IFXModel,pModel);

	TEST( IFXSUCCESS( pSGUtils->GetModelFromName(L"PickClodMesh", &pModel) ), IFXException);
	TEST( IFXSUCCESS( pModel->GetModifierChain(&pModChain) ), IFXException );
	TEST( IFXSUCCESS( pModChain->AddModifier( *pShadingModifier ) ), IFXException );

	//IFXRELEASE(pMeshGroup);
	//IFXRELEASE(pMeshGenerator);
	//IFXRELEASE(pGenerator);

	return IFX_OK;
}

IFXRESULT VerifyRenderableData(IFXAuthorCLODMesh* pAuthorClod, IFXMeshGroup* pMeshGroup)
{
	IFXRESULT ir  = IFX_OK;
	int j,k;

	if( !pMeshGroup || !pAuthorClod){ ir = IFX_FALSE; return ir;}

	int numGroup = pMeshGroup->GetNumMeshes();
	//if( numGroup != pLineSet->GetLineSetDesc()->m_uNumMaterials )
	//  { ir = IFX_FALSE; return ir; }

	// checking the sum of number of Faces in meshgroup
	// it should be equal to number of lines at author line set
	IFXMesh* pMesh = NULL;
	U32 numFacesLS  = 0;

	IFXTRACE_GENERIC(L"[AuthorCLOD Test] AuthorClodMesh with %i faces, %i positons, %i normales, %i materials \n",
		pAuthorClod->GetMeshDesc()->NumFaces,
		pAuthorClod->GetMeshDesc()->NumPositions,
		pAuthorClod->GetMeshDesc()->NumNormals,
		pAuthorClod->GetMeshDesc()->NumMaterials);

	IFXTRACE_GENERIC(L"[AuthorCLOD Test] Mesh Group with %i meshes\n",
		numGroup);
	for ( j = 0; j < numGroup; j++ ) {

		pMeshGroup->GetMesh(j,pMesh);
		int n = pMesh->GetNumFaces();
		IFXTRACE_GENERIC(L"[AuthorCLOD Test] Mesh at Group %i Faces %i Positions %i\n",
			j, n, pMesh->GetNumVertices());

		IFXInterleavedData* pData = 0;
		IFXVector3Iter vPosIter;
		IFXVector3Iter vNormIter;
		IFXU32Iter vColorIter;

		ir = pMesh->GetInterleavedVertexData(pData);
		U32 uVectorNum = 0;
		pData->GetVectorIter(uVectorNum++, vPosIter);
		pData->GetVectorIter(uVectorNum++, vNormIter);
		pData->GetVectorIter(uVectorNum, vColorIter);

		int stride = vPosIter.GetStride();
		IFXTRACE_GENERIC(L"[Mesh] Strides:  Vert %d Norm %d Color %d\n",
			vPosIter.GetStride(), vNormIter.GetStride(), vColorIter.GetStride());

		IFXVector3 Vec, Norm; 
		IFXVector3* pVec  = &Vec;
		IFXVector3* pNorm = &Norm;
		for ( k= 0; k < (int)pMesh->GetNumVertices(); k++) {
			pVec = vPosIter.Get(); 
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
		
		pData->Release();
		pMesh->Release();
	}

	IFXTRACE_GENERIC(L"[AuthorCLOD Test] AuthorClodMesh:Faces %i\n",
		pAuthorClod->GetMeshDesc()->NumFaces );


	if (numFacesLS !=  pAuthorClod->GetMeshDesc()->NumFaces) {
		ir = IFX_FALSE;
		return ir;
	}

	// AuthorLineSet
	float dist  = 0.0f;
	float dist2 = 0.0f;
	IFXVector3 point1, point2, point3;
	for( j = 0; j < (int)numFacesLS; j++ ) {

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
	for ( j = 0; j < numGroup; j++ ){

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


		IFXVector3 v1, v2, v3;
		IFXVector3 *pv1 = &v1;
		IFXVector3 *pv2 = &v2;
		IFXVector3 *pv3 = &v3;
		for( k = 0; k< (int)numFacesLS; k++ ) {

			IFXFace* pFace  = faceIter.Index(k);
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
	
		pMesh->Release();
	
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
