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

/**
	@file	U3DViewer.h

			The header file that defines the U3DViewer class.
*/

#ifndef U3DVIEWER_H__
#define U3DVIEWER_H__

#include "U3DSceneUtilities.h"
#include "IFXAutoRelease.h"
#include "IFXDevice.h"
#include "IFXPerformanceTimer.h"
#include "IFXTaskCallback.h"
#include "IFXRenderWindow.h"


#include <windows.h>
#include <commdlg.h>

/**
This enumeration defines the supported predefined renderers.

@note All predefined renderers may not be available on all
systems.  If available they may not be stable on some
systems depending upon their video card and drivers.
*/
enum U3DRenderer
{
  U3D_RENDERER_OPENGL,  ///< Identifies the OpenGL renderer.
#if defined( WIN32 )
  U3D_RENDERER_DIRECTX7,  ///< Identifies the DirectX 7 renderer.
  U3D_RENDERER_DIRECTX8,  ///< Identifies the DirectX 8 renderer.
#endif
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

typedef IFXRESULT (* RENDERCALLBACK) (U3DSceneUtilities *pSGUtils);

typedef struct  
{
  TCHAR m_Filename[512];
  U3DRenderer m_Renderer;
  BOOL m_bLoadingAtomic;
  U32 m_TimeOut;
  U3DCuller m_Culler;
} UserOptions;

/**
	This class encapsulates methods and data that are used to
	render the exported scene using IFX rendering functionality.
*/
class U3DViewer
{
public:
  U3DViewer();
  ~U3DViewer();
  IFXRESULT SetupWindows();
  IFXRESULT EventLoop();
  IFXRESULT SGViewProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
  INT_PTR RemoteLoadingDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
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

  void SetMenuStates();
  IFXRESULT SetWindowSize();
  void CreateProgressBar();
  void DestroyProgressBar();
  IFXRESULT CreateToolBar();

  static IFXRESULT	HandleNotifications( IFXTaskData* pTaskData );

  // IFX scene resources
  IFXDECLAREMEMBER(IFXCoreServices,m_pCoreServices);    ///< IFX core services
  IFXDECLAREMEMBER(IFXPalette,m_pGeneratorPalette);  ///< IFX generator palette
  IFXDECLAREMEMBER(IFXPalette,m_pNodePalette);     ///< IFX node palette
  IFXDECLAREMEMBER(IFXSceneGraph,m_pSceneGraph);      ///< IFX scenegraph

  CArrayList<U32> m_pViewIndeces; ///< Indeces of Views in node palette
  U32         m_uViewCounter; ///< Counter of loaded views, equal number of elements in m_pViews
  U32         m_uActiveViewIndex; ///< Active view index in m_pViews array
  U32         m_uActiveViewInstance; ///< Active view node instance number

  IFXDECLAREMEMBER(IFXDevice,m_pDevice); ///< The rendering device
  IFXDECLAREMEMBER(IFXRenderServices,m_pRenderServices); ///< The render services that maintain the renderer information

  // Misc pointers
  U3DSceneUtilities* m_pSGUtils; ///< The utility class, for when we need to access the IFX resources in a callback

  IFXCLODModifier* m_pCLODModifier;

  IFXRenderWindow m_ViewerWindow;

  HMENU       m_hMenu;
  DWORD       m_dwWindowStyle;
  HANDLE      m_hInstance;

  HWND        m_hStatusBar;
  HWND        m_hProgress;
  HWND        m_hToolBar;
  HWND		  m_hCLODLevelEdit;
  HWND		  m_hFPSEdit;

  U3DRenderer m_uRendererID;
  U3DCuller   m_uCullerID;
  BOOL        m_bRendererInitialized;
  BOOL        m_bPreserveViewSettings;
  BOOL        m_bLoadingAtomic;
  TCHAR       m_strU3DFilename[512];
  OPENFILENAME ofn;

  BOOL        m_bMultiPicking;
  U32		  m_UseTargetFramerate;
  F32		  m_TargetFrameRate;

  IFXVector3  m_pivotPoint;

  IFXDECLAREMEMBER(IFXPerformanceTimer,m_pITimer);
  U32         m_Frames;
  U32         StartTime;

  U32         m_TimeOut;
  UserOptions m_UserOptions;

  BOOL		m_bRenderIt;
  BOOL		m_bFindView;
  BOOL		m_startToFindCameras;
  BOOL		m_ZeroWindow;
  U32		m_ViewID;

  IFXModel* m_pPickedModel;
  U32 m_PickedInstance;

  IFXString m_AppName;
};


#endif
