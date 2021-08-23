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
  @file ConverterDriver.cpp

      This module defines driver for IDTF converter. It contains main
    function.
*/


//***************************************************************************
//  Defines
//***************************************************************************

//***************************************************************************
//  Includes
//***************************************************************************

#include <string.h>

#include "ConverterResult.h"
#include "IFXDebug.h"
#include "IFXCOM.h"

#include "ConverterHelpers.h"
#include "ConverterOptions.h"
#include "FileParser.h"
#include "SceneConverter.h"
#include "SceneUtilities.h"
#include "IFXOSUtilities.h"

using namespace U3D_IDTF;

//***************************************************************************
//  Constants
//***************************************************************************


//***************************************************************************
//  Enumerations
//***************************************************************************


//***************************************************************************
//  Classes, structures and types
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************


//***************************************************************************
//  Local data
//***************************************************************************


//***************************************************************************
//  Local function prototypes
//***************************************************************************
wchar_t* mbs_to_wcs(const char *str);

//***************************************************************************
//  Public methods
//***************************************************************************


//***************************************************************************
//  Protected methods
//***************************************************************************


//***************************************************************************
//  Private methods
//***************************************************************************


//***************************************************************************
//  Global functions
//***************************************************************************

/**
  This is the driver for IDTF converter.

  @param   int argc    The number of arguments in the command line.
  @param   char *argv[]  The arguments themselves, including the command itself.

  @return  int   The last result code.
*/

#ifdef WIN32
int wmain(int argc, wchar_t* argw[])
#else
int main(int argc, char* argv[])
#endif
{		
	IFXRESULT result = IFX_OK;
#ifndef WIN32
	wchar_t **argw = NULL;
	int i;
#endif

	result = IFXSetDefaultLocale();
	IFXTRACE_GENERIC(L"[Converter] IFXSetDefaultLocale %i\n", result);

	if( IFXSUCCESS(result) )
	{
#ifndef WIN32
		argw = (wchar_t**)malloc(argc*sizeof(wchar_t*));
		if (argw == NULL)
			result = IFX_E_OUT_OF_MEMORY;

		if( IFXSUCCESS(result) )
		{
			memset(argw, 0, argc*sizeof(wchar_t*));

			for (i=0; i<argc; i++) 
			{
				argw[i] = mbs_to_wcs(argv[i]);

				if (argw[i] == NULL)
				{
					result = IFX_E_OUT_OF_MEMORY;
					break;
				}
			}
		}
#endif
	}

	if( IFXSUCCESS(result) )
	{
		IFXDEBUG_STARTUP();
		result = IFXCOMInitialize();
	}

	if( IFXSUCCESS(result) )
	{
		ConverterOptions converterOptions;
		FileOptions fileOptions;
		
		if( IFXSUCCESS(result) )
			SetDefaultOptionsX( &converterOptions, &fileOptions );
		
		if( IFXSUCCESS(result) )
		{
			result = ReadAndSetUserOptionsX( 
								argc, argw, 
								&converterOptions, &fileOptions );		
		}
		
		if( IFXSUCCESS(result) )
		{
			SceneUtilities sceneUtils;
			FileParser fileParser;

			result = fileParser.Initialize( fileOptions.inFile.Raw() );

			if( IFXSUCCESS(result) )
				result = sceneUtils.InitializeScene( 
											fileOptions.profile, 
											fileOptions.scalingFactor );

			if( IFXSUCCESS(result) )
			{
				SceneConverter converter( 
									&fileParser, &sceneUtils, 
									&converterOptions );
				result = converter.Convert();
			}

			//----------------------------------------------
			// Scene now built and in the U3D engine.
			// It is now time to examine the scene and/or
			// dump it to a debug file or a U3D file.
			//----------------------------------------------

			// Write out the scene to a U3D file if this is enabled.
			if ( IFXSUCCESS( result ) && ( fileOptions.exportOptions > 0 ) )
			{
				result = sceneUtils.WriteSceneToFile( 
											fileOptions.outFile,
											fileOptions.exportOptions );
			}

			// If enabled, dump the scene to the debug file.
			if ( IFXSUCCESS( result ) && ( fileOptions.debugLevel > 0 ) )
			{
				U8 file[MAXIMUM_FILENAME_LENGTH];
				result = fileOptions.outFile.ConvertToRawU8( file, MAXIMUM_FILENAME_LENGTH );

				if ( IFXSUCCESS( result ) )
					result = sceneUtils.WriteDebugInfo( (const char*)file );
			}
		}
	}

	printf("Exit code = %x\n", result);

	IFXRESULT comResult = IFXCOMUninitialize();
	IFXTRACE_GENERIC( L"[Converter] IFXCOMUninitialize %i\n", comResult );
	
	IFXDEBUG_SHUTDOWN();

#ifndef WIN32
	if (argw != NULL)
	{
		for (i=0; i<argc; i++) 
		{
			if(argw[i] != NULL) free(argw[i]);
		}
		free(argw);
	}
#endif

	return result;
}

//***************************************************************************
//  Local functions
//***************************************************************************

wchar_t* mbs_to_wcs(const char *str)
{
	size_t size = strlen(str) + 1;
	wchar_t *res = NULL;
	
	res = (wchar_t*)malloc(size*sizeof(wchar_t));

	if (res != NULL)
	{
		size = mbstowcs(res, str, size);
	
		if (size == (size_t)-1)
		{
			free(res);
			res = NULL;
		}
	}

	return res;
}
