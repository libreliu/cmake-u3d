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

#define IFX_INIT_GUID

#include "u3dplayer.h"

#include "IFXCOM.h"
#include "IFXException.h"
#include "IFXCoreCIDs.h"
#include "IFXDids.h"
#include "IFXImportingCIDs.h"
#include "IFXModifierChain.h"
#include "IFXModifierDataPacket.h"
#include "IFXRenderingCIDs.h"

#define CHECK_RESULT(result) \
		{ \
		IFXTRACE_GENERIC(L"%s:%i %i=%s\n", __FILE__, __LINE__, result, DescribeIFXError(result)); \
		}
		/*if (IFXFAILURE(result)) */

#define U3D_RENDERER_OPENGL 0

char* DescribeIFXError(IFXRESULT result);

U3DSamplePlayer::U3DSamplePlayer() :
IFXDEFINEMEMBER(m_pCoreServicesMain),
IFXDEFINEMEMBER(m_pCoreServices),
IFXDEFINEMEMBER(m_pRenderServices),
IFXDEFINEMEMBER(m_pDevice),
IFXDEFINEMEMBER(m_pSceneGraph),
IFXDEFINEMEMBER(m_pScheduler),
IFXDEFINEMEMBER(m_pNodePalette)
{
	IFXDEBUG_STARTUP();

    m_LoadManagerCounter = 0;
    m_bAtomicLoading = TRUE;
    m_bInit = FALSE;
    m_bEndOfU3DFile = FALSE;
    m_bCancelLoading = FALSE;

	*m_pLoadManagers = 0;

	m_nodeFound = FALSE;
}

U3DSamplePlayer::~U3DSamplePlayer()
{
//	U32 i;
//	for(i=0;i< NUM_OF_LM;i++) {
		IFXRELEASE(m_pLoadManagers[0]);
//	}
    IFXDEBUG_SHUTDOWN();
}

IFXRESULT U3DSamplePlayer::InitRender(IFXRenderWindow *winInfo)
{
	IFXRESULT result = IFX_OK;

	result = IFXCreateComponent(CID_IFXRenderServices,
								IID_IFXRenderServices,
								(void**)&m_pRenderServices);

	if (IFXSUCCESS(result)) 
		result = m_pRenderServices->RegisterRenderer(U3D_RENDERER_OPENGL,
													 CID_IFXRenderOGL,
													 CID_IFXRenderDeviceOGL);

	if (IFXSUCCESS(result)) 
		result = IFXCreateComponent(CID_IFXDevice, IID_IFXDevice, (void**)&m_pDevice);
	if (IFXSUCCESS(result))
		result = m_pDevice->Initialize();
	if (IFXSUCCESS(result))
		result = m_pDevice->SetRenderer(U3D_RENDERER_OPENGL);
	if (IFXSUCCESS(result))
		result = m_pDevice->SetWindow(*winInfo);

	return result;
}

IFXRESULT U3DSamplePlayer::FindNextModel(U32* currentID, IFXModel** pModel)
{
	IFXRESULT result = IFX_OK;
	U32 lastNodeID = 0;
	U32 nodeID;
	BOOL notFound = FALSE;

	if (!m_pNodePalette)
		result = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(result))
		result = m_pNodePalette->Last(&lastNodeID);
		
	do {
		if (IFXSUCCESS(result))
		{
			if (!m_nodeFound)
			{
				result = m_pNodePalette->First(&nodeID);
				m_nodeFound = TRUE;
			}
			else
				result = m_pNodePalette->Next(&nodeID);
		}
		if (IFXSUCCESS(result))
		{
			*currentID = nodeID;
			if (nodeID == lastNodeID)
				m_nodeFound = FALSE;
		}

		if (IFXSUCCESS(result))
		{
			result = m_pNodePalette->GetResourcePtr( nodeID, IID_IFXModel, (void**)pModel );
			if (result == IFX_E_UNSUPPORTED && nodeID < lastNodeID)
			{
				notFound = TRUE;
				result = IFX_OK;
			}
			else
				notFound = FALSE;
		}
	} while (notFound);

	return result;
}

