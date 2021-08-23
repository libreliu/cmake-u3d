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

/*
	This code developerd to test bones animation related issues (bone animation scaling, transformations, BWM)
*/

#include "UTHeaders.h"

#include "IFXModifierChain.h"
#include "IFXMixerConstruct.h"
#include "IFXMotionResource.h"
#include "IFXAnimationModifier.h"
#include "IFXBoneWeightsModifier.h"
#include "IFXVertexMap.h"

#include "IFXCheckX.h"
#include "U3DViewer.h"

#include "SampleSkeletonv2.h"
#include "IFXAuthorGeomCompiler.h"

/** 
	true means "animate our object instead of loaded one" 
	@todo external loading should be removed from this test
*/
const bool useMyObj = true;

/// Adds camera with some configuration to an scene
IFXRESULT cameraSetup (IFXSceneGraph* pSceneGraph);

/// This functions creates a cube mesh
IFXRESULT CreateCubeMesh(IFXAuthorCLODMesh* pClodMesh);

/// This functions creates few cube meshs 
IFXRESULT CreateCubesMesh(IFXAuthorCLODMesh* pClodMesh, U32 number);

/// Adds lights with some rMatrix to an scene
IFXRESULT lightSetup(IFXString lightResourceName,
					 IFXString lightNodeName,
					 IFXSceneGraph* pSceneGraph,
					 IFXMatrix4x4& rMatrix);

IFXRESULT  createModel(IFXSceneGraph* pSceneGraph, U3DSceneUtilities* m_pSGUtils);



