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

#ifndef SampleShaderR_H
#include "SampleShaderR.h"
#endif

#include "IFXSphereGenerator.h"
#include "IFXModifierChain.h"

#include "IFXShadingModifier.h"

#include "IFXStdio.h"
#include "IFXTextureObject.h"
#include "IFXWriteManager.h"
#include "IFXWriteBuffer.h"


IFXRESULT SampleShaderR::Run( U3DSceneUtilities* pSGUtils )
{
	IFXModifier* pPlane = NULL;
	U32 uID = 0;
	SGNodeHeader nh;
	IFXView* pViewNode = NULL;
	IFXRenderClear renderClear;
	IFXLight *pLight = NULL;
	IFXModifier     *pGenerator = NULL;
	U32           uResourceID = 0;
	U32           uCloneID  = 0;
	IFXShaderLitTexture   *pSelectShader = NULL;
	IFXMaterialResource   *pSelectMaterial = NULL;
	IFXPalette        *pNodePalette = NULL;
	IFXVector3        translateVector(10.0f, 10.0f, 10.0f);
	struct SGNodeHeader   nodeHeader;
	IFXPalette        *pShaderPalette = NULL;
	U32           uShaderID = 0;
	IFXShaderLitTexture   *pShader = NULL;
	IFXVector4 colorWhite(1.0f, 1.0f, 1.0f);

	TEST(IFXSUCCESS(pSGUtils->InitializeScene()), IFXException);
	wcscpy(nh.Name, L"Camera1");
	wcscpy(nh.ParentName, PALETTE_DEFAULT_GROUP);
	nh.NodeTm.Reset();
	TEST(IFXSUCCESS(pSGUtils->CreateCameraNode(&nh, &pViewNode)), IFXException);
	// Set up the camera based on its type
	pViewNode->SetAttributes( 0 );
	TEST(IFXSUCCESS(pViewNode->SetProjection(5.0)), IFXException);
	IFXRELEASE(pViewNode);

	wcscpy(nh.Name, L"Light1");
	wcscpy(nh.ParentName, PALETTE_DEFAULT_GROUP);
	TEST(IFXSUCCESS(pSGUtils->CreateLightNode(&nh, &pLight)), IFXException);

	// Get the light resource palette
	IFXDECLARELOCAL( IFXPalette, pLightResourcePalette );
	IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );
	TEST(IFXSUCCESS(pSGUtils->m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph )), IFXException);
	TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::LIGHT, &pLightResourcePalette )), IFXException);
	U32 uLightResourceId = 0;

	// Look for the light resource in the light resource palette
	IFXLightResource* pLightResource = NULL;
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
		IFXRELEASE(pUnk);
	}

	// Link the light node to the light resource pallete entry
	TEST(IFXSUCCESS( pLight->SetLightResourceID(uLightResourceId) ), IFXException);
	// Ambient light
	pLightResource->SetType(IFXLightResource::DIRECTIONAL);
	pLightResource->SetColor(colorWhite);
    IFXRELEASE(pLightResource);
    IFXRELEASE(pLight);

	pNodePalette = pSGUtils->m_pNodePalette;
	TEST( pNodePalette != NULL, IFXException );

	// --- Create a sphere primitive to indicate the picking location
	TEST(IFXSUCCESS(CreatePrimitive("PickSphere", CID_IFXSphereGenerator, 0,
		&uResourceID, &pGenerator, pSGUtils->m_pGeneratorPalette, pSGUtils->m_pSceneGraph) ), IFXException);

	// Create a model node in the node palette for the sphere
	wcscpy(nodeHeader.Name, L"PickSphere");
	wcscpy(nodeHeader.ParentName, PALETTE_DEFAULT_GROUP);
	nodeHeader.NodeTm.Reset();
	nodeHeader.NodeTm.Translate3x4(IFXVector3(0, 0, -1000));

	TEST(IFXSUCCESS(pSGUtils->CreateModelNode(&nodeHeader, uResourceID)), IFXException);

	// Set some sphere properties
	IFXDECLARELOCAL( IFXSphereGenerator, pSphereGenerator );
	TEST(IFXSUCCESS(pGenerator->QueryInterface( IID_IFXSphereGenerator, (void**)&pSphereGenerator )), IFXException);
	TEST(IFXSUCCESS(pSphereGenerator->SetRadius(20.0f)), IFXException);
	pSphereGenerator->SetResolution( 8 );
	IFXDECLARELOCAL( IFXModifierChain, pModChain );
	pSphereGenerator->GetModifierChain(&pModChain);

	TEST( IFXSUCCESS( InitShaderModifier(pSGUtils,L"PickSphere",pModChain)), IFXException);

	IFXRELEASE(pSphereGenerator);
	IFXRELEASE(pGenerator);

	// Access the shader and turn on line rendering for the default
	// shader so we can see the bounding sphere.

	// Get the shader palette
	pShaderPalette = pSGUtils->m_pShaderPalette;
	TEST(pShaderPalette != NULL, IFXException);

	// Get the Shader on this model
	TEST(IFXSUCCESS(pShaderPalette->Find(L"", &uShaderID)), IFXException);
	TEST(IFXSUCCESS(pShaderPalette->GetResourcePtr(uShaderID, IID_IFXShaderLitTexture, (void **)&pShader)), IFXException);
	TEST( IFXSUCCESS( pShader->SetDrawLines(TRUE)), IFXException);
	TEST( IFXSUCCESS( pShader->SetLightingEnabled(TRUE) ), IFXException);
	IFXRELEASE(pShader);

	/////////////////////////////////////////
	//Creating and applying shader modifier
	////////////////////////////////////////

	TEST( IFXSUCCESS( InitShaderModifier(pSGUtils,L"PickSphere",pModChain)), IFXException);
	//TEST( IFXSUCCESS( InitShaderModifier(pSGUtils,L"PickSphere")), IFXException);

	/////////////////////////////////////////////////////////////
	// Creating and applying first SubDiv modifier 
	/////////////////////////////////////////////////////////////

	//TEST( IFXSUCCESS( InitSubDivModifier(pSGUtils, L"PickSphere") ), IFXException);
	//TEST( IFXSUCCESS( InitSubDivModifier(pSGUtils, L"PickSphere") ), IFXException);


	return IFX_OK;
}