IFXRESULT U3DSamplePlayer::FindView(IFXView** pView)
{
	IFXRESULT result = IFX_OK;
	U32 lastNodeID = 0;
	U32 nodeID;
	BOOL notFound = FALSE, 
		nodeFound = FALSE;

	if (!m_pNodePalette)
		result = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(result))
		result = m_pNodePalette->Last(&lastNodeID);
		
	do {
		if (IFXSUCCESS(result))
		{
			if (!nodeFound)
			{
				result = m_pNodePalette->First(&nodeID);
				nodeFound = TRUE;
			}
			else
				result = m_pNodePalette->Next(&nodeID);
		}
		if (IFXSUCCESS(result))
		{
			if (nodeID == lastNodeID)
				nodeFound = FALSE;
		}

		if (IFXSUCCESS(result))
		{
			result = m_pNodePalette->GetResourcePtr( nodeID, IID_IFXView, (void**)pView );
			if (result == IFX_E_UNSUPPORTED && nodeID < lastNodeID)
			{
				notFound = TRUE;
				result = IFX_OK;
			}
			else
				notFound = FALSE;
		}
	} while (notFound);

	return result;
}

IFXRESULT U3DSamplePlayer::SetView(IFXView* pView)
{
	IFXRESULT result = IFX_OK;

	if (NULL == pView)
		result = IFX_E_NOT_INITIALIZED;
	if (IFXSUCCESS(result)) 
		result = m_pDevice->AppendView(pView, 0);
	if (IFXSUCCESS(result)) 
	{
		IFXF32Rect viewRect;
		viewRect.Set(0, 0, 1, 1);
		result = pView->SetViewport(viewRect);
		pView->SetAttributes(pView->GetAttributes()|1);
	}

	return result;
}

IFXRESULT U3DSamplePlayer::ClearBackbuffer(IFXView* pView)
{
	IFXRESULT result = IFX_OK;
	IFXF32Rect viewRect;
	U32 viewAttr = 0;
	F32 nearClip, farClip;

	if (NULL == pView)
		result = IFX_E_NOT_INITIALIZED;
	if (IFXSUCCESS(result)) 
		result = pView->GetViewport(viewRect);
	if (IFXSUCCESS(result)) 
	{
		viewAttr = pView->GetAttributes();
		nearClip = pView->GetNearClip();
		farClip = pView->GetFarClip();
		result = pView->SetViewport(IFXF32Rect(0.0f, 0.0f, 1.0f, 1.0f));
	}
	if (IFXSUCCESS(result)) 
		pView->SetAttributes(0x00000001 | viewAttr);
	if (IFXSUCCESS(result)) 
		result = pView->SetNearClip(2.0f);
	if (IFXSUCCESS(result)) 
		result = pView->SetFarClip(1.0f);

	if (IFXSUCCESS(result)) 
		result = m_pDevice->Render();

	if (IFXSUCCESS(result)) 
		result = pView->SetViewport(viewRect);
	if (IFXSUCCESS(result)) 
		pView->SetAttributes(viewAttr);
	if (IFXSUCCESS(result)) 
		result = pView->SetNearClip(nearClip);
	if (IFXSUCCESS(result)) 
		result = pView->SetFarClip(farClip);

	return result;
}

IFXRESULT U3DSamplePlayer::GetMeshGroup(IFXModel* pModel, IFXMeshGroup** pMeshGroup)
{
	IFXRESULT result = IFX_OK;
	U32 index;

	if (NULL == pModel)
		result = IFX_E_NOT_INITIALIZED;

	IFXDECLARELOCAL(IFXModifierChain, pModifierChain);
	IFXDECLARELOCAL(IFXModifierDataPacket, pDataPacket);

	if (IFXSUCCESS(result))
		result = pModel->GetModifierChain(&pModifierChain);
	if (IFXSUCCESS(result))
		result = pModifierChain->GetDataPacket(pDataPacket);
	if (IFXSUCCESS(result))
		result = pDataPacket->GetDataElementIndex(DID_IFXRenderableGroup, index);
	if (IFXSUCCESS(result))
		result = pDataPacket->GetDataElement(index, IID_IFXMeshGroup, (void**)pMeshGroup);

	return result;
}

IFXRESULT U3DSamplePlayer::Render()
{
	IFXRESULT result;

	result = m_pDevice->Render();

	return result;
}

