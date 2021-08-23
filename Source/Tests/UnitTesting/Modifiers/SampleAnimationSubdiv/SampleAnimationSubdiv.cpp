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

#include "UTHeaders.h"

#ifndef SampleAnimationSubdiv_H
#include "SampleAnimationSubdiv.h"
#endif

#include "IFXSphereGenerator.h"
#include "IFXModifierChain.h"
#include "IFXShadingModifier.h"

#include "IFXStdio.h"
#include "IFXTextureObject.h"
#include "IFXWriteManager.h"
#include "IFXLoadManager.h"
#include "IFXWriteBuffer.h"
#include "IFXReadBuffer.h"

#include "IFXMixerConstruct.h"
#include "IFXMotionResource.h"
#include "IFXAnimationModifier.h"
#include "IFXSubdivModifier.h"

#include "IFXNameMap.h"

IFXRESULT SampleAnimationSubdiv::Run( U3DSceneUtilities* pSGUtils )
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

	IFXDECLARELOCAL(IFXWriteManager, pIFXWriteManager);
	IFXDECLARELOCAL(IFXWriteBuffer, pIFXWriteBuffer);
	IFXDECLARELOCAL(IFXStdio, pIFXStdio);

	////////////////////////////////////////

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
	wcscpy(nodeHeader.ParentName, PALETTE_DEFAULT_GROUP);
	nodeHeader.NodeTm.Reset();
	nodeHeader.NodeTm.Translate3x4(IFXVector3(0, 0, -1000));

	TEST(IFXSUCCESS(pSGUtils->CreateModelNode(&nodeHeader, uResourceID)), IFXException);
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

	InitSubDivModifier(pSGUtils,L"PickSphere");
	InitAnimationModifier(pSGUtils,L"PickSphere");//L"Light1");


	/*
	/////////////////////////////////////////// create motion resource

	{
	IFXDECLARELOCAL(IFXUnknown,pObject);
	TEST(IFXSUCCESS(IFXCreateComponent( CID_IFXMotionResource, IID_IFXUnknown, (void**)&pObject )), IFXException);

	IFXDECLARELOCAL(IFXMarker,pMarker );
	TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXMarker, (void**)&pMarker )), IFXException);

	IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
	TEST(IFXSUCCESS(pSGUtils->m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph )), IFXException);
	TEST(IFXSUCCESS(pMarker->SetSceneGraph( pSceneGraph )), IFXException);

	IFXString sName("PickSphere-Key");

	IFXDECLARELOCAL(IFXPalette,pSGPalette);
	TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::MOTION, &pSGPalette )), IFXException);

	U32 iResultPaletteFind = pSGPalette->Find( &sName, &uResourceID );
	if ( IFX_E_CANNOT_FIND == iResultPaletteFind  ) 
	{
	TEST(IFXSUCCESS(pSGPalette->Add( &sName, &uResourceID )), IFXException);
	}

	TEST(IFXSUCCESS(pSGPalette->SetResourcePtr( uResourceID, pObject )), IFXException);

	IFXDECLARELOCAL( IFXMotionResource, pMotionResource );
	TEST(IFXSUCCESS( pObject->QueryInterface( IID_IFXMotionResource, (void**)&pMotionResource )), IFXException )

	pMotionResource->GetMotionRef()->SetName(sName);


	U32 uTrackCount = 1;

	IFXString trackName("PickSphere");
	U32 uTrackID = 0;
	IFXRESULT iFindResult = pMotionResource->FindTrack( &trackName, &uTrackID );
	if( IFXFAILURE( iFindResult ) )    
	{
	TEST(IFXSUCCESS(pMotionResource->AddTrack( &trackName, &uTrackID )), IFXException);
	}

	// 6. TimeCount
	U32 uTimeCount = 5;
	IFXKeyFrame *pKeyFrameArray = NULL;
	pKeyFrameArray = new IFXKeyFrame[uTimeCount];
	if( NULL == pKeyFrameArray ) 
	{
	TEST(IFXSUCCESS(IFX_E_OUT_OF_MEMORY), IFXException);
	}

	IFXQuaternion vRotation(1.0f,0.0f,0.0f,0.0f);
	IFXQuaternion vRotation1(1.0f,0.0f,2.0f,-1.0f);
	IFXQuaternion vRotation2(1.0f,0.0f,-2.0f,1.0f);
	IFXVector3 vScaleQ(1.0f,1.0f,1.0f);
	IFXVector3 vScaleQ1(1.0f,1.5f,0.1f);

	pKeyFrameArray[0].SetTime( 0.0f );
	pKeyFrameArray[0].Location() = IFXVector3( 0.0f, 0.0f, 0.0f );
	pKeyFrameArray[0].Rotation() = vRotation;
	pKeyFrameArray[0].Scale() = vScaleQ;

	pKeyFrameArray[1].SetTime( 0.9001f );
	pKeyFrameArray[1].Location() = IFXVector3( 25.0f, 0.0f, 0.0f );
	pKeyFrameArray[1].Rotation() = vRotation1;
	pKeyFrameArray[1].Scale() = vScaleQ1;

	pKeyFrameArray[2].SetTime( 1.6682f );
	pKeyFrameArray[2].Location() = IFXVector3( 0.0f, 25.0f, 25.0f );
	pKeyFrameArray[2].Rotation() = vRotation2;
	pKeyFrameArray[2].Scale() = vScaleQ;

	pKeyFrameArray[3].SetTime( 2.4663f );
	pKeyFrameArray[3].Location() = IFXVector3( 0.0f, 0.0f, 25.0f );
	pKeyFrameArray[3].Rotation() = vRotation;
	pKeyFrameArray[3].Scale() = vScaleQ1;

	pKeyFrameArray[4].SetTime( 3.3333f );
	pKeyFrameArray[4].Location() = IFXVector3( 0.0f, 0.0f, 0.0f );
	pKeyFrameArray[4].Rotation() = vRotation;
	pKeyFrameArray[4].Scale() = vScaleQ;

	// Set the key frame array to the motion resource
	TEST(IFXSUCCESS(pMotionResource->ClearTrack( uTrackID )), IFXException);
	TEST(IFXSUCCESS(pMotionResource->InsertKeyFrames( uTrackID,uTimeCount,pKeyFrameArray )), IFXException);
	IFXDELETE_ARRAY( pKeyFrameArray );

	// Now, add a fully-mapped mixer to the mixer palette for this motion
	U32 uMixerID = 0;
	IFXDECLARELOCAL(IFXPalette,pMixerPalette);
	TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::MIXER, &pMixerPalette )), IFXException);
	iFindResult = pMixerPalette->Find( &sName,&uMixerID );
	// if not found, add it to the palette
	if ( IFXFAILURE( iFindResult ) ) 
	{
	TEST(IFXSUCCESS(pMixerPalette->Add( &sName,&uMixerID )), IFXException);
	}

	// now, try to get the motion resource pointer
	IFXDECLARELOCAL(IFXMixerConstruct, pMixerConstruct );
	IFXRESULT iGetResourceResult = pMixerPalette->GetResourcePtr(uMixerID,IID_IFXMixerConstruct,(void**)&pMixerConstruct);
	// if there was no resource pointer, then create one
	if (IFXFAILURE(iGetResourceResult)) 
	{
	TEST(IFXSUCCESS(IFXCreateComponent( CID_IFXMixerConstruct, IID_IFXMixerConstruct,( void** )&pMixerConstruct )), IFXException);
	IFXASSERT(pMotionResource);
	pMixerConstruct->SetMotionResource(pMotionResource);

	// Set the scenegraph
	TEST(IFXSUCCESS(pMixerConstruct->SetSceneGraph( pSceneGraph )), IFXException);

	// set the resource pointer in the palette
	IFXDECLARELOCAL(IFXUnknown,pUnknown);
	TEST(IFXSUCCESS(pMixerConstruct->QueryInterface(IID_IFXUnknown, (void**)&pUnknown)), IFXException);
	TEST(IFXSUCCESS(pMixerPalette->SetResourcePtr( uMixerID,pUnknown )), IFXException);
	}
	}

	/////////////////////////////////////////// create animation modifier

	IFXDECLARELOCAL(IFXModifierChain,pModifierChain);

	{
	IFXString sName("Light1");

	//              IFXCID cObjectCID = CID_IFXKeyFrameModifier;

	IFXDECLARELOCAL(IFXUnknown,pObject);
	TEST(IFXSUCCESS(IFXCreateComponent( CID_IFXAnimationModifier, IID_IFXUnknown, (void**)&pObject )), IFXException);

	IFXDECLARELOCAL(IFXMarker,pMarker );
	TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXMarker, (void**)&pMarker )), IFXException);

	IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
	TEST(IFXSUCCESS(pSGUtils->m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph )), IFXException);
	TEST(IFXSUCCESS(pMarker->SetSceneGraph( pSceneGraph )), IFXException);

	IFXModifier *pMod = NULL;
	TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXModifier, (void**)&pMod)), IFXException);

	IFXAnimationModifier *pAnimMod = NULL;
	TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXAnimationModifier, (void**)&pAnimMod)), IFXException);

	pAnimMod->SetAsKeyframe();

	{
	U32 uResourceID;

	IFXDECLARELOCAL(IFXPalette,pSGPalette);
	TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::NODE, &pSGPalette )), IFXException);

	U32 iResultPaletteFind = pSGPalette->Find( &sName, &uResourceID );
	if ( IFX_E_CANNOT_FIND == iResultPaletteFind  ) 
	{
	TEST(IFXSUCCESS(pSGPalette->Add( &sName, &uResourceID )), IFXException);
	}

	// If this is a chain object
	{
	// get the modifier at the head of the chain
	IFXDECLARELOCAL(IFXModifier,pHeadModifier);
	U32 iResultGetResource = pSGPalette->GetResourcePtr(uResourceID,IID_IFXModifier,(void**)&pHeadModifier);

	// Get the modifier chain from the head modifier
	TEST(IFXSUCCESS(pHeadModifier->GetModifierChain(&pModifierChain)), IFXException);

	// Put the object (the new modifier) into the modifier chain
	IFXDECLARELOCAL(IFXModifier,pObjectAsModifier);
	TEST(IFXSUCCESS(pMod->QueryInterface( IID_IFXModifier, (void**)&pObjectAsModifier )), IFXException);
	TEST(IFXSUCCESS(pModifierChain->AddModifier( *pObjectAsModifier, IFXModifierChain::END_OF_MODCHAIN, FALSE )), IFXException);

	U32 uModCount = 0;
	TEST(IFXSUCCESS( pModifierChain->GetModifierCount( uModCount )) , IFXException);
	}
	}

	IFXAnimationModifier *pAM = NULL;
	TEST(IFXSUCCESS(pMod->QueryInterface( IID_IFXAnimationModifier, (void**)&pAM)), IFXException);

	pAM->Playing() = TRUE;
	pAM->RootLock() = FALSE;
	pAM->AutoBlend() = TRUE;

	pAM->TimeScale() = 1.0f;

	IFXDECLARELOCAL(IFXPalette,pMixerPalette);
	TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::MIXER, &pMixerPalette )), IFXException);

	IFXString mixerName("PickSphere-Key");

	U32 id = 0 ;
	IFXRESULT iResultFind = pMixerPalette->Find( &mixerName, &id );
	if (IFXFAILURE(iResultFind)) 
	{
	TEST(IFXSUCCESS(pMixerPalette->Add( &mixerName, &id )), IFXException);
	}

	F32 fVal = 1.0f;
	BOOL bTrue = TRUE;
	BOOL bFalse = FALSE;
	TEST(IFXSUCCESS(pAM->Queue(mixerName, NULL, NULL, NULL, &fVal, &bTrue, 0,1)), IFXException);

	IFXRELEASE(pMixerPalette);

	pAM->BlendTime() = 0.5f;
	pAM->AxisLock() = 0;
	pAM->Concatenation() = 0;

	}

	*/

	/////////////////////////////////////////// create motion resource

	//saving to file
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXWriteManager, IID_IFXWriteManager, (void**)&pIFXWriteManager)), IFXException);
	TEST(IFXSUCCESS(pIFXWriteManager->Initialize(pSGUtils->m_pCoreServices)), IFXException);
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioWriteBuffer, IID_IFXWriteBuffer, (void**)&pIFXWriteBuffer)), IFXException);
	TEST(IFXSUCCESS(pIFXWriteBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio)), IFXException);
	TEST(IFXSUCCESS(pIFXStdio->Open( L"output.u3d" ) ), IFXException);
	TEST(IFXSUCCESS(pSGUtils->m_pSceneGraph->Mark()), IFXException);
	TEST(IFXSUCCESS(pIFXWriteManager->Write(pIFXWriteBuffer, IFXEXPORT_EVERYTHING)), IFXException); // ^ IFXEXPORT_FILEREFERENCES
	TEST(IFXSUCCESS(pIFXStdio->Close()), IFXException);
	return IFX_OK;
}