/** 
	Returns reordered AM position number for initial one, renumbering investigation

	@note Returns first position from modified AM only (and there should be only one relative position in reordered mesh)

	@exception IFXException In case of any error occured.
*/
U32 getCLODPositionForInitialOneX(const U32 originalInd, IFXAuthorCLODResource* pAuthorClodResource)
{
	IFXRESULT res = IFX_OK;

	IFXDECLARELOCAL(IFXMeshMap, pMeshMap);
	IFXVertexMap *pVertexMap;

	pAuthorClodResource->GetAuthorMeshMap(&pMeshMap);
	pVertexMap = pMeshMap->GetPositionMap();
	U32 numEntries = pVertexMap->GetNumMapEntries();  // just fyi

	U32 indexMesh = 0, 
		indexVertex = 0;

	int count = pVertexMap->GetNumVertexCopies(originalInd);
	
	if (count > 1)
		throw new IFXException(IFX_E_INVALID_INDEX, (const IFXCHAR *)(L"Reordered Mesh contains more the 1 copy for original index, not expected in this particular test"));

	res = pVertexMap->GetVertexCopy(originalInd, count - 1 /* ==0 */, &indexMesh, &indexVertex);

	if (IFX_OK != res)
		throw new IFXException(IFX_E_NOT_FOUND, (const IFXCHAR *)(L"Strange output from pVertexMap->GetVertexCopy()"));

	return indexVertex;
}

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

			//////////////////////
			//TEST(IFXSUCCESS(pSceneGraph->Mark()), IFXException);
			if (!useMyObj)
			{
				TEST(IFXSUCCESS(pIFXLoadManager->Load(pIFXReadBuffer, IFXEXPORT_EVERYTHING)), IFXException); // ^ IFXEXPORT_FILEREFERENCES		
			}
			TEST(IFXSUCCESS(pIFXStdio->Close()), IFXException);
		}

		// Adding new objects to the scene
		{
			IFXRESULT result = IFX_OK;

			// Camera initialization
			cameraSetup(pSceneGraph);

			// Light Setup
			{
				IFXMatrix4x4 lightMatrix;

				lightMatrix.Reset();
				lightMatrix.Set(
					IFXVector3(1.000000f, 0.000000f, 0.000000f),
					IFXVector3(0.000000f, 1.000000f, 0.000000f),
					IFXVector3(0.000000f, 0.000000f, 1.000000f),
					IFXVector3(31.295425f, -134.068436f, 19.701351f));

				lightSetup (IFXString("LightResource1"), IFXString ("PointLight1"), pSceneGraph, 
					lightMatrix);

				lightMatrix.Reset();
				lightMatrix.Set(
					IFXVector3(0.707107f,	-0.707107f, 0.000000f  ),
					IFXVector3(0.353553f,	0.353553f,	0.866025f	),
					IFXVector3(-0.612372f,	-0.612372f, 0.500000f	),
					IFXVector3(-68.041374f,	-68.041374f, 55.555546f));

				lightSetup (IFXString("LightResource2"), 
					IFXString ("PointLight2"),
					pSceneGraph,   lightMatrix);

				lightMatrix.Reset();
				lightMatrix.Set(
					IFXVector3(-0.707107f, 0.707107f,  0.000000f ),
					IFXVector3(-0.353553f, -0.353553f, -0.866025f),
					IFXVector3(0.612372f, 0.612372f,   -0.500000f ),
					IFXVector3(68.041374f, 68.041374f, -55.555546f));

				lightSetup (IFXString("LightResource3"), IFXString ("PointLight3"),
					pSceneGraph,   lightMatrix);

				lightMatrix.Reset();
				lightMatrix.Set(
					IFXVector3(1.0f, -0.0f,  -0.000000f ),
					IFXVector3(0.0f, 1.0f, 0.0f),
					IFXVector3(0.0f, -0.0f,   1.000000f ),
					IFXVector3(0.0f, -68.041374f, 55.555546f));

				lightSetup (IFXString("LightResource4"), IFXString ("PointLight4"),
					pSceneGraph,   lightMatrix);
			}

			// Create Model with associated mesh
			createModel( pSceneGraph,  m_pSGUtils);
		}

		
		///////////////////////////////////////////////////////////////////////
		// Skeleton preparation
		///////////////////////////////////////////////////////////////////////
		
		IFXString name1(L"Scapula");
		IFXString name2(L"Humerus");
		IFXString name3(L"Ulna");
		IFXString name4(L"Radius");

		U32 uBoneCount = 4; // count
		U32 i = 0;

		IFXSkeleton* pSkeleton = NULL;
		IFXBoneInfo* pBoneInfo = new IFXBoneInfo[ uBoneCount ];

		pBoneInfo[0].stringBoneName = name1;

		pBoneInfo[1].stringParentName = name1;
		pBoneInfo[1].stringBoneName = name2;
		
		pBoneInfo[2].stringParentName = name2;
		pBoneInfo[2].stringBoneName = name3;

		pBoneInfo[3].stringParentName = name3;
		pBoneInfo[3].stringBoneName = name4;

		for( i = 0; i < uBoneCount; ++i )
		{
			pBoneInfo[i].uBoneAttributes = 0; // att.
			pBoneInfo[i].fBoneLength = 5; // len.

			pBoneInfo[i].v3BoneDisplacement.Set( 0.0f, 0.0f, 0.0f ); // displ. & orient.
			pBoneInfo[i].v4BoneRotation.Set( 1.0f, 0.0f, 0.0f, 0.0f );
		}

		TEST( IFXSUCCESS( IFXCreateComponent( CID_IFXSkeleton, IID_IFXSkeleton, (void **)&pSkeleton )), IFXException );

		// setup bone's iParentBoneID
		U32 j = 0;
		for( i = 0; i < uBoneCount; ++i )
		{
			pBoneInfo[i].iParentBoneID = -1;

			for( j = 0; j < uBoneCount; ++j )
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

		// Init bind pose position (done automatically by importer when read from file)
		{
			IFXBonesManager *pBonesManager = pSkeleton->GetBonesManagerNR();
			TEST(NULL != pBonesManager, IFXException );

			IFXRESULT result = pBonesManager->UpdateReferencePositions();
			TEST(IFX_OK == result, IFXException );
		}

		///////////////////////////////////////////////////////////////////////
		// Transmit (setup)  skeleton to AuthorCLODResource
		///////////////////////////////////////////////////////////////////////

		// Get the resource palette
		IFXDECLARELOCAL( IFXPalette, pPal );
		TEST(IFXSUCCESS(  pSceneGraph->GetPalette( IFXSceneGraph::GENERATOR, &pPal)), IFXException );

		U32 id = 0;
		IFXRESULT res = IFX_OK;

		if (!useMyObj)
		{
			TEST( IFXSUCCESS( pPal->Find(L"Box01", &id)), IFXException );
		}
		else
		{
			TEST( IFXSUCCESS( res = pPal->Find(L"ModelResource1", &id)), IFXException );
		}

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
					return IFX_E_UNDEFINED;
			}
		}

		///////////////////////////////////////////////////////////////////////
		// try to add skeleton animation
		{
			{
				// create motion resource
				IFXDECLARELOCAL(IFXMotionResource, pMotionResource );
				TEST(IFXSUCCESS(IFXCreateComponent( CID_IFXMotionResource, IID_IFXMotionResource, (void**)&pMotionResource )), IFXException);
				IFXDECLARELOCAL(IFXMarker,pMarker );
				TEST(IFXSUCCESS(pMotionResource->QueryInterface( IID_IFXMarker, (void**)&pMarker )), IFXException);
				TEST(IFXSUCCESS(pMarker->SetSceneGraph( pSceneGraph )), IFXException);

				IFXString sName;

				if (!useMyObj)
					sName = "Box01-Key";
				else
					sName = "ModelNode1-Key";

				IFXDECLARELOCAL(IFXPalette, pSGPalette);
				TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::MOTION, &pSGPalette )), IFXException);

				U32 iResultPaletteFind = pSGPalette->Find( &sName, &uResourceID );
				
				if ( IFX_E_CANNOT_FIND == iResultPaletteFind )
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
				const U32 uTimeCount = 3;
				IFXVector3 vPos( 0.0f, 0.0f, 0.0f );
				IFXQuaternion vRotation(1.0f,0.0f,0.0f,0.0f);
				IFXQuaternion vRotation1(1.0f,0.0f,0.0f,1.0f);
				IFXQuaternion vRotation2(1.0f,0.0f,-2.0f,1.0f);
				const IFXVector3 vScaleQ(1.0f,1.0f,1.0f);
				IFXVector3 vScaleQ1(1.0f,1.5f,0.1f);
				IFXVector3 vScaleQ2(1.0f,0.4f,1.0f);

				U32 i;
				for( i = 0; i < uTrackCount; ++i )
				{
					IFXKeyFrame *pKeyFrameArray = NULL;
					pKeyFrameArray = new IFXKeyFrame[uTimeCount];

					if( NULL == pKeyFrameArray )
						return IFX_E_OUT_OF_MEMORY;

					pKeyFrameArray[0].SetTime( 0.0f );
					pKeyFrameArray[0].Location() = vPos;
					pKeyFrameArray[0].Rotation() = vRotation;
					pKeyFrameArray[0].Scale() = vScaleQ;

					pKeyFrameArray[1].SetTime( 2.0f );
					{
						const IFXVector3 axis(0.0f, 0.0f, 1.0f);
						const F32 rad = 3.14f/3;
						pKeyFrameArray[1].Rotation() = IFXQuaternion(rad, axis);
					}
//					pKeyFrameArray[1].Rotation() = vRotation1;
					pKeyFrameArray[1].Scale() = vScaleQ2;

					switch( i )
					{
					case 0:
						pKeyFrameArray[1].Location() = IFXVector3( 0.0f, 0.0f, 0.0f );
						pKeyFrameArray[1].Rotation() = vRotation;
						break;
					case 1:
						pKeyFrameArray[1].Location() = vPos;//IFXVector3( -10.0f, 0.0f, 0.0f );
						break;
					case 2:
						pKeyFrameArray[1].Location() = vPos;//IFXVector3( 0.0f, 0.0f, 10.0f );
						break;
					case 3:
						pKeyFrameArray[1].Location() = vPos;//IFXVector3( -10.0f, 0.0f, 0.0f );
						break;
					default:
						break;
					}

					pKeyFrameArray[2].Location() = vPos;
					pKeyFrameArray[2].SetTime( 6.0f );
					pKeyFrameArray[2].Rotation() = vRotation;
					pKeyFrameArray[2].Scale() = vScaleQ;

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
					
					if (pMotionResource)
						pMixerConstruct->SetMotionResource(pMotionResource);
					else
						return IFX_E_UNDEFINED;

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
				IFXString sName;
				if (!useMyObj)
					sName = "Box01";
				else
					sName = "ModelResource1";

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

					pBWM->SetTotalBoneWeightCountForAuthorMesh( 50);

					U32 *pBones	= new U32[4];
					F32 *pWeights = new F32[3];

					pBones[0] = 0;
					pBones[1] = 1;
					pBones[2] = 2;
					pBones[3] = 3;

					pWeights[0] = 0.2f;
					pWeights[1] = 0.8f;
					pWeights[2] = 1.0f;

					try 
					{
						IFXDECLARELOCAL( IFXAuthorCLODResource, pAR );
						TEST( IFXSUCCESS( pPal->GetResourcePtr( id, IID_IFXAuthorCLODResource, (void**)&pAR )), IFXException);

						for (int i = 0; i < 8; ++i)
						{
							int ind = getCLODPositionForInitialOneX(i, pAR);
							TEST(IFXSUCCESS(pBWM->SetBoneWeightsForAuthorMesh( ind, 1, &pBones[0], &pWeights[2] )),IFXException);
						}
						for (int i = 8; i < 16; ++i)
							TEST(IFXSUCCESS(pBWM->SetBoneWeightsForAuthorMesh( getCLODPositionForInitialOneX(i, pAR), 1, &pBones[1], &pWeights[2] )),IFXException);
						for (int i = 16; i < 24; ++i)	
							TEST(IFXSUCCESS(pBWM->SetBoneWeightsForAuthorMesh( getCLODPositionForInitialOneX(i, pAR), 1, &pBones[2], &pWeights[2] )),IFXException);
						for (int i = 24; i < 40; ++i)
						{
							int ind = getCLODPositionForInitialOneX(i, pAR);
							TEST(IFXSUCCESS(pBWM->SetBoneWeightsForAuthorMesh( ind, 1, &pBones[3], &pWeights[2] )),IFXException);
						}
						for (int i = 40; i < 48; ++i)
						{
							int ind = getCLODPositionForInitialOneX(i, pAR);
							TEST(IFXSUCCESS(pBWM->SetBoneWeightsForAuthorMesh( ind, 1, &pBones[0], &pWeights[2] )),IFXException);
						}					
					}
					catch (...)
					{
						Log("Caught exception from getCLODPositionForInitialOneX(...) ");
						TEST(FALSE, IFXException);
					}

					delete [] pBones;
					delete [] pWeights;
				}
				IFXDELETE_ARRAY( pBoneInfo );
			}

			// create animation modifier
			{
				IFXString sName;
				if (!useMyObj)
					sName = "Box01";
				else
					sName = "ModelNode1";

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
					if ( IFX_E_CANNOT_FIND == iResultPaletteFind)
						TEST(IFXSUCCESS(pSGPalette->Add( &sName, &uResourceID )), IFXException);

					// If this is a chain object
					{
						// get the modifier at the head of the chain
						IFXDECLARELOCAL(IFXModifier,pHeadModifier);
						U32 iResultGetResource = pSGPalette->GetResourcePtr(uResourceID,IID_IFXModifier,(void**)&pHeadModifier);

						// Get the modifier chain from the head modifier
						IFXDECLARELOCAL(IFXModifierChain,pModifierChain);
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

					IFXString mixerName;
					
					if (!useMyObj)
						mixerName = "Box01-Key";
					else
						mixerName = "ModelNode1-Key";

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
			//saving to file with 2 skeletons
			IFXDECLARELOCAL(IFXWriteManager, pIFXWriteManager);
			TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXWriteManager, IID_IFXWriteManager, (void**)&pIFXWriteManager)), IFXException);
			TEST(IFXSUCCESS(pIFXWriteManager->Initialize(m_pSGUtils->m_pCoreServices)), IFXException);
			IFXDECLARELOCAL(IFXWriteBuffer, pIFXWriteBuffer);
			TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioWriteBuffer, IID_IFXWriteBuffer, (void**)&pIFXWriteBuffer)), IFXException);
			IFXDECLARELOCAL(IFXStdio, pIFXStdio);
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




