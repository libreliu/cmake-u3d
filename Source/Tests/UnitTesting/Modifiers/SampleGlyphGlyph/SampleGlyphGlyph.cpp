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

#ifndef SAMPLEGLYPHGLYPH_H
#include "SampleGlyphGlyph.h"
#endif

#include "IFXSphereGenerator.h"
#include "IFXModifierChain.h"
#include "IFXGlyph2DModifier.h"
#include "IFXGlyphCommandList.h"

#include "IFXMixerConstruct.h"
#include "IFXMotionResource.h"
#include "IFXAnimationModifier.h"

#include "IFXNameMap.h"

void GlyphStamper( IFXGlyphCommandList* pComList, U32 x = 0, U32 y = 0, U32 w = 100, U32 h = 50 )
{
	pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_STARTGLYPH );
	pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_STARTPATH );
	pComList->AddMoveToBlock( x, y );
	pComList->AddLineToBlock( x, y+h );
	pComList->AddLineToBlock( x+w, y+h );
	pComList->AddLineToBlock( x+w, y );
	pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_ENDPATH );
	pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_STARTPATH );
	pComList->AddMoveToBlock( x+3, y+3 );
	pComList->AddLineToBlock( x+w-3, y+3 );
	pComList->AddLineToBlock( x+w-3, y+h-3 );
	pComList->AddLineToBlock( x+3, y+h-3 );
	pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_ENDPATH );
	pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_ENDGLYPH, 70, 20 );
}

IFXRESULT AddGlyph2DModifier(
							 IFXModel * in_pModel,
							 IFXGlyph2DModifier ** out_ppGlyph2DModifier,
							 F64 defaultWidth, F64 defaultSpacing, F64 defaultHeight,
							 IFXGlyphCommandList* pCommandList,
							 IFXModifierChain* pChain = NULL )
{
	IFXRESULT     result = IFX_OK;
	IFXGlyph2DModifier* pGlyph2DModifier = NULL;
	IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );

	if ( !in_pModel || !out_ppGlyph2DModifier)
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
	}

	if ( IFXSUCCESS(result) )
	{
		pGlyph2DModifier->Initialize( defaultWidth, defaultSpacing, defaultHeight );
		IFXSimpleList* pGlyphData = NULL;
		pCommandList->GetList( &pGlyphData );
		pGlyph2DModifier->SetGlyphCommandList( pGlyphData );
		IFXRELEASE( pGlyphData );
	}
	// Add it to the end of the modifier chain associated with the IFXModel
	if ( in_pModel && IFXSUCCESS(result) )
	{
		if( !pChain )
		{
			IFXDECLARELOCAL( IFXModifierChain, pModChain );
			result = in_pModel->GetModifierChain( &pModChain );
			if ( pModChain && pGlyph2DModifier && IFXSUCCESS( result ) )
				result = pModChain->AddModifier( *pGlyph2DModifier );
		}
		else
			result = pChain->AddModifier( *pGlyph2DModifier );
	}

	if ( out_ppGlyph2DModifier && IFXSUCCESS(result) )
	{
		// Releasing pShading is left to the caller
		*out_ppGlyph2DModifier = pGlyph2DModifier;
	}

	return result;
}

