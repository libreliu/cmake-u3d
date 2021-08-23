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

#ifndef SAMPLESSHADERMODIFIER_H
#include "SampleShaderModifier.h"
#endif

#include "IFXSphereGenerator.h"
#include "IFXModifierChain.h"
#include "IFXShadingModifier.h"

#include "IFXStdio.h"
#include "IFXTextureObject.h"
#include "IFXWriteManager.h"
#include "IFXWriteBuffer.h"

IFXRESULT SampleShaderModifier::Run( U3DSceneUtilities* pSGUtils )
{
	SGNodeHeader nh;
	SGNodeHeader   nodeHeader;
	IFXLight *pLight = NULL;
	U32           uResourceID = 0;
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
	//  Log("Light %s \tParent %s", nh.Name, nh.ParentName);
	Log("Light");
	//@TODO: UNICODE  Log(nh.Name);
	Log("Parent");
	//@TODO: UNICODE  Log(nh.ParentName);

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
	IFXRELEASE(pLightResource);
	IFXRELEASE(pLight);

	IFXModifier* pGenerator = NULL;

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
	IFXSphereGenerator  *pSphereGenerator = NULL;
	TEST(IFXSUCCESS(pGenerator->QueryInterface( IID_IFXSphereGenerator, (void**)&pSphereGenerator )), IFXException);
	TEST(IFXSUCCESS(pSphereGenerator->SetRadius(20.0f)), IFXException);
	pSphereGenerator->SetResolution( 6 );

	IFXRELEASE(pSphereGenerator);
	IFXRELEASE(pGenerator);

	//create new material
	{
		IFXDECLARELOCAL( IFXMaterialResource, pMResource );

		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXMaterialResource, IID_IFXMaterialResource, (void**)&pMResource)), IFXException);
		IFXVector4 color(1,(F32)0.1,(F32)0.3,1);
		pMResource->SetSpecular(color);
		pMResource->SetDiffuse(color);
		pMResource->SetSceneGraph( pSGUtils->m_pSceneGraph );


		U32 idx = 1;
		pSGUtils->m_pMaterialPalette->Add( L"NewMaterial", &idx );
		pSGUtils->m_pMaterialPalette->SetResourcePtr( idx, pMResource );
	}

	//create another material
	{
		IFXDECLARELOCAL( IFXMaterialResource, pMResource );

		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXMaterialResource, IID_IFXMaterialResource, (void**)&pMResource)), IFXException);
		pMResource->SetOpacity((F32)0.5);
		pMResource->SetSceneGraph( pSGUtils->m_pSceneGraph );

		U32 idx = 2;
		pSGUtils->m_pMaterialPalette->Add( L"AnotherMaterial", &idx );
		pSGUtils->m_pMaterialPalette->SetResourcePtr( idx, pMResource );
	}

	IFXDECLARELOCAL( IFXTextureObject, pTextureObject);
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
		sImageInfo.m_name   = textureName;
		sImageInfo.m_height = 2;
		sImageInfo.m_width  = 2;
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

	// Add an entry for the texture to the texture palette
	if( pSGUtils->m_pTexturePalette && IFXSUCCESS(result) )
		result = pSGUtils->m_pTexturePalette->Add( textureName.Raw(), &textureId );

	// Point the texture palette entry to the texture object we created above.
	if( pSGUtils->m_pTexturePalette && IFXSUCCESS(result) )
		result = pSGUtils->m_pTexturePalette->SetResourcePtr( textureId, pTextureObject );

	IFXDECLARELOCAL( IFXShaderLitTexture, pShaderLitTexture );

	//create new shader lit texture
	{
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderLitTexture, IID_IFXShaderLitTexture, (void**)&pShaderLitTexture)), IFXException);
		TEST( IFXSUCCESS( pShaderLitTexture->SetDrawLines(TRUE)), IFXException);
		TEST( IFXSUCCESS( pShaderLitTexture->SetLightingEnabled(TRUE) ), IFXException);
		TEST( IFXSUCCESS( pShaderLitTexture->SetMaterialID( 1 )), IFXException);

		pShaderLitTexture->SetChannels( 1 );
		pShaderLitTexture->SetTextureID( 0, textureId );

		pShaderLitTexture->SetSceneGraph( pSGUtils->m_pSceneGraph );

		//add it to the shader palette
		U32 idxsh = 1;
		TEST(IFXSUCCESS(pSGUtils->m_pShaderPalette->Add(L"NewShader", &idxsh)), IFXException);
		TEST(IFXSUCCESS(pSGUtils->m_pShaderPalette->SetResourcePtr( idxsh, pShaderLitTexture )), IFXException);
	}

	//create another shader lit texture
	{
		IFXDECLARELOCAL( IFXShaderLitTexture, pShaderLitTexture );

		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderLitTexture, IID_IFXShaderLitTexture, (void**)&pShaderLitTexture)), IFXException);
		TEST( IFXSUCCESS( pShaderLitTexture->SetDrawLines(TRUE)), IFXException);
		TEST( IFXSUCCESS( pShaderLitTexture->SetLightingEnabled(TRUE) ), IFXException);
		TEST( IFXSUCCESS( pShaderLitTexture->SetMaterialID( 2 )), IFXException);
		pShaderLitTexture->SetSceneGraph( pSGUtils->m_pSceneGraph );

		//add it to the shader palette
		U32 idxsh = 2;
		TEST(IFXSUCCESS(pSGUtils->m_pShaderPalette->Add(L"AnotherShader", &idxsh)), IFXException);
		TEST(IFXSUCCESS(pSGUtils->m_pShaderPalette->SetResourcePtr( idxsh, pShaderLitTexture )), IFXException);
	}

	//create shader set
	IFXDECLARELOCAL( IFXShaderList, pShaderList );
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderList, IID_IFXShaderList, (void**)&pShaderList)), IFXException);
	pShaderList->Allocate(2);
	pShaderList->SetShader(0,2);
	pShaderList->SetShader(1,1);

	// Create a shader modifier
	{
		IFXDECLARELOCAL( IFXShadingModifier, pShadingModifier );

		TEST( IFXSUCCESS( IFXCreateComponent( CID_IFXShadingModifier, IID_IFXShadingModifier,
			(void**)&pShadingModifier ) ), IFXException);
		pShadingModifier->SetSceneGraph( pSGUtils->m_pSceneGraph );
		TEST(IFXSUCCESS(pShadingModifier->SetElementShaderList( 0, pShaderList )), IFXException);
	/*	TEST(IFXSUCCESS(pShadingModifier->SetElementShaderList( 1, pShaderList )), IFXException);
		TEST(IFXSUCCESS(pShadingModifier->SetElementShaderList( 2, pShaderList )), IFXException);
		TEST(IFXSUCCESS(pShadingModifier->SetElementShaderList( 3, pShaderList )), IFXException);
		TEST(IFXSUCCESS(pShadingModifier->SetElementShaderList( 2, NULL )), IFXException);*/

	/*	IFXShaderList* pNewSet = NULL;
		U32 val = 0;
		TEST(IFXSUCCESS(pShadingModifier->GetElementShaderList( 0, &pNewSet )), IFXException);
		TEST(IFXSUCCESS(pNewSet->GetShader( 0, &val )), IFXException);
		TEST(IFXSUCCESS(pShaderPalette->Find(L"NewShader", &uShaderID)), IFXException);
		TEST(IFXSUCCESS(pShaderPalette->GetResourcePtr(uShaderID, IID_IFXShaderLitTexture, (void **)&pShader)), IFXException);
		char s[10];
		itoa( pShader->GetMaterialID(), s, 10 );
		Log( s );
		IFXRELEASE(pShader);*/
		//  CIFXString ss;
		//  pShaderPalette->GetName(1, &ss );

		//  TEST(IFXSUCCESS(pShaderPalette->GetResourcePtr(idxsh, IID_IFXShaderLitTexture, (void**)&pShader)), IFXException);
		//  U32 f = pShader->GetMaterialID();
		//  IFXUnknown* pUnknown = NULL;
		//  IFXMaterialResource* pMaterialResource = NULL;
		//  TEST(IFXSUCCESS(pMPalette->GetResourcePtr( f, &pUnknown )), IFXException);
		//  TEST(IFXSUCCESS(pUnknown->QueryInterface(IID_IFXMaterialResource, (void**)&pMaterialResource)), IFXException);
		//  TEST(IFXSUCCESS(pMaterialResource->SetOpacity(0.99)), IFXException);

		//  TEST(IFXSUCCESS(pShaderPalette->Find("NewShader", &uShaderID)), IFXException);
		/*  TEST(IFXSUCCESS(pShaderPalette->Find("DefaultShader", &uShaderID)), IFXException);
		TEST(IFXSUCCESS(pShaderPalette->GetResourcePtr(uShaderID, IID_IFXShaderLitTexture, (void **)&pShader)), IFXException);
		pShader->SetMaterialID( idx );
		*/

		// Add it to the end of the modifier chain associated with the IFXModel
		IFXDECLARELOCAL( IFXModifierChain, pModChain );
		IFXDECLARELOCAL( IFXModel, pModel );
		TEST( IFXSUCCESS( pSGUtils->GetModelFromName(L"PickSphere", &pModel) ), IFXException);
		TEST( IFXSUCCESS( pModel->GetModifierChain(&pModChain) ), IFXException );
		TEST( IFXSUCCESS( pModChain->AddModifier( *pShadingModifier ) ), IFXException );
	}

	return IFX_OK;
}
