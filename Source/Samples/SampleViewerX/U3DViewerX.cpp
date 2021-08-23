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

#include "U3DViewer.h"

#include "IFXCheckX.h"
#include "IFXRenderingCIDs.h"
#include "IFXSchedulingCIDs.h"
#include "IFXPalette.h"
#include "IFXScheduler.h"
#include "IFXDevice.h"

#include <wchar.h>
#include <signal.h>

int g_sigQuit;

void SignalQuitHandler(int sig)
{
	printf("\nSIGQUIT signal was arrived!!!\n\n");
	g_sigQuit = TRUE;
}

U3DViewer::U3DViewer()
{
	m_pDevice = NULL,
	m_pRenderServices = NULL;
	m_pSGUtils = NULL;

	m_uActiveViewID = 0;
	m_uActiveViewInstance = 0;
	m_bRendererInitialized = FALSE;
	m_bLoadingAtomic = TRUE;
	m_bPreserveViewSettings = FALSE;

	m_uRendererID = U3D_DEFAULT_RENDERER;
	m_uCullerID = U3D_DEFAULT_CULLER;

	m_TimeOut = 0;

	m_uViewCounter = 0;

	m_views[0] = NULL;

	g_sigQuit = FALSE;
	signal(SIGQUIT, SignalQuitHandler);
}


U3DViewer::~U3DViewer()
{

	U32 i;
	for (i = 0; i < m_uViewCounter; i++)
	{
		IFXRELEASE( m_views[i] );
	}

	IFXDELETE(m_pSGUtils);

	IFXRELEASE(m_pDevice);
	IFXRELEASE(m_pRenderServices);

	Display* dpy = (Display*)m_wInfo.GetWindowPtr();
	if (dpy)
	{
		Window win = m_wInfo.GetDrawable();
		if (win)
			XDestroyWindow(dpy, win);

		XFree(m_wInfo.GetVisual());

		XCloseDisplay(dpy);
	}
}


IFXRESULT U3DViewer::Initialize(const BOOL RemoteLoading)
{
	return InitializeSG( NULL, RemoteLoading );
}

IFXRESULT U3DViewer::InitializeSG( U3DSceneUtilities* pSGUtils, const BOOL RemoteLoading )
{
	IFXRESULT result = IFX_OK;
	U8 uLoad = 0;

	if( NULL != m_pSGUtils )
	{
		U8 i;
		for (i = 0; i < m_uViewCounter; i++)
		{
			IFXRELEASE( m_views[i] );
		}
		IFXDELETE(m_pSGUtils);
		m_uActiveViewInstance = 0;

		m_startToFindCameras = TRUE;
		m_uViewCounter = 0;
	}

	if( NULL == pSGUtils )
	{
		pSGUtils = new U3DSceneUtilities;
		result = pSGUtils->InitializeScene();
		uLoad = 1;
	}

	if ( !pSGUtils || !pSGUtils->m_pNodePalette )
		result = IFX_E_INVALID_POINTER;

	// Set up pointers to IFX scene resources and add references that
	// indicate our use of them.
	if ( IFXSUCCESS(result) && uLoad )
	{
		//LOAD FILE
		try
		{
			pSGUtils->LoadU3DFileInit( m_strU3DFilename, m_bLoadingAtomic, RemoteLoading );
		}
		catch(IFXException e)
		{
			result = e.GetIFXResult();
			IFXDELETE(m_pSGUtils);
			m_uActiveViewInstance = 0;
		}
		catch ( ... )
		{
			result = IFX_E_UNDEFINED;
		}
	}

	if ( IFXSUCCESS(result) )
	{
		m_pSGUtils = pSGUtils;
	}

	//Initialize the Renderer.
	if ( IFXSUCCESS(result) && !m_bRendererInitialized )
	{
		result = InitializeRenderer();
	}
	m_uViewCounter = 0;

	return result;
}

