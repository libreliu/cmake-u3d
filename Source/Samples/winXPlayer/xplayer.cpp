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

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include "u3dplayer.h"

#include "IFXQuaternion.h"
#include "IFXOSUtilities.h"

#define MAXFILENAMELENGTH 512
#define DEFAULT_DISPLAYNAME ""

#define	ToRadians(x)	((x)*3.1415926535f/180.0f)

int g_sigQuit;

void SignalQuitHandler(int sig)
{
	printf("\nSIGQUIT signal was arrived!!!\n\n");
	g_sigQuit = TRUE;
}

IFXRESULT RotateView( F32 fInX, 
				F32 fInY,
				BOOL mod,
				BOOL yKeyDown,
				IFXVector3		pivotPoint,
				IFXMatrix4x4	*pViewMatrix)
{
	IFXRESULT result = IFX_OK;
	F32 radians;

	if (pViewMatrix) {
		if ( mod )
		{
			if ( fInX != 0 )
			{
				// Roll the camera in camera space
				radians = ToRadians(fInX * 0.5f);
				pViewMatrix->Rotate3x4( radians, IFX_Z_AXIS );
			}
		}
		else
		{
			if ( fInY != 0 )
			{
				F32 *rawMatrix=pViewMatrix->Raw();

				// Tilt camera in world space about pivot
				radians = ToRadians(-fInY * 0.5f);

				IFXVector3 right(rawMatrix[0], rawMatrix[1], rawMatrix[2]);
				right.Normalize();

				// Build axis-angle rotation matrix
				IFXQuaternion	quat;
				quat.MakeRotation(radians, right);

				IFXMatrix4x4	mAngleAxisRot = quat;
				IFXMatrix4x4	mIntermediate(rawMatrix);

				// rather than actually compositing these matrices, build
				// the translation component of TRT^-1 directly (faster).

				rawMatrix[12] = -(pivotPoint.X()*rawMatrix[0] +
					pivotPoint.Y()*rawMatrix[4] +
					pivotPoint.Z()*rawMatrix[8]) + pivotPoint.X();
				rawMatrix[13] = -(pivotPoint.X()*rawMatrix[1] +
					pivotPoint.Y()*rawMatrix[5] +
					pivotPoint.Z()*rawMatrix[9]) + pivotPoint.Y();
				rawMatrix[14] = -(pivotPoint.X()*rawMatrix[2] +
					pivotPoint.Y()*rawMatrix[6] +
					pivotPoint.Z()*rawMatrix[10]) + pivotPoint.Z();


				// Now apply this rotation about the pivot point
				pViewMatrix->Multiply3x4(mAngleAxisRot, mIntermediate);		
			}

			if ( fInX != 0 )
			{
				F32 *rawMatrix=pViewMatrix->Raw();

				// compute up vector
				IFXVector3 up(0.0f, 0.0f, 0.0f);
				if ( yKeyDown )
					up.Y() = 1.0f;		// Y is UP
				else
					up.Z() = 1.0f;		// Z is UP

				// compute rotation angle
				radians = ToRadians(-fInX * 0.5f);

				// Rotate camera about Up axis in world space about pivot

				// Build axis-angle rotation matrix
				IFXQuaternion	quat;
				quat.MakeRotation(radians, up);

				IFXMatrix4x4	mAngleAxisRot = quat;
				IFXMatrix4x4	mIntermediate(rawMatrix);

				// rather than actually compositing these matrices, build
				// the translation component of TRT^-1 directly (faster).

				rawMatrix[12] = -(pivotPoint.X()*rawMatrix[0] +
					pivotPoint.Y()*rawMatrix[4] +
					pivotPoint.Z()*rawMatrix[8]) + pivotPoint.X();
				rawMatrix[13] = -(pivotPoint.X()*rawMatrix[1] +
					pivotPoint.Y()*rawMatrix[5] +
					pivotPoint.Z()*rawMatrix[9]) + pivotPoint.Y();
				rawMatrix[14] = -(pivotPoint.X()*rawMatrix[2] +
					pivotPoint.Y()*rawMatrix[6] +
					pivotPoint.Z()*rawMatrix[10]) + pivotPoint.Z();


				// Now apply this rotation about the pivot point
				pViewMatrix->Multiply3x4(mAngleAxisRot, mIntermediate);
			}
		}
	} else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}

//
// CreateWindow creates the Window for rendering with specified size
// return: Window win;
// 
static Window CreateWindow( Display *dpy,
				  IFXRect size,
				  XVisualInfo *visinfo )
{
   int scrnum;
   XSetWindowAttributes attr;
   unsigned long mask;
   Window root;
   Window win;

   scrnum = DefaultScreen( dpy );

   root = RootWindow(dpy, scrnum);
   /* window attributes */
   attr.background_pixel = 0;
   attr.border_pixel = 0;
   attr.colormap = XCreateColormap( dpy, root, visinfo->visual, AllocNone);
   attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
   mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

   win = XCreateWindow( dpy, root, 
						size.m_X, size.m_Y, size.m_Width, size.m_Height,
						0, visinfo->depth, InputOutput,
						visinfo->visual, mask, &attr );
   return win;
}

