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

#ifndef SampleCLODModR_H
#include "SampleCLODModR.h"
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

#include "IFXCLODModifier.h"
#include "IFXMixerConstruct.h"
#include "IFXMotionResource.h"
#include "IFXAnimationModifier.h"
//#include "IFXBoneWeightsModifier.h"

#include "IFXNameMap.h"

IFXRESULT SampleCLODModR::Run( U3DSceneUtilities* pSGUtils )
{
	U32           uResourceID = 0;

	////////////////////////////////////////

	TEST(IFXSUCCESS(pSGUtils->InitializeScene()), IFXException);

	// load file
	{
		IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
		IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);
		IFXDECLARELOCAL(IFXStdio, pIFXStdio);

		//load CLOD with bones from U3D file
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager)), IFXException);
		TEST(IFXSUCCESS(pIFXLoadManager->Initialize(pSGUtils->m_pCoreServices)), IFXException);
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer)), IFXException);
		TEST(IFXSUCCESS(pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio)), IFXException);

		IFXString src;
		src = getenv("U3D_SOURCE");
	//	src += "Tests/UnitTesting/Modifiers/SampleCLODModR/simpleBox.u3d";
		src += "Tests/UnitTesting/Modifiers/SampleCLODModR/mp_box.u3d";

		TEST(IFXSUCCESS(pIFXStdio->Open( (IFXCHAR*)src.Raw() )), IFXException);

		IFXDECLARELOCAL(IFXScheduler, pScheduler);

		TEST(IFXSUCCESS(pSGUtils->m_pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler)), IFXException);

		//	TEST(IFXSUCCESS(pSGUtils->m_pSceneGraph->Mark()), IFXException);
		TEST(IFXSUCCESS(pIFXLoadManager->Load(pIFXReadBuffer, IFXEXPORT_EVERYTHING)), IFXException); // ^ IFXEXPORT_FILEREFERENCES
		EIFXLoadState eState = IFX_LOAD_STATE_UNLOADED;
		while (eState < IFX_LOAD_STATE_MEDIA_LOADED) {
			TEST(IFXSUCCESS(pScheduler->Service()), IFXException);
			TEST(IFXSUCCESS(pIFXLoadManager->GetAggregateLoadState(eState)), IFXException);
		};

		TEST(IFXSUCCESS(pIFXStdio->Close()), IFXException);
	}

	IFXDECLARELOCAL(IFXModifierChain, pModifierChain);

	IFXRESULT iFindResult;
	IFXDECLARELOCAL( IFXMotionResource, pMotionResource );
	IFXString sName(L"Box01-Key");

	/////////////////////////////////////////// create motion resource
	{
		IFXDECLARELOCAL(IFXUnknown,pObject);
		IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
		IFXDECLARELOCAL(IFXMarker,pMarker );
		TEST(IFXSUCCESS(IFXCreateComponent( CID_IFXMotionResource, IID_IFXUnknown, (void**)&pObject )), IFXException);
		TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXMarker, (void**)&pMarker )), IFXException);
		TEST(IFXSUCCESS(pSGUtils->m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph )), IFXException);
		TEST(IFXSUCCESS(pMarker->SetSceneGraph( pSceneGraph )), IFXException);


		IFXDECLARELOCAL(IFXPalette,pSGPalette);
		TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::MOTION, &pSGPalette )), IFXException);

		U32 iResultPaletteFind = pSGPalette->Find( &sName, &uResourceID );
		if ( IFX_E_CANNOT_FIND == iResultPaletteFind  ) 
		{
			TEST(IFXSUCCESS(pSGPalette->Add( &sName, &uResourceID )), IFXException);
		}

		TEST(IFXSUCCESS(pSGPalette->SetResourcePtr( uResourceID, pObject )), IFXException);

		TEST(IFXSUCCESS( pObject->QueryInterface( IID_IFXMotionResource, (void**)&pMotionResource )), IFXException )

		pMotionResource->GetMotionRef()->SetName(sName);


		U32 uTrackCount = 1;

		IFXString trackName(L"Box01");
		U32 uTrackID = 0;
		iFindResult = pMotionResource->FindTrack( &trackName, &uTrackID );
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

		pKeyFrameArray[1].SetTime( 0.50f );
		pKeyFrameArray[1].Location() = IFXVector3( 0.0f, 600.0f, -150.0f );
		pKeyFrameArray[1].Rotation() = vRotation;
		pKeyFrameArray[1].Scale() = vScaleQ;

		pKeyFrameArray[2].SetTime( 1.0f );
		pKeyFrameArray[2].Location() = IFXVector3( 0.0f, 1200.0f, -300.0f );
		pKeyFrameArray[2].Rotation() = vRotation;
		pKeyFrameArray[2].Scale() = vScaleQ;

		pKeyFrameArray[3].SetTime( 1.50f );
		pKeyFrameArray[3].Location() = IFXVector3( 0.0f, 600.0f, -150.0f );
		pKeyFrameArray[3].Rotation() = vRotation;
		pKeyFrameArray[3].Scale() = vScaleQ;

		pKeyFrameArray[4].SetTime( 2.0f );
		pKeyFrameArray[4].Location() = IFXVector3( 0.0f, 0.0f, 0.0f );
		pKeyFrameArray[4].Rotation() = vRotation;
		pKeyFrameArray[4].Scale() = vScaleQ;

		// Set the key frame array to the motion resource
		TEST(IFXSUCCESS(pMotionResource->ClearTrack( uTrackID )), IFXException);
		TEST(IFXSUCCESS(pMotionResource->InsertKeyFrames( uTrackID,uTimeCount,pKeyFrameArray )), IFXException);
		IFXDELETE_ARRAY( pKeyFrameArray );
	}

	// Now, add a fully-mapped mixer to the mixer palette for this motion
	{
		IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
		TEST(IFXSUCCESS(pSGUtils->m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph )), IFXException);

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

	IFXRELEASE( pModifierChain );

	{
		IFXString sName(L"LightBoxModel");

		IFXDECLARELOCAL(IFXUnknown,pObject);
		IFXDECLARELOCAL(IFXMarker,pMarker );
		IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
		IFXDECLARELOCAL( IFXModifier, pMod );

		TEST(IFXSUCCESS(IFXCreateComponent( CID_IFXAnimationModifier, IID_IFXUnknown, (void**)&pObject )), IFXException);
		TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXMarker, (void**)&pMarker )), IFXException);
		TEST(IFXSUCCESS(pSGUtils->m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph )), IFXException);
		TEST(IFXSUCCESS(pMarker->SetSceneGraph( pSceneGraph )), IFXException);
		TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXModifier, (void**)&pMod)), IFXException);
		IFXDECLARELOCAL(IFXAnimationModifier, pAnimMod );
		TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXAnimationModifier, (void**)&pAnimMod)), IFXException);
		pAnimMod->SetAsKeyframe();

		U32 uResourceID;

		IFXDECLARELOCAL(IFXPalette,pSGPalette);
		TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::GENERATOR, &pSGPalette )), IFXException);

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

			U32 uModCount = 0;
			TEST(IFXSUCCESS( pModifierChain->GetModifierCount( uModCount )) , IFXException);

			TEST(IFXSUCCESS(pModifierChain->AddModifier( *pObjectAsModifier, IFXModifierChain::END_OF_MODCHAIN, TRUE )), IFXException);
		}

		IFXDECLARELOCAL(IFXAnimationModifier, pAM );
		TEST(IFXSUCCESS(pMod->QueryInterface( IID_IFXAnimationModifier, (void**)&pAM)), IFXException);

		pAM->Playing() = TRUE;
		pAM->RootLock() = FALSE;
		pAM->AutoBlend() = TRUE;

		pAM->TimeScale() = 1.0f;

		IFXDECLARELOCAL(IFXPalette,pMixerPalette);
		TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::MIXER, &pMixerPalette )), IFXException);

		IFXString mixerName(L"Box01-Key");

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

	IFXRELEASE(pModifierChain);

	///////////////////////////////////////////////////////////////
	/////////////////////////////////////////// create CLOD modifier

	{
		IFXString sName("LightBoxModel");

		IFXDECLARELOCAL(IFXUnknown,pObject);
		TEST(IFXSUCCESS(IFXCreateComponent( CID_IFXCLODModifier, IID_IFXUnknown, (void**)&pObject )), IFXException);
		IFXDECLARELOCAL(IFXMarker,pMarker );
		TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXMarker, (void**)&pMarker )), IFXException);
		IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
		TEST(IFXSUCCESS(pSGUtils->m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph )), IFXException);
		TEST(IFXSUCCESS(pMarker->SetSceneGraph( pSceneGraph )), IFXException);
		IFXDECLARELOCAL(IFXModifier, pMod);
		TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXModifier, (void**)&pMod)), IFXException);
		IFXDECLARELOCAL(IFXCLODModifier, pCLODModifier);
		TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXCLODModifier, (void**)&pCLODModifier)), IFXException);
		pCLODModifier->SetCLODScreenSpaceControllerState(FALSE);
		pCLODModifier->SetLODBias(0.002f);
		pCLODModifier->SetCLODLevel(0.5f);

		U32 uResourceID;

		IFXDECLARELOCAL(IFXPalette,pSGPalette);
		TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::GENERATOR, &pSGPalette )), IFXException);

		U32 iResultPaletteFind = pSGPalette->Find( &sName, &uResourceID );
		if ( IFX_E_CANNOT_FIND == iResultPaletteFind  ) 
		{
			//TEST(IFXSUCCESS(IFX_E_CANNOT_FIND), IFXException);
			return IFX_E_CANNOT_FIND;
		}

		// If this is a chain object
		{
			// get the modifier at the head of the chain
			IFXDECLARELOCAL(IFXModifier,pHeadModifier);
			U32 iResultGetResource = pSGPalette->GetResourcePtr(uResourceID,IID_IFXModifier,(void**)&pHeadModifier);

			// Get the modifier chain from the head modifier
			TEST(IFXSUCCESS(pHeadModifier->GetModifierChain(&pModifierChain)), IFXException);

			TEST(IFXSUCCESS(pModifierChain->AddModifier( *pMod, IFXModifierChain::END_OF_MODCHAIN, TRUE )), IFXException);

			U32 uModCount = 0;
			TEST(IFXSUCCESS( pModifierChain->GetModifierCount( uModCount )) , IFXException);
		}
	}

	// write file
	{
		IFXDECLARELOCAL(IFXWriteManager, pIFXWriteManager);
		IFXDECLARELOCAL(IFXWriteBuffer, pIFXWriteBuffer);
		IFXDECLARELOCAL(IFXStdio, pIFXStdio);

		//saving to file
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXWriteManager, IID_IFXWriteManager, (void**)&pIFXWriteManager)), IFXException);
		TEST(IFXSUCCESS(pIFXWriteManager->Initialize(pSGUtils->m_pCoreServices)), IFXException);
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioWriteBuffer, IID_IFXWriteBuffer, (void**)&pIFXWriteBuffer)), IFXException);
		TEST(IFXSUCCESS(pIFXWriteBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio)), IFXException);
		IFXString src1;
		src1 = getenv("U3D_SOURCE");
		src1 += "Tests/UnitTesting/Modifiers/SampleCLODModR/output.u3d";
		TEST(IFXSUCCESS(pIFXStdio->Open( (IFXCHAR*)src1.Raw() ) ), IFXException);
		TEST(IFXSUCCESS(pSGUtils->m_pSceneGraph->Mark()), IFXException);
		TEST(IFXSUCCESS(pIFXWriteManager->Write(pIFXWriteBuffer, IFXEXPORT_EVERYTHING)), IFXException); // ^ IFXEXPORT_FILEREFERENCES
		TEST(IFXSUCCESS(pIFXStdio->Close()), IFXException);
	}

	return IFX_OK;
}
