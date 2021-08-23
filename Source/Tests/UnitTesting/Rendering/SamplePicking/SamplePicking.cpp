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

#include "SamplePicking.h"

#include "UTHeaders.h"

#include "IFXCheckX.h"
#include "IFXRenderingCIDs.h"
#include "U3DViewer.h"

IFXRESULT SamplePicking::StartUp()
{
	IFXRESULT result = IFX_OK;

	m_pSGUtils = new U3DSceneUtilities;

	if( NULL == m_pSGUtils )
		result = IFX_E_OUT_OF_MEMORY;

	return result;
}

IFXRESULT SamplePicking::ShutDown()
{
	IFXRESULT result = IFX_OK;

	IFXDELETE( m_pSGUtils );

	TEST( IFXSUCCESS( result = IFXCOMUninitialize() ), IFXException );

	return result;
}

SamplePicking::SamplePicking()
{	
	TEST( IFXSUCCESS( IFXCOMInitialize() ), IFXException );

	//1
	m_position[0][0]  = 0.0f;		m_position[0][1] = -300.5f;		m_position[0][2]  = 64.7f;
	m_direction[0][0] = -24.5f;		m_direction[0][1] = 1588.0f;	m_direction[0][2]  = -410.0f;
	//2
	m_position[1][0]  = 0.0f;		m_position[1][1] = -300.5f;		m_position[1][2]  = 64.7f;
	m_direction[1][0] = 9.5f;		m_direction[1][1] = 1586.0f;	m_direction[1][2]  = -421.f;
	//3
	m_position[2][0]  = 0.0f;		m_position[2][1] = -300.0f;		m_position[2][2]  = 64.7f;
	m_direction[2][0] = -58.5f;		m_direction[2][1] = 1587.f;		m_direction[2][2]  = -417.0f;
	//4
	m_position[3][0]  = 0.0f;		m_position[3][1] = -300.5f;		m_position[3][2]  = 64.7f;
	m_direction[3][0] = -22.5f;		m_direction[3][1] = 1583.7f;	m_direction[3][2]  = -430.0f;
	//5
	m_position[4][0]  = 0.0f;		m_position[4][1] = -300.5f;		m_position[4][2]  = 64.7f;
	m_direction[4][0] = -21.5f;		m_direction[4][1] = 1585.f;		m_direction[4][2]  = -422.0f;
	//6
	m_position[5][0]  = 0.0f;		m_position[5][1] = -300.5f;		m_position[5][2]  = 64.7f;
	m_direction[5][0] = -24.5f;     m_direction[5][1] = 1594.0f;	m_direction[5][2] = -391.0f;
	//7
	m_position[6][0]  = 0.0f;		m_position[6][1] = -300.0f;		m_position[6][2]  = 64.7f;
	m_direction[6][0] = -24.5f;		m_direction[6][1] = 1560.0f;	m_direction[6][2] = -577.1f;
}

SamplePicking::~SamplePicking()
{	
}

IFXRESULT SamplePicking::Run()
{
	TEST(IFXSUCCESS(m_pSGUtils->InitializeScene()), IFXException);

	IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
	IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManagerCheck);
	IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);
	IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBufferCheck);
	IFXDECLARELOCAL(IFXWriteManager, pIFXWriteManager);
	IFXDECLARELOCAL(IFXWriteBuffer, pIFXWriteBuffer);
	IFXDECLARELOCAL(IFXStdio, pIFXStdio1);
	IFXDECLARELOCAL(IFXScheduler, pScheduler);
	IFXDECLARELOCAL(IFXMetaDataX, pMetaDataX);
	

	TEST(IFXSUCCESS(m_pSGUtils->m_pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler)), IFXException);
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager)), IFXException);
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManagerCheck)), IFXException);
	TEST(IFXSUCCESS(pIFXLoadManager->Initialize(m_pSGUtils->m_pCoreServices)), IFXException);
	TEST(IFXSUCCESS(pIFXLoadManagerCheck->Initialize(m_pSGUtils->m_pCoreServices)), IFXException);
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer)), IFXException);
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBufferCheck)), IFXException);
	

	TEST(IFXSUCCESS(pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio1)), IFXException);

	IFXString src;
	src = getenv("U3D_SOURCE");
	src += "Tests/UnitTesting/Rendering/SamplePicking/boxfor_picking.u3d";
	m_pSGUtils->m_pCoreServices->SetBaseURL(src);

	TEST(IFXSUCCESS(pIFXStdio1->Open( (IFXCHAR*) src.Raw() )), IFXException);

	TEST(IFXSUCCESS(pIFXLoadManager->Load(pIFXReadBuffer, TRUE)), IFXException);

	EIFXLoadState eState = IFX_LOAD_STATE_UNLOADED;
	while (eState < IFX_LOAD_STATE_MEDIA_LOADED) {
		TEST(IFXSUCCESS(pScheduler->Service()), IFXException);
		TEST(IFXSUCCESS(pIFXLoadManager->GetAggregateLoadState(eState)), IFXException);
	};

	TEST(IFXSUCCESS(pIFXStdio1->Close()), IFXException);

	return IFX_OK;
}

