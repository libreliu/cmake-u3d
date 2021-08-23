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

#ifndef SAMPLESUBDIV_H
#include "SampleSubdiv.h"
#endif

#include "IFXSphereGenerator.h"
#include "IFXModifierChain.h"
#include "IFXSubdivModifier.h"


IFXRESULT SampleSubdiv::Run( U3DSceneUtilities* pSGUtils )
{
	U32 uID = 0;
	SGNodeHeader nh;
	IFXLight *pLight = NULL;
	IFXModifier     *pGenerator = NULL;
	U32           uResourceID = 0;
	U32           uCloneID  = 0;
	struct SGNodeHeader   nodeHeader;
	IFXPalette        *pShaderPalette = NULL;
	U32           i, uShaderID = 0;
	IFXShaderLitTexture   *pShader = NULL;
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

	// --- Create a sphere primitive to indicate the picking location
	TEST(IFXSUCCESS(CreatePrimitive("PickSphere", CID_IFXSphereGenerator, 0,
		&uResourceID, &pGenerator, pSGUtils->m_pGeneratorPalette, pSGUtils->m_pSceneGraph) ), IFXException);

	// Create a model node in the node palette for the sphere
	wcscpy(nodeHeader.Name, L"PickSphere");
	nodeHeader.NumberOfParents = 2;
	wcscpy(nodeHeader.ParentNames[0], PALETTE_DEFAULT_GROUP);
	wcscpy(nodeHeader.ParentNames[1], PALETTE_DEFAULT_GROUP);
	for (i = 0; i < NUM_OF_PARENTS; i++)
		nodeHeader.NodeTms[i].Reset();
	nodeHeader.NodeTms[0].Translate3x4(IFXVector3(0, 0, -1000));
	nodeHeader.NodeTms[1].Translate3x4(IFXVector3(20, -10, -1000));

	TEST(IFXSUCCESS(pSGUtils->CreateModelNode(&nodeHeader, uResourceID, TRUE)), IFXException);
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
	  /*IFXShaderLitTexture::WIRE |*/ IFXShaderLitTexture::MATERIAL
	  )), IFXException);

	// Set some sphere properties
	IFXSphereGenerator  *pSphereGenerator = NULL;
	TEST(IFXSUCCESS(pGenerator->QueryInterface( IID_IFXSphereGenerator, (void**)&pSphereGenerator )), IFXException);
	TEST(IFXSUCCESS(pSphereGenerator->SetRadius(20.0f)), IFXException);
	pSphereGenerator->SetResolution( 8 );
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

	IFXSubdivModifier *pSubDiv = NULL;
	IFXSubdivModifier *pSubDivTest = NULL;
	IFXModel        *pModel = NULL;
	IFXModifier    *pModifier = NULL;

	TEST( IFXSUCCESS( pSGUtils->GetModelFromName(L"PickSphere", &pModel) ), IFXException);

	// Create a subdivision modifier with an IFXModifier interface
	TEST( IFXSUCCESS( IFXCreateComponent( CID_IFXSubdivModifier, IID_IFXSubdivModifier, (void**)&pSubDiv ) ), IFXException );
	// Initialize it
	TEST( IFXSUCCESS( pSubDiv->SetSceneGraph( pSGUtils->m_pSceneGraph ) ), IFXException );
	// Add it to the end of the modifier chain associated with the IFXModel
	IFXModifierChain* pModChain = NULL;

	TEST( IFXSUCCESS( pModel->GetModifierChain(&pModChain) ), IFXException );

	// Don't allow double subdivision modifiers
	U32 found = 0;
	U32         ModifierCount = 0;
	U32         CurrentModifier = 0;
	// Find out how many modifiers/generators it contains
	TEST( IFXSUCCESS( pModChain->GetModifierCount(ModifierCount) ), IFXException );

	// Get the first modifier after the IFXAuthorCLODResource
	if (ModifierCount > 1)
		CurrentModifier = 1; // Skip the pCLODResource in the modifier chain

	// Now iterate through all the modifiers looking for the one of interest
	for (; CurrentModifier < ModifierCount; CurrentModifier++)
	{
		TEST( IFXSUCCESS( pModChain->GetModifier(CurrentModifier, pModifier) ), IFXException );

		if (pModifier )
		{
			if ( IFX_OK == pModifier->QueryInterface( IID_IFXSubdivModifier,
				(void**)&pSubDivTest ) )
			{
				IFXRELEASE(pSubDivTest);
				found = 1;
			}
			IFXRELEASE(pModifier);
		}
		TEST( found == 0, IFXException );
		if (1 == found)
			Log( "Another subdivision found...\n" );

	}
	TEST( IFXSUCCESS( pModChain->AddModifier( *pSubDiv ) ), IFXException );
	TEST( IFXSUCCESS( pSubDiv->SetEnable( TRUE ) ), IFXException );
	TEST( IFXSUCCESS( pSubDiv->SetDepth( 3 ) ), IFXException );
	TEST( IFXSUCCESS( pSubDiv->SetAdaptive( TRUE ) ), IFXException );
	TEST( IFXSUCCESS( pSubDiv->SetTension( 10 ) ), IFXException );

	IFXRELEASE(pModChain);
	IFXRELEASE(pSubDiv);

	return IFX_OK;
}