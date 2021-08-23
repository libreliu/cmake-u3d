//***************************************************************************
//
//  Copyright (c) 2005 - 2006 Intel Corporation
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

#include <windows.h>
#include "IFXException.h"
#include "IFXBoundSphereDataElement.h"
#include "IFXDids.h"
#include "IFXGlyph2DModifier.h"

#include "U3DViewer.h"
#include "TrueType.h"
#include "IFXCOM.h"

int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	IFXRESULT result = IFX_OK;
	result = IFXCOMInitialize();
	try
	{
		U3DViewer viewer;

		if ( IFXSUCCESS(result))
			result = viewer.SetupWindows();

		if ( IFXSUCCESS(result))
		{
			U3DSceneUtilities* pSGUtils = new U3DSceneUtilities; //object is deleted in ~U3DViewer
			result = RunSample( pSGUtils );
			if ( IFXSUCCESS(result))
				result = viewer.InitializeSG( pSGUtils );
		}

		// Render
		if ( IFXSUCCESS(result))
			result = viewer.EventLoop();
		if ( IFXSUCCESS(result))
			result = IFXCOMUninitialize();
		else
			IFXCOMUninitialize();
	}
	catch (IFXException& e)
	{
		result = e.GetIFXResult();
	}

	return result;
}