IFXRESULT SamplePicking::Pick2DSample()
{
	IFXRESULT	rc = IFX_FALSE;
	IFXDECLARELOCAL(IFXNode,pNode);
	IFXDECLARELOCAL(IFXSimpleList,pPickObjectList);
	IFXModel* pPickedModel;

	U32	NodeID  = 0;

	if (!m_pSGUtils)
		return IFX_E_INVALID_POINTER;

	IFXPalette* pNodePalette = m_pSGUtils->m_pNodePalette;
    if (!pNodePalette) {
		rc = IFX_E_NOT_INITIALIZED;	
	}
	if (pNodePalette && IFXSUCCESS(rc) ) {
		rc = pNodePalette->GetResourcePtr( NodeID, IID_IFXNode, (void**) &pNode );
	}

	if ( IFXSUCCESS(rc) ) {

		U32 ViewInstance = 0;
		IFXString ModelName;
		IFXVector3 position, direction;
		for( U32 i =0; i < NUMELEMENTS; i++ ) {

			position  = m_position[i];
			direction = m_direction[i];
			
			if (pNode && IFXSUCCESS(rc) ) {
				rc = pNode->Pick(ViewInstance, position, direction, &pPickObjectList);
			}

			if ( IFXSUCCESS(rc) && pPickObjectList )
			{
				IFXPickObject* pPickObject = NULL;
				IFXUnknown* pUnk = NULL;
				rc = pPickObjectList->Get( 0, &pUnk );
				pUnk->QueryInterface( IID_IFXPickObject, (void**)&pPickObject );
				if ( IFXSUCCESS(rc) ) {
					pPickedModel = pPickObject->GetModel();
				}
				IFXRELEASE(pPickObject);
				IFXRELEASE(pUnk);
			}

			U32 uRID;
			if (NULL != pPickedModel) {
				rc = pNodePalette->FindByResourcePtr(pPickedModel, &uRID);
				if (IFXSUCCESS(rc))
					rc = pNodePalette->GetName(uRID, &ModelName);		
			}

			rc = Check2DModelNames(i,ModelName);
			if( rc == IFX_TRUE)
				return rc;			
		}
	}

	return rc;
}


