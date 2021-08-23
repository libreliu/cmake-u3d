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

#ifndef SAMPLESPHERE_H
#include "SampleSphere.h"
#endif

#include "IFXSphereGenerator.h"

IFXRESULT SampleSphere::Run( U3DSceneUtilities* pSGUtils )
{
	IFXModifier*			pGenerator = NULL;
	IFXShaderLitTexture*	pSelectShader = NULL;
	IFXMaterialResource*	pSelectMaterial = NULL;
	IFXPalette*				pShaderPalette = NULL;
	IFXShaderLitTexture*	pShader = NULL;

	U32 uID = 0;
	SGNodeHeader nh;
	U32           uResourceID = 0;
	U32           uCloneID  = 0;
	struct SGNodeHeader   nodeHeader;
	U32           uShaderID = 0;

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
		IFXLight *pLight = NULL;
		IFXVector4 colorWhite(1.0f, 1.0f, 1.0f);

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
	}

	// --- Create a sphere primitive to indicate the picking location
	TEST(IFXSUCCESS(CreatePrimitive("PickSphere", CID_IFXSphereGenerator, 0,
		&uResourceID, &pGenerator, pSGUtils->m_pGeneratorPalette, pSGUtils->m_pSceneGraph) ), IFXException);

	// Create a model node in the node palette for the sphere
	wcscpy(nodeHeader.Name, L"PickSphere");
	wcscpy(nodeHeader.ParentName, PALETTE_DEFAULT_GROUP);
	nodeHeader.NodeTm.Reset();
	nodeHeader.NodeTm.Translate3x4(IFXVector3(0, 0, -1000));

	TEST(IFXSUCCESS(pSGUtils->CreateModelNode(&nodeHeader, uResourceID)), IFXException);

	IFXVector4 Ambient = IFXVector4(1,(F32)0.5,(F32)0.1);
	IFXVector4 Emission = IFXVector4(1,(F32)0.1,(F32)0.1);
	IFXVector4 Diffuse = IFXVector4(1,(F32)0.1,(F32)0.8);
	IFXVector4 Specular = IFXVector4(0.5,(F32)0.1,(F32)0.1);
	//Set material resource
	TEST(IFXSUCCESS(pSGUtils->SetMaterialResource(L"PickSphere",
		NULL, //&Ambient, 
		&Diffuse, 
		NULL, //&Emission, 
		NULL, //&Specular,
		0.5f,
		0.f,
		TRUE,
		/*IFXShaderLitTexture::WIRE |*/ IFXShaderLitTexture::MATERIAL
		)), IFXException);

	// Set some sphere properties
	IFXSphereGenerator  *pSphereGenerator = NULL;
	TEST(IFXSUCCESS(pGenerator->QueryInterface( IID_IFXSphereGenerator, (void**)&pSphereGenerator )), IFXException);
	TEST(IFXSUCCESS(pSphereGenerator->SetRadius(20.0f)), IFXException);
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
	pShader->SetDrawLines(TRUE);
	pShader->SetLightingEnabled(TRUE);
	IFXRELEASE(pShader);

	return IFX_OK;
}