IFXRESULT SampleAnimationSubdiv::InitAnimationModifier(U3DSceneUtilities *pSGUtils, IFXString ModelName)
{
	U32           uResourceID = 0;
	/////////////////////////////////////////// create motion resource

	{
		IFXDECLARELOCAL(IFXUnknown,pObject);
		TEST(IFXSUCCESS(IFXCreateComponent( CID_IFXMotionResource, IID_IFXUnknown, (void**)&pObject )), IFXException);

		IFXDECLARELOCAL(IFXMarker,pMarker );
		TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXMarker, (void**)&pMarker )), IFXException);

		IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
		TEST(IFXSUCCESS(pSGUtils->m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph )), IFXException);
		TEST(IFXSUCCESS(pMarker->SetSceneGraph( pSceneGraph )), IFXException);

		IFXString sName("PickSphere-Key");

		IFXDECLARELOCAL(IFXPalette,pSGPalette);
		TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::MOTION, &pSGPalette )), IFXException);

		U32 iResultPaletteFind = pSGPalette->Find( &sName, &uResourceID );
		if ( IFX_E_CANNOT_FIND == iResultPaletteFind  ) 
		{
			TEST(IFXSUCCESS(pSGPalette->Add( &sName, &uResourceID )), IFXException);
		}

		TEST(IFXSUCCESS(pSGPalette->SetResourcePtr( uResourceID, pObject )), IFXException);

		IFXDECLARELOCAL( IFXMotionResource, pMotionResource );
		TEST(IFXSUCCESS( pObject->QueryInterface( IID_IFXMotionResource, (void**)&pMotionResource )), IFXException )

			pMotionResource->GetMotionRef()->SetName(sName);


		U32 uTrackCount = 1;

		IFXString trackName("PickSphere");
		U32 uTrackID = 0;
		IFXRESULT iFindResult = pMotionResource->FindTrack( &trackName, &uTrackID );
		if( IFXFAILURE( iFindResult ) )    
		{
			TEST(IFXSUCCESS(pMotionResource->AddTrack( &trackName, &uTrackID )), IFXException);
		}

		// 6. TimeCount
		U32 uTimeCount = 5;
		IFXKeyFrame *pKeyFrameArray = NULL;
		pKeyFrameArray = new IFXKeyFrame[uTimeCount];
		if( NULL == pKeyFrameArray ) 
		{
			//TEST(IFXSUCCESS(IFX_E_OUT_OF_MEMORY), IFXException);
			return IFX_E_OUT_OF_MEMORY;
		}

		IFXQuaternion vRotation(1.0f,0.0f,0.0f,0.0f);
		IFXQuaternion vRotation1(1.0f,0.0f,2.0f,-1.0f);
		IFXQuaternion vRotation2(1.0f,0.0f,-2.0f,1.0f);
		IFXVector3 vScaleQ(1.0f,1.0f,1.0f);
		IFXVector3 vScaleQ1(1.0f,1.5f,0.1f);

		pKeyFrameArray[0].SetTime( 0.0f );
		pKeyFrameArray[0].Location() = IFXVector3( 0.0f, 0.0f, 0.0f );
		pKeyFrameArray[0].Rotation() = vRotation;
		pKeyFrameArray[0].Scale() = vScaleQ;

		pKeyFrameArray[1].SetTime( 0.9001f );
		pKeyFrameArray[1].Location() = IFXVector3( 25.0f, 0.0f, 0.0f );
		pKeyFrameArray[1].Rotation() = vRotation1;
		pKeyFrameArray[1].Scale() = vScaleQ1;

		pKeyFrameArray[2].SetTime( 1.6682f );
		pKeyFrameArray[2].Location() = IFXVector3( 0.0f, 25.0f, 25.0f );
		pKeyFrameArray[2].Rotation() = vRotation2;
		pKeyFrameArray[2].Scale() = vScaleQ;

		pKeyFrameArray[3].SetTime( 2.4663f );
		pKeyFrameArray[3].Location() = IFXVector3( 0.0f, 0.0f, 25.0f );
		pKeyFrameArray[3].Rotation() = vRotation;
		pKeyFrameArray[3].Scale() = vScaleQ1;

		pKeyFrameArray[4].SetTime( 3.3333f );
		pKeyFrameArray[4].Location() = IFXVector3( 0.0f, 0.0f, 0.0f );
		pKeyFrameArray[4].Rotation() = vRotation;
		pKeyFrameArray[4].Scale() = vScaleQ;

		// Set the key frame array to the motion resource
		TEST(IFXSUCCESS(pMotionResource->ClearTrack( uTrackID )), IFXException);
		TEST(IFXSUCCESS(pMotionResource->InsertKeyFrames( uTrackID,uTimeCount,pKeyFrameArray )), IFXException);
		IFXDELETE_ARRAY( pKeyFrameArray );

		// Now, add a fully-mapped mixer to the mixer palette for this motion
		U32 uMixerID = 0;
		IFXDECLARELOCAL(IFXPalette,pMixerPalette);
		TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::MIXER, &pMixerPalette )), IFXException);
		iFindResult = pMixerPalette->Find( &sName,&uMixerID );
		// if not found, add it to the palette
		if ( IFXFAILURE( iFindResult ) ) 
		{
			TEST(IFXSUCCESS(pMixerPalette->Add( &sName,&uMixerID )), IFXException);
		}

		// now, try to get the motion resource pointer
		IFXDECLARELOCAL(IFXMixerConstruct, pMixerConstruct );
		IFXRESULT iGetResourceResult = pMixerPalette->GetResourcePtr(uMixerID,IID_IFXMixerConstruct,(void**)&pMixerConstruct);
		// if there was no resource pointer, then create one
		if (IFXFAILURE(iGetResourceResult)) 
		{
			TEST(IFXSUCCESS(IFXCreateComponent( CID_IFXMixerConstruct, IID_IFXMixerConstruct,( void** )&pMixerConstruct )), IFXException);
			IFXASSERT(pMotionResource);
			pMixerConstruct->SetMotionResource(pMotionResource);

			// Set the scenegraph
			TEST(IFXSUCCESS(pMixerConstruct->SetSceneGraph( pSceneGraph )), IFXException);

			// set the resource pointer in the palette
			IFXDECLARELOCAL(IFXUnknown,pUnknown);
			TEST(IFXSUCCESS(pMixerConstruct->QueryInterface(IID_IFXUnknown, (void**)&pUnknown)), IFXException);
			TEST(IFXSUCCESS(pMixerPalette->SetResourcePtr( uMixerID,pUnknown )), IFXException);
		}
	}

	/////////////////////////////////////////// create animation modifier

	IFXDECLARELOCAL(IFXModifierChain,pModifierChain);

	{
		//  IFXString sName("Light1");

		//              IFXCID cObjectCID = CID_IFXKeyFrameModifier;

		IFXDECLARELOCAL(IFXUnknown,pObject);
		TEST(IFXSUCCESS(IFXCreateComponent( CID_IFXAnimationModifier, IID_IFXUnknown, (void**)&pObject )), IFXException);

		IFXDECLARELOCAL(IFXMarker,pMarker );
		TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXMarker, (void**)&pMarker )), IFXException);

		IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
		TEST(IFXSUCCESS(pSGUtils->m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph )), IFXException);
		TEST(IFXSUCCESS(pMarker->SetSceneGraph( pSceneGraph )), IFXException);

		IFXModifier *pMod = NULL;
		TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXModifier, (void**)&pMod)), IFXException);

		IFXAnimationModifier *pAnimMod = NULL;
		TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXAnimationModifier, (void**)&pAnimMod)), IFXException);

		pAnimMod->SetAsKeyframe();

		{
			U32 uResourceID;

			IFXDECLARELOCAL(IFXPalette,pSGPalette);
			TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::NODE, &pSGPalette )), IFXException);

			U32 iResultPaletteFind = pSGPalette->Find( &ModelName, &uResourceID );
			if ( IFX_E_CANNOT_FIND == iResultPaletteFind  ) 
			{
				TEST(IFXSUCCESS(pSGPalette->Add( &ModelName, &uResourceID )), IFXException);
			}

			// If this is a chain object
			{
				// get the modifier at the head of the chain
				IFXDECLARELOCAL(IFXModifier,pHeadModifier);
				U32 iResultGetResource = pSGPalette->GetResourcePtr(uResourceID,IID_IFXModifier,(void**)&pHeadModifier);

				// Get the modifier chain from the head modifier
				TEST(IFXSUCCESS(pHeadModifier->GetModifierChain(&pModifierChain)), IFXException);

				// Put the object (the new modifier) into the modifier chain
				IFXDECLARELOCAL(IFXModifier,pObjectAsModifier);
				TEST(IFXSUCCESS(pMod->QueryInterface( IID_IFXModifier, (void**)&pObjectAsModifier )), IFXException);
				TEST(IFXSUCCESS(pModifierChain->AddModifier( *pObjectAsModifier, IFXModifierChain::END_OF_MODCHAIN, FALSE )), IFXException);

				U32 uModCount = 0;
				TEST(IFXSUCCESS( pModifierChain->GetModifierCount( uModCount )) , IFXException);
			}
		}

		IFXAnimationModifier *pAM = NULL;
		TEST(IFXSUCCESS(pMod->QueryInterface( IID_IFXAnimationModifier, (void**)&pAM)), IFXException);

		pAM->Playing() = TRUE;
		pAM->RootLock() = FALSE;
		pAM->AutoBlend() = TRUE;

		pAM->TimeScale() = 1.0f;

		IFXDECLARELOCAL(IFXPalette,pMixerPalette);
		TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::MIXER, &pMixerPalette )), IFXException);

		IFXString mixerName("PickSphere-Key");

		U32 id = 0 ;
		IFXRESULT iResultFind = pMixerPalette->Find( &mixerName, &id );
		if (IFXFAILURE(iResultFind)) 
		{
			TEST(IFXSUCCESS(pMixerPalette->Add( &mixerName, &id )), IFXException);
		}

		F32 fVal = 1.0f;
		BOOL bTrue = TRUE;
		BOOL bFalse = FALSE;
		TEST(IFXSUCCESS(pAM->Queue(mixerName, NULL, NULL, NULL, &fVal, &bTrue, 0,1)), IFXException);

		IFXRELEASE(pMixerPalette);

		pAM->BlendTime() = 0.5f;
		pAM->AxisLock() = 0;
		pAM->Concatenation() = 0;

	}
	return IFX_OK;

}

IFXRESULT SampleAnimationSubdiv::InitSubDivModifier(U3DSceneUtilities *pSGUtils, IFXString ModelName)
{
	/////////////////////////////////////////////////////////////
	// Creating and applying SubDiv modifier
	/////////////////////////////////////////////////////////////
	IFXSubdivModifier *pSubDiv = NULL;
	IFXSubdivModifier *pSubDivTest = NULL;
	IFXModel        *pModel = NULL;
	IFXModifier    *pModifier = NULL;

	TEST( IFXSUCCESS( pSGUtils->GetModelFromName(ModelName, &pModel) ), IFXException);

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
	TEST( IFXSUCCESS( pSubDiv->SetDepth( 5 ) ), IFXException );
	TEST( IFXSUCCESS( pSubDiv->SetAdaptive( FALSE ) ), IFXException );
	TEST( IFXSUCCESS( pSubDiv->SetTension( 10 ) ), IFXException );

	IFXRELEASE(pModChain);
	IFXRELEASE(pSubDiv);

	return IFX_OK;


}