IFXRESULT lightSetup(IFXString lightResourceName,
					 IFXString lightNodeName,
					 IFXSceneGraph* pSceneGraph,
					 IFXMatrix4x4& rMatrix)
{
	IFXRESULT result = IFX_OK;

	U32	lightResourceID	= 0;
	U32	lightNodeID	= 0;

	// Light parameters
	IFXLightResource::LightType	lightType =	IFXLightResource::DIRECTIONAL;
	F32	lightAttenuation[3]	= {1, 0, 0};
	IFXVector4 lightColor(1, 1,	1);
	F32	lightIntensity = 1.0f;

	// Create Light	node
	IFXDECLARELOCAL(IFXLight, pLightNode);	
	if (IFXSUCCESS(result))
		result = IFXCreateComponent(CID_IFXLight, IID_IFXLight,	(void**)&pLightNode);

	// Initialize the light
	if (IFXSUCCESS(result))
		result = pLightNode->SetSceneGraph(pSceneGraph);

	IFXDECLARELOCAL(IFXPalette, pNodePalette);
	if (IFXSUCCESS(result))
		result = pSceneGraph->GetPalette(IFXSceneGraph::NODE, &pNodePalette);

	// Set the parent of the light to be the World.	
	// As with all nodes, Light	nodes can appear anywhere 
	// in the scene	hierarchy.
	{
		IFXDECLARELOCAL(IFXNode, pWorldNode);
		if (IFXSUCCESS(result))
			result = pNodePalette->GetResourcePtr(0, IID_IFXNode, (void**)&pWorldNode);
		if (IFXSUCCESS(result))
			result = pLightNode->AddParent(pWorldNode);
	}

	// Add an entry	into the Node 
	// Hierarchy Palette for the new Light node.	
	if (IFXSUCCESS(result))
		result = pNodePalette->Add(lightNodeName, &lightNodeID);

	// Initialize the new palette entry	with the Light node
	if (IFXSUCCESS(result))
		result = pNodePalette->SetResourcePtr(lightNodeID, pLightNode);

	// Set Light Node translation Matrix
	if (IFXSUCCESS(result))
	{
		IFXMatrix4x4 matrix;
		matrix.Reset();

		result = pLightNode->SetMatrix(0, &rMatrix);
	}

	// Create Light	resource
	IFXDECLARELOCAL(IFXLightResource, pLightResource);
	if (IFXSUCCESS(result))
		result = IFXCreateComponent(CID_IFXLightResource, IID_IFXLightResource,
		(void**)&pLightResource);

	// Initialize the light	resource
	if (IFXSUCCESS(result))
		result = pLightResource->SetSceneGraph(pSceneGraph);

	// Set light parameters
	if (IFXSUCCESS(result))
	{	
		pLightResource->SetType(lightType);
		pLightResource->SetAttenuation(lightAttenuation);
		pLightResource->SetColor(lightColor);
		pLightResource->SetIntensity(lightIntensity);
	}

	// Get light palette from scenegraph.
	IFXDECLARELOCAL(IFXPalette,	pLightPalette);
	if (IFXSUCCESS(result))
		result = pSceneGraph->GetPalette(IFXSceneGraph::LIGHT, &pLightPalette);

	// Add an entry	into the Light Resource	Palette	for	the	new	light resource.
	if (IFXSUCCESS(result))
		result = pLightPalette->Add(lightResourceName, &lightResourceID);

	// Initialize the new palette entry	with the light resource.
	if (IFXSUCCESS(result))
		result = pLightPalette->SetResourcePtr(lightResourceID,	pLightResource);

	// Set the light to	use	the	light resource created above.
	// This	creates	the	scene link between the light and 
	// the light resource.
	if (IFXSUCCESS(result))
		result = pLightNode->SetLightResourceID(lightResourceID);

	return result;
}


