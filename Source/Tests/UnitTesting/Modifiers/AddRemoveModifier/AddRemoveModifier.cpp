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

#ifndef SampleAnimation_H
#include "AddRemoveModifier.h"
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

#include "IFXNameMap.h"

/**
* The purpose of this test is to check preserving of parent-child relations in case if parent MC 
* was changed (Modifier is added/Modifier is deleted).
* 
* This test loads scene with three level hierarchy: Box01(parent) <- Box02(child) <- Box03(grandchild).
* Box01 MC: Node + Shading Modifier + Animation Modifier.
* Box02 MC: Node + Shading Modifier.
* Box03 MC: Node + Shading Modifier.
* 1. We create Motion Resource, Animation Modifier and apply the Modifier to the end of Box02 MC.
* After this Box03 should start to observe Box02's Animation Modifier's DP (animation of Box02 and Box03 
* will change).
* 2. Then we remove Animation Modifier from Box02's MC. Box03 should start to observe Box02 Node's DP.
* (Because SM doesn't have Transform DE as output).
*/
IFXRESULT AddRemoveModifier::Run( U3DSceneUtilities* pSGUtils )
{
	U32           uResourceID = 0;

	IFXRESULT result = IFX_OK;

	TEST(IFXSUCCESS(pSGUtils->InitializeScene()), IFXException);

	{
		IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
		IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);
		IFXDECLARELOCAL(IFXStdio, pIFXStdio);

		//load CLOD with bones from U3D file
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager)), IFXException);
		TEST(IFXSUCCESS(pIFXLoadManager->Initialize(pSGUtils->m_pCoreServices)), IFXException);
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer)), IFXException);
		TEST(IFXSUCCESS(pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio)), IFXException);

		IFXString src(getenv("U3D_SOURCE"));
		src += L"Tests/UnitTesting/Modifiers/AddRemoveModifier/threeLevelHierarchy.u3d";

		TEST(IFXSUCCESS(pIFXStdio->Open( (IFXCHAR*)src.Raw() )), IFXException);

		IFXDECLARELOCAL(IFXScheduler, pScheduler);

		TEST(IFXSUCCESS(pSGUtils->m_pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler)), IFXException);

		TEST(IFXSUCCESS(pIFXLoadManager->Load(pIFXReadBuffer, IFXEXPORT_EVERYTHING)), IFXException); // ^ IFXEXPORT_FILEREFERENCES
		EIFXLoadState eState = IFX_LOAD_STATE_UNLOADED;
		while (eState < IFX_LOAD_STATE_MEDIA_LOADED) 
		{
			TEST(IFXSUCCESS(pScheduler->Service()), IFXException);
			TEST(IFXSUCCESS(pIFXLoadManager->GetAggregateLoadState(eState)), IFXException);
		};

		TEST(IFXSUCCESS(pIFXStdio->Close()), IFXException);
	}

	/////////////////////////////////////////// create motion resource

	{
		IFXDECLARELOCAL(IFXUnknown,pObject);
		TEST(IFXSUCCESS(IFXCreateComponent( CID_IFXMotionResource, IID_IFXUnknown, (void**)&pObject )), IFXException);
		IFXDECLARELOCAL(IFXMarker,pMarker );
		TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXMarker, (void**)&pMarker )), IFXException);
		IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
		TEST(IFXSUCCESS(pSGUtils->m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph )), IFXException);
		TEST(IFXSUCCESS(pMarker->SetSceneGraph( pSceneGraph )), IFXException);

		IFXString sName(L"Box02");

		IFXDECLARELOCAL(IFXPalette,pSGPalette);
		TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::MOTION, &pSGPalette )), IFXException);

		U32 iResultPaletteFind = pSGPalette->Find( &sName, &uResourceID );
		if ( IFX_E_CANNOT_FIND == iResultPaletteFind  ) 
		{
			TEST(IFXSUCCESS(pSGPalette->Add( &sName, &uResourceID )), IFXException);
		}

		TEST(IFXSUCCESS(pSGPalette->SetResourcePtr( uResourceID, pObject )), IFXException);

		IFXDECLARELOCAL( IFXMotionResource, pMotionResource );
		TEST(IFXSUCCESS( pObject->QueryInterface( IID_IFXMotionResource, (void**)&pMotionResource )), IFXException );

		pMotionResource->GetMotionRef()->SetName(sName);

		U32 uTrackCount = 1;

		IFXString trackName(L"Box02");
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
		IFXString sName(L"Box02");

		IFXDECLARELOCAL( IFXAnimationModifier, pAnimMod );
		TEST(IFXSUCCESS(IFXCreateComponent( CID_IFXAnimationModifier, IID_IFXAnimationModifier, (void**)&pAnimMod )), IFXException);

		pAnimMod->SetAsKeyframe();

		IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
		TEST(IFXSUCCESS(pSGUtils->m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph )), IFXException);

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
				IFXDECLARELOCAL( IFXModifier, pMod );
				TEST(IFXSUCCESS(pAnimMod->QueryInterface( IID_IFXModifier, (void**)&pMod)), IFXException);

				// get the modifier at the head of the chain
				IFXDECLARELOCAL(IFXModifier,pHeadModifier);
				U32 iResultGetResource = pSGPalette->GetResourcePtr(uResourceID,IID_IFXModifier,(void**)&pHeadModifier);

				// Get the modifier chain from the head modifier
				TEST(IFXSUCCESS(pHeadModifier->GetModifierChain(&pModifierChain)), IFXException);

				// Initialize modifier
				TEST(IFXSUCCESS(pMod->SetSceneGraph( pSceneGraph )), IFXException);

				// Add Animation Modifier to Box02 MC
				TEST(IFXSUCCESS(pModifierChain->AddModifier( *pMod )), IFXException);

				U32 uModCount = 0;
				TEST(IFXSUCCESS( pModifierChain->GetModifierCount( uModCount )) , IFXException);
			}
		}

		// set animation modifier parameters
		{
			pAnimMod->Playing() = TRUE;
			pAnimMod->RootLock() = FALSE;
			pAnimMod->AutoBlend() = TRUE;

			pAnimMod->TimeScale() = 1.0f;

			IFXDECLARELOCAL(IFXPalette,pMixerPalette);
			TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::MIXER, &pMixerPalette )), IFXException);

			IFXString mixerName(L"Box02");

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

			IFXRELEASE(pMixerPalette);

			pAnimMod->BlendTime() = 0.5f;
			pAnimMod->AxisLock() = 0;
			pAnimMod->Concatenation() = 0;
		}
		

		// Remove Animation Modifier from Box02 MC
		TEST(IFXSUCCESS(pModifierChain->RemoveModifier()), IFXException);
	}
	
	{
		IFXDECLARELOCAL(IFXWriteManager, pIFXWriteManager);
		IFXDECLARELOCAL(IFXWriteBuffer, pIFXWriteBuffer);
		IFXDECLARELOCAL(IFXStdio, pIFXStdio);

		IFXString dst(getenv("U3D_SOURCE"));
		dst += L"Tests/UnitTesting/Modifiers/AddRemoveModifier/output.u3d";

		//save to file
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXWriteManager, IID_IFXWriteManager, (void**)&pIFXWriteManager)), IFXException);
		TEST(IFXSUCCESS(pIFXWriteManager->Initialize(pSGUtils->m_pCoreServices)), IFXException);
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioWriteBuffer, IID_IFXWriteBuffer, (void**)&pIFXWriteBuffer)), IFXException);
		TEST(IFXSUCCESS(pIFXWriteBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio)), IFXException);
		TEST(IFXSUCCESS(pIFXStdio->Open( (IFXCHAR*)dst.Raw() ) ), IFXException);
		TEST(IFXSUCCESS(pSGUtils->m_pSceneGraph->Mark()), IFXException);
		TEST(IFXSUCCESS(pIFXWriteManager->Write(pIFXWriteBuffer, IFXEXPORT_EVERYTHING)), IFXException); // ^ IFXEXPORT_FILEREFERENCES
		TEST(IFXSUCCESS(pIFXStdio->Close()), IFXException);
	}

	return IFX_OK;
}