IFXRESULT U3DSamplePlayer::DrawMesh(IFXMesh* pMesh)
{
	IFXRESULT result = IFX_OK;
/*
	if (NULL == pMesh)
		result = IFX_E_NOT_INITIALIZED;

	IFXVector3Iter vPosIter;
	IFXVector3Iter vNormIter;
	U32 vectorNum = 0;
	U32 outCount = 0; 
	U32 uNumIndices = 0;
	IFXVertexAttributes vaAttribs;
	IFXU16FaceIter f16FaceIter;
	void* pvData = 0;
	IFXDECLARELOCAL(IFXInterleavedData, pData);
	
	if (IFXSUCCESS(result))
	{
		outCount = pMesh->GetNumFaces();
		vaAttribs = pMesh->GetAttributes();
		result = pMesh->GetInterleavedVertexData(pData);
	}

	if (IFXSUCCESS(result))
	{
		if (vaAttribs.m_uData.m_bHasPositions)
		{
			result = pData->GetVectorIter(vectorNum++, vPosIter);
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, vPosIter.GetStride(), vPosIter.Get());
		}
		else
			glDisableClientState(GL_VERTEX_ARRAY);
	}

	if (IFXSUCCESS(result))
	{
		if (vaAttribs.m_uData.m_bHasNormals)
		{
			result = pData->GetVectorIter(vectorNum++, vNormIter);
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, vNormIter.GetStride(), vNormIter.Get()) ;
		}
		else
			glDisableClientState(GL_NORMAL_ARRAY);
	}

	if (IFXSUCCESS(result))
	{
		if(vaAttribs.m_uData.m_bHasDiffuseColors)
		{
			//if(m_Material.GetUseVertexColors())
			//{
			//	pData->GetVectorIter(uVectorNum, vColorIter);
			//	glEnableClientState(GL_COLOR_ARRAY);
			//	glColorPointer(4, GL_UNSIGNED_BYTE, vColorIter.GetStride(), vColorIter.Get());
			//}
			//else
			{
				glDisableClientState(GL_COLOR_ARRAY);
			}

			vectorNum++;
		}
		else
			glDisableClientState(GL_COLOR_ARRAY);
	}

	if(vaAttribs.m_uData.m_bHasSpecularColors)
		vectorNum++;

	///textures are not supported yet
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	///if MeshType == 0
	pMesh->GetU16FaceIter(f16FaceIter);
	pvData = (void*)f16FaceIter.Get();

	uNumIndices = outCount * 3;
	glDrawElements( GL_TRIANGLES, uNumIndices, GL_UNSIGNED_SHORT, pvData);

	m_Render.SwapBuffers();
*/
	return result;
}

IFXRESULT U3DSamplePlayer::ProcessScheduler()
{
	IFXRESULT result;
	result = m_pScheduler->Service();
	return result;
}

IFXRESULT U3DSamplePlayer::InitScene(IFXCHAR* fileName)
{
    IFXRESULT result = IFX_OK;

    m_pCoreServicesMain = NULL;

    result = IFXCreateComponent( CID_IFXCoreServices, IID_IFXCoreServices,
        (void**)&m_pCoreServicesMain );
    CHECK_RESULT(result);

	if ( m_pCoreServicesMain && IFXSUCCESS(result) )
	{
		result = m_pCoreServicesMain->Initialize(0);
        CHECK_RESULT(result);
	}
    if(IFXSUCCESS(result))
    {
        IFXRELEASE(m_pCoreServices);
        result = m_pCoreServicesMain->GetWeakInterface(&m_pCoreServices);
        CHECK_RESULT(result);
    }
	if ( m_pCoreServices && IFXSUCCESS(result) )
    {
        result = m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&m_pSceneGraph );
        CHECK_RESULT(result);
    }
	if (m_pSceneGraph)
	{
		if ( IFXSUCCESS(result) )
		{
			result = m_pSceneGraph->GetPalette( IFXSceneGraph::NODE, &m_pNodePalette );
            CHECK_RESULT(result);
		}
	}

    //Scheduler Get and setup
    if ( m_pCoreServices && IFXSUCCESS(result) )
    {
        result = m_pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&m_pScheduler);
        CHECK_RESULT(result);
    }

    //LoadManager initialization
    if (m_pScheduler == NULL && result == IFX_E_NOT_INITIALIZED) 
    {
        m_bAtomicLoading = TRUE;
        result = IFX_OK;
    }

    if (*m_pLoadManagers)
		m_LoadManagerCounter++;
	else
		m_LoadManagerCounter = 0;

    if ( IFXSUCCESS(result) )
    {
        result = IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, 
                                    (void**)&m_pLoadManagers[m_LoadManagerCounter]);
        CHECK_RESULT(result);
    }
	if ( IFXSUCCESS(result) )
    {
        result = m_pLoadManagers[m_LoadManagerCounter]->Initialize(m_pCoreServices);
        CHECK_RESULT(result);
    }

	IFXDECLARELOCAL(IFXReadBuffer, pReadBuffer);
	if ( IFXSUCCESS(result) )
    {
        result = IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, 
                                    (void**)&pReadBuffer);
        CHECK_RESULT(result);
    }

	IFXDECLARELOCAL(IFXStdio, pStdio);
	if ( IFXSUCCESS(result) )
    {
        result = pReadBuffer->QueryInterface(IID_IFXStdio,(void**)&pStdio);
        CHECK_RESULT(result);
    }
	if ( IFXSUCCESS(result) )
    {
		result = pStdio->Open(fileName);
        CHECK_RESULT(result);
    }

	if ( IFXSUCCESS(result) )
    {
        result = m_pLoadManagers[m_LoadManagerCounter]->Load(pReadBuffer,
                                                             m_bAtomicLoading, "n", 
                                                             IFXCOLLISIONPOLICY_PREPENDCOLLIDED);
		EIFXLoadState loadState;
		do 
		{
			result = m_pScheduler->Service();
			if (IFXSUCCESS(result))
				result = m_pLoadManagers[m_LoadManagerCounter]->GetAggregateLoadState(loadState);
		} while (IFX_LOAD_STATE_MEDIA_LOADED != loadState && IFXSUCCESS(result));

        CHECK_RESULT(result);
    }/**/

	m_bEndOfU3DFile = FALSE;
	m_bCancelLoading = FALSE;

	return result;
}

