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

#ifndef SAMPLESHADERS_H
#include "SampleShaders.h"
#endif

#include "IFXModifierChain.h"
#include "IFXSphereGenerator.h"

#define	_SHADER_SET_INDEX	0

IFXRESULT SampleShaders::Run( U3DSceneUtilities* pSGUtils )
{
	U32 uID = 0;
	SGNodeHeader nh;
	IFXLight *pLight = NULL;
	IFXModifier     *pGenerator = NULL;
	U32           uResourceID = 0;
	U32           uCloneID  = 0;
	struct SGNodeHeader   nodeHeader;
	IFXPalette        *pShaderPalette = NULL;
	U32           uShaderID = 0;
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
	// Create a light node with this name
	printf("Light %s \t Parent %s\n", nh.Name, nh.ParentName);
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

	wcscpy(nodeHeader.Name, L"All");
	wcscpy(nodeHeader.ParentName, PALETTE_DEFAULT_GROUP);
	nodeHeader.NodeTm.Reset();
	nodeHeader.NodeTm.Translate3x4(IFXVector3(0, 0, -1000));
	pSGUtils->CreateGroupNode( &nodeHeader );



	// --- Create a sphere primitive to indicate the picking location
	TEST(IFXSUCCESS(CreatePrimitive("PickSphere", CID_IFXSphereGenerator, 0,
	&uResourceID, &pGenerator, pSGUtils->m_pGeneratorPalette, pSGUtils->m_pSceneGraph) ), IFXException);

	// Create a model node in the node palette for the sphere
	wcscpy(nodeHeader.Name, L"PickSphere");
	wcscpy(nodeHeader.ParentName, L"All");
	nodeHeader.NodeTm.Reset();
	//  nodeHeader.NodeTm.Translate3x4(IFXVector3(0, 0, -1000));

	TEST(IFXSUCCESS(pSGUtils->CreateModelNode(&nodeHeader, uResourceID)), IFXException);

	//  CIFXGroup* pGroup = NULL;
	//  pSGUtils->GetIFXNodeByIndex( IFXSceneGraph::NODE, IID_IFXGroup, (void**)&pGroup, TRUE );

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



	IFXShader       *pMShader = NULL;
	IFXModel        *pModel = NULL;
	IFXShaderLitTexture   *pShaderLitTexture = NULL;

	TEST(IFXSUCCESS(pSGUtils->GetModelFromName(L"PickSphere", &pModel)), IFXException);


	U32       uNumShaders = 0;
	IFXUnknown    *pUnknown = NULL;
	IFXShaderList  *pShaderList = NULL;

	// Get the first shader set from the model's modifier chain.
	{
		IFXRESULT	result	= IFX_OK;

		IFXDECLARELOCAL(IFXModifierChain, pModifierChain);

		if ( IFXSUCCESS( result ) )
			result = pModel->GetModifierChain( &pModifierChain );

		IFXDECLARELOCAL(IFXModifierDataPacket, pDataPacket);

		if ( IFXSUCCESS( result ) )
			result = pModifierChain->GetDataPacket( pDataPacket );

		IFXDECLARELOCAL(IFXModifierDataElementIterator, pRenderableIterator);

		if ( IFXSUCCESS( result ) )
			result = pDataPacket->GetIterator( IFX_DID_RENDERABLE, &pRenderableIterator );

		if ( IFXSUCCESS( result ) )
		{
			IFXUnknown	*pUnknownNR		= ( IFXUnknown* ) pRenderableIterator->First();

			if ( pUnknownNR )
			{
				IFXDECLARELOCAL( IFXRenderable, pRenderable );

				result = pUnknownNR->QueryInterface( 
										IID_IFXRenderable,
										( void** ) &pRenderable );

				if ( IFXSUCCESS( result ) )
				{
					U32	elementCount	= pRenderable->GetNumElements();

					if ( _SHADER_SET_INDEX < elementCount )
					{
						result = pRenderable->GetElementShaderList( _SHADER_SET_INDEX, &pShaderList );

						if ( IFXSUCCESS( result ) )
							pDataPacket->InvalidateDataElement( pRenderableIterator->CurIndex() );
					}
					else
						result = IFX_E_INVALID_RANGE;
				}
			}
		}

		TEST( IFXSUCCESS( result ), IFXException );
	}

	uNumShaders = pShaderList->GetNumShaders();

	if (uNumShaders > 0)
	TEST(IFXSUCCESS(pShaderList->GetShader(0, &uShaderID)), IFXException);
	TEST(IFXSUCCESS(pSGUtils->m_pShaderPalette->GetResourcePtr(uShaderID, &pUnknown)), IFXException);
	TEST(IFXSUCCESS(pUnknown->QueryInterface(IID_IFXShader, (void**) &pMShader)), IFXException);
	IFXRELEASE(pUnknown);

	TEST(IFXSUCCESS(pMShader->QueryInterface(  IID_IFXShaderLitTexture,
	(void**)&pShaderLitTexture )), IFXException);
	//  IFXRenderContext* pr;
	//  TEST(IFXSUCCESS(pMShader->QueryInterface(  IID_IFXRenderContext,
	//    (void**)&pr )), IFXException);
	IFXRELEASE(pShader);
	IFXPalette* pPalette;
	TEST(IFXSUCCESS(pSGUtils->m_pSceneGraph->GetPalette( IFXSceneGraph::MATERIAL, &pPalette )), IFXException);
	pUnknown = NULL;
	TEST(IFXSUCCESS(pPalette->GetResourcePtr( pShaderLitTexture->GetMaterialID(), &pUnknown )), IFXException);
	IFXMaterialResource* pMaterialResource = NULL;
	TEST(IFXSUCCESS(pUnknown->QueryInterface(IID_IFXMaterialResource, (void**)&pMaterialResource)), IFXException);
	TEST(IFXSUCCESS(pMaterialResource->SetOpacity((F32)0.99)), IFXException);
	TEST(IFXSUCCESS(pMaterialResource->SetReflectivity(1)), IFXException);
	IFXVector4 r = IFXVector4(1,(F32)0.1,(F32)0.1);
	IFXVector4 g = IFXVector4(0,1,0);
	TEST(IFXSUCCESS(pMaterialResource->SetEmission(r)), IFXException);
	TEST(IFXSUCCESS(pMaterialResource->SetSpecular(g)), IFXException);
	IFXRenderMaterial pMaterial = pShaderLitTexture->GetRenderMaterial();
	//    pMaterial.SetRenderStyle(IFX_WIREFRAME);
	TEST(IFXSUCCESS(pShaderLitTexture->SetFlags(IFXShaderLitTexture::WIRE | IFXShaderLitTexture::MATERIAL)), IFXException);
	IFXRELEASE(pShaderLitTexture);
	IFXRELEASE(pShader);



	// --- Create a sphere primitive to indicate the picking location
	TEST(IFXSUCCESS(CreatePrimitive("Sphere2", CID_IFXSphereGenerator, 0,
	&uResourceID, &pGenerator, pSGUtils->m_pGeneratorPalette, pSGUtils->m_pSceneGraph) ), IFXException);

	// Create a model node in the node palette for the sphere
	wcscpy(nodeHeader.Name, L"Sphere2");
	wcscpy(nodeHeader.ParentName, L"All");
	nodeHeader.NodeTm.Reset();
	nodeHeader.NodeTm.Translate3x4(IFXVector3(100, 0, -1000));

	TEST(IFXSUCCESS(pSGUtils->CreateModelNode(&nodeHeader, uResourceID)), IFXException);

	//  CIFXGroup* pGroup = NULL;
	//  pSGUtils->GetIFXNodeByIndex( IFXSceneGraph::NODE, IID_IFXGroup, (void**)&pGroup, TRUE );

	// Set some sphere properties
	TEST(IFXSUCCESS(pGenerator->QueryInterface( IID_IFXSphereGenerator, (void**)&pSphereGenerator )), IFXException);
	TEST(IFXSUCCESS(pSphereGenerator->SetRadius(20.0f)), IFXException);
	IFXRELEASE(pSphereGenerator);
	IFXRELEASE(pGenerator);
	// Access the shader and turn on line rendering for the default
	// shader so we can see the bounding sphere.

	/*  // Get the shader palette
	pShaderPalette = pSGUtils->m_pShaderPalette;
	TEST(pShaderPalette != NULL, IFXException);

	// Get the Shader on this model
	TEST(IFXSUCCESS(pShaderPalette->Find("DefaultShader", &uShaderID)), IFXException);
	TEST(IFXSUCCESS(pShaderPalette->GetResourcePtr(uShaderID, IID_IFXShaderLitTexture, (void **)&pShader)), IFXException);
	pShader->SetDrawLines(TRUE);
	pShader->SetLightingEnabled(TRUE);
	*/
	TEST(IFXSUCCESS(pSGUtils->GetModelFromName(L"Sphere2", &pModel)), IFXException);

	// Get the first shader set from the model's modifier chain.
	{
		IFXRESULT	result	= IFX_OK;

		IFXDECLARELOCAL(IFXModifierChain, pModifierChain);

		if ( IFXSUCCESS( result ) )
			result = pModel->GetModifierChain( &pModifierChain );

		IFXDECLARELOCAL(IFXModifierDataPacket, pDataPacket);

		if ( IFXSUCCESS( result ) )
			result = pModifierChain->GetDataPacket( pDataPacket );

		IFXDECLARELOCAL(IFXModifierDataElementIterator, pRenderableIterator);

		if ( IFXSUCCESS( result ) )
			result = pDataPacket->GetIterator( IFX_DID_RENDERABLE, &pRenderableIterator );

		if ( IFXSUCCESS( result ) )
		{
			IFXUnknown	*pUnknownNR		= ( IFXUnknown* ) pRenderableIterator->First();

			if ( pUnknownNR )
			{
				IFXDECLARELOCAL( IFXRenderable, pRenderable );

				result = pUnknownNR->QueryInterface( 
										IID_IFXRenderable,
										( void** ) &pRenderable );

				if ( IFXSUCCESS( result ) )
				{
					U32	elementCount	= pRenderable->GetNumElements();

					if ( _SHADER_SET_INDEX < elementCount )
					{
						result = pRenderable->GetElementShaderList( _SHADER_SET_INDEX, &pShaderList );

						if ( IFXSUCCESS( result ) )
							pDataPacket->InvalidateDataElement( pRenderableIterator->CurIndex() );
					}
					else
						result = IFX_E_INVALID_RANGE;
				}
			}
		}

		TEST( IFXSUCCESS( result ), IFXException );
	}

	IFXShader* s2Shader = NULL;
	IFXShaderLitTexture* ps2ShaderLT = NULL;
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderLitTexture, IID_IFXShader, (void**)&s2Shader)), IFXException);
	TEST(IFXSUCCESS(s2Shader->QueryInterface(  IID_IFXShaderLitTexture,
	(void**)&ps2ShaderLT )), IFXException);
	TEST(IFXSUCCESS(ps2ShaderLT->SetFlags(IFXShaderLitTexture::FLAT | IFXShaderLitTexture::MATERIAL)), IFXException);
	U32 idx = 0;
	TEST(IFXSUCCESS(pShaderPalette->Add(L"Sphere2Shader", &idx)), IFXException);
	TEST(IFXSUCCESS(pShaderPalette->SetResourcePtr( idx, s2Shader )), IFXException);

	TEST(IFXSUCCESS(uNumShaders = pShaderList->GetNumShaders()), IFXException);
	pShaderList->SetNumShaders( 2 );

	//  idx = 1;
	//  TEST(IFXSUCCESS(pShaderList->SetShader(0,idx)), IFXException);
	TEST(IFXSUCCESS(uNumShaders = pShaderList->GetNumShaders()), IFXException);

	//  if (uNumShaders > 0)
	//    TEST(IFXSUCCESS(pShaderList->GetShader(0, &uShaderID)), IFXException);
	TEST(IFXSUCCESS(pSGUtils->m_pShaderPalette->GetResourcePtr(idx, &pUnknown)), IFXException);
	TEST(IFXSUCCESS(pUnknown->QueryInterface(IID_IFXShader, (void**) &pMShader)), IFXException);
	IFXRELEASE(pUnknown);

	TEST(IFXSUCCESS(pMShader->QueryInterface(  IID_IFXShaderLitTexture,
	(void**)&pShaderLitTexture )), IFXException);
	//  TEST(IFXSUCCESS(pMShader->QueryInterface(  IID_IFXRenderContext,
	//    (void**)&pr )), IFXException);
	IFXRELEASE(pShader);
	TEST(IFXSUCCESS(pSGUtils->m_pSceneGraph->GetPalette( IFXSceneGraph::MATERIAL, &pPalette )), IFXException);
	pUnknown = NULL;
	TEST(IFXSUCCESS(pPalette->GetResourcePtr( pShaderLitTexture->GetMaterialID(), &pUnknown )), IFXException);
	TEST(IFXSUCCESS(pUnknown->QueryInterface(IID_IFXMaterialResource, (void**)&pMaterialResource)), IFXException);
	TEST(IFXSUCCESS(pMaterialResource->SetOpacity((F32)0.99)), IFXException);
	TEST(IFXSUCCESS(pMaterialResource->SetReflectivity(1)), IFXException);
	TEST(IFXSUCCESS(pMaterialResource->SetEmission(r)), IFXException);
	TEST(IFXSUCCESS(pMaterialResource->SetSpecular(g)), IFXException);
	//    pMaterial.SetRenderStyle(IFX_WIREFRAME);
	TEST(IFXSUCCESS(pShaderLitTexture->SetFlags(IFXShaderLitTexture::FLAT | IFXShaderLitTexture::MATERIAL)), IFXException);
	IFXRELEASE(pShaderLitTexture);
	IFXRELEASE(pShader);

	return IFX_OK;
}
