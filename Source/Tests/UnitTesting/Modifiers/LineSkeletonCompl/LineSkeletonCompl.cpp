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

#ifndef LineSkeletonCompl_H
#include "LineSkeletonCompl.h"
#endif
#include "IFXAuthorLineSetResource.h"

IFXRESULT LineSkeletonCompl::Run( U3DSceneUtilities* pSGUtils )
{
	U32 uResourceID = 0;
	TEST(IFXSUCCESS(pSGUtils->InitializeScene()), IFXException);

	IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
	IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);
	IFXDECLARELOCAL(IFXStdio, pIFXStdio);

	//load CLOD with bones from U3D file
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager)), IFXException);
	TEST(IFXSUCCESS(pIFXLoadManager->Initialize(pSGUtils->m_pCoreServices)), IFXException);
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer)), IFXException);
	TEST(IFXSUCCESS(pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio)), IFXException);

	IFXString src_base	= getenv("U3D_SOURCE");
	IFXString src		= src_base + "Tests/UnitTesting/Modifiers/LineSkeletonCompl/lineset.u3d";
	IFXRESULT res		= pIFXStdio->Open( (IFXCHAR*) src.Raw() );

	IFXString name1("L1");
	IFXString name2("L2");
	IFXString name3("L3");
	IFXString name4("L4");
	IFXString name5("L5");
	IFXString name6("L6");
	IFXString name7("L7");
	IFXString name8("L8");
	IFXString name9("L9");
	IFXString name10("L10");
	IFXString name11("L11");
	IFXString name12("L12");

  if( IFXSUCCESS( res ) )
  {
	TEST(IFXSUCCESS(pSGUtils->m_pSceneGraph->Mark()), IFXException);
	TEST(IFXSUCCESS(pIFXLoadManager->Load(pIFXReadBuffer, IFXEXPORT_EVERYTHING)), IFXException); // ^ IFXEXPORT_FILEREFERENCES
	TEST(IFXSUCCESS(pIFXStdio->Close()), IFXException);
	IFXRELEASE(pIFXStdio);
	
    U32 uBoneCount = 12; 
	U32 i = 0;
    IFXSkeleton *pSkeleton = NULL;
    IFXBoneInfo *pBoneInfo = new IFXBoneInfo[ uBoneCount ];
	for( i = 0; i < uBoneCount; i++ ) {
		switch(i)
		{
			case 0:
				pBoneInfo[i].stringBoneName = name1;
				pBoneInfo[i].stringParentName = name12;
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
			case 4:
				pBoneInfo[i].stringBoneName = name5;
				pBoneInfo[i].stringParentName = name4;
				break;
			case 5:
				pBoneInfo[i].stringBoneName = name5;
				pBoneInfo[i].stringParentName = name4;
				break;
			case 6:
				pBoneInfo[i].stringBoneName = name6;
				pBoneInfo[i].stringParentName = name5;
				break;
			case 7:
				pBoneInfo[i].stringBoneName = name7;
				pBoneInfo[i].stringParentName = name6;
				break;
			case 8:
				pBoneInfo[i].stringBoneName = name8;
				pBoneInfo[i].stringParentName = name7;
				break;
			case 9:
				pBoneInfo[i].stringBoneName = name9;
				pBoneInfo[i].stringParentName = name1;
				break;
			case 10:
				pBoneInfo[i].stringBoneName = name10;
				pBoneInfo[i].stringParentName = name9;
				break;
			case 11:
				pBoneInfo[i].stringBoneName = name11;
				pBoneInfo[i].stringParentName = name10;
				break;
			default:
				break;
		}
		pBoneInfo[i].uBoneAttributes	= 0; 
		pBoneInfo[i].fBoneLength		= 0.77f; 

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

    // Get the scene graph
    IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );
    TEST( IFXSUCCESS( pSGUtils->m_pCoreServices->GetSceneGraph(IID_IFXSceneGraph,(void**)&pSceneGraph)), IFXException );

    // Get the resource palette
    IFXDECLARELOCAL( IFXPalette, pPal );
    TEST(IFXSUCCESS(  pSceneGraph->GetPalette( IFXSceneGraph::GENERATOR, &pPal)), IFXException );

    U32 id=0;    
	TEST( IFXSUCCESS( pPal->Find(L"LineSet", &id)), IFXException );

    // 1.1.1a If the light exists, Use it
	IFXDECLARELOCAL( IFXAuthorLineSetResource, pAR );
    TEST( IFXSUCCESS( pPal->GetResourcePtr( id, IID_IFXAuthorLineSetResource, (void**)&pAR )), IFXException);

    if( pAR )
		pAR->SetBones( pSkeleton );
    else
		return IFX_E_UNDEFINED;

    IFXRELEASE( pSkeleton );
    // test bones
    {
      pSkeleton = NULL;
      pSkeleton = pAR->GetBones();

      // Try to get skeleton from LineSet resource
      if( NULL == pSkeleton )  {
		  return IFX_E_UNDEFINED;
      }  else  {
        // Get bones number from the skeleton
        U32 uNum;
        TEST( IFXSUCCESS( pSkeleton->GetNumBones( uNum ) ), IFXException );
        if( uNum != uBoneCount )
        {
			return IFX_E_UNDEFINED;
        }
      }
    }
	
	/**********************************************************************************************************
     try to add skeleton animation
	*/
	{
      {
        // create motion resource
        IFXDECLARELOCAL(IFXUnknown,pObject);
        TEST(IFXSUCCESS(IFXCreateComponent( CID_IFXMotionResource, IID_IFXUnknown, (void**)&pObject )), IFXException);
        IFXDECLARELOCAL(IFXMarker,pMarker );
        TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXMarker, (void**)&pMarker )), IFXException);
        IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
        TEST(IFXSUCCESS(pSGUtils->m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph )), IFXException);
        TEST(IFXSUCCESS(pMarker->SetSceneGraph( pSceneGraph )), IFXException);

        IFXString sName("Box01-Key");

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
		//4
        iFindResult = pMotionResource->FindTrack( &name4, &uTrackID[3] );
        if( IFXFAILURE( iFindResult ) )
        {
            TEST(IFXSUCCESS(pMotionResource->AddTrack( &name4, &uTrackID[3] )), IFXException);
        }

		//5
        iFindResult = pMotionResource->FindTrack( &name5, &uTrackID[4] );
        if( IFXFAILURE( iFindResult ) )
        {
            TEST(IFXSUCCESS(pMotionResource->AddTrack( &name5, &uTrackID[4] )), IFXException);
        }
		//6
        iFindResult = pMotionResource->FindTrack( &name6, &uTrackID[5] );
        if( IFXFAILURE( iFindResult ) )
        {
            TEST(IFXSUCCESS(pMotionResource->AddTrack( &name6, &uTrackID[5] )), IFXException);
        }
		//7
        iFindResult = pMotionResource->FindTrack( &name7, &uTrackID[6] );
        if( IFXFAILURE( iFindResult ) )
        {
            TEST(IFXSUCCESS(pMotionResource->AddTrack( &name7, &uTrackID[6] )), IFXException);
        }
		//8
        iFindResult = pMotionResource->FindTrack( &name8, &uTrackID[7] );
        if( IFXFAILURE( iFindResult ) )
        {
            TEST(IFXSUCCESS(pMotionResource->AddTrack( &name8, &uTrackID[7] )), IFXException);
        }
		//9
        iFindResult = pMotionResource->FindTrack( &name9, &uTrackID[8] );
        if( IFXFAILURE( iFindResult ) )
        {
            TEST(IFXSUCCESS(pMotionResource->AddTrack( &name9, &uTrackID[8] )), IFXException);
        }

		//10
        iFindResult = pMotionResource->FindTrack( &name10, &uTrackID[9] );
        if( IFXFAILURE( iFindResult ) )
        {
            TEST(IFXSUCCESS(pMotionResource->AddTrack( &name10, &uTrackID[9] )), IFXException);
        }
		//11
        iFindResult = pMotionResource->FindTrack( &name11, &uTrackID[10] );
        if( IFXFAILURE( iFindResult ) )
        {
            TEST(IFXSUCCESS(pMotionResource->AddTrack( &name11, &uTrackID[10] )), IFXException);
        }
		//12
        iFindResult = pMotionResource->FindTrack( &name4, &uTrackID[11] );
        if( IFXFAILURE( iFindResult ) )
        {
            TEST(IFXSUCCESS(pMotionResource->AddTrack( &name4, &uTrackID[11] )), IFXException);
        }

        // 6. TimeCount
        U32 uTimeCount = 2;
        IFXVector3 vPos( .0f, .0f, .0f );
        IFXQuaternion vRotation(1.0f,0.0f,0.0f,0.0f);
		IFXVector3 vScaleQ(1.0f,1.0f,1.0f);

        for( U32 i = 0; i < uTrackCount; i++ )
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
				default:
					break;
			}

			pKeyFrameArray[1].SetTime( 5.0f );
			pKeyFrameArray[1].Rotation() = vRotation;
			pKeyFrameArray[1].Scale() = vScaleQ;

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
		/**
		create bone weights modifier
		*/
		{
			IFXDECLARELOCAL(IFXModifierChain, pModifierChain);
			IFXString sName("LineSet");

			IFXDECLARELOCAL(IFXUnknown,pObject);
			TEST(IFXSUCCESS(IFXCreateComponent( CID_IFXBoneWeightsModifier, IID_IFXUnknown, (void**)&pObject )), IFXException);
			IFXDECLARELOCAL(IFXMarker,pMarker );
			TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXMarker, (void**)&pMarker )), IFXException);
			IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
			TEST(IFXSUCCESS(pSGUtils->m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph )), IFXException);
			TEST(IFXSUCCESS(pMarker->SetSceneGraph( pSceneGraph )), IFXException);
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

			IFXDECLARELOCAL( IFXBoneWeightsModifier, pBWM );
			TEST(IFXSUCCESS(pMod->QueryInterface( IID_IFXBoneWeightsModifier, (void**)&pBWM)), IFXException);

			pBWM->SetTotalBoneWeightCountForAuthorMesh( 12 );

			U32 *pBones   = new U32[uBoneCount];
			F32 *pWeights = new F32[6];

			pBones[0] = 0;
			pBones[1] = 1;
			pBones[2] = 2;
			pBones[3] = 3;
			pBones[4] = 4;
			pBones[5] = 5;
			pBones[6] = 6;
			pBones[7] = 7;
			pBones[8] = 8;
			pBones[9] = 9;
			pBones[10] = 10;
			pBones[11] = 11;

            //originally was (before SetBoneWeightsForAuthorMesh fix)
            //pWeights[0] = 0.25f;
			//pWeights[1] = 0.50f;
			//pWeights[2] = 1.0f;
			//pWeights[3] = 0.5f;
			//pWeights[4] = 0.10f;
			//pWeights[5] = 0.15f;

            pWeights[0] = 0.5f;
            pWeights[1] = 0.50f;
            pWeights[2] = 0.5f;
            pWeights[3] = 0.5f;
            pWeights[4] = 0.10f;
            pWeights[5] = 0.15f;

            // set bone weights and test return values
            {
                IFXRESULT res = IFX_OK;

                res = res | pBWM->SetBoneWeightsForAuthorMesh( 0, 2, &pBones[0], &pWeights[0] );
                res = res | pBWM->SetBoneWeightsForAuthorMesh( 1, 2, &pBones[1], &pWeights[1] );
                res = res | pBWM->SetBoneWeightsForAuthorMesh( 2, 2, &pBones[1], &pWeights[1] );
                res = res | pBWM->SetBoneWeightsForAuthorMesh( 3, 2, &pBones[1], &pWeights[1] );
                res = res | pBWM->SetBoneWeightsForAuthorMesh( 4, 2, &pBones[2], &pWeights[2] );

                IFXASSERT(IFX_OK == res);
            }

			delete [] pBones;
			delete [] pWeights;
			IFXDELETE_ARRAY( pBoneInfo );
      }

	    /**********************************************************************************************************
		create animation modifier
		*/
		
		{
			IFXDECLARELOCAL(IFXModifierChain,pModifierChain);
			IFXString sName("LineSet");

			IFXDECLARELOCAL(IFXUnknown,pObject);
			TEST(IFXSUCCESS(IFXCreateComponent( CID_IFXAnimationModifier, IID_IFXUnknown, (void**)&pObject )), IFXException);
			IFXDECLARELOCAL(IFXMarker,pMarker );
			TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXMarker, (void**)&pMarker )), IFXException);
			IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
			TEST(IFXSUCCESS(pSGUtils->m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph )), IFXException);
			TEST(IFXSUCCESS(pMarker->SetSceneGraph( pSceneGraph )), IFXException);
			IFXDECLARELOCAL( IFXModifier, pMod );
			TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXModifier, (void**)&pMod)), IFXException);
			IFXDECLARELOCAL( IFXAnimationModifier, pAnimMod );
			TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXAnimationModifier, (void**)&pAnimMod)), IFXException);
			pAnimMod->SetAsBones();

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

			IFXDECLARELOCAL( IFXAnimationModifier, pAM );
			TEST(IFXSUCCESS(pMod->QueryInterface( IID_IFXAnimationModifier, (void**)&pAM)), IFXException);

			pAM->Playing()  = TRUE;
			pAM->RootLock() = FALSE;
			pAM->TimeScale() = 22.0f;

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
		}
		
    }
  }

   return IFX_OK;
}