char* DescribeIFXError(IFXRESULT result)
{
    switch (result) {
        case IFX_E_UNDEFINED: return "IFX_E_UNDEFINED";
        case IFX_E_UNSUPPORTED: return "IFX_E_UNSUPPORTED";
        case IFX_E_OUT_OF_MEMORY: return("IFX_E_OUT_OF_MEMORY");
        case IFX_E_INVALID_FILE: return("IFX_E_INVALID_FILE");
        case IFX_E_INVALID_HANDLE: return("IFX_E_INVALID_HANDLE");
        case IFX_E_INVALID_POINTER: return("IFX_E_INVALID_POINTER");
        case IFX_E_INVALID_RANGE: return("IFX_E_INVALID_RANGE");
        case IFX_E_ALREADY_INITIALIZED: return("IFX_E_ALREADY_INITIALIZED");
        case IFX_E_NOT_INITIALIZED: return "IFX_E_NOT_INITIALIZED";
        case IFX_E_CANNOT_CHANGE: return("IFX_E_CANNOT_CHANGE");
        case IFX_E_ABORTED: return("IFX_E_ABORTED");
        case IFX_E_WRITE_FAILED: return("IFX_E_WRITE_FAILED");
        case IFX_E_READ_FAILED: return("IFX_E_READ_FAILED");
        case IFX_E_CANNOT_FIND: return("IFX_E_CANNOT_FIND");
        case IFX_E_SUBSYSTEM_OUT_OF_MEMORY: return("IFX_E_SUBSYSTEM_OUT_OF_MEMORY");
        case IFX_E_SUBSYSTEM_FAILURE: return("IFX_E_SUBSYSTEM_FAILURE");
        case IFX_E_PARAMETER_NOT_INITIALIZED: return("IFX_E_PARAMETER_NOT_INITIALIZED");
        case IFX_E_BAD_PARAM: return("IFX_E_BAD_PARAM");
        case IFX_E_NOT_DONE: return("IFX_E_NOT_DONE");
        case IFX_E_KEY_ALREADY_EXISTS: return("IFX_E_KEY_ALREADY_EXISTS");
        case IFX_E_RESOURCE_NOT_AVAILABLE: return("IFX_E_RESOURCE_NOT_AVAILABLE");
        case IFX_E_INVALID_VECTOR: return("IFX_E_INVALID_VECTOR");
        case IFX_E_INVALID_POINT: return("IFX_E_INVALID_POINT");
        case IFX_E_INVALID_INDEX: return("IFX_E_INVALID_INDEX");
        case IFX_E_COMPONENT: return("IFX_E_COMPONENT: Component identifier specified is not supported.");

        case IFX_W_CANNOT_UNLOAD: return("IFX_W_CANNOT_UNLOAD");
        case IFX_W_FINISHED: return("IFX_W_FINISHED");
        case IFX_W_ALREADY_EXISTS: return("IFX_W_ALREADY_EXISTS");
        case IFX_W_UNEXPECTED_FILE_VERSION: return("IFX_W_UNEXPECTED_FILE_VERSION");

        case IFX_CANCEL: return("IFX_CANCEL");
        case IFX_TRUE: return("IFX_TRUE");
        case IFX_OK: return("IFX_OK");

        default: return "UNKNOWN result code";
    }
}