IFXRESULT SamplePicking::Pick3DSample()
{
	IFXRESULT	rc = IFX_FALSE;
	IFXDECLARELOCAL(IFXNode,pNode);
	IFXDECLARELOCAL(IFXSimpleList,pPickList);
	IFXModel* pPickedModel;

	U32	NodeID  = 0;

	if (!m_pSGUtils)
		return IFX_E_INVALID_POINTER;

	IFXPalette* pNodePalette = m_pSGUtils->m_pNodePalette;
    if (!pNodePalette) {
		rc = IFX_E_NOT_INITIALIZED;	
	}
	if (pNodePalette && IFXSUCCESS(rc) ) {
		rc = pNodePalette->GetResourcePtr( NodeID, IID_IFXNode, (void**) &pNode );
	}

	IFXUnknown *pUnknown = NULL;
	IFXPickObject* pPickObject = NULL;
	IFXUnknown* pUnk = NULL;

	if ( IFXSUCCESS(rc) ) {

		U32 PickCount, Instance, uRID;
		U32 ViewInstance = 0;
		IFXString ModelName;
		IFXVector3 position, direction;
		for( U32 numElement = 0; numElement < NUMELEMENTS; numElement++ ) {

			position  = m_position[numElement];
			direction = m_direction[numElement];			
			if (pNode && IFXSUCCESS(rc) ) {
				rc = pNode->Pick(ViewInstance, position, direction, &pPickList);
			}
			PickCount = 0;
			if (pPickList && IFXSUCCESS(rc)) {
				rc = pPickList->GetCount(&PickCount);
			}

			if ( IFXSUCCESS(rc)) {

				for (U32 i = 0; i < PickCount; i++) {
					if (IFXSUCCESS(rc))
					{
						rc = pPickList->Get(i, &pUnknown);
					}

					if (pUnknown && IFXSUCCESS(rc) )
					{
						rc = pUnknown->QueryInterface(IID_IFXPickObject, (void **)&pPickObject);
					}

					IFXRELEASE(pUnknown);

					if (pPickObject && IFXSUCCESS(rc)) {
						pPickedModel = pPickObject->GetModel();  
						Instance = pPickObject->GetInstance();

						if (!pPickedModel)
							rc = IFX_E_INVALID_POINTER;
					}

					if (pNodePalette && IFXSUCCESS(rc))
						rc = pNodePalette->FindByResourcePtr(pPickedModel, &uRID);

					if (pNodePalette && IFXSUCCESS(rc))
						rc = pNodePalette->GetName(uRID, &ModelName);

					rc = Check3DModelNames(numElement,ModelName);
					if( rc == IFX_TRUE )
						return rc;

				}			
			}
		}
	}

	IFXRELEASE(pPickObject);
	IFXRELEASE(pUnk);
	return rc;
}

IFXRESULT SamplePicking::Check2DModelNames( const int numElements, const IFXString ModelName )
{
	IFXRESULT rc = IFX_TRUE;

		switch(numElements) {
			case 0:
				if (ModelName == "Box01") {
					rc = IFX_OK;
				}
				break;
			case 1:
				if(ModelName == "Box02") {
					rc = IFX_OK;
				}
				break;
			case 2:
				if(ModelName == "Box04") {
					rc = IFX_OK;
				}
				break;
			case 3:
				if(ModelName == "Box05") {
					rc = IFX_OK;
				}
				break;
			case 4:
				if(ModelName == "Box05") {
					rc = IFX_OK;
				}
				break;
			case 5:
				if(ModelName == "Box06") {
					rc = IFX_OK;
				}
				break;
			case 6:
				if(ModelName == "Box07") {
					rc = IFX_OK;
				}
				break;			
			default:
				rc = IFX_TRUE;
			break;
		}	
		if( rc == IFX_TRUE )	
			return rc;

	return rc;
}


IFXRESULT SamplePicking::Check3DModelNames( const int numElements, const IFXString ModelName )
{
	IFXRESULT rc = IFX_TRUE;

		switch(numElements) {
		case 0:
			if ( ModelName == "Box01" || ModelName == "Box03") {
				rc = IFX_OK;
			}
			break;
		case 1:
			if(ModelName == "Box02") {
				rc = IFX_OK;
			}
			break;
		case 2:
			if(ModelName == "Box04") {
				rc = IFX_OK;
			}
			break;
		case 3:
			if(ModelName == "Box05" || ModelName == "Box01" ) {
				rc = IFX_OK;
			}
			break;
		case 4:
			if(ModelName == "Box05" || ModelName == "Box01" || ModelName == "Box03") {
				rc = IFX_OK;
			}
			break;
		case 5:
			if(ModelName == "Box06") {
				rc = IFX_OK;
			}
			break;
		case 6:
			if(ModelName == "Box07") {
				rc = IFX_OK;
			}
			break;			
		default:
			rc = IFX_TRUE;
		break;
	}	
	return rc;
}
