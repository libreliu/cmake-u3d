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

//***************************************************************************
//  Includes
//***************************************************************************

#include <wchar.h>
#include "IFXDebug.h"
#include "IFXCheckX.h"
#include "IFXCOM.h"
#include "SceneUtilities.h"

int wmain(int argc, wchar_t* argv[])
{
	IFXRESULT result = IFX_OK;

	try
	{
		IFXDEBUG_STARTUP();

		IFXCHECKX( IFXCOMInitialize() );

		wchar_t* inputFile = NULL;
		wchar_t* outputFile = NULL;
		
		if( 1 == argc )
		{
			printf("BaseMeshConverter usage:\n");
			printf("BaseMeshConverter <filename1> <filename2>\n");
			printf("where\n");
			printf("<filename1> is the name of source U3D file\n");
			printf("<filename2> is the name of U3D file with compressed base mesh\n");

			return -1;
		}

		inputFile = argv[1];
		outputFile = argv[2];

		wprintf( L"Input file - %s\n", inputFile );
		wprintf( L"Output file - %s\n", outputFile );

		{
			SceneUtilities sceneUtils;

			sceneUtils.LoadU3DFileX( inputFile );
			sceneUtils.ConvertToCompressedBaseMeshX();
			sceneUtils.WriteU3DFileX( outputFile );
		}

		IFXRESULT result = IFXCOMUninitialize();
		IFXTRACE_GENERIC( L"[BaseMeshConverter]: IFXCOMUninitialize %i\n", result );
		
		IFXDEBUG_SHUTDOWN();
	}
	catch(IFXException& e)
	{
		result = e.GetIFXResult();

		wprintf(
			L"Received an exception with error return code "
			L"0x%08lX with description\n\"%ls\"\n",
			result,
			e.GetText().Raw() );
	}
	catch(...)
	{
		wprintf( L"Received an unknown exception\n" );
	}
}
