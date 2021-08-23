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

#ifndef ImprovedSubdivShader_H
#include "ImprovedSubdivShader.h"
#endif

#include "IFXSphereGenerator.h"
#include "IFXModifierChain.h"
#include "IFXSubdivModifier.h"

#include "IFXStdio.h"
#include "IFXTextureObject.h"
#include "IFXWriteManager.h"
#include "IFXWriteBuffer.h"


IFXRESULT ImprovedSubdivShader::Run( U3DSceneUtilities* pSGUtils )
{
	SGNodeHeader nh;
	struct SGNodeHeader   nodeHeader;

	TEST(IFXSUCCESS(pSGUtils->InitializeScene()), IFXException);

	// Create camera
	{
		IFXDECLARELOCAL( IFXView, pViewNode );

		wcscpy(nh.Name, L"Camera1");
		wcscpy(nh.ParentName, PALETTE_DEFAULT_GROUP);
		nh.NodeTm.Reset();
		TEST(IFXSUCCESS(pSGUtils->CreateCameraNode(&nh, &pViewNode)), IFXException);
		// Set up the camera based on its type
		pViewNode->SetAttributes( 0 );
		TEST(IFXSUCCESS(pViewNode->SetProjection(5.0)), IFXException);
	}

	// Create light
	{
		IFXDECLARELOCAL(IFXLight, pLight);
		IFXDECLARELOCAL(IFXLightResource, pLightResource);
		IFXDECLARELOCAL( IFXPalette, pLightResourcePalette );
		IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );
		IFXVector4 colorWhite(1.0f, 1.0f, 1.0f);

		wcscpy(nh.Name, L"Light1");
		wcscpy(nh.ParentName, PALETTE_DEFAULT_GROUP);
		TEST(IFXSUCCESS(pSGUtils->CreateLightNode(&nh, &pLight)), IFXException);

		// Get the light resource palette
		TEST(IFXSUCCESS(pSGUtils->m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph )), IFXException);
		TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::LIGHT, &pLightResourcePalette )), IFXException);
		U32 uLightResourceId = 0;

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
	}

	// --- Create a sphere primitive to indicate the picking location
	{
		IFXDECLARELOCAL(IFXModifier, pGenerator);
		IFXDECLARELOCAL(IFXSphereGenerator, pSphereGenerator);
		U32 uResourceID;

		TEST(IFXSUCCESS(CreatePrimitive("PickSphere", CID_IFXSphereGenerator, 0,
			&uResourceID, &pGenerator, pSGUtils->m_pGeneratorPalette, pSGUtils->m_pSceneGraph) ), IFXException);

		// Set some sphere properties
		TEST(IFXSUCCESS(pGenerator->QueryInterface( IID_IFXSphereGenerator, (void**)&pSphereGenerator )), IFXException);
		TEST(IFXSUCCESS(pSphereGenerator->SetRadius(20.0f)), IFXException);
		pSphereGenerator->SetResolution( 8 );

		// Create a model node in the node palette for the sphere
		wcscpy(nodeHeader.Name, L"PickSphere");
		wcscpy(nodeHeader.ParentName, PALETTE_DEFAULT_GROUP);
		nodeHeader.NodeTm.Reset();
		nodeHeader.NodeTm.Translate3x4(IFXVector3(0, 0, -1000));

		TEST(IFXSUCCESS(pSGUtils->CreateModelNode(&nodeHeader, uResourceID)), IFXException);
	}

	// Access the shader and turn on line rendering for the default
	// shader so we can see the bounding sphere.
