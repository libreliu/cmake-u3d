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

#ifndef __U3DViewer_H__
#define __U3DViewer_H__

#include "U3DSceneUtilities.h"
#include "IFXAutoRelease.h"
#include "IFXRenderWindow.h"

/**
	This enumeration defines the supported predefined renderers.

	@note All predefined renderers may not be available on all
	systems.  If available they may not be stable on some
	systems depending upon their video card and drivers.
*/
enum U3DRenderer
{
	U3D_RENDERER_OPENGL,  ///< Identifies the OpenGL renderer.
	U3D_RENDERER_NULL,    ///< This enumerator is an invalid one that can be used to signify one isn't defined.

	U3D_NUM_RENDERERS,    ///< Total number of valid enumerators.  This enumerator is an invalid one.

	U3D_DEFAULT_RENDERER  = U3D_RENDERER_OPENGL,  ///< Identifies the default renderer.
	U3D_INVALID_RENDERER  = U3D_NUM_RENDERERS
};

enum U3DCuller
{
	U3D_SCENEGRAPH_CULLER,
	U3D_OCTREE_CULLER,
	U3D_NUM_CULLERS,
	U3D_DEFAULT_CULLER = U3D_SCENEGRAPH_CULLER
};

//typedef IFXRESULT (* RENDERCALLBACK) (U3DSceneUtilities *pSGUtils);

typedef struct
{
	IFXCHAR m_Filename[512];
	U3DRenderer m_Renderer;
	BOOL m_bLoadingAtomic;
	U32 m_TimeOut;
	U3DCuller m_Culler;
} UserOptions;


/**
	This class encapsulates methods and data that are used to
	render the scene using rendering functionality.
*/
class U3DViewer
{
public:
	U3DViewer();
	~U3DViewer();
	IFXRESULT SetupWindows();
	IFXRESULT EventLoop();
	IFXRESULT Initialize(const BOOL RemoteLoading = FALSE);
	IFXRESULT InitializeSG( U3DSceneUtilities* pSGUtils, const BOOL RemoteLoading = FALSE );
	IFXRESULT ParseArguments(const IFXCHAR *cmdline);

private:
	IFXRESULT InitializeRenderer();
	IFXRESULT SetViewport();
	IFXRESULT SetRenderer(U3DRenderer Renderer);
	IFXRESULT Render();
	IFXRESULT FindView();
	IFXRESULT ClearBackBuffer();
	IFXRESULT SetWindowSize();

	CArrayList<IFXView*> m_views;
	U32		m_uViewCounter;
	U32     m_uActiveViewID;
	U32     m_uActiveViewInstance;
	BOOL	m_startToFindCameras;

	IFXDevice *m_pDevice; ///< The rendering device
	IFXRenderServices *m_pRenderServices; ///< The render services that maintain the renderer information

	// Misc pointers
	U3DSceneUtilities*  m_pSGUtils; ///< The utility class, for when we need to access the IFX resources in a callback

	U3DRenderer m_uRendererID;
	U3DCuller   m_uCullerID;
	BOOL        m_bRendererInitialized;
	BOOL        m_bPreserveViewSettings;
	BOOL        m_bLoadingAtomic;

	U32         m_TimeOut;
	UserOptions m_UserOptions;

	IFXCHAR m_strU3DFilename[512];

	IFXRenderWindow m_wInfo;
};


#endif