IFXRESULT SampleShaderR::InitShaderModifier(U3DSceneUtilities *pSGUtils, IFXString ModelName,IFXModifierChain* pModChain)
{
	/////////////////////////////////////////
	//Creating and applying shader modifier
	////////////////////////////////////////
	IFXPalette *pShaderPalette = NULL;
	IFXDECLARELOCAL( IFXModel, pModel );
	IFXDECLARELOCAL( IFXShadingModifier, pShadingModifier );
	TEST( IFXSUCCESS( pSGUtils->GetModelFromName(ModelName, &pModel) ), IFXException);

	// Get the shader palette
	pShaderPalette = pSGUtils->m_pShaderPalette;
	TEST(pShaderPalette != NULL, IFXException);

	//create new material
	IFXDECLARELOCAL( IFXMaterialResource, pMResource );
	IFXPalette *pMPalette = NULL;
	IFXDECLARELOCAL( IFXShaderLitTexture, pShaderLitTexture );
	pMPalette = pSGUtils->m_pMaterialPalette;
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXMaterialResource, IID_IFXMaterialResource, (void**)&pMResource)), IFXException);
	IFXVector4 color(1,(F32)0.1,(F32)0.3,1);
	pMResource->SetDiffuse(color);
	pMResource->SetSceneGraph( pSGUtils->m_pSceneGraph );


	U32 idx = 1;
	pMPalette->Add( L"NewMaterial", &idx );
	pMPalette->SetResourcePtr( idx, pMResource );

	//create another material
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXMaterialResource, IID_IFXMaterialResource, (void**)&pMResource)), IFXException);
	pMResource->SetOpacity((F32)0.5);
	pMResource->SetSceneGraph( pSGUtils->m_pSceneGraph );

	idx = 2;
	pMPalette->Add( L"AnotherMaterial", &idx );
	pMPalette->SetResourcePtr( idx, pMResource );

	//create new shader lit texture
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderLitTexture, IID_IFXShaderLitTexture, (void**)&pShaderLitTexture)), IFXException);
	TEST( IFXSUCCESS( pShaderLitTexture->SetDrawLines(TRUE)), IFXException);
	TEST( IFXSUCCESS( pShaderLitTexture->SetLightingEnabled(TRUE) ), IFXException);
	TEST( IFXSUCCESS( pShaderLitTexture->SetMaterialID( 1 )), IFXException);

	IFXDECLARELOCAL( IFXTextureObject, pTextureObject);
	IFXDECLARELOCAL( IFXUnknown, pUnknown );
	STextureSourceInfo sImageInfo;
	U32 textureId;
	U32 pBuffer[3];
	IFXString textureName = "TestTexture";
	IFXRESULT result = IFX_OK;

	pBuffer[2] = 0x33FFFF33; pBuffer[1] = 0x66FF3366; pBuffer[0] = 0xFFFF3366;

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

	pShaderLitTexture->SetTextureID( 0, textureId );
	pShaderLitTexture->SetChannels( 1 );

	pShaderLitTexture->SetSceneGraph( pSGUtils->m_pSceneGraph );

	//add it to the shader palette
	U32 idxsh = 1;
	TEST(IFXSUCCESS(pShaderPalette->Add(L"NewShader", &idxsh)), IFXException);
	TEST(IFXSUCCESS(pShaderPalette->SetResourcePtr( idxsh, pShaderLitTexture )), IFXException);

	//create another shader lit texture
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderLitTexture, IID_IFXShaderLitTexture, (void**)&pShaderLitTexture)), IFXException);
	TEST( IFXSUCCESS( pShaderLitTexture->SetDrawLines(TRUE)), IFXException);
	TEST( IFXSUCCESS( pShaderLitTexture->SetLightingEnabled(TRUE) ), IFXException);
	TEST( IFXSUCCESS( pShaderLitTexture->SetMaterialID( 2 )), IFXException);
	pShaderLitTexture->SetSceneGraph( pSGUtils->m_pSceneGraph );

	//add it to the shader palette
	idxsh = 2;
	TEST(IFXSUCCESS(pShaderPalette->Add(L"AnotherShader", &idxsh)), IFXException);
	TEST(IFXSUCCESS(pShaderPalette->SetResourcePtr( idxsh, pShaderLitTexture )), IFXException);

	//create shader set
	IFXDECLARELOCAL( IFXShaderList, pShaderList );
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderList, IID_IFXShaderList, (void**)&pShaderList)), IFXException);
	pShaderList->Allocate(2);
	pShaderList->SetShader(0,2);
	pShaderList->SetShader(1,1);

	// Create a shader modifier
	TEST( IFXSUCCESS( IFXCreateComponent( CID_IFXShadingModifier, IID_IFXShadingModifier,
		(void**)&pShadingModifier ) ), IFXException);
	//  pShadingModifier->Initialize( pSGUtils->m_pSceneGraph, 10 );
	pShadingModifier->SetSceneGraph( pSGUtils->m_pSceneGraph );
	TEST(IFXSUCCESS(pShadingModifier->SetElementShaderList( 0, pShaderList )), IFXException);
	TEST(IFXSUCCESS(pShadingModifier->SetElementShaderList( 1, pShaderList )), IFXException);
	TEST(IFXSUCCESS(pShadingModifier->SetElementShaderList( 2, pShaderList )), IFXException);
	TEST(IFXSUCCESS(pShadingModifier->SetElementShaderList( 3, pShaderList )), IFXException);
	TEST(IFXSUCCESS(pShadingModifier->SetElementShaderList( 2, NULL, FALSE )), IFXException);
	IFXDECLARELOCAL( IFXShaderList, pNewSet );

	// Add it to the end of the modifier chain associated with the IFXModel
	// IFXModifierChain* pModChain = NULL;
	if(!pModChain)
		TEST( IFXSUCCESS( pModel->GetModifierChain(&pModChain) ), IFXException );
	TEST( IFXSUCCESS( pModChain->AddModifier( *pShadingModifier ) ), IFXException );
	return IFX_OK;
}
