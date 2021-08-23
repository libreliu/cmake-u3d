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
#ifndef __LINE_SET_LOADING_H__
#include "LineSetLoading.h"
#endif

#include "UTHeaders.h"

#include "IFXLoadManager.h"
#include "IFXWriteManager.h"
#include "IFXStdio.h"
#include "IFXReadBuffer.h"
#include "IFXWriteBuffer.h"
#include "IFXPickObject.h"

#include "LineSetInitialize.h"

float distance(IFXVector3 &pnt1, IFXVector3& pnt2 );


IFXAuthorLineSet* pALS			= NULL;
IFXAuthorLineSetResource* pLSG	= NULL;
IFXModifier*				pGen = NULL;


IFXRESULT LineSetLoading::Write()
{
	IFXRESULT result = IFXCOMInitialize();

	if(IFXSUCCESS(result))
	{
		IFXString src(getenv("U3D_SOURCE"));
		BOOL IsFinished = FALSE;

		src	+=	"Tests/UnitTesting/Loading/LineSetLoading/Grid16LineSet.u3d";

		U3DSceneUtilities SGUtils;
		TEST(IFXSUCCESS(SGUtils.InitializeScene()), IFXException);

		IFXDECLARELOCAL(IFXWriteManager, pIFXWriteManager);
		IFXDECLARELOCAL(IFXWriteBuffer, pIFXWriteBuffer);
		IFXDECLARELOCAL(IFXStdio, pIFXStdio2);

		// fill SceneGraph
		CreateLineSetObject(&SGUtils);

		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXWriteManager, IID_IFXWriteManager, (void**)&pIFXWriteManager)), IFXException);
		TEST(IFXSUCCESS(pIFXWriteManager->Initialize(SGUtils.m_pCoreServices)), IFXException);
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioWriteBuffer, IID_IFXWriteBuffer, (void**)&pIFXWriteBuffer)), IFXException);
		TEST(IFXSUCCESS(pIFXWriteBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio2)), IFXException);

		TEST(IFXSUCCESS(pIFXStdio2->Open( (IFXCHAR*) src.Raw() )), IFXException);
		TEST(IFXSUCCESS(SGUtils.m_pSceneGraph->Mark()), IFXException);
		TEST(IFXSUCCESS(pIFXWriteManager->Write(pIFXWriteBuffer, IFXEXPORT_EVERYTHING)), IFXException); // ^ IFXEXPORT_FILEREFERENCES
		TEST(IFXSUCCESS(pIFXStdio2->Close()), IFXException);

		IFXRELEASE(pGen);
		IFXRELEASE(pALS);
		IFXRELEASE(pLSG);
	}

	if(IFXSUCCESS(result))
		result = IFXCOMUninitialize();

	return result;
}