IFXRESULT cameraSetup (IFXSceneGraph* pSceneGraph)
{
	IFXRESULT result = IFX_OK;

	IFXString viewNodeName("Camera1");
	U32 viewNodeID = 0;
	IFXString viewResourceName("CameraResource1");
	U32 viewResourceID = 0;
	IFXView* pViewNode = NULL;
	IFXViewResource* pViewResource = NULL;
	IFXPalette* pViewPalette = NULL;
	U32 worldNodeID = 0;

	// Viewport settings
	F32 viewportWidth = 500;
	F32 viewportHeight = 500;
	F32 viewProjection = 34.515877f;

	// Create View node
	if (IFXSUCCESS(result))
		result = IFXCreateComponent(CID_IFXView, IID_IFXView, (void**)&pViewNode);

	// Initialize the View Node object
	if (IFXSUCCESS(result))
		result = pViewNode->SetSceneGraph(pSceneGraph);


	IFXDECLARELOCAL(IFXPalette, pNodePalette);
	if (IFXSUCCESS(result))
		result = pSceneGraph->GetPalette(IFXSceneGraph::NODE, &pNodePalette);

	// Retrieve the World node from the Node Hierarchy Palette
	// and use it as the parent node of the view.
	IFXDECLARELOCAL(IFXNode, pWorldNode);
	if (IFXSUCCESS(result))
		result = pNodePalette->GetResourcePtr(0, IID_IFXNode, (void**)&pWorldNode);

	// Set the World to be the parent of the View Node
	if (IFXSUCCESS(result))
		result = pViewNode->AddParent(pWorldNode);

	// Add an entry for the view node to the node palette
	if (IFXSUCCESS(result))
		result = pNodePalette->Add(viewNodeName, &viewNodeID);

	// Point the node palette entry for the view to the view component
	if (IFXSUCCESS(result))
		result = pNodePalette->SetResourcePtr(viewNodeID, pViewNode);

	// Set the view node's local transtalion matrix
	if (IFXSUCCESS(result))
	{
		IFXMatrix4x4 matrix;
		matrix.Reset();
		matrix.Set(
			IFXVector3(1.000000f, 0.000000f, 0.000000f),
			IFXVector3(0.000000f, 0.258819f, 0.965926f),
			IFXVector3(0.000000f, -0.965926f, 0.258819f),
			IFXVector3(0.000000f, -241.481461f, 64.704765f)
			);
		result = pViewNode->SetMatrix(0, &matrix);
	}

	// Create View resource
	if (IFXSUCCESS(result))
		result = IFXCreateComponent(CID_IFXViewResource, IID_IFXViewResource,
		(void**)&pViewResource);

	// Init View Resource with IFXSceneGraph reference
	if (IFXSUCCESS(result))
		result = pViewResource->SetSceneGraph(pSceneGraph);

	// Get View Palette reference
	if (IFXSUCCESS(result))
		result = pSceneGraph->GetPalette(IFXSceneGraph::VIEW, &pViewPalette);

	// Add IFXViewResource to the View Palette
	if (IFXSUCCESS(result))
		result = pViewPalette->Add(viewResourceName, &viewResourceID);

	// Assign View resource to View node.
	if (IFXSUCCESS(result))
		result = pViewNode->SetViewResourceID(viewResourceID);

	// Initialize the new palette entry with the View Resource
	if (IFXSUCCESS(result))
		result = pViewPalette->SetResourcePtr(viewResourceID, pViewResource);

	// Set root node for this resource. The camera will only see those
	// nodes at or below the node in the parent/child hierarchy.
	if (IFXSUCCESS(result))
		result = pViewResource->SetRootNode(worldNodeID, 0);

	// Set viewport
	if (IFXSUCCESS(result))
	{	
		//Set View Port
		IFXF32Rect rect;
		rect.m_Width = viewportWidth;
		rect.m_Height = viewportHeight;
		result = pViewNode->SetViewport(rect);
	}

	// Set projection
	if (IFXSUCCESS(result))
		result = pViewNode->SetProjection(viewProjection);

	// 3-point perspective projection and screen position units in screen pixels
	if (IFXSUCCESS(result))
		pViewNode->SetAttributes(IFX_PERSPECTIVE3 | IFX_SCREENPIXELS); 

	// Release interfaces
	IFXRELEASE(pViewPalette)
	IFXRELEASE(pViewResource)
	IFXRELEASE(pViewNode)

	return result;
}