/*
	{
		IFXDECLARELOCAL(IFXShaderLitTexture, pShader);
		U32 uShaderID;

		// Get the Shader on this model
		TEST(IFXSUCCESS(pSGUtils->m_pShaderPalette->Find(L"", &uShaderID)), IFXException);
		TEST(IFXSUCCESS(pSGUtils->m_pShaderPalette->GetResourcePtr(uShaderID, IID_IFXShaderLitTexture, (void **)&pShader)), IFXException);
		TEST( IFXSUCCESS( pShader->SetDrawLines(TRUE)), IFXException);
		TEST( IFXSUCCESS( pShader->SetLightingEnabled(TRUE) ), IFXException);
	}
*/
	IFXDECLARELOCAL(IFXModifierChain, pModChain);
	IFXDECLARELOCAL(IFXModel, pModel);

	TEST( IFXSUCCESS( pSGUtils->GetModelFromName(L"PickSphere", &pModel) ), IFXException);
	TEST( IFXSUCCESS( pModel->GetModifierChain(&pModChain) ), IFXException );

	/////////////////////////////////////////////////////////////
	// Creating and applying first SubDiv modifier
	/////////////////////////////////////////////////////////////
	{
		IFXDECLARELOCAL(IFXSubdivModifier, pSubDiv1);

		// Create a subdivision modifier with an IFXModifier interface
		TEST( IFXSUCCESS( IFXCreateComponent( CID_IFXSubdivModifier, IID_IFXSubdivModifier, (void**)&pSubDiv1 ) ), IFXException );

		// Initialize it
		TEST( IFXSUCCESS( pSubDiv1->SetSceneGraph( pSGUtils->m_pSceneGraph ) ), IFXException );

		// Don't allow double subdivision modifiers
		U32 found = 0;
		U32 ModifierCount = 0;
		U32 CurrentModifier = 0;

		TEST( IFXSUCCESS( pSubDiv1->SetEnable( TRUE ) ), IFXException );
		TEST( IFXSUCCESS( pSubDiv1->SetDepth( 5 ) ), IFXException );
		TEST( IFXSUCCESS( pSubDiv1->SetAdaptive( FALSE ) ), IFXException );
		TEST( IFXSUCCESS( pSubDiv1->SetTension( 10 ) ), IFXException );

		TEST( IFXSUCCESS( pModChain->AddModifier( *pSubDiv1 ) ), IFXException );
	}

	/////////////////////////////////////////
	//Creating and applying shader modifier
	////////////////////////////////////////
	{
		U32 materialId1;
		U32 materialId2;

		//create new material
		{
			IFXDECLARELOCAL(IFXMaterialResource, pMResource);

			TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXMaterialResource, IID_IFXMaterialResource, (void**)&pMResource)), IFXException);
			IFXVector4 color(1,(F32)0.9,(F32)0.3,1);
			pMResource->SetDiffuse(color);
			pMResource->SetSceneGraph( pSGUtils->m_pSceneGraph );

			TEST(IFXSUCCESS(pSGUtils->m_pMaterialPalette->Add( L"NewMaterial", &materialId1 )), IFXException);
			TEST(IFXSUCCESS(pSGUtils->m_pMaterialPalette->SetResourcePtr( materialId1, pMResource )), IFXException);
		}

		//create another material
		{
			IFXDECLARELOCAL(IFXMaterialResource, pMResource);

			TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXMaterialResource, IID_IFXMaterialResource, (void**)&pMResource)), IFXException);
			pMResource->SetOpacity((F32)0.7);
			pMResource->SetSceneGraph( pSGUtils->m_pSceneGraph );

			TEST(IFXSUCCESS(pSGUtils->m_pMaterialPalette->Add( L"AnotherMaterial", &materialId2 )), IFXException);
			TEST(IFXSUCCESS(pSGUtils->m_pMaterialPalette->SetResourcePtr( materialId2, pMResource )), IFXException);
		}

		U32 textureId;

		// create texture object
		{
			IFXDECLARELOCAL( IFXTextureObject, pTextureObject);

			STextureSourceInfo sImageInfo;
			U32 pBuffer[3];
			IFXString textureName = "TestTexture";
			IFXRESULT result = IFX_OK;

			pBuffer[2] = 0x33FFFF33; pBuffer[1] = 0x66FF3366; pBuffer[0] = 0xFFFF3366;

			result = IFXCreateComponent( CID_IFXTextureObject, IID_IFXTextureObject, (void**)&pTextureObject );

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

			// Add an entry for the texture to the texture palette
			if( pSGUtils->m_pTexturePalette && IFXSUCCESS(result) )
				result = pSGUtils->m_pTexturePalette->Add( textureName.Raw(), &textureId );

			// Point the texture palette entry to the texture object we created above.
			if( pSGUtils->m_pTexturePalette && IFXSUCCESS(result) )
				result = pSGUtils->m_pTexturePalette->SetResourcePtr( textureId, pTextureObject );
		}

		//create new shader lit texture
		{
			IFXDECLARELOCAL(IFXShaderLitTexture, pShaderLitTexture);

			TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderLitTexture, IID_IFXShaderLitTexture, (void**)&pShaderLitTexture)), IFXException);
			pShaderLitTexture->SetSceneGraph( pSGUtils->m_pSceneGraph );
			TEST( IFXSUCCESS( pShaderLitTexture->SetDrawLines(TRUE)), IFXException);
			TEST( IFXSUCCESS( pShaderLitTexture->SetLightingEnabled(TRUE) ), IFXException);
			TEST( IFXSUCCESS( pShaderLitTexture->SetMaterialID( materialId1 )), IFXException);

			pShaderLitTexture->SetTextureID( 0, textureId );

			//add it to the shader palette
			U32 idxsh = 1;
			TEST(IFXSUCCESS(pSGUtils->m_pShaderPalette->Add(L"NewShader", &idxsh)), IFXException);
			TEST(IFXSUCCESS(pSGUtils->m_pShaderPalette->SetResourcePtr( idxsh, pShaderLitTexture )), IFXException);
		}

		//create another shader lit texture
		{
			IFXDECLARELOCAL(IFXShaderLitTexture, pShaderLitTexture);

			TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderLitTexture, IID_IFXShaderLitTexture, (void**)&pShaderLitTexture)), IFXException);
			pShaderLitTexture->SetSceneGraph( pSGUtils->m_pSceneGraph );
			TEST( IFXSUCCESS( pShaderLitTexture->SetDrawLines(TRUE)), IFXException);
			TEST( IFXSUCCESS( pShaderLitTexture->SetLightingEnabled(TRUE) ), IFXException);
			TEST( IFXSUCCESS( pShaderLitTexture->SetMaterialID( materialId2 )), IFXException);

			//add it to the shader palette
			U32 idxsh = 2;
			TEST(IFXSUCCESS(pSGUtils->m_pShaderPalette->Add(L"AnotherShader", &idxsh)), IFXException);
			TEST(IFXSUCCESS(pSGUtils->m_pShaderPalette->SetResourcePtr( idxsh, pShaderLitTexture )), IFXException);
		}

		// Create a shader modifier
		{
			IFXDECLARELOCAL(IFXShadingModifier, pShadingModifier);
			IFXDECLARELOCAL(IFXShaderList, pShaderList);

			//create shader set
			TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderList, IID_IFXShaderList, (void**)&pShaderList)), IFXException);
			pShaderList->Allocate(2);
			pShaderList->SetShader(0,2);
			pShaderList->SetShader(1,1);

			TEST( IFXSUCCESS( IFXCreateComponent( CID_IFXShadingModifier, IID_IFXShadingModifier, (void**)&pShadingModifier ) ), IFXException);

			pShadingModifier->SetSceneGraph( pSGUtils->m_pSceneGraph );
			TEST(IFXSUCCESS(pShadingModifier->SetElementShaderList( 0, pShaderList )), IFXException);
			TEST(IFXSUCCESS(pShadingModifier->SetElementShaderList( 1, pShaderList )), IFXException);
			TEST(IFXSUCCESS(pShadingModifier->SetElementShaderList( 2, pShaderList )), IFXException);
			TEST(IFXSUCCESS(pShadingModifier->SetElementShaderList( 3, pShaderList )), IFXException);
			TEST(IFXSUCCESS(pShadingModifier->SetElementShaderList( 2, NULL, FALSE )), IFXException);

			// Add it to the end of the modifier chain associated with the IFXModel
			TEST( IFXSUCCESS( pModChain->AddModifier( *pShadingModifier ) ), IFXException );
		}
	}

	return IFX_OK;
}
