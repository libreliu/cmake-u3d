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

#define CRTDBG_MAP_ALLOC

#include <windows.h>

#include "IFXCOM.h"
#include "IFXException.h"
#include "IFXCoreCIDs.h"
#include "IFXRenderingCIDs.h"
#include "IFXImportingCIDs.h"
#include "IFXBoundSphereDataElement.h"
#include "IFXDids.h"
#include "IFXImageCodec.h"
#include "IFXExportingCIDs.h"
#include "IFXWriteManager.h"
#include "IFXWriteBuffer.h"
#include "IFXPickObject.h"
#include "IFXOSUtilities.h"
#include "U3DViewer.h"


class U3DSamplePlayer
{
public:
	U3DViewer m_viewer;

	U3DSamplePlayer();
	~U3DSamplePlayer();
};


int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	IFXRESULT result = IFX_OK;
	wchar_t * ptr;
	size_t s;

	try {		
		result = IFXSetDefaultLocale();
		IFXTRACE_GENERIC(L"[SamplePlayer] IFXSetDefaultLocale %i\n", result);

		IFXDEBUG_STARTUP();
		result = IFXCOMInitialize();
		IFXTRACE_GENERIC(L"[SamplePlayer] IFXCOMInitialize %i\n", result);
		{
			U3DSamplePlayer u3dapp;

			if ( IFXSUCCESS(result)) {
				result = u3dapp.m_viewer.SetupWindows();
				IFXTRACE_GENERIC(L"[SamplePlayer] m_viewer.SetupWindows %i\n", result);
			}

			s = mbstowcs(NULL, lpCmdLine, 0);
			ptr = new wchar_t[s+1];
			mbstowcs(ptr, lpCmdLine, s);
			ptr[s]=L'\0';
			if ( IFXSUCCESS(result))
				result = u3dapp.m_viewer.ParseArguments(ptr);
			delete(ptr);

			// Render until the user is done
			if ( IFXSUCCESS(result)) {
				result = u3dapp.m_viewer.EventLoop();
				IFXTRACE_GENERIC(L"[SamplePlayer] m_viewer.EventLoop %i\n", result);
			}
		}
		IFXRESULT result2 = IFXCOMUninitialize();
		if (IFXSUCCESS(result))
			result = result2;
		IFXTRACE_GENERIC(L"[SamplePlayer] IFXCOMUninitialize %i\n", result2);
		IFXDEBUG_SHUTDOWN();
	}
	catch (IFXException& e) {
		result = e.GetIFXResult();
	}

	return result;
}


U3DSamplePlayer::U3DSamplePlayer()
{
}

U3DSamplePlayer::~U3DSamplePlayer()
{
}
