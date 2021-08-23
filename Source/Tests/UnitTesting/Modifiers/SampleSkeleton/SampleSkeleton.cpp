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

#include "IFXModifierChain.h"

#include "IFXMixerConstruct.h"
#include "IFXMotionResource.h"
#include "IFXAnimationModifier.h"
#include "IFXBoneWeightsModifier.h"

#include "IFXCheckX.h"
#include "U3DViewer.h"

#include "SampleSkeleton.h"
#include "IFXNameMap.h"


SampleSkeleton::SampleSkeleton()
: m_pSGUtils( NULL )
{
	TEST( IFXSUCCESS( IFXCOMInitialize() ), IFXException );
}

SampleSkeleton::~SampleSkeleton()
{
}

IFXRESULT SampleSkeleton::StartUp()
{
	IFXRESULT result = IFX_OK;

	m_pSGUtils = new U3DSceneUtilities;

	if( NULL == m_pSGUtils )
		result = IFX_E_OUT_OF_MEMORY;

	return result;
}

IFXRESULT SampleSkeleton::ShutDown()
{
	IFXRESULT result = IFX_OK;

	result =  IFXCOMUninitialize();

	return result;
}

IFXRESULT SampleSkeleton::Run()
{
	U32 uResourceID = 0;

	TEST(IFXSUCCESS(m_pSGUtils->InitializeScene()), IFXException);

	{
		// Get the scene graph
		IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );
		TEST( IFXSUCCESS( m_pSGUtils->m_pCoreServices->GetSceneGraph(IID_IFXSceneGraph,(void**)&pSceneGraph)), IFXException );

		// load file
		{
			IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
			IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);
			IFXDECLARELOCAL(IFXStdio, pIFXStdio);

			//load CLOD with bones from U3D file
			TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager)), IFXException);
			TEST(IFXSUCCESS(pIFXLoadManager->Initialize(m_pSGUtils->m_pCoreServices)), IFXException);
			TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer)), IFXException);
			TEST(IFXSUCCESS(pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio)), IFXException);

			IFXString src = getenv("U3D_SOURCE");
			src += L"Tests/UnitTesting/Modifiers/SampleSkeleton/simpleBox.u3d";

			IFXRESULT res = pIFXStdio->Open( (IFXCHAR*)src.Raw() );
			TEST(IFXSUCCESS( res ), IFXException );

		    TEST(IFXSUCCESS(pIFXLoadManager->Load(pIFXReadBuffer, IFXEXPORT_EVERYTHING)), IFXException); // ^ IFXEXPORT_FILEREFERENCES
			TEST(IFXSUCCESS(pIFXStdio->Close()), IFXException);
		}

		// add new light with name mapping
		/*{
			IFXRESULT result = IFX_OK;
			IFXDECLARELOCAL( IFXLight, pLight );
			IFXString lightName( L"Light1" );
			SGNodeHeader nh;

			if ( IFXSUCCESS(result) )
			{
				IFXDECLARELOCAL( IFXNameMap, pNameMap );
				U32 scopeId;

				result = m_pSGUtils->m_pCoreServices->GetNameMap( IID_IFXNameMap, (void**)&pNameMap );

				if ( IFXSUCCESS(result) )
					result = pNameMap->GenerateScopeId( scopeId );
			
				if ( IFXSUCCESS(result) )
					result = pNameMap->NewScope( scopeId, IFXCOLLISIONPOLICY_PREPENDCOLLIDED );
			
				if ( IFXSUCCESS(result) )
					result = pNameMap->Map(scopeId, IFXSceneGraph::NODE, lightName);
			}

			if ( IFXSUCCESS(result) )
			{
				wcscpy( nh.Name, lightName.Raw() );
				wcscpy( nh.ParentName, PALETTE_DEFAULT_GROUP );
			}

			TEST( IFXSUCCESS( m_pSGUtils->CreateLightNode( &nh, &pLight )), IFXException );
		}*/

		IFXString name1(L"Scapula");
		IFXString name2(L"Humerus");
		IFXString name3(L"Ulna");
		IFXString name4(L"Radius");
		IFXString unn(L"<unnamed>");

		U32 uBoneCount = 4; // count
		U32 i = 0;

		IFXSkeleton* pSkeleton = NULL;
		IFXBoneInfo* pBoneInfo = new IFXBoneInfo[ uBoneCount ];

		for( i = 0; i < uBoneCount; i++ )
		{
			switch(i)
			{
			case 0:
				pBoneInfo[i].stringBoneName = name1;
				pBoneInfo[i].stringParentName = unn;
				break;
			case 1:
				pBoneInfo[i].stringBoneName = name2;
				pBoneInfo[i].stringParentName = name1;
				break;
			case 2:
				pBoneInfo[i].stringBoneName = name3;
				pBoneInfo[i].stringParentName = name2;
				break;
			case 3:
				pBoneInfo[i].stringBoneName = name4;
				pBoneInfo[i].stringParentName = name3;
				break;
			default:
				// huh?
				break;
			}

			pBoneInfo[i].uBoneAttributes = 0; // att.
			pBoneInfo[i].fBoneLength = 5; // len.

			pBoneInfo[i].v3BoneDisplacement.Set( 0.0f, 0.0f, 0.0f ); // displ. & orient.
			pBoneInfo[i].v4BoneRotation.Set( 0.0f, 0.0f, 0.0f, 0.0f );
		}


		TEST( IFXSUCCESS( IFXCreateComponent( CID_IFXSkeleton, IID_IFXSkeleton, (void **)&pSkeleton )), IFXException );

		U32 j = 0;
		for( i = 0; i < uBoneCount; i++ )
		{
			pBoneInfo[i].iParentBoneID = -1;

			for( j = 0; j < uBoneCount; j++ )
			{
				if( i != j && pBoneInfo[i].stringParentName == pBoneInfo[j].stringBoneName )
				{
					pBoneInfo[i].iParentBoneID = j;
					j = uBoneCount;
				}
			}
		}

		for( i = 0; i < uBoneCount; i++ )
		{
			TEST( IFXSUCCESS( pSkeleton->SetBoneInfo( i, &pBoneInfo[i] )), IFXException );
		}

		// Get the resource palette
		IFXDECLARELOCAL( IFXPalette, pPal );
		TEST(IFXSUCCESS(  pSceneGraph->GetPalette( IFXSceneGraph::GENERATOR, &pPal)), IFXException );

		U32 id=0;
		TEST( IFXSUCCESS( pPal->Find(L"Box01", &id)), IFXException );

		{
			IFXDECLARELOCAL( IFXAuthorCLODResource, pAR );
			TEST( IFXSUCCESS( pPal->GetResourcePtr( id, IID_IFXAuthorCLODResource, (void**)&pAR )), IFXException);

			if( pAR )
				pAR->SetBones( pSkeleton );
			else
				return IFX_E_UNDEFINED;

			IFXRELEASE( pSkeleton );

			// test bones
			pSkeleton = pAR->GetBones();

			// Try to get skeletom from CLOD resource
			if( NULL == pSkeleton )
			{
				return IFX_E_UNDEFINED;
			}
			else
			{
				// Get bones number from the skeleton
				U32 uNum;
				TEST( IFXSUCCESS( pSkeleton->GetNumBones( uNum ) ), IFXException );
				if( uNum != uBoneCount )
				{
					return IFX_E_UNDEFINED;
				}
			}
		}

		// try to add skeleton animation
		{
			{
				// create motion resource
				IFXDECLARELOCAL( IFXMotionResource, pMotionResource );
				TEST(IFXSUCCESS(IFXCreateComponent( CID_IFXMotionResource, IID_IFXMotionResource, (void**)&pMotionResource )), IFXException);
				IFXDECLARELOCAL(IFXMarker,pMarker );
				TEST(IFXSUCCESS(pMotionResource->QueryInterface( IID_IFXMarker, (void**)&pMarker )), IFXException);
				TEST(IFXSUCCESS(pMarker->SetSceneGraph( pSceneGraph )), IFXException);

				IFXString sName("Box01-Key");

				IFXDECLARELOCAL(IFXPalette,pSGPalette);
				TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::MOTION, &pSGPalette )), IFXException);

				U32 iResultPaletteFind = pSGPalette->Find( &sName, &uResourceID );
				if ( IFX_E_CANNOT_FIND == iResultPaletteFind  )
				{
					TEST(IFXSUCCESS(pSGPalette->Add( &sName, &uResourceID )), IFXException);
				}

				TEST(IFXSUCCESS(pSGPalette->SetResourcePtr( uResourceID, pMotionResource )), IFXException);

				// set name to motion resource
				pMotionResource->GetMotionRef()->SetName(sName);

				// number of tracks is equal to number of bones
				U32 uTrackCount = uBoneCount;

				// create tracks
				U32 *uTrackID = new U32[ uBoneCount ];
				IFXRESULT iFindResult = IFX_OK;

				iFindResult = pMotionResource->FindTrack( &name1, &uTrackID[0] );
				if( IFXFAILURE( iFindResult ) )
				{
					TEST(IFXSUCCESS(pMotionResource->AddTrack( &name1, &uTrackID[0] )), IFXException);
				}

				iFindResult = pMotionResource->FindTrack( &name2, &uTrackID[1] );
				if( IFXFAILURE( iFindResult ) )
				{
					TEST(IFXSUCCESS(pMotionResource->AddTrack( &name2, &uTrackID[1] )), IFXException);
				}

				iFindResult = pMotionResource->FindTrack( &name3, &uTrackID[2] );
				if( IFXFAILURE( iFindResult ) )
				{
					TEST(IFXSUCCESS(pMotionResource->AddTrack( &name3, &uTrackID[2] )), IFXException);
				}

				iFindResult = pMotionResource->FindTrack( &name4, &uTrackID[3] );
				if( IFXFAILURE( iFindResult ) )
				{
					TEST(IFXSUCCESS(pMotionResource->AddTrack( &name4, &uTrackID[3] )), IFXException);
				}

				// 6. TimeCount
				U32 uTimeCount = 3;
				IFXVector3 vPos( 0.0f, 0.0f, 0.0f );
				IFXQuaternion vRotation(1.0f,0.0f,0.0f,0.0f);
				IFXQuaternion vRotation1(1.0f,0.0f,0.0f,1.5f);
				IFXQuaternion vRotation2(1.0f,0.0f,-2.0f,1.0f);
				IFXVector3 vScaleQ(1.0f,1.0f,1.0f);
				IFXVector3 vScaleQ1(1.0f,1.5f,0.1f);

				U32 i;
				for( i = 0; i < uTrackCount; i++ )
				{
					IFXKeyFrame *pKeyFrameArray = NULL;
					pKeyFrameArray = new IFXKeyFrame[uTimeCount];

					if( NULL == pKeyFrameArray )
					{
						return IFX_E_OUT_OF_MEMORY;
					}

					pKeyFrameArray[0].SetTime( 0.0f );
					pKeyFrameArray[0].Location() = vPos;
					pKeyFrameArray[0].Rotation() = vRotation;
					pKeyFrameArray[0].Scale() = vScaleQ;

					switch( i )
					{
					case 0:
						pKeyFrameArray[1].Location() = IFXVector3( 0.0f, 0.0f, 0.0f );
						break;
					case 1:
						pKeyFrameArray[1].Location() = IFXVector3( -10.0f, 0.0f, 0.0f );
						break;
					case 2:
						pKeyFrameArray[1].Location() = IFXVector3( 0.0f, 0.0f, 10.0f );
						break;
					case 3:
						pKeyFrameArray[1].Location() = IFXVector3( -10.0f, 0.0f, 0.0f );
						break;
					default:
						break;
					}

					pKeyFrameArray[1].SetTime( 1.0f );
					pKeyFrameArray[1].Rotation() = vRotation;
					pKeyFrameArray[1].Scale() = vScaleQ;

					pKeyFrameArray[2].Location() = vPos;
					pKeyFrameArray[2].SetTime( 2.0f );
					pKeyFrameArray[2].Rotation() = vRotation;
					pKeyFrameArray[2].Scale() = vScaleQ;

					//pKeyFrameArray[3].SetTime( 3.0f );
					//pKeyFrameArray[3].Location() = vPos;
					//pKeyFrameArray[3].Rotation() = vRotation;
					//pKeyFrameArray[3].Scale() = vScaleQ;

					// Set the key frame array to the motion resource
					TEST(IFXSUCCESS(pMotionResource->ClearTrack( uTrackID[i] )), IFXException);
					TEST(IFXSUCCESS(pMotionResource->InsertKeyFrames( uTrackID[i], uTimeCount, pKeyFrameArray )), IFXException);

					IFXDELETE_ARRAY( pKeyFrameArray );
				}

				delete [] uTrackID;

				// Now, add a fully-mapped mixer to the mixer palette for this motion
				U32 uMixerID = 0;
				IFXDECLARELOCAL(IFXPalette,pMixerPalette);
				TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::MIXER, &pMixerPalette )), IFXException);
				iFindResult = pMixerPalette->Find( &sName, &uMixerID );
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

			// create bone weights modifier
			{
				IFXString sName("Box01");

				IFXDECLARELOCAL(IFXUnknown,pObject);
				TEST(IFXSUCCESS(IFXCreateComponent( CID_IFXBoneWeightsModifier, IID_IFXUnknown, (void**)&pObject )), IFXException);
				IFXDECLARELOCAL(IFXMarker,pMarker2 );
				TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXMarker, (void**)&pMarker2 )), IFXException);
				TEST(IFXSUCCESS(pMarker2->SetSceneGraph( pSceneGraph )), IFXException);
				IFXDECLARELOCAL( IFXModifier, pMod );
				TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXModifier, (void**)&pMod)), IFXException);

				{
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
						IFXDECLARELOCAL(IFXModifierChain, pModifierChain);
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

				{
					IFXDECLARELOCAL( IFXBoneWeightsModifier, pBWM );
					TEST(IFXSUCCESS(pMod->QueryInterface( IID_IFXBoneWeightsModifier, (void**)&pBWM)), IFXException);

					pBWM->SetTotalBoneWeightCountForAuthorMesh( 20 );

					U32 *pBones = new U32[4];
					F32 *pWeights = new F32[3];

					pBones[0] = 0;
					pBones[1] = 1;
					pBones[2] = 2;
					pBones[3] = 3;

					pWeights[0] = 0.2f;
					pWeights[1] = 0.8f;
					pWeights[2] = 1.0f;

					TEST(IFXSUCCESS(pBWM->SetBoneWeightsForAuthorMesh( 0, 1, &pBones[2], &pWeights[2] )),IFXException);
                    TEST(IFXSUCCESS(pBWM->SetBoneWeightsForAuthorMesh( 2, 1, &pBones[2], &pWeights[2] )),IFXException);
					TEST(IFXSUCCESS(pBWM->SetBoneWeightsForAuthorMesh( 4, 1, &pBones[2], &pWeights[2] )),IFXException);
					TEST(IFXSUCCESS(pBWM->SetBoneWeightsForAuthorMesh( 5, 1, &pBones[2], &pWeights[2] )),IFXException);

					TEST(IFXSUCCESS(pBWM->SetBoneWeightsForAuthorMesh( 10, 2, &pBones[1], &pWeights[0] )),IFXException);
					TEST(IFXSUCCESS(pBWM->SetBoneWeightsForAuthorMesh( 12, 2, &pBones[1], &pWeights[0] )),IFXException);
					TEST(IFXSUCCESS(pBWM->SetBoneWeightsForAuthorMesh( 13, 2, &pBones[1], &pWeights[0] )),IFXException);
					TEST(IFXSUCCESS(pBWM->SetBoneWeightsForAuthorMesh( 19, 2, &pBones[1], &pWeights[0] )),IFXException);

					TEST(IFXSUCCESS(pBWM->SetBoneWeightsForAuthorMesh( 8, 2, &pBones[2], &pWeights[0] )),IFXException);
					TEST(IFXSUCCESS(pBWM->SetBoneWeightsForAuthorMesh( 9, 2, &pBones[2], &pWeights[0] )),IFXException);
					TEST(IFXSUCCESS(pBWM->SetBoneWeightsForAuthorMesh( 16, 2, &pBones[0], &pWeights[0] )),IFXException);
					TEST(IFXSUCCESS(pBWM->SetBoneWeightsForAuthorMesh( 17, 2, &pBones[0], &pWeights[0] )),IFXException);

					TEST(IFXSUCCESS(pBWM->SetBoneWeightsForAuthorMesh( 11, 2, &pBones[1], &pWeights[0] )),IFXException);
					TEST(IFXSUCCESS(pBWM->SetBoneWeightsForAuthorMesh( 14, 2, &pBones[1], &pWeights[0] )),IFXException);
					TEST(IFXSUCCESS(pBWM->SetBoneWeightsForAuthorMesh( 15, 2, &pBones[1], &pWeights[0] )),IFXException);
					TEST(IFXSUCCESS(pBWM->SetBoneWeightsForAuthorMesh( 18, 2, &pBones[1], &pWeights[0] )),IFXException);

					TEST(IFXSUCCESS(pBWM->SetBoneWeightsForAuthorMesh( 1, 1, &pBones[3], &pWeights[2] )),IFXException);
					TEST(IFXSUCCESS(pBWM->SetBoneWeightsForAuthorMesh( 3, 1, &pBones[3], &pWeights[2] )),IFXException);
					TEST(IFXSUCCESS(pBWM->SetBoneWeightsForAuthorMesh( 6, 1, &pBones[3], &pWeights[2] )),IFXException);
					TEST(IFXSUCCESS(pBWM->SetBoneWeightsForAuthorMesh( 7, 1, &pBones[3], &pWeights[2] )),IFXException);

					delete [] pBones;
					delete [] pWeights;
				}
				IFXDELETE_ARRAY( pBoneInfo );
			}

			// create animation modifier
			{
				IFXString sName("Box01");

				IFXDECLARELOCAL(IFXUnknown,pObject);
				TEST(IFXSUCCESS(IFXCreateComponent( CID_IFXAnimationModifier, IID_IFXUnknown, (void**)&pObject )), IFXException);
				IFXDECLARELOCAL(IFXMarker,pMarker );
				TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXMarker, (void**)&pMarker )), IFXException);
				TEST(IFXSUCCESS(pMarker->SetSceneGraph( pSceneGraph )), IFXException);
				IFXDECLARELOCAL( IFXModifier, pMod );
				TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXModifier, (void**)&pMod)), IFXException);
				IFXDECLARELOCAL( IFXAnimationModifier, pAnimMod );
				TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXAnimationModifier, (void**)&pAnimMod)), IFXException);
				pAnimMod->SetAsBones();

				{
					IFXDECLARELOCAL(IFXPalette,pSGPalette);
					TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::NODE, &pSGPalette )), IFXException);

					U32 iResultPaletteFind = pSGPalette->Find( &sName, &uResourceID );
					if ( IFX_E_CANNOT_FIND == iResultPaletteFind  )
					{
						TEST(IFXSUCCESS(pSGPalette->Add( &sName, &uResourceID )), IFXException);
					}

					// If this is a chain object
					{
						IFXDECLARELOCAL(IFXModifierChain,pModifierChain);

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

				{
					IFXDECLARELOCAL( IFXAnimationModifier, pAM );
					TEST(IFXSUCCESS(pMod->QueryInterface( IID_IFXAnimationModifier, (void**)&pAM)), IFXException);

					pAM->Playing() = TRUE;
					pAM->RootLock() = FALSE;
					// pAM->AutoBlend() = TRUE;

					pAM->TimeScale() = 1.0f;

					IFXDECLARELOCAL(IFXPalette,pMixerPalette);
					TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::MIXER, &pMixerPalette )), IFXException);

					IFXString mixerName("Box01-Key");

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

					// pAM->BlendTime() = 0.5f;
					// pAM->AxisLock() = 0;
					// pAM->Concatenation() = 0;

					// BOOL loop = TRUE;
					// pAM->Setup();
					// pAM->Push( sName, NULL, NULL, NULL, NULL, &loop,  NULL);
				}
			}
		}

		{
			IFXDECLARELOCAL(IFXWriteManager, pIFXWriteManager);
			IFXDECLARELOCAL(IFXWriteBuffer, pIFXWriteBuffer);
			IFXDECLARELOCAL(IFXStdio, pIFXStdio);

			//saving to file with 2 skeletons
			TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXWriteManager, IID_IFXWriteManager, (void**)&pIFXWriteManager)), IFXException);
			TEST(IFXSUCCESS(pIFXWriteManager->Initialize(m_pSGUtils->m_pCoreServices)), IFXException);
			TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioWriteBuffer, IID_IFXWriteBuffer, (void**)&pIFXWriteBuffer)), IFXException);
			TEST(IFXSUCCESS(pIFXWriteBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio)), IFXException);
			TEST(IFXSUCCESS(pIFXStdio->Open( L"output.u3d" ) ), IFXException);
			TEST(IFXSUCCESS(pSceneGraph->Mark()), IFXException);
			TEST(IFXSUCCESS(pIFXWriteManager->Write(pIFXWriteBuffer, IFXEXPORT_EVERYTHING)), IFXException); // ^ IFXEXPORT_FILEREFERENCES
			TEST(IFXSUCCESS(pIFXStdio->Close()), IFXException);
		}
	}

	return IFX_OK;
}

IFXRESULT SampleSkeleton::View( char* pParams )
{
	IFXRESULT result = IFX_OK;
	wchar_t* ptr = NULL;

	if( NULL != pParams )
	{
		U32 s = mbstowcs(NULL, pParams, 0);

		if( s > 0 )
		{
			ptr = new wchar_t[s+1];

			if( NULL != ptr )
			{
				mbstowcs(ptr, pParams, s);
				ptr[s]=L'\0';
			}
			else
				result = IFX_E_OUT_OF_MEMORY;
		}
	}

	if( IFXSUCCESS(result) )
	{
		try
		{
			U3DViewer viewer;

			if( NULL != ptr )
				IFXCHECKX( viewer.ParseArguments( ptr ) );

			IFXCHECKX( viewer.SetupWindows() );
			IFXCHECKX( viewer.InitializeSG( m_pSGUtils ) );

			// Render until the user is done
			IFXCHECKX( viewer.EventLoop() );
		}
		catch(...)
		{
			IFXDELETE_ARRAY( ptr );
			IFXTRACE_GENERIC(L"Viewing failed\n");
			result = IFX_OK;
		}
	}

	IFXDELETE_ARRAY( ptr );

	return result;
}
