//***************************************************************************
//
//  Copyright (c) 2004 - 2006 Intel Corporation
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

#ifndef SampleGlyphR_H
#include "SampleGlyphR.h"
#endif

#include "IFXSphereGenerator.h"
#include "IFXModifierChain.h"
#include "IFXGlyph2DModifier.h"
#include "IFXGlyphCommandList.h"

IFXRESULT AddGlyph2DModifier(
							 IFXModifierChain* pModChain,
							 IFXModel * in_pModel,
							 F64 defaultWidth, F64 defaultSpacing, F64 defaultHeight,
							 IFXGlyphCommandList* pCommandList)
{
	IFXRESULT     result = IFX_OK;
	IFXDECLARELOCAL( IFXGlyph2DModifier, pGlyph2DModifier );
	IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );

	if ( !in_pModel )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	// Get the pointer to the scenegraph from the IFXModel
	if ( in_pModel && IFXSUCCESS(result) )
	{
		result = in_pModel->GetSceneGraph(&pSceneGraph);

		if ( IFXSUCCESS(result) && !pSceneGraph)
			result = IFX_E_INVALID_POINTER;
	}

	// Create a mesh Shading modifier
	if ( IFXSUCCESS(result) )
	{
		result = IFXCreateComponent( CID_IFXGlyph2DModifier, IID_IFXGlyph2DModifier,
			(void**)&pGlyph2DModifier );
	}

	// Initialize it
	if ( pGlyph2DModifier && IFXSUCCESS(result) )
	{
		result = pGlyph2DModifier->SetSceneGraph( pSceneGraph ) ;
		pGlyph2DModifier->SetAttributes( IFXGlyph2DModifier::BILLBOARD );
	}

	if ( IFXSUCCESS(result) )
	{
		IFXDECLARELOCAL( IFXSimpleList, pGlyphData );
		pCommandList->GetList( &pGlyphData );
		pGlyph2DModifier->SetGlyphCommandList( pGlyphData );
		IFXRELEASE( pGlyphData );
	}
	// Add it to the end of the modifier chain associated with the IFXModel
	if ( in_pModel && IFXSUCCESS(result) )
	{
		if ( pModChain && pGlyph2DModifier && IFXSUCCESS( result ) )
			result = pModChain->AddModifier( *pGlyph2DModifier );
	}

	return result;
}

