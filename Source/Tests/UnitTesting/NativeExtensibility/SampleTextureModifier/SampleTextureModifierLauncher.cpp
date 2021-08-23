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
//Category: TextureModifier

#include "IFXCheckX.h"
#include "SampleTextureModifier.h"

#ifdef WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	char* pParams = lpCmdLine;
#else
int main(int argc, char **argv)
{
	char* pParams = NULL;

	if( argc > 1 )
		pParams = argv[1];
#endif
	IFXRESULT result = IFX_OK;
	SampleTextureModifier test;

	try
	{
		IFXCHECKX( test.NewLog() );
		IFXCHECKX( test.StartUp() );
		IFXCHECKX( test.Run() );
		IFXCHECKX( test.View( pParams ) );
		IFXCHECKX( test.ShutDown() );
		IFXCHECKX( test.Report() );
		IFXCHECKX( test.ReleaseLog() );
	}
	catch (IFXException& e)
	{
		result = e.GetIFXResult();
		test.Report();
		test.ReleaseLog();
	}
	catch (...)
	{
		test.Report();
		test.ReleaseLog();
	}
	return result;
}