/**
	Set up the viewer window for rendering in accordance with
	the user's preferences.
*/
IFXRESULT U3DViewer::SetupWindows()
{
	IFXRESULT result = IFX_OK;

	Display* dpy = XOpenDisplay("");

	if( NULL == dpy )
	{
		IFXTRACE_GENERIC(L"[U3DViewerX]: Cannot connect to X server\n");
		result = IFX_E_UNDEFINED;
	}
	else
	{
		int attrib[] = {
						GLX_RGBA,
						GLX_RED_SIZE, 1,
						GLX_GREEN_SIZE, 1,
						GLX_BLUE_SIZE, 1,
						GLX_DEPTH_SIZE, 1,
						GLX_DOUBLEBUFFER,
						GLX_LEVEL, 0,
						None
					   };

		int scrnum = DefaultScreen( dpy );

		XVisualInfo* visinfo = glXChooseVisual( dpy, scrnum, attrib );

		if( NULL == visinfo )
		{
			IFXTRACE_GENERIC(L"[U3DViewerX]: Couldn't get an RGB, Double-buffered visual\n");
			result = IFX_E_UNDEFINED;
		}
		else
		{
			Window root = RootWindow(dpy, scrnum);

			/* window attributes */
			XSetWindowAttributes attr;
			attr.background_pixel = 0;
			attr.border_pixel = 0;
			attr.colormap = XCreateColormap( dpy, root, visinfo->visual, AllocNone);
			attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
			unsigned long mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

			IFXRect winsize = IFXRect(0, 0, 640, 480);
			Window win = XCreateWindow(
						dpy, root, 0, 0, 640, 480,
						0, visinfo->depth, InputOutput,
						visinfo->visual, mask, &attr );

			m_wInfo.SetWindowPtr(dpy);
			m_wInfo.SetVisual(visinfo);
			m_wInfo.SetDrawable(win);
			m_wInfo.SetWindowSize(winsize);

			XMapWindow(dpy, win);
		}
	}

	return result;
}

IFXRESULT U3DViewer::SetRenderer(U3DRenderer Renderer)
{
	IFXRESULT result = IFX_OK;

	if (NULL != m_pRenderServices && NULL != m_pDevice)
	{
		if (m_pRenderServices->GetSupported(Renderer))
			m_uRendererID = Renderer;
		else
			if (m_pRenderServices->GetSupported(U3D_DEFAULT_RENDERER))
				m_uRendererID = U3D_DEFAULT_RENDERER;
			else
				result = IFX_E_UNSUPPORTED;

		if ( IFXSUCCESS(result) )
		{
			result = m_pDevice->SetRenderer( m_uRendererID );
		}

		if (m_wInfo.GetWindowPtr() != NULL )
		{
			IFXRect clientRect = m_wInfo.GetWindowSize();

			if(	clientRect.m_Height > 0 &&
				clientRect.m_Width > 0 &&
				m_views[m_uActiveViewID])
			{
				IFXCHECKX(SetViewport());
			}
		}
	}
	else
	{
		result = IFX_E_NOT_INITIALIZED;
	}

	return result;
}

///  Sets up to render 3D scene representation
IFXRESULT U3DViewer::InitializeRenderer()
{
	IFXRESULT result = IFX_OK;

	if (!m_pRenderServices)
	{
		// Initialize the renderer services
		result = IFXCreateComponent(
						CID_IFXRenderServices,
						IID_IFXRenderServices,
						(void**)&m_pRenderServices);

		if ( IFXSUCCESS(result) )
		{
			result = m_pRenderServices->RegisterRenderer(
						U3D_RENDERER_NULL,
						CID_IFXRenderNULL,
						CID_IFXRenderDeviceNULL );
		}

		if ( IFXSUCCESS(result) )
		{
			result = m_pRenderServices->RegisterRenderer(
						U3D_RENDERER_OPENGL,
						CID_IFXRenderOGL,
						CID_IFXRenderDeviceOGL );
		}
	}

	// if there is no device, create one and initialize it
	/// @todo: What happens if there is already a device?  Is there any setup that need to be repeated?
	if (!m_pDevice && IFXSUCCESS(result) )
	{
		result = IFXCreateComponent( CID_IFXDevice, IID_IFXDevice, (void**)&m_pDevice );

		if( IFXSUCCESS(result) )
			result = m_pDevice->Initialize( );

		if ( IFXSUCCESS(result) )
			result = m_pDevice->SetWindow(m_wInfo);

		// Set the renderer to use
		if ( IFXSUCCESS(result) )
			result = SetRenderer(m_uRendererID);
	}

	if ( IFXSUCCESS(result) )
		m_bRendererInitialized = TRUE;

	return result;
}

