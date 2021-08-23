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
//Exec: 5
//Timeout: 1
//Category: Nodes
//Category: Light
//Category: View
//Category: Model
//Category: Shaders

#include "IFXBoundSphereDataElement.h"
#include "IFXSphereGenerator.h"
#include "IFXDids.h"
#include "IFXCheckX.h"
#include "IFXException.h"
#include "U3DViewer.h"
#include "SampleSphere.h"

#if 1

class U3DViewWrapper
{
public:
	U3DViewer* m_pViewer;

	U3DViewWrapper();
	~U3DViewWrapper();
};


#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInstance,
           HINSTANCE hPrevInstance,
           LPSTR lpCmdLine,
           int nCmdShow)
#else
int main(int argc, char **argv)
#endif
{
	IFXRESULT result = IFX_OK;
	wchar_t * ptr;
	size_t s;

	try {
		U3DViewWrapper u3dapp;
		SampleSphere test;

		if ( IFXSUCCESS(result)) {
			result = u3dapp.m_pViewer->SetupWindows();
		}

		if ( IFXSUCCESS(result)) {
			U3DSceneUtilities* pSGUtils = new U3DSceneUtilities;
			test.NewLog();
			test.Run( pSGUtils );
			test.Report();
			test.ReleaseLog();
			result = u3dapp.m_pViewer->InitializeSG( pSGUtils );
		}

#ifdef WIN32
	s = mbstowcs(NULL, lpCmdLine, 0);
	ptr = new wchar_t[s+1];
	mbstowcs(ptr, lpCmdLine, s);
#else
	s = mbstowcs(NULL, *argv, 0);
	ptr = new wchar_t[s+1];
	mbstowcs(ptr, *argv, s);
#endif
		ptr[s]=L'\0';
		if ( IFXSUCCESS(result))
			result = u3dapp.m_pViewer->ParseArguments(ptr);
		delete(ptr);

		// Render until the user is done
		if ( IFXSUCCESS(result)) {
			result = u3dapp.m_pViewer->EventLoop();
		}

		if( test.GetNumFailed() )
			result |= 1;
	}
	catch (IFXException& e) {
		result = e.GetIFXResult();
	}
	return result;
}


U3DViewWrapper::U3DViewWrapper()
{
	IFXCHECKX(IFXCOMInitialize());
	m_pViewer = new(U3DViewer);

}

U3DViewWrapper::~U3DViewWrapper()
{
	delete(m_pViewer);
	IFXCHECKX(IFXCOMUninitialize());
}

#else

int main()
{
	IFXCOMInitialize();
	U3DSceneUtilities* pSGUtils = new U3DSceneUtilities;
	if( pSGUtils )
	{
		SampleSphere test;
		test.NewLog();
		test.Run( pSGUtils );
		test.Report();
		test.ReleaseLog();
		delete pSGUtils;
		IFXCOMUninitialize();
		if( test.GetNumFailed() ) return 1;
		else return 0;
	}
}

#endif
