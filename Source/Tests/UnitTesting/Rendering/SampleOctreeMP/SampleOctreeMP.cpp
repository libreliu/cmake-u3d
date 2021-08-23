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

#ifndef SAMPLEOCTREE_H
#include "SampleOctreeMP.h"
#endif

#include "IFXRenderingCIDs.h"

IFXRESULT SampleOctree::Run( U3DSceneUtilities* pSGUtils )
{
	TEST(IFXSUCCESS(pSGUtils->InitializeScene()), IFXException);

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
	src += L"Tests/UnitTesting/SamplePlayer/SimpleTestMP.u3d";

	IFXRESULT res = pIFXStdio->Open( (IFXCHAR*) src.Raw() );
	TEST(IFXSUCCESS(res), IFXException );
	TEST(IFXSUCCESS(pIFXLoadManager->Load(pIFXReadBuffer, IFXEXPORT_EVERYTHING)), IFXException); // ^ IFXEXPORT_FILEREFERENCES
	TEST(IFXSUCCESS(pIFXStdio->Close()), IFXException);

	IFXDECLARELOCAL(IFXSpatialSetQuery, pCullingSubsystem);
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXOctreeCuller, IID_IFXSpatialSetQuery, (void**)&pCullingSubsystem)), IFXException);
	U32 nItems = 0;
	pSGUtils->m_pNodePalette->GetPaletteSize(&nItems);
	for (U32 i = 0; i < nItems; i++)
	{
		IFXDECLARELOCAL(IFXView, pView);
		if (IFXSUCCESS(pSGUtils->m_pNodePalette->GetResourcePtr(i, IID_IFXView, (void**)&pView)))
			TEST(IFXSUCCESS(pView->SetCullingSubsystem(*pCullingSubsystem)), IFXException);
	}

	return IFX_OK;
}
