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

#ifndef __U3DPLAYER__
#define __U3DPLAYER__

#include "IFXCOM.h"
#include "IFXCoreServices.h"
#include "IFXSceneGraph.h"
#include "IFXScheduler.h"
#include "IFXLoadManager.h"
#include "IFXStdio.h"
#include "IFXRenderServices.h"
#include "IFXDevice.h"
#include "IFXAutoRelease.h"

#include "IFXRenderWindow.h"

#include <OpenGL/gl.h>

#define NUM_OF_LM 10

class U3DSamplePlayer
{
public:
	U3DSamplePlayer();
    ~U3DSamplePlayer();

	IFXRESULT InitRender(IFXRenderWindow *winInfo);
	IFXRESULT InitScene(IFXCHAR* fileName);
	IFXRESULT FindNextModel(U32* currentID, IFXModel** pModel);
	IFXRESULT FindView(IFXView** pView);
	IFXRESULT SetView(IFXView* pView);
	IFXRESULT ClearBackbuffer(IFXView* pView);
	IFXRESULT GetMeshGroup(IFXModel* pModel, IFXMeshGroup** pMeshGroup);
	IFXRESULT DrawMesh(IFXMesh* pMesh);
	IFXRESULT Render();
	IFXRESULT ProcessScheduler();
private:
    IFXDECLAREMEMBER(IFXCoreServices,m_pCoreServicesMain);
    IFXDECLAREMEMBER(IFXCoreServices, m_pCoreServices);
	IFXDECLAREMEMBER(IFXRenderServices, m_pRenderServices);
	IFXDECLAREMEMBER(IFXDevice, m_pDevice);
    IFXDECLAREMEMBER(IFXSceneGraph, m_pSceneGraph);
	IFXDECLAREMEMBER(IFXScheduler, m_pScheduler);
	IFXDECLAREMEMBER(IFXPalette, m_pNodePalette);

	IFXLoadManager*   m_pLoadManagers[NUM_OF_LM];
	U32         m_LoadManagerCounter;
	
	BOOL        m_bInit;
	BOOL        m_bEndOfU3DFile;
	BOOL        m_bCancelLoading;
	BOOL		m_bAtomicLoading;

	BOOL m_nodeFound;

	IFXRenderWindow m_RenderWindow;
};


#endif