IFXRESULT createModel(IFXSceneGraph* pSceneGraph, U3DSceneUtilities* m_pSGUtils)
{
	IFXRESULT result = IFX_OK;

	IFXString modelNodeName("ModelNode1");
	U32 modelNodeID = 0;
	IFXString modelResourceName("ModelResource1");
	U32 modelResourceID = 0;

	IFXDECLARELOCAL(IFXAuthorCLODResource, pAuthorClodResource);
	IFXDECLARELOCAL(IFXAuthorCLODMesh, pAuthorClodMesh);
	IFXDECLARELOCAL(IFXAuthorGeomCompiler, pCompiler);
	IFXDECLARELOCAL(IFXPalette, pGeneratorPalette);
	IFXDECLARELOCAL(IFXModel, pModelNode);

	// Create Model node component
	if (IFXSUCCESS(result))
		result = IFXCreateComponent(CID_IFXModel, IID_IFXModel, (void**)&pModelNode);

	// Initialize model object.
	if (IFXSUCCESS(result))
		result = pModelNode->SetSceneGraph(pSceneGraph);

	IFXDECLARELOCAL(IFXPalette, pNodePalette);
	if (IFXSUCCESS(result))
		result = pSceneGraph->GetPalette(IFXSceneGraph::NODE, &pNodePalette);

	// Set the parent of the model to be the World. 
	{

		IFXDECLARELOCAL(IFXNode, pWorldNode);
		if (IFXSUCCESS(result))
			result = pNodePalette->GetResourcePtr(0, IID_IFXNode, (void**)&pWorldNode);

		if (IFXSUCCESS(result))
			result = pModelNode->AddParent(pWorldNode);
	}

	// Add an entry into the Node 
	// Hierarchy Palette for the new model node.	
	if (IFXSUCCESS(result))
		result = pNodePalette->Add(modelNodeName, &modelNodeID);

	// Initialize the new palette entry with the model node.
	if (IFXSUCCESS(result))
		result = pNodePalette->SetResourcePtr(modelNodeID, pModelNode);

	// Setting the model's local tranlation matrix.
	if (IFXSUCCESS(result))
	{
		IFXMatrix4x4 matrix;
		matrix.Reset();
		matrix.Set(
			IFXVector3(1.000000f, 0.000000f, 0.000000f),
			IFXVector3(0.000000f, 1.000000f, 0.000000f),
			IFXVector3(0.000000f, 0.000000f, 1.000000f),
			IFXVector3(-3.336568f*0, -63.002571f*0, 0.000000f)
			);
		result = pModelNode->SetMatrix(0, &matrix);
	}

	// Create Author mesh component
	if (IFXSUCCESS(result))
		result = IFXCreateComponent(CID_IFXAuthorMesh, IID_IFXAuthorCLODMesh,
		(void**)&pAuthorClodMesh);

	// Fill IFXAuthorMesh objects data 
	//	if (IFXSUCCESS(result))
	//		result = CreateCubeMesh(pAuthorClodMesh);
	if (IFXSUCCESS(result))
		result = CreateCubesMesh(pAuthorClodMesh, 6);



	// Create mesh compiler that will create updates for progressive CLOD mesh
	if (IFXSUCCESS(result))
		result = IFXCreateComponent(CID_IFXAuthorGeomCompiler, IID_IFXAuthorGeomCompiler,
		(void**)&pCompiler);

	// Initialize mesh compiler component with SceneGraph reference. 
	if (IFXSUCCESS(result))
		result = pCompiler->SetSceneGraph(pSceneGraph);

	// Set mesh compiler parameters and compile mesh (create progressive updates)
	if (IFXSUCCESS(result))
	{
		IFXAuthorGeomCompilerParams aparams;
		// Set minimal resolution to make all mesh progressive (no base mesh)
		aparams.CompressParams.bSetMinimumResolution = TRUE;
		aparams.CompressParams.uMinimumResolution = 0;
		// Set default quality
		aparams.CompressParams.bSetDefaultQuality = TRUE;
		aparams.CompressParams.uDefaultQuality = IFX_DEFAULT_QUALITY_FACTOR;
		// Set compiler flag to use assignments above
		aparams.bCompressSettings = TRUE;
		// Progress function could be set to be called during CLOD creation process
		aparams.pProgressCallback = NULL;
		result = pCompiler->Compile(modelResourceName, pAuthorClodMesh,
			&pAuthorClodResource, FALSE, &aparams);
	}


	// Vertex renumbering investigation
	{
		IFXRESULT res = IFX_OK;
		//IFXDECLARELOCAL(IFXAuthorCLODResource, pACR);
		//res = pAuthorClodResource->QueryInterface(IID_IFXAuthorCLODResource, (void**)&pACR);

		IFXDECLARELOCAL(IFXMeshMap, pMeshMap);
		IFXVertexMap *pVertexMap;

		pAuthorClodResource->GetAuthorMeshMap(&pMeshMap);
		pVertexMap = pMeshMap->GetPositionMap();
		int numEntries = pVertexMap->GetNumMapEntries();

		U32 *indexMesh = 0, *indexVertex = 0;
		
		indexMesh = new U32;
		indexVertex = new U32;

		int count = 0;
		U32 originalInd = 1;
		for (int originalInd = 0; originalInd < 8; ++originalInd)
		{
			count = pVertexMap->GetNumVertexCopies(originalInd);
			res = pVertexMap->GetVertexCopy(originalInd, count -1, indexMesh, indexVertex);
		}
		IFXASSERT (IFX_OK==res);
	}


	// Get Generator palette.
	if (IFXSUCCESS(result))
		result = pSceneGraph->GetPalette(IFXSceneGraph::GENERATOR, &pGeneratorPalette);

	// Add an entry into the Generator Resource Palette for the new AuthorClod resource.
	if (IFXSUCCESS(result))
		result = pGeneratorPalette->Add(modelResourceName, &modelResourceID);

	// Initialize the new palette entry with the Light node.
	if (IFXSUCCESS(result))
		result = pGeneratorPalette->SetResourcePtr(modelResourceID, pAuthorClodResource);

	// Record the generator palette ID of the generator in the model component.
	if (IFXSUCCESS(result))
		result = pModelNode->SetResourceIndex(modelResourceID);

	IFXVector4 Ambient = IFXVector4(1,(F32)0.5,(F32)0.1);
	IFXVector4 Emission = IFXVector4(1,(F32)0.1,(F32)0.1);
	IFXVector4 Diffuse = IFXVector4(1,(F32)0.1,(F32)0.8);
	IFXVector4 Specular = IFXVector4(0.5,(F32)0.1,(F32)0.1);
	//Set material resource
	/// @todo should test return values
	result = m_pSGUtils->SetMaterialResource(modelNodeName,
		NULL, //&Ambient, 
		&Diffuse, 
		NULL, //&Emission, 
		NULL, //&Specular,
		0.5f,
		0.9f,
		TRUE,
		/*IFXShaderLitTexture::WIRE |*/ IFXShaderLitTexture::MATERIAL
		);

	return result;

} // Create Model with associated mesh