IFXRESULT SampleGlyphGlyph::Run( U3DSceneUtilities* pSGUtils )
{
	SGNodeHeader nh;
	U32           uResourceID = 0;
	struct SGNodeHeader   nodeHeader;
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

		const F32 initnode[] = {sqrt(F32(2))/2,sqrt(F32(2))/2,0,0,
			-sqrt(F32(2))/2,sqrt(F32(2))/2,0,0,
			0,0,1,0,
			0,0,2000,1};
		IFXMatrix4x4 tmnode = IFXMatrix4x4( initnode );
		TEST( IFXSUCCESS( pViewNode->SetMatrix( 0, &tmnode ) ), IFXException );

		IFXRELEASE(pViewNode);
	}

	{
		IFXDECLARELOCAL( IFXLight, pLight );
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
			TEST(IFXSUCCESS( IFXCreateComponent( CID_IFXLightResource, IID_IFXLightResource, (void**)&pLightResource ) ), IFXException);
			IFXDECLARELOCAL( IFXUnknown, pUnk );
			TEST(IFXSUCCESS( pLightResource->QueryInterface( IID_IFXUnknown, (void**)&pUnk ) ), IFXException);

			IFXDECLARELOCAL( IFXMarker, pMarker );
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
	}

	IFXDECLARELOCAL( IFXModel, pModel );
	{
		IFXDECLARELOCAL( IFXModifier, pGenerator );

		{
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
			TEST(IFXSUCCESS(pSphereGenerator->SetResolution(6)), IFXException);
		
			IFXRELEASE(pSphereGenerator);
		}

		TEST( IFXSUCCESS( pSGUtils->GetModelFromName(L"PickSphere", &pModel) ), IFXException);

		{
			IFXGlyph2DModifier   *pGlyph = NULL;
			IFXDECLARELOCAL( IFXGlyphCommandList, pComList );
			IFXCreateComponent( CID_IFXGlyphCommandList, IID_IFXGlyphCommandList, (void**)&pComList );
			pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_STARTGLYPHSTRING );
			GlyphStamper( pComList, 10, 10, 150, 70 );
	//		GlyphStamper( pComList, -20, -20, 150,70 );

			pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_ENDGLYPHSTRING );
			IFXDECLARELOCAL( IFXModifierChain, pChain );
			pGenerator->GetModifierChain( &pChain );
	//		AddGlyph2DModifierToChain( pChain, pModel, 10, 10, 10, pComList );
			AddGlyph2DModifier( pModel, &pGlyph, 10, 10, 10, pComList, pChain );
			IFXMatrix4x4 offset;
			offset.MakeIdentity();
			offset[12] = -17;
			offset[13] = -23;
			offset[14] = -30;
			pGlyph->SetTransform( offset );
			pGlyph->SetAttributes( IFXGlyph2DModifier::BILLBOARD );
			IFXRELEASE(pGlyph);
		}
	}

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

  /////////////////////////////////////////// create motion resource

  {
    IFXDECLARELOCAL( IFXMotionResource, pMotionResource );
    IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);

    TEST(IFXSUCCESS(IFXCreateComponent( CID_IFXMotionResource, IID_IFXMotionResource, (void**)&pMotionResource )), IFXException);
    TEST(IFXSUCCESS(pSGUtils->m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph )), IFXException);
    TEST(IFXSUCCESS(pMotionResource->SetSceneGraph( pSceneGraph )), IFXException);

    IFXString sName("PickSphere");

    IFXDECLARELOCAL(IFXPalette,pSGPalette);
    TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::MOTION, &pSGPalette )), IFXException);

    U32 iResultPaletteFind = pSGPalette->Find( &sName, &uResourceID );
    if ( IFX_E_CANNOT_FIND == iResultPaletteFind  ) 
    {
      TEST(IFXSUCCESS(pSGPalette->Add( &sName, &uResourceID )), IFXException);
    }

    TEST(IFXSUCCESS(pSGPalette->SetResourcePtr( uResourceID, pMotionResource )), IFXException);

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

  {
	IFXDECLARELOCAL(IFXModifierChain,pModifierChain);

    IFXString sName("PickSphere");

    IFXDECLARELOCAL(IFXAnimationModifier, pAnimMod );

    TEST(IFXSUCCESS(IFXCreateComponent( CID_IFXAnimationModifier, IID_IFXAnimationModifier, (void**)&pAnimMod )), IFXException);
    IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
    TEST(IFXSUCCESS(pSGUtils->m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph )), IFXException);
    TEST(IFXSUCCESS(pAnimMod->SetSceneGraph( pSceneGraph )), IFXException);
    pAnimMod->SetAsKeyframe();

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
    TEST(IFXSUCCESS(pAnimMod->QueryInterface( IID_IFXModifier, (void**)&pObjectAsModifier )), IFXException);
    TEST(IFXSUCCESS(pModifierChain->AddModifier( *pObjectAsModifier, IFXModifierChain::END_OF_MODCHAIN, FALSE )), IFXException);

    U32 uModCount = 0;
    TEST(IFXSUCCESS( pModifierChain->GetModifierCount( uModCount )) , IFXException);
    }

	pAnimMod->Playing() = TRUE;
	pAnimMod->RootLock() = FALSE;
	pAnimMod->AutoBlend() = TRUE;

	pAnimMod->TimeScale() = 1.0f;

	IFXDECLARELOCAL(IFXPalette,pMixerPalette);
	TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::MIXER, &pMixerPalette )), IFXException);

	IFXString mixerName("PickSphere");

	U32 id = 0 ;
	IFXRESULT iResultFind = pMixerPalette->Find( &mixerName, &id );
	if (IFXFAILURE(iResultFind)) 
	{
	TEST(IFXSUCCESS(pMixerPalette->Add( &mixerName, &id )), IFXException);
	}

	F32 fVal = 1.0f;
	BOOL bTrue = TRUE;
	BOOL bFalse = FALSE;
	TEST(IFXSUCCESS(pAnimMod->Queue(mixerName, NULL, NULL, NULL, &fVal, &bTrue, 0,1)), IFXException);

	pAnimMod->BlendTime() = 0.5f;
	pAnimMod->AxisLock() = 0;
	pAnimMod->Concatenation() = 0;
  }
		{
			IFXGlyph2DModifier   *pGlyph = NULL;
			IFXDECLARELOCAL( IFXGlyphCommandList, pComList );
			IFXCreateComponent( CID_IFXGlyphCommandList, IID_IFXGlyphCommandList, (void**)&pComList );
			pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_STARTGLYPHSTRING );
			GlyphStamper( pComList );

			pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_ENDGLYPHSTRING );
			AddGlyph2DModifier( pModel, &pGlyph, 10, 10, 10, pComList );
			IFXMatrix4x4 offset;
			offset.MakeIdentity();
			offset[12] = -17;
			offset[13] = -23;
			offset[14] = -30;
			pGlyph->SetTransform( offset );
			pGlyph->SetAttributes( IFXGlyph2DModifier::BILLBOARD );
			IFXRELEASE(pGlyph);
		}

		U32 numMeshes = 0;
		{
			IFXGlyph2DModifier   *pGlyph = NULL;
			IFXDECLARELOCAL( IFXGlyphCommandList, pComList );
			IFXCreateComponent( CID_IFXGlyphCommandList, IID_IFXGlyphCommandList, (void**)&pComList );
			pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_STARTGLYPHSTRING );
			GlyphStamper( pComList );

			pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_ENDGLYPHSTRING );
			AddGlyph2DModifier( pModel, &pGlyph, 10, 10, 10, pComList );
			IFXMatrix4x4 offset;
			offset.MakeIdentity();
			offset[12] = -17;
			offset[13] = -23;
			offset[14] = -30;
			pGlyph->SetTransform( offset );
			IFXDECLARELOCAL( IFXModifierDataPacket, pPacket );
			pGlyph->GetDataPacket( pPacket );
			U32 meshIdx;
			pPacket->GetDataElementIndex( DID_IFXRenderableGroup, meshIdx );
			IFXDECLARELOCAL( IFXUnknown, pUnk );
			IFXDECLARELOCAL( IFXMeshGroup, pMeshGroup );
			BOOL nr = FALSE;
			if( meshIdx != (U32)-1 )
			{
				pGlyph->GenerateOutput( meshIdx, (void*&)pUnk, nr );
				pUnk->QueryInterface( IID_IFXMeshGroup, (void**) &pMeshGroup );
				numMeshes = pMeshGroup->GetNumMeshes();
				IFXRELEASE( pMeshGroup );
			}
			IFXRELEASE(pGlyph);
		}
	{
		//create shader set
		IFXDECLARELOCAL( IFXShaderList, pShaderList );
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderList, IID_IFXShaderList, (void**)&pShaderList)), IFXException);
		pShaderList->Allocate(1);
		pShaderList->SetShader(0,1);
		IFXDECLARELOCAL( IFXShaderList, pShaderList2 );
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderList, IID_IFXShaderList, (void**)&pShaderList2)), IFXException);
		pShaderList2->Allocate(1);
		pShaderList2->SetShader(0,2);
		// Create a shader modifier
		IFXDECLARELOCAL( IFXShadingModifier, pShadingModifier );

		TEST( IFXSUCCESS( IFXCreateComponent( CID_IFXShadingModifier, IID_IFXShadingModifier,
			(void**)&pShadingModifier ) ), IFXException);
		pShadingModifier->SetSceneGraph( pSGUtils->m_pSceneGraph );
		TEST(IFXSUCCESS(pShadingModifier->SetElementShaderList( numMeshes-1, pShaderList2, FALSE )), IFXException);
		TEST(IFXSUCCESS(pShadingModifier->SetElementShaderList( 0, pShaderList )), IFXException);
		IFXDECLARELOCAL( IFXModifierChain, pModChain );
		TEST( IFXSUCCESS( pModel->GetModifierChain(&pModChain) ), IFXException );
		TEST( IFXSUCCESS( pModChain->AddModifier( *pShadingModifier ) ), IFXException );
	}

	return IFX_OK;
}
