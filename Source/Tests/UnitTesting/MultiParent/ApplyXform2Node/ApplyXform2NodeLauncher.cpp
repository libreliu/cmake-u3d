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
//Category: Rendering

#include "IFXException.h"
#include "U3DViewer.h"
#include "ApplyXform2Node.h"

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
{
	IFXRESULT result = IFX_OK;
	wchar_t * ptr;
	size_t s;

	try {
		result = IFXCOMInitialize();
		U3DViewWrapper u3dapp;
		ApplyXform2Node test;

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

		s = mbstowcs(NULL, lpCmdLine, 0);
		ptr = new wchar_t[s+1];
		mbstowcs(ptr, lpCmdLine, s);
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
	result |= IFXCOMUninitialize();
	return result;
}
#else
int main(int argc, char **argv)
{
	IFXRESULT result = IFX_OK;
	wchar_t * ptr;
	size_t s;

	try {
		result = IFXCOMInitialize();
		U3DViewWrapper u3dapp;
		ApplyXform2Node test;

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

		s = mbstowcs(NULL, *argv, 0);
		ptr = new wchar_t[s+1];
		mbstowcs(ptr, *argv, s);

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
	result |= IFXCOMUninitialize();
	return result;
}

#endif


U3DViewWrapper::U3DViewWrapper()
{
	m_pViewer = new(U3DViewer);

}

U3DViewWrapper::~U3DViewWrapper()
{
	delete(m_pViewer);

}

#else

int main()
{
	IFXCOMInitialize();
	U3DSceneUtilities* pSGUtils = new U3DSceneUtilities;
	if( pSGUtils )
	{
		ApplyXform2Node test;
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