IFXRESULT U3DViewer::FindView()
{
	IFXRESULT result = IFX_OK;
	U32	paletteSize = 0;

	IFXPalette* pNodePalette = m_pSGUtils->m_pNodePalette;

	if (NULL != pNodePalette)
	{
		result = pNodePalette->GetPaletteSize(&paletteSize);

		if (IFXSUCCESS(result))
		{
			m_startToFindCameras = FALSE;

			// Get the view resource associated with this ID.
			U32 i;
			for( i = 1; i < paletteSize; ++i )
			{
				IFXDECLARELOCAL(IFXView, pView);

				if (IFXSUCCESS(result))
				{
					result = pNodePalette->GetResourcePtr( i, IID_IFXView, (void**) &pView );
				}

				if (IFXSUCCESS(result))
				{
					IFXADDREF(pView);

					if (m_views.size() > m_uViewCounter)
						IFXRELEASE(m_views[m_uViewCounter]);

					m_views[m_uViewCounter] = pView;
					IFXRELEASE(pView);
					m_uViewCounter++;
				}

				if( result == IFX_E_UNSUPPORTED )
					result = IFX_OK;
			}

			if (m_uActiveViewID >= m_uViewCounter)
			{
				m_uActiveViewID = 0;
				m_uActiveViewInstance = 0;
			}
		}
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

/**
This function clears the back buffer so that garbage pixels are not 
drawn to the screen when the used viewports do not cover the entire 
surface.

@note	For optimal performance, the device's back buffer is not 
automatically cleared when rendering.  Currently, there is no 
direct way to explictly clear the back buffer.  Normally, 
viewport(s) cover the entire renderable surface and since they 
can clear the portions used, this functionality is generally not 
necessary.  However, if the full area is not covered, the 
ability to do this is needed.  That's why this function is 
included and used.

@note	This function clears the back buffer using the U3D API by 
setting the viewport to be the entire window and configures bad 
clip plane values which currently (at least) result in no 
geometry being rendered.  Rather than creating a View Resource 
and View Node for this purpose, it uses the current view and only 
performs these changes temporarily.

@todo	Add a new U3D API function that exposes a more straight forward 
technique to clear the back buffer.
*/
IFXRESULT U3DViewer::ClearBackBuffer()
{
	const F32 ENTIRE_DEVICE = 1.0f;
	const U32 SCREEN_INDEPENDENT_UNITS = 0x00000001;
	const F32 BAD_NEAR_CLIP = 2.0f;
	const F32 BAD_FAR_CLIP = 1.0f;

	IFXRESULT result = IFX_OK;

	if (!(m_views[m_uActiveViewID] && m_pDevice))
		result = IFX_E_NOT_INITIALIZED;

	IFXF32Rect	viewRect;

	//Get the current values so they can be restored before exiting.
	if (IFXSUCCESS(result))
		result = m_views[m_uActiveViewID]->GetViewport(viewRect);

	U32 viewAttr = 0;
	F32 nearClip, farClip;

	if (IFXSUCCESS(result))
	{
		viewAttr = m_views[m_uActiveViewID]->GetAttributes();
		nearClip = m_views[m_uActiveViewID]->GetNearClip();
		farClip = m_views[m_uActiveViewID]->GetFarClip();
	}

	//set the view to use entire device and set
	//far clip in front of near clip
	if (IFXSUCCESS(result))
	{
		IFXF32Rect	clearRect( 0.0f, 0.0f, ENTIRE_DEVICE, ENTIRE_DEVICE );

		result = m_views[m_uActiveViewID]->SetViewport(clearRect);
	}
	if (IFXSUCCESS(result))
		m_views[m_uActiveViewID]->SetAttributes(SCREEN_INDEPENDENT_UNITS | viewAttr);
	if (IFXSUCCESS(result))
		result = m_views[m_uActiveViewID]->SetNearClip(BAD_NEAR_CLIP);
	if (IFXSUCCESS(result))
		result = m_views[m_uActiveViewID]->SetFarClip(BAD_FAR_CLIP);

	//render the blank scene (same call as in the main render loop
	//only with the viewport changes).
	if (IFXSUCCESS(result))		
		result = m_pDevice->Render();

	//restore the values to the view
	if (IFXSUCCESS(result))
		result = m_views[m_uActiveViewID]->SetViewport(viewRect);
	if (IFXSUCCESS(result))
		m_views[m_uActiveViewID]->SetAttributes(viewAttr);
	if (IFXSUCCESS(result))
		result = m_views[m_uActiveViewID]->SetNearClip(nearClip);
	if (IFXSUCCESS(result))
		result = m_views[m_uActiveViewID]->SetFarClip(farClip);

	return result;
}

IFXRESULT U3DViewer::SetViewport()
{
	IFXRESULT result = IFX_OK;

	if (m_views[m_uActiveViewID] && m_pDevice)
	{
		IFXDECLARELOCAL(IFXSpatialSetQuery, pCullingSubsystem);

		// Point the device to this view
		IFXRESULT temporaryResult = IFX_OK;

		// First remove all previous views associated with the device.
		while ( IFXSUCCESS( temporaryResult ) )
			temporaryResult = m_pDevice->RemoveLastView();

		// Then associate this view with the device.
		result = m_pDevice->AppendView( m_views[m_uActiveViewID], m_uActiveViewInstance );

		if (IFXSUCCESS(result))
		{
			if (m_uCullerID == U3D_SCENEGRAPH_CULLER)
				result = IFXCreateComponent(CID_IFXSceneGraphCuller, IID_IFXSpatialSetQuery,(void**)&pCullingSubsystem);
			else
				result = IFXCreateComponent(CID_IFXOctreeCuller, IID_IFXSpatialSetQuery,(void**)&pCullingSubsystem);
		}

		if (IFXSUCCESS(result))
			result = m_views[m_uActiveViewID]->SetCullingSubsystem(*pCullingSubsystem);

		if (IFXSUCCESS(result) && !m_bPreserveViewSettings)
		{
			IFXF32Rect ViewRect;
			ViewRect.Set(0, 0, 1, 1);
			result = m_views[m_uActiveViewID]->SetViewport(ViewRect);
			m_views[m_uActiveViewID]->SetAttributes(m_views[m_uActiveViewID]->GetAttributes()|1);
		}

		// Whenever the device's window and viewport are changed, do a one 
		// time clearing of the entire window.  This is only performed just 
		// in case the viewport is modified such that it does not occupy the 
		// entire drawable area of the device's window so that no visual 
		// garbage remains from previous rendered frames.  If your 
		// application will always have viewport(s) covering the entire 
		// window, then it is not necessary to perform the logic in the 
		// ClearWindow function.
		if(IFXSUCCESS(result))
			result = ClearBackBuffer();
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

/**
	Render frames until the user decides to quit or some other
	terminal event occurs.
*/
IFXRESULT U3DViewer::EventLoop()
{
	IFXRESULT   result = IFX_OK;
	Display* dpy = (Display*)m_wInfo.GetWindowPtr();

	result = FindView();

	if(IFXSUCCESS(result))
		result = SetViewport();

	while (IFXSUCCESS(result) && result != IFX_CANCEL && !g_sigQuit) 
	{
		while (XPending(dpy) > 0) 
		{
            XEvent event;
			XNextEvent( dpy, &event );

			switch (event.type) {
			case Expose:
				///@todo: redraw
				break;
			case ConfigureNotify:
				///@todo: resize
				break;
			case KeyPress:
				{
					char keyEvent[10];
					int keyCode, tmp;
					keyCode = XLookupKeysym(&event.xkey, 0);

					tmp = XLookupString(&event.xkey, keyEvent, sizeof(keyEvent), NULL, NULL);
					if (keyEvent[0] == 27) 
					{
						//Escape code
						result = IFX_CANCEL;
					}
				}
			}
		}

		if (result != IFX_CANCEL) 
		{
			if (IFXSUCCESS(result))
				m_pSGUtils->ProcessScheduler();

			if (IFXSUCCESS(result))
				result = m_pDevice->Render();
		}
    }

	return result;
}

/**
	Parses CommandLine and fills UserOptions structure in U3DSamplePlayer

	@param    const IFXCHAR* cmdline   The application's CommandLine
*/
IFXRESULT U3DViewer::ParseArguments(const IFXCHAR *cmdline)
{
	IFXRESULT result = IFX_OK;
	IFXCHAR *pdest;
	U32 length;

	m_UserOptions.m_bLoadingAtomic = m_bLoadingAtomic;
	wcscpy(m_UserOptions.m_Filename, m_strU3DFilename);
	m_UserOptions.m_Renderer = m_uRendererID;
	m_UserOptions.m_Culler = m_uCullerID;
	m_UserOptions.m_TimeOut = m_TimeOut;

	if (cmdline[0]) {
		for (;;) {
			while (cmdline[0]==' ') cmdline++;
			if (cmdline[0]!='-') break;
			switch (cmdline[1]) {
		case 'r': //Renderer mode: DX7, DX8 or OGL
			if (!wcsncmp(cmdline,L"-rogl",5)) {
				m_UserOptions.m_Renderer = U3D_RENDERER_OPENGL;
			} else if (!wcsncmp(cmdline,L"-rnul",5)) {
				m_UserOptions.m_Renderer = U3D_RENDERER_NULL;
			} else {
				IFXTRACE_GENERIC(L"[SamplePlayer] Unknown renderer. Switched to default one\n");
			}
			cmdline+=5;
			break;
		case 'c': //Culling mode: SG, OC
			if (!wcsncmp(cmdline,L"-csgr",5)) {
				m_UserOptions.m_Culler = U3D_SCENEGRAPH_CULLER;
			} else if (!wcsncmp(cmdline,L"-coct",5)) {
				m_UserOptions.m_Culler = U3D_OCTREE_CULLER;
			} else {
				IFXTRACE_GENERIC(L"[SamplePlayer] Unknown culler. Switched to default one\n");
			}
			cmdline+=5;
			break;
		case 'l': //Loading mode: non-atomic is default, you can switch it to atomic mode by -latom
			{
				if (!wcsncmp(cmdline,L"-latom",6)) {
					m_UserOptions.m_bLoadingAtomic = TRUE;
					IFXTRACE_GENERIC(L"[SamplePlayer] atomic loading\n");
					cmdline+=4;
				} else if (!wcsncmp(cmdline,L"-lnatom",7)) {
					m_UserOptions.m_bLoadingAtomic = FALSE;
					IFXTRACE_GENERIC(L"[SamplePlayer] asynchronous loading\n");
					cmdline+=5;
				} else {
					IFXTRACE_GENERIC(L"[SamplePlayer] Unknown loading mode. Switched to default one\n");
				}
				cmdline+=2;
			}
			break;
		case 'i': //Input file: -ifilename.ext
			cmdline+=2;
			pdest = wcschr( cmdline, ' ' );
			if (!pdest) {
				pdest = wcschr( cmdline, 0 );
			}
			if (pdest) {
				length = (U32)(pdest - cmdline);
				wcsncpy(m_UserOptions.m_Filename, cmdline, length);
				m_UserOptions.m_Filename[length] = '\0';

				cmdline+=length;
			} else {
				IFXTRACE_GENERIC(L"[SamplePlayer] Unknown filename format.\n");
			}
			break;
		case 't': //Timeout in seconds
			cmdline+=2;
			pdest = wcschr( cmdline, ' ' );
			if (!pdest) {
				pdest = wcschr( cmdline, 0 );
			}
			if (pdest) {
				length = (U32)(pdest - cmdline);
				m_UserOptions.m_TimeOut = 30; //_wtoi(cmdline);
				cmdline+=length;
			} else {
				IFXTRACE_GENERIC(L"[SamplePlayer] Unknown filename format.\n");
			}
			break;
		default:
			IFXTRACE_GENERIC(L"[SamplePlayer] Unknown option: %ls\n", cmdline);
			}
		}
		m_bLoadingAtomic = m_UserOptions.m_bLoadingAtomic;
		m_uRendererID = m_UserOptions.m_Renderer;
		m_uCullerID = m_UserOptions.m_Culler;
		if (m_UserOptions.m_Filename[0] != 0) {
			wcscpy(m_strU3DFilename, m_UserOptions.m_Filename);
			m_TimeOut = m_UserOptions.m_TimeOut;

			result = Initialize();
		}
	}

	return result;
}