/*
	This routine generates set of cyllinders with following profile:
	      ..--^^|
	..--^^		|
	!...........|
*/
IFXRESULT CreateCubesMesh(IFXAuthorCLODMesh* pClodMesh, U32 number)
{
	if (!pClodMesh) return IFX_E_INVALID_POINTER;
	IFXRESULT result = IFX_OK;

	// Positions
	F32 positions[] =
	{
		-2.000000, -2.000000, 0.000000,
		2.000000, -2.000000, 0.000000,
		-2.000000, 2.000000, 0.000000,
		2.000000, 2.000000, 0.000000,
		-2.000000, -2.000000, 2.000000,
		2.000000, -2.000000, 1.000000,
		-2.000000, 2.000000, 2.000000,
		2.000000, 2.000000, 1.000000
	};
	U32 positionsCount = sizeof(positions) / sizeof(F32) / 3;

	// Face positions
	U32 meshFaces[] = 
	{
		0, 2, 3,
		3, 1, 0,
		4, 5, 7,
		7, 6, 4,
		0, 1, 5,
		5, 4, 0,
		1, 3, 7,
		7, 5, 1,
		3, 2, 6,
		6, 7, 3,
		2, 0, 4,
		4, 6, 2
	};
	U32 facesCount = sizeof(meshFaces) / sizeof(U32) / 3;

	// Normals
	F32 normals[] =
	{
		0.000000, 0.000000, -1.000000,
		0.000000, 0.000000, -1.000000,
		0.000000, 0.000000, -1.000000,
		0.000000, 0.000000, -1.000000,
		0.000000, 0.000000, -1.000000,
		0.000000, 0.000000, -1.000000,

		0.000000, 0.000000, 1.000000,
		0.000000, 0.000000, 1.000000,
		0.000000, 0.000000, 1.000000,
		0.000000, 0.000000, 1.000000,
		0.000000, 0.000000, 1.000000,
		0.000000, 0.000000, 1.000000,

		0.000000, -1.000000, 0.000000,
		0.000000, -1.000000, 0.000000,
		0.000000, -1.000000, 0.000000,
		0.000000, -1.000000, 0.000000,
		0.000000, -1.000000, 0.000000,
		0.000000, -1.000000, 0.000000,

		1.000000, 0.000000, 0.000000,
		1.000000, 0.000000, 0.000000,
		1.000000, 0.000000, 0.000000,
		1.000000, 0.000000, 0.000000,
		1.000000, 0.000000, 0.000000,
		1.000000, 0.000000, 0.000000,

		0.000000, 1.000000, 0.000000,
		0.000000, 1.000000, 0.000000,
		0.000000, 1.000000, 0.000000,
		0.000000, 1.000000, 0.000000,
		0.000000, 1.000000, 0.000000,
		0.000000, 1.000000, 0.000000,

		-1.000000, 0.000000, 0.000000,
		-1.000000, 0.000000, 0.000000,
		-1.000000, 0.000000, 0.000000,
		-1.000000, 0.000000, 0.000000,
		-1.000000, 0.000000, 0.000000,
		-1.000000, 0.000000, 0.000000
	};
	U32 normalsCount = sizeof(normals) / sizeof(F32) / 3;

	// Face normals
	U32 faceNormals[] = 
	{
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
		9, 10, 11,
		12, 13, 14,
		15, 16, 17,
		18, 19, 20,
		21, 22, 23,
		24, 25, 26,
		27, 28, 29,
		30, 31, 32,
		33, 34, 35
	};
	U32 faceNormalsCount = sizeof(faceNormals) / sizeof(U32) / 3;

	// Texture coordinates
	IFXVector4	tc1(0.0f,0.0f,0.0f,0.0f),
		tc2(1.0f,0.0f,0.0f,0.0f),
		tc3(1.0f,1.0f,0.0f,0.0f),
		tc4(0.0f,1.0f,0.0f,0.0f);
	U32 texCoordsCount = 4;

	// Initialize AuthorMesh descriptor
	IFXAuthorMeshDesc desc;
	desc.NumBaseVertices = 0;
	desc.NumPositions = number * positionsCount;
	desc.NumNormals   = number * normalsCount;
	desc.NumFaces     = number * facesCount;
	desc.NumSpecularColors = 0;
	desc.NumTexCoords =  number * texCoordsCount;
	desc.NumMaterials = 1;
	desc.NumDiffuseColors = 0;

	// Allocate memory for mesh
	if (IFXSUCCESS(result))
		result = pClodMesh->Allocate(&desc);

	// Initialize AuthorMesh
	if (IFXSUCCESS(result))
		result = pClodMesh->SetMeshDesc(&desc);

	// Set positions
	IFXVector3 vector;
	U32 i;
	for (i = 0; (i < number * positionsCount) && IFXSUCCESS(result); ++i)
	{
		// box size ~2, so I use 5 as distance between objects
		vector.X() = positions[(i % positionsCount)*3] + 5 * (i / positionsCount) +2 ; 
		vector.Y() = positions[(i % positionsCount)*3+1];
		vector.Z() = positions[(i % positionsCount)*3+2];
		result = pClodMesh->SetPosition(i, &vector);
	}

	// Set face positions
	IFXAuthorFace face;
	for (i = 0; (i < number * facesCount) && IFXSUCCESS(result); ++i)
	{
		int a = (i % facesCount)*3 + (i / facesCount) * facesCount;
		face.Set(
			meshFaces[(i % facesCount)*3] + (i / facesCount) * 8,
			meshFaces[(i % facesCount)*3+1] + (i / facesCount) * 8, 
			meshFaces[(i % facesCount)*3+2] + (i / facesCount) * 8
			);
		result = pClodMesh->SetPositionFace(i, &face);
	}

	// Set normals
	for (i = 0; (i < number * normalsCount) && IFXSUCCESS(result); i++)
	{
		vector.X() = normals[(i % normalsCount)*3];
		vector.Y() = normals[(i % normalsCount)*3+1];
		vector.Z() = normals[(i % normalsCount)*3+2];
		result = pClodMesh->SetNormal(i, &vector);
	}

	// Set face normals
	for (i = 0; (i < number * faceNormalsCount) && IFXSUCCESS(result); ++i)
	{
		face.Set(faceNormals[(i % faceNormalsCount)*3], faceNormals[(i % faceNormalsCount)*3+1], faceNormals[(i % faceNormalsCount)*3+2]);
		result = pClodMesh->SetNormalFace(i, &face);
	}

	// Set material
	if (IFXSUCCESS(result))
	{
		// Determine the material we will use.
		IFXAuthorMaterial material;
		material.m_uNumTextureLayers		= 1;
		material.m_uTexCoordDimensions[0]	= 2;
		material.m_uOriginalMaterialID		= 0;
		material.m_uDiffuseColors			= FALSE;
		material.m_uSpecularColors			= FALSE;
		material.m_uNormals					= TRUE;

		result = pClodMesh->SetMaterial(0, &material);
	}

	// Set texture coordinates
	if (IFXSUCCESS(result))
		result = pClodMesh->SetTexCoord(0, &tc1);
	if (IFXSUCCESS(result))
		result = pClodMesh->SetTexCoord(1, &tc2);
	if (IFXSUCCESS(result))
		result = pClodMesh->SetTexCoord(2, &tc3);
	if (IFXSUCCESS(result))
		result = pClodMesh->SetTexCoord(3, &tc4);

	// Set face texture coordinates
	for (i = 0; (i < number * facesCount) && IFXSUCCESS(result); i += 2)
	{
		IFXAuthorFace texFace;
		texFace.Set(0, 1, 2);
		if (IFXSUCCESS(result))
			result = pClodMesh->SetTexFace(0, i, &texFace);
		texFace.Set(2, 3, 0);
		if (IFXSUCCESS(result))
			result = pClodMesh->SetTexFace(0, i+1, &texFace);
	}

	// Set face materials
	for (i = 0; (i < number * facesCount) && IFXSUCCESS(result); ++i)
	{
		result = pClodMesh->SetFaceMaterial(i, 0);
	}

	return result;
}