int main(int argc, char **argv)
{
	IFXRESULT result = IFX_OK;
	Window win;
	Display *dpy = NULL;
	XVisualInfo *visinfo = NULL;

	g_sigQuit = FALSE;
	signal(SIGQUIT, SignalQuitHandler);

	IFXCOMInitialize();
{
	U3DSamplePlayer u3dApp;

	char *displayName = DEFAULT_DISPLAYNAME;
	IFXCHAR fileName[MAXFILENAMELENGTH];

	IFXRenderWindow renderWin;
	IFXRect winsize;

	int attrib[] = { GLX_RGBA,
			 GLX_RED_SIZE, 1,
			 GLX_GREEN_SIZE, 1,
			 GLX_BLUE_SIZE, 1,
			 GLX_DEPTH_SIZE, 1,
			 GLX_DOUBLEBUFFER,
			 None };
	
	if (argc == 1) 
	{
		printf("\nno parameters!!!\n use name of input *.u3d file as single parameter\n");
		result = IFX_E_INVALID_FILE;
	}
	if (IFXSUCCESS(result))
	{
		result = IFXSetDefaultLocale();
		IFXTRACE_GENERIC(L"[winXPlayer] IFXSetDefaultLocale %i\n", result);
	}

	if (IFXSUCCESS(result))
	{
		if (!mbstowcs(fileName, argv[1], MAXFILENAMELENGTH)) 
		{
			printf("\ninvalid filename\n");
			result = IFX_E_INVALID_FILE;
		}
		
	}

	if (IFXSUCCESS(result)) 
		result = u3dApp.InitScene(fileName);

	if (IFXSUCCESS(result)) 
	{
		dpy = XOpenDisplay(displayName);
		if (NULL == dpy)
			result = IFX_E_RESOURCE_NOT_AVAILABLE;
	}

	if (IFXSUCCESS(result)) 
	{
		U32 scrnum = DefaultScreen( dpy );
		visinfo = glXChooseVisual( dpy, scrnum, attrib );
		if (!visinfo) {
		   printf("Error: couldn't get an RGB, Double-buffered visual\n");
		   result = IFX_E_UNSUPPORTED;
		}
	}

	if (IFXSUCCESS(result)) 
	{
		winsize.Set(0, 0, 640, 480);
		win = CreateWindow(dpy, winsize, visinfo);
		///@todo: do proper error handling for XCreateWindow
	}

	if (IFXSUCCESS(result)) 
	{
		renderWin.SetWindowPtr(dpy);
		renderWin.SetVisual(visinfo);
		renderWin.SetDrawable(win);
		renderWin.SetWindowSize(winsize);

		///initialize Renderer
		result = u3dApp.InitRender(&renderWin);
	}


	//ViewNode
	IFXDECLARELOCAL(IFXView, pView);
	if (IFXSUCCESS(result))
		result = u3dApp.FindView(&pView);
	if (IFXSUCCESS(result))
		result = u3dApp.SetView(pView);

	if (IFXSUCCESS(result))
		result = u3dApp.ClearBackbuffer(pView);

	while (IFXSUCCESS(result) && result != IFX_CANCEL && !g_sigQuit) 
	{
		while (XPending(dpy) > 0) 
		{
            XEvent event;
			XNextEvent( dpy, &event );

			switch (event.type) {
			case Expose:
				///@todo: redraw
				printf("\nExpose\n\n");
				break;
			case ConfigureNotify:
				///@todo: resize
				printf("\nConfigureNotify\n\n");
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
				break;
			}
		}
		if (result != IFX_CANCEL) 
		{
			if (IFXSUCCESS(result))
				result = u3dApp.ProcessScheduler();
			if (IFXSUCCESS(result))
				result = u3dApp.Render();

			//calculate frame rate
			if (IFXSUCCESS(result))
			{
			   static int t0 = -1;
			   static int frames = 0;
			   struct timezone tZone;
			   struct timeval tVal;
			   gettimeofday(&tVal, &tZone);

			   if (t0 < 0)
				  t0 = tVal.tv_sec;

			   frames++;

			   if (tVal.tv_sec - t0 >= 5.0) {
				  GLfloat seconds = tVal.tv_sec - t0;
				  GLfloat fps = frames / seconds;
				  printf("%d frames in %3.1f seconds = %6.3f FPS\n", frames, seconds,
						 fps);
				  t0 = tVal.tv_sec;
				  frames = 0;
			   }

			   IFXVector3 pivotPoint;
			   IFXMatrix4x4 viewMatrix;

			   pivotPoint.Set(0.0f, 0.0f, 0.0f);
			   viewMatrix = (IFXMatrix4x4)pView->GetMatrix(0);

			   result = RotateView(0.1, 0.1, FALSE, FALSE, pivotPoint, &viewMatrix);

			   result = pView->SetMatrix(0, &viewMatrix);
			}

		}
     }
}
	IFXCOMUninitialize();

	if (dpy)
	{
		XFree(visinfo);
		XDestroyWindow(dpy, win);
		XCloseDisplay(dpy);
	}

	return result;
}