IFXRESULT LineSetLoading::CreateLineSetObject( U3DSceneUtilities* pSGUtils )
{
	U32 uID = 0;
	SGNodeHeader nh;
	IFXView* pViewNode = NULL;
	IFXRenderClear renderClear;
	IFXLight *pLight = NULL;
	IFXModifier     *pGenerator = NULL;
	U32           uResourceID = 0;
	U32           uCloneID  = 0;
	IFXPalette        *pNodePalette = NULL;
	struct SGNodeHeader   nodeHeader;
	IFXPalette        *pShaderPalette = NULL;
	U32           uShaderID = 0;
	IFXVector4 colorWhite(1.0f, 1.0f, 1.0f);

	wcscpy(nh.Name, L"Camera1");
	wcscpy(nh.ParentName, PALETTE_DEFAULT_GROUP);
	nh.NodeTm.Reset();

/*
	TEST(IFXSUCCESS(pSGUtils->CreateCameraNode(&nh, &pViewNode)), IFXException);
	TEST(IFXSUCCESS(pViewNode->SetProjectionMode(IFX_PERSPECTIVE)), IFXException);
	pViewNode->SetAttributes( 0 );

	TEST(IFXSUCCESS(pViewNode->SetProjection(5.0)), IFXException);
	renderClear = pViewNode->GetViewResource()->GetRenderClear();
*/
	IFXVector3 vBGColor( 2, 3, 4 );
	TEST(IFXSUCCESS(renderClear.SetColorValue(vBGColor)), IFXException);
	TEST(IFXSUCCESS(renderClear.SetDepthValue(1.0f)), IFXException);
	TEST(IFXSUCCESS(renderClear.SetColorCleared(TRUE)), IFXException);
	IFXRELEASE(pViewNode);

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
	IFXRELEASE(pLightResource);
	IFXRELEASE(pSceneGraph);

	//pLightResourcePal = pLightResourcePalette;

	// Link the light node to the light resource pallete entry
	TEST(IFXSUCCESS( pLight->SetLightResourceID(uLightResourceId) ), IFXException);
	// Ambient light
	pLight->GetLightResource()->SetType(IFXLightResource::DIRECTIONAL);
	pLight->GetLightResource()->SetColor(colorWhite);
	IFXRELEASE(pLight);

	pNodePalette = pSGUtils->m_pNodePalette;
	TEST( pNodePalette != NULL, IFXException );

	//
	// --- Create a LineSet primitive to indicate the picking location
	//
	TEST(IFXSUCCESS(CreatePrimitive("PickLineSet", CID_IFXAuthorLineSetResource, 0,
		&uResourceID, &pGenerator, pSGUtils->m_pGeneratorPalette, pSGUtils->m_pSceneGraph) ), IFXException);

	// Create a model node in the node palette for the sphere
	wcscpy(nodeHeader.Name, L"PickLineSet");
	wcscpy(nodeHeader.ParentName, PALETTE_DEFAULT_GROUP);
	nodeHeader.NodeTm.Reset();
	nodeHeader.NodeTm.Translate3x4(IFXVector3(0, 0, -400));

	// Create Author LineSet
	IFXAuthorLineSet* pAuthorLS = NULL;
	TEST(	IFXSUCCESS(IFXCreateComponent(CID_IFXAuthorLineSet, IID_IFXAuthorLineSet,(void**)&pAuthorLS)),IFXException);
	IFXASSERT(pAuthorLS);

	IFXAuthorLineSetResource  *pLineSetGenerator = NULL;
	// Initialize Author Line Set by some sample data
	// 1 - st version
	//TEST(IFXSUCCESS(InitAuthorLineSet(pAuthorLS)),IFXException);

	pALS = pAuthorLS;
	//  a more complex ( ith 9 materials ) vesrions
	//TEST(IFXSUCCESS(InitGridAuthorLineSet(pAuthorLS)),IFXException);
	TEST(IFXSUCCESS(InitGrid256AuthorLineSet(pAuthorLS)),IFXException);

	// the smallest version only for debug
	//TEST(IFXSUCCESS(Init_Simplest_AuthorLineSet(pAuthorLS)),IFXException);

	TEST(IFXSUCCESS(pGenerator->QueryInterface( IID_IFXAuthorLineSetResource, (void**)&pLineSetGenerator )), IFXException);

	TEST(IFXSUCCESS(pLineSetGenerator->SetAuthorLineSet(pAuthorLS)), IFXException);

	pLSG = pLineSetGenerator;
	pGen = pGenerator;

	TEST(IFXSUCCESS(pSGUtils->CreateModelNode(&nodeHeader, uResourceID)), IFXException);

	// Access the shader and turn on line rendering for the default
	// shader so we can see the bounding sphere.

	// Get the shader palette
	pShaderPalette = pSGUtils->m_pShaderPalette;
	TEST(pShaderPalette != NULL, IFXException);

	return IFX_OK;
} // CreateLineSetObject


IFXRESULT LineSetLoading::Load()
{
	IFXString src;
	BOOL IsFinished = FALSE;

	src = getenv("U3D_SOURCE");
	src	+=	"Tests/UnitTesting/Primitives/LineSetLoading/SimpleLineSet.u3d";

	U3DSceneUtilities SGUtils;
	TEST(IFXSUCCESS(SGUtils.InitializeScene()), IFXException);

	IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
	IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);
	IFXDECLARELOCAL(IFXStdio, pIFXStdio_2);

	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager)), IFXException);
	TEST(IFXSUCCESS(pIFXLoadManager->Initialize(SGUtils.m_pCoreServices)), IFXException);
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer)), IFXException);
	TEST(IFXSUCCESS(pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio_2)), IFXException);

	TEST(IFXSUCCESS(pIFXStdio_2->Open( (IFXCHAR*) src.Raw() )), IFXException);
	TEST(IFXSUCCESS(pIFXLoadManager->Load(pIFXReadBuffer, TRUE)), IFXException);
	TEST(IFXSUCCESS(pIFXStdio_2->Close()), IFXException);

	return IFX_OK;
} // Load()


float distance(IFXVector3 &pnt1, IFXVector3 &pnt2 ) {

	float dist =0.0f;

	dist = sqrt(  ( pnt1.X() - pnt2.X() )*( pnt1.X() - pnt2.X() ) +
		( pnt1.Y() - pnt2.Y() )*( pnt1.Y() - pnt2.Y() ) +
		( pnt1.Z() - pnt2.Z() )*( pnt1.Z() - pnt2.Z() )
		);

	return dist;
};



