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

#ifndef APPLYXFORM2NODE_H
#include "GetSubgraphBound.h"
#endif

#include "IFXRenderingCIDs.h"

#define	ToRadians(x)	((x)*3.1415926535f/180.0f)

IFXRESULT GetSubgraphBound::Run( U3DSceneUtilities* pSGUtils )
{
	TEST(IFXSUCCESS(pSGUtils->InitializeScene()), IFXException);

	IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
	IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);
	IFXDECLARELOCAL(IFXStdio, pIFXStdio);

	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager)), IFXException);
	TEST(IFXSUCCESS(pIFXLoadManager->Initialize(pSGUtils->m_pCoreServices)), IFXException);
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer)), IFXException);
	TEST(IFXSUCCESS(pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio)), IFXException);
	
	IFXString src = getenv("U3D_SOURCE");
	src += L"Tests/UnitTesting/MultiParent/GetSubgraphBound/GetSubgraphBound.u3d";
	
	IFXRESULT res = pIFXStdio->Open( (IFXCHAR*)src.Raw() );
	TEST(IFXSUCCESS( res ), IFXException );

	TEST(IFXSUCCESS(pIFXLoadManager->Load(pIFXReadBuffer, IFXEXPORT_EVERYTHING)), IFXException);
	TEST(IFXSUCCESS(pIFXStdio->Close()), IFXException);
	
	{
		U32 i, NumInstances = 0;
		IFXVector4 BoundSphere, PrevBoundSphere;
		IFXDECLARELOCAL(IFXNode, pNode);
		TEST(IFXSUCCESS(pSGUtils->FindNode(L"Box00", &pNode)), IFXException);
		TEST(IFXSUCCESS(pNode->GetNumberOfInstances(&NumInstances)), IFXException);
		for (i = 0; i < NumInstances; i++)
		{
			TEST(IFXSUCCESS(pNode->GetSubgraphBound(BoundSphere, i)), IFXException);
			if (i > 0)
			{
				TEST((int)(BoundSphere.X()*1000) != (int)(PrevBoundSphere.X()*1000), IFXException);
				TEST((int)(BoundSphere.Y()*1000) == (int)(PrevBoundSphere.Y()*1000), IFXException);
				TEST((int)(BoundSphere.Z()*1000) == (int)(PrevBoundSphere.Z()*1000), IFXException);
				TEST((int)(BoundSphere.Radius()*1000) == (int)(PrevBoundSphere.Radius()*1000), IFXException);
			}
			PrevBoundSphere = BoundSphere;
		}
	}

	return IFX_OK;
}
