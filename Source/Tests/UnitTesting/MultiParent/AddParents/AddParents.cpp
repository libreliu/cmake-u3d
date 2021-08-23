//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
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

#ifndef ADDPARENTS_H
#include "AddParents.h"
#endif

#define NUM_OF_LM 10
#define NUM_OF_PARENTS  10

IFXRESULT AddParents::Run(U3DSceneUtilities* pSGUtils)
{
	IFXRESULT result = IFX_OK;
	SGNodeHeader ModelNodeHeader;
	U32 uResourceID = 0;
	U32 i = 0;

	TEST(IFXSUCCESS(pSGUtils->InitializeScene()), IFXException);

	//Create and set New CAMERA node
	{
		IFXDECLARELOCAL(IFXView, pViewNode);
		SGNodeHeader ViewNodeHeader;

		wcscpy(ViewNodeHeader.Name, L"CustomCameraNode");
		ViewNodeHeader.NumberOfParents = 1;
		wcscpy(ViewNodeHeader.ParentNames[0], PALETTE_DEFAULT_GROUP);
		for (i = 0; i < NUM_OF_PARENTS; i++)
			ViewNodeHeader.NodeTms[i].Reset();

		TEST(IFXSUCCESS(pSGUtils->CreateCameraNode(&ViewNodeHeader, &pViewNode, TRUE)), IFXException);

		// Set up the camera based on its type
		pViewNode->SetAttributes( 2 );
		TEST(IFXSUCCESS(pViewNode->SetProjection(5.0)), IFXException);
	}

	// Create a model node in the node palette for the sphere
	{
		IFXDECLARELOCAL(IFXModifier, pGenerator);

		wcscpy(ModelNodeHeader.Name, L"Box");
		ModelNodeHeader.NumberOfParents = 1;
		wcscpy(ModelNodeHeader.ParentNames[0], PALETTE_DEFAULT_GROUP);
		for (i = 0; i < NUM_OF_PARENTS; i++) {
			ModelNodeHeader.NodeTms[i].Reset();
			ModelNodeHeader.NodeTms[i].Translate3x4(IFXVector3(0, 0, -250));
		}
		TEST(IFXSUCCESS(CreatePrimitive("Box", CID_IFXBoxGenerator, 0,
			&uResourceID, &pGenerator, pSGUtils->m_pGeneratorPalette, pSGUtils->m_pSceneGraph) ), IFXException);

		TEST(IFXSUCCESS(pSGUtils->CreateModelNode(&ModelNodeHeader, uResourceID, TRUE)), IFXException);
		IFXVector4 Diffuse = IFXVector4(0.5,(F32)0.1,(F32)0.1);
		IFXVector4 Emission = IFXVector4(0.5,(F32)0.5,(F32)0.1);
		//Set material resource
		TEST(IFXSUCCESS(pSGUtils->SetMaterialResource(ModelNodeHeader.Name,
			NULL, //&Ambient, 
			&Diffuse, 
			&Emission, 
			NULL, //&Specular,
			0.5f,
			0.0f,
			TRUE,
			IFXShaderLitTexture::MATERIAL
			)), IFXException);
	}

	// Create a model node in the node palette for the sphere
	{
		IFXDECLARELOCAL(IFXModifier, pGenerator);

		wcscpy(ModelNodeHeader.Name, L"Sphere");
		ModelNodeHeader.NumberOfParents = 1;
		wcscpy(ModelNodeHeader.ParentNames[0], PALETTE_DEFAULT_GROUP);
		for (i = 0; i < NUM_OF_PARENTS; i++) {
			ModelNodeHeader.NodeTms[i].Reset();
			ModelNodeHeader.NodeTms[i].Translate3x4(IFXVector3(50, 0, -200));
		}
		TEST(IFXSUCCESS(CreatePrimitive("Sphere", CID_IFXSphereGenerator, 0,
			&uResourceID, &pGenerator, pSGUtils->m_pGeneratorPalette, pSGUtils->m_pSceneGraph) ), IFXException);

		TEST(IFXSUCCESS(pSGUtils->CreateModelNode(&ModelNodeHeader, uResourceID, TRUE)), IFXException);
		IFXVector4 Diffuse = IFXVector4(0.5,(F32)0.1,(F32)0.1);
		IFXVector4 Emission = IFXVector4(0.5,(F32)0.5,(F32)0.1);
		//Set material resource
		TEST(IFXSUCCESS(pSGUtils->SetMaterialResource(ModelNodeHeader.Name,
			NULL, //&Ambient, 
			&Diffuse, 
			&Emission, 
			NULL, //&Specular,
			0.5f,
			0.0f,
			TRUE,
			IFXShaderLitTexture::MATERIAL
			)), IFXException);
	}

	{
		IFXDECLARELOCAL(IFXModifier, pGenerator);

		wcscpy(ModelNodeHeader.Name, L"Cylinder1");
		ModelNodeHeader.NumberOfParents = 3;
		wcscpy(ModelNodeHeader.ParentNames[0], L"Box");
		wcscpy(ModelNodeHeader.ParentNames[1], L"Sphere");
		wcscpy(ModelNodeHeader.ParentNames[2], L"Box");
		for (i = 0; i < NUM_OF_PARENTS; i++)
			ModelNodeHeader.NodeTms[i].Reset();

		TEST(IFXSUCCESS(CreatePrimitive("Cylinder1", CID_IFXCylinderGenerator, 0,
			&uResourceID, &pGenerator, pSGUtils->m_pGeneratorPalette, pSGUtils->m_pSceneGraph) ), IFXException);

		TEST(IFXSUCCESS(pSGUtils->CreateModelNode(&ModelNodeHeader, uResourceID, TRUE)), IFXException);
		IFXVector4 Diffuse = IFXVector4(0.5,(F32)1,(F32)1);
		IFXVector4 Emission = IFXVector4(0.5,(F32)0.5,(F32)0.1);
		//Set material resource
		TEST(IFXSUCCESS(pSGUtils->SetMaterialResource(ModelNodeHeader.Name,
			NULL, //&Ambient, 
			&Diffuse, 
			&Emission, 
			NULL, //&Specular,
			0.5f,
			0.0f,
			TRUE,
			IFXShaderLitTexture::MATERIAL
			)), IFXException);
	}

	{
		SGNodeHeader GroupNodeHeader;

		wcscpy(GroupNodeHeader.Name, L"Group1");
		GroupNodeHeader.NumberOfParents = 2;
		wcscpy(GroupNodeHeader.ParentNames[0], L"Cylinder1");
		wcscpy(GroupNodeHeader.ParentNames[1], L"Box");
		for (i = 0; i < NUM_OF_PARENTS; i++) {
			GroupNodeHeader.NodeTms[i].Reset();
			GroupNodeHeader.NodeTms[i].Translate3x4(IFXVector3(50, 0, 0));
		}
		TEST(IFXSUCCESS(pSGUtils->CreateGroupNode(&GroupNodeHeader, TRUE)), IFXException);
	}

	{
		IFXDECLARELOCAL(IFXModifier, pGenerator);

		wcscpy(ModelNodeHeader.Name, L"Sphere2");
		ModelNodeHeader.NumberOfParents = 1;
		wcscpy(ModelNodeHeader.ParentNames[0], L"Group1");
		for (i = 0; i < NUM_OF_PARENTS; i++) {
			ModelNodeHeader.NodeTms[i].Reset();
			ModelNodeHeader.NodeTms[i].Translate3x4(IFXVector3(0, 10, 0));
		}
		TEST(IFXSUCCESS(CreatePrimitive("Sphere2", CID_IFXSphereGenerator, 0,
			&uResourceID, &pGenerator, pSGUtils->m_pGeneratorPalette, pSGUtils->m_pSceneGraph) ), IFXException);

		TEST(IFXSUCCESS(pSGUtils->CreateModelNode(&ModelNodeHeader, uResourceID, TRUE)), IFXException);
		IFXVector4 Diffuse = IFXVector4(0.5,(F32)0.1,(F32)0.1);
		IFXVector4 Emission = IFXVector4(0.5,(F32)0.5,(F32)0.1);
		//Set material resource
		TEST(IFXSUCCESS(pSGUtils->SetMaterialResource(ModelNodeHeader.Name,
			NULL, //&Ambient, 
			&Diffuse, 
			&Emission, 
			NULL, //&Specular,
			0.5f,
			0.0f,
			TRUE,
			IFXShaderLitTexture::MATERIAL
			)), IFXException);
	}

	// Create Light node with resource
	{
		IFXDECLARELOCAL(IFXPalette, pLightResourcePalette);
		IFXDECLARELOCAL(IFXSceneGraph, pSceneGraph);
		IFXDECLARELOCAL(IFXLightResource, pLightResource);
		IFXDECLARELOCAL(IFXLight, pLight);
		SGNodeHeader LightNodeHeader;
		IFXVector4 colorGreen(1.0f, 2.0f, 1.0f);

		wcscpy(LightNodeHeader.Name, L"Light1");
		LightNodeHeader.NumberOfParents = 2;
		wcscpy(LightNodeHeader.ParentNames[0], L"Box");
		wcscpy(LightNodeHeader.ParentNames[1], L"Sphere2");
		for (i = 0; i < NUM_OF_PARENTS; i++)
			LightNodeHeader.NodeTms[i].Reset();
		TEST(IFXSUCCESS(pSGUtils->CreateLightNode(&LightNodeHeader, &pLight, TRUE)), IFXException);

		// Get the light resource palette
		TEST(IFXSUCCESS(pSGUtils->m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph )), IFXException);
		TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::LIGHT, &pLightResourcePalette )), IFXException);

		U32 uLightResourceId = 0;

		// create a light resource and add the light resource to the palette
		TEST(IFXSUCCESS(pLightResourcePalette->Add( LightNodeHeader.Name, &uLightResourceId )), IFXException);
		TEST(IFXSUCCESS(IFXCreateComponent( CID_IFXLightResource, IID_IFXLightResource, (void**)&pLightResource )), IFXException);
		
		TEST(IFXSUCCESS(pLightResource->SetSceneGraph( pSceneGraph )), IFXException);
		TEST(IFXSUCCESS(pLightResourcePalette->SetResourcePtr( uLightResourceId, pLightResource )), IFXException);

		// Link the light node to the light resource pallete entry
		TEST(IFXSUCCESS(pLight->SetLightResourceID(uLightResourceId)), IFXException);
		// Ambient light
		pLightResource->SetType(IFXLightResource::SPOT);
		pLightResource->SetColor(colorGreen);
	}

	//locate nodes to specific places
	if (pSGUtils->m_pNodePalette) {
		IFXDECLARELOCAL(IFXNode, pMyNode);
		U32 myModelID = 0;
		IFXMatrix4x4 localMatrix;

		// Get our model
		TEST(IFXSUCCESS(pSGUtils->m_pNodePalette->Find(L"Cylinder1", &myModelID)), IFXException);
		TEST(IFXSUCCESS(pSGUtils->m_pNodePalette->GetResourcePtr(myModelID, IID_IFXNode, (void **)&pMyNode)), IFXException);

		//local matrix related with parent 1
		if (pMyNode) {
			localMatrix = pMyNode->GetMatrix(0);
			localMatrix.Translate3x4(IFXVector3(-60, 0, 0));
			TEST(IFXSUCCESS(pMyNode->SetMatrix(0, &localMatrix)), IFXException);

			//local matrix related with parent 2
			localMatrix = pMyNode->GetMatrix(1);
			localMatrix.Translate3x4(IFXVector3(60, 0, 0));
			TEST(IFXSUCCESS(pMyNode->SetMatrix(1, &localMatrix)), IFXException);

			//local matrix related with parent 2
			localMatrix = pMyNode->GetMatrix(2);
			localMatrix.Translate3x4(IFXVector3(0, 60, 0));
			TEST(IFXSUCCESS(pMyNode->SetMatrix(2, &localMatrix)), IFXException);
		}
	}

	return result;
}