IFXRESULT SampleGlyphR::Run( U3DSceneUtilities* pSGUtils )
{
	IFXModifier* pGenerator = NULL;
	U32           uResourceID;
	IFXVector4 colorWhite(1.0f, 1.0f, 1.0f);

	TEST(IFXSUCCESS(pSGUtils->InitializeScene()), IFXException);

	//Create and set New CAMERA node
	IFXDECLARELOCAL(IFXView, pViewNode);
	SGNodeHeader ViewNodeHeader;
	U32 i;

	wcscpy(ViewNodeHeader.Name, L"CustomCameraNode");
	ViewNodeHeader.NumberOfParents = 1;
	wcscpy(ViewNodeHeader.ParentNames[0], PALETTE_DEFAULT_GROUP);

	for (i = 0; i < NUM_OF_PARENTS; i++)
		ViewNodeHeader.NodeTms[i].Reset();

	TEST(IFXSUCCESS(pSGUtils->CreateCameraNode(&ViewNodeHeader, &pViewNode, TRUE)), IFXException);

	// Set up the camera based on its type
	pViewNode->SetAttributes( 2 );
	TEST(IFXSUCCESS(pViewNode->SetProjection(5.0)), IFXException);

	const F32 initnode[] = {sqrt(F32(2))/2,sqrt(F32(2))/2,0,0,
		-sqrt(F32(2))/2,sqrt(F32(2))/2,0,0,
		0,0,1,0,
		0,0,100,1};
	IFXMatrix4x4 tmnode = IFXMatrix4x4( initnode );
	TEST( IFXSUCCESS( pViewNode->SetMatrix( 0, &tmnode ) ), IFXException );

	// create light
	IFXLight *pLight = NULL;
	SGNodeHeader nh;

	wcscpy(nh.Name, L"Light1");
	wcscpy(nh.ParentName, PALETTE_DEFAULT_GROUP);
	// Create a light node with this name
	//  Log("Light %s \tParent %s", nh.Name, nh.ParentName);
	Log("Light");
	//@TODO: UNICODE  Log(nh.Name);
	Log("Parent");
	//@TODO: UNICODE  Log(nh.ParentName);

	TEST(IFXSUCCESS(pSGUtils->CreateLightNode(&nh, &pLight)), IFXException);
	TEST( IFXSUCCESS( pLight->SetMatrix( 0, &tmnode ) ), IFXException );

	// Get the light resource palette
	IFXDECLARELOCAL( IFXPalette, pLightResourcePalette );
	IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );

	TEST(IFXSUCCESS(pSGUtils->m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph )), IFXException);
	TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::LIGHT, &pLightResourcePalette )), IFXException);
	U32 uLightResourceId;

	// Look for the light resource in the light resource palette
	IFXLightResource* pLightResource = NULL;

	// create a light resource and add the light resource to the palette
	IFXRESULT rc = pLightResourcePalette->Add( nh.Name, &uLightResourceId );
	if (IFXSUCCESS(rc))
	{
		TEST(IFXSUCCESS( IFXCreateComponent( CID_IFXLightResource, IID_IFXLightResource, (void**)&pLightResource ) ), IFXException);
		TEST(IFXSUCCESS(pLightResource->SetSceneGraph( pSceneGraph )), IFXException);
		TEST(IFXSUCCESS( pLightResourcePalette->SetResourcePtr( uLightResourceId, pLightResource ) ), IFXException);
	}

	// Link the light node to the light resource pallete entry
	TEST(IFXSUCCESS( pLight->SetLightResourceID(uLightResourceId) ), IFXException);
	// Ambient light
	pLightResource->SetType(IFXLightResource::DIRECTIONAL);
	pLightResource->SetColor(colorWhite);

	IFXRELEASE(pLightResource);
	IFXRELEASE(pLight);

	// --- Create a sphere primitive to indicate the picking location
	TEST(IFXSUCCESS(CreatePrimitive("PickSphere", CID_IFXSphereGenerator, 0,
		&uResourceID, &pGenerator, pSGUtils->m_pGeneratorPalette, pSGUtils->m_pSceneGraph) ), IFXException);

	// Create a model node in the node palette for the sphere
	{
		IFXSphereGenerator  *pSphereGenerator = NULL;
		SGNodeHeader   nodeHeader;

		wcscpy(nodeHeader.Name, L"PickSphere");
		wcscpy(nodeHeader.ParentName, PALETTE_DEFAULT_GROUP);
		nodeHeader.NodeTm.Reset();
		nodeHeader.NodeTm.Translate3x4(IFXVector3(0, 0, -100));

		TEST(IFXSUCCESS(pSGUtils->CreateModelNode(&nodeHeader, uResourceID)), IFXException);

		// Set some sphere properties
		TEST(IFXSUCCESS(pGenerator->QueryInterface( IID_IFXSphereGenerator, (void**)&pSphereGenerator )), IFXException);
		TEST(IFXSUCCESS(pSphereGenerator->SetRadius(20.0f)), IFXException);
		pSphereGenerator->SetResolution( 6 );

		IFXRELEASE(pSphereGenerator);
	}

	IFXModel        *pModel = NULL;
	{
		IFXDECLARELOCAL( IFXGlyphCommandList, pComList );

		TEST( IFXSUCCESS( pSGUtils->GetModelFromName(L"PickSphere", &pModel) ), IFXException);

		IFXCreateComponent( CID_IFXGlyphCommandList, IID_IFXGlyphCommandList, (void**)&pComList );

		pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_STARTGLYPHSTRING );
		pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_STARTGLYPH );
		pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_STARTPATH );
		pComList->AddMoveToBlock( 1, 1 );
		pComList->AddLineToBlock( 1, 50 );
		pComList->AddLineToBlock( 100, 50 );
		pComList->AddLineToBlock( 100, 1 );
		pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_ENDPATH );
		pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_STARTPATH );
		pComList->AddMoveToBlock( 10, 10 );
		pComList->AddLineToBlock( 90, 10 );
		pComList->AddLineToBlock( 90, 40 );
		pComList->AddLineToBlock( 10, 40 );
		pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_ENDPATH );
		pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_ENDGLYPH );
		pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_ENDGLYPHSTRING );

		IFXModifierChain* pModChain = NULL;
		pGenerator->GetModifierChain(&pModChain);

		AddGlyph2DModifier( pModChain,pModel, 10, 10, 10, pComList );

		IFXRELEASE( pModChain );
	}

	IFXRELEASE(pGenerator);

	U32 idx;
	{
		//create new material
		IFXDECLARELOCAL( IFXMaterialResource, pMResource );
		IFXDECLARELOCAL( IFXShaderLitTexture, pShaderLitTexture );
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXMaterialResource, IID_IFXMaterialResource, (void**)&pMResource)), IFXException);
		IFXVector4 color(1,(F32)0.1,(F32)0.3,1);
		pMResource->SetSpecular(color);
		pMResource->SetDiffuse(color);
		pMResource->SetSceneGraph( pSGUtils->m_pSceneGraph );
		pSGUtils->m_pMaterialPalette->Add( L"NewMaterial", &idx );
		pSGUtils->m_pMaterialPalette->SetResourcePtr( idx, pMResource );
	}
	{
		//create new material
		IFXDECLARELOCAL( IFXMaterialResource, pMResource );
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXMaterialResource, IID_IFXMaterialResource, (void**)&pMResource)), IFXException);
		IFXVector4 color(1,(F32)1,(F32)0.3,1);
		pMResource->SetSpecular(color);
		pMResource->SetDiffuse(color);
		pMResource->SetSceneGraph( pSGUtils->m_pSceneGraph );
		pSGUtils->m_pMaterialPalette->Add( L"AnotherMaterial", &idx );
		pSGUtils->m_pMaterialPalette->SetResourcePtr( idx, pMResource );
	}

	U32 idxsh;

	{
		//create new shader lit texture
		IFXDECLARELOCAL( IFXShaderLitTexture, pShaderLitTexture );
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderLitTexture, IID_IFXShaderLitTexture, (void**)&pShaderLitTexture)), IFXException);
		TEST( IFXSUCCESS( pShaderLitTexture->SetMaterialID( 1 )), IFXException);
		pShaderLitTexture->SetSceneGraph( pSGUtils->m_pSceneGraph );

		TEST(IFXSUCCESS(pSGUtils->m_pShaderPalette->Add(L"NewShader", &idxsh)), IFXException);
		TEST(IFXSUCCESS(pSGUtils->m_pShaderPalette->SetResourcePtr( idxsh, pShaderLitTexture )), IFXException);
	}
	{
		//create new shader lit texture
		IFXDECLARELOCAL( IFXShaderLitTexture, pShaderLitTexture );
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderLitTexture, IID_IFXShaderLitTexture, (void**)&pShaderLitTexture)), IFXException);
		TEST( IFXSUCCESS( pShaderLitTexture->SetMaterialID( 2 )), IFXException);
		pShaderLitTexture->SetSceneGraph( pSGUtils->m_pSceneGraph );

		TEST(IFXSUCCESS(pSGUtils->m_pShaderPalette->Add(L"AnotherShader", &idxsh)), IFXException);
		TEST(IFXSUCCESS(pSGUtils->m_pShaderPalette->SetResourcePtr( idxsh, pShaderLitTexture )), IFXException);
	}

	{
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
		IFXDECLARELOCAL( IFXModifierChain, pModChain );
		TEST( IFXSUCCESS( pModel->GetModifierChain(&pModChain) ), IFXException );
		TEST( IFXSUCCESS( pModChain->AddModifier( *pShadingModifier ) ), IFXException );
	}
	{
		//create shader set
		IFXDECLARELOCAL( IFXShaderList, pShaderList2 );
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderList, IID_IFXShaderList, (void**)&pShaderList2)), IFXException);
		pShaderList2->Allocate(1);
		pShaderList2->SetShader(0,2);
		// Create a shader modifier
		IFXDECLARELOCAL( IFXShadingModifier, pShadingModifier );

		TEST( IFXSUCCESS( IFXCreateComponent( CID_IFXShadingModifier, IID_IFXShadingModifier,
			(void**)&pShadingModifier ) ), IFXException);
		pShadingModifier->SetSceneGraph( pSGUtils->m_pSceneGraph );
		TEST(IFXSUCCESS(pShadingModifier->SetElementShaderList( 1, pShaderList2 )), IFXException);
		IFXDECLARELOCAL( IFXModifierChain, pModChain );
		TEST( IFXSUCCESS( pModel->GetModifierChain(&pModChain) ), IFXException );
		TEST( IFXSUCCESS( pModChain->AddModifier( *pShadingModifier ) ), IFXException );
	}

	IFXRELEASE( pModel );
	return IFX_OK;
}
