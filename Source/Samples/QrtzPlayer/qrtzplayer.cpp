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

#include <Carbon/Carbon.h>
#include <OpenGL/gl.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include "u3dplayer.h"

#include "IFXQuaternion.h"
#include "IFXOSUtilities.h"

#define MAXFILENAMELENGTH 512
#define DEFAULT_DISPLAYNAME ""

#define ToRadians(x)  ((x)*3.1415926535f/180.0f)

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
        IFXVector3    pivotPoint,
        IFXMatrix4x4  *pViewMatrix)
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
        IFXQuaternion quat;
        quat.MakeRotation(radians, right);

        IFXMatrix4x4  mAngleAxisRot = quat;
        IFXMatrix4x4  mIntermediate(rawMatrix);

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
          up.Y() = 1.0f;    // Y is UP
        else
          up.Z() = 1.0f;    // Z is UP

        // compute rotation angle
        radians = ToRadians(-fInX * 0.5f);

        // Rotate camera about Up axis in world space about pivot

        // Build axis-angle rotation matrix
        IFXQuaternion quat;
        quat.MakeRotation(radians, up);

        IFXMatrix4x4  mAngleAxisRot = quat;
        IFXMatrix4x4  mIntermediate(rawMatrix);

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

int main(int argc, char **argv)
{
	IFXRESULT result = IFX_OK;
	WindowRef window = 0;
	CFStringRef theTitle = CFSTR("QrtzPlayer");
	Rect viewRect = {100, 100, 580, 740};
	IFXRect winsize(0, 0, 640, 480);

	g_sigQuit = FALSE;
	signal(SIGQUIT, SignalQuitHandler);

	IFXCOMInitialize();
	{
		U3DSamplePlayer u3dApp;

		IFXCHAR fileName[MAXFILENAMELENGTH];

		IFXRenderWindow renderWin;		

		if (argc == 1)
		{
			printf("\nno parameters!!!\n use name of input *.u3d file as single parameter\n");
			result = IFX_E_INVALID_FILE;
		}

		if (IFXSUCCESS(result))
		{
			result = IFXSetDefaultLocale();
			IFXTRACE_GENERIC(L"[QrtzPlayer] IFXSetDefaultLocale %i\n", result);
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
			CreateNewWindow( kDocumentWindowClass, kWindowStandardDocumentAttributes | kWindowStandardHandlerAttribute, &viewRect, &window );

			SetWindowTitleWithCFString( window, theTitle );

			if (window)
			{
				ShowWindow( window );

				renderWin.SetWindowPtr(window);
				renderWin.SetWindowSize(winsize);

				//initialize Renderer
				result = u3dApp.InitRender(&renderWin);
			}
			else
			{
				IFXTRACE_GENERIC(L"%s:%i Cann't create window\n");
				result = IFX_E_UNSUPPORTED;
			}
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
			Boolean  gotEvent;
			EventRecord  event;

			gotEvent = WaitNextEvent(everyEvent, &event, 0, NULL);

			if (gotEvent)
			{
				switch (event.what) 
				{
					case keyDown:
					case autoKey:
						char key = event.message & charCodeMask;
						if (event.modifiers & cmdKey)
						if (event.what == keyDown)
						if (key == kEscapeCharCode)
						{
							//Escape code
							result = IFX_CANCEL;
						}
						break;
					
					case kHighLevelEvent :
						AEProcessAppleEvent(&event);
						break;
					
					default :
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

	if (window)
	{
		DisposeWindow(window);
	}

	return result;
}
