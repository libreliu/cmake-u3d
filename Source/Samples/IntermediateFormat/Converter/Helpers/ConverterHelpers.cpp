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
  @file ConverterHelpers.cpp

      This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <wchar.h>

#include "IFXResult.h"
#include "IFXDebug.h"
#include "IFXCheckX.h"
#include "ConverterOptions.h"
#include "IFXOSLoader.h"

namespace U3D_IDTF
{

//***************************************************************************
//  Defines
//***************************************************************************


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

void DumpHelpInfo( wchar_t* argv[] );
void ParseParameterFileX( const wchar_t* fileName, 
						  ConverterOptions* pConverterOptions,
						  FileOptions* pFileOptions );

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

extern "C"
void SetDefaultOptionsX( 
			ConverterOptions* pConverterOptions, 
			FileOptions* pFileOptions )
{
	IFXCHECKX( pFileOptions->outFile.Assign( L"Debugout.u3d" ) );
	pFileOptions->exportOptions	= IFXExportOptions(65535);
	pFileOptions->profile		= 0;
	pFileOptions->scalingFactor = 1.0f;
	pFileOptions->debugLevel	= 0;

	pConverterOptions->positionQuality	= 300;
	pConverterOptions->texCoordQuality	= 300;
	pConverterOptions->normalQuality	= 300;
	pConverterOptions->diffuseQuality	= 300;
	pConverterOptions->specularQuality	= 300;
	pConverterOptions->geoQuality		= 300;
	pConverterOptions->textureQuality	= 75;
	pConverterOptions->animQuality		= 1000;
	pConverterOptions->textureLimit		= 0;
	pConverterOptions->removeZeroAreaFaces = TRUE;
	pConverterOptions->zeroAreaFaceTolerance = 100.0f * FLT_EPSILON;
	pConverterOptions->excludeNormals = FALSE;
}

extern "C"
IFXRESULT ReadAndSetUserOptionsX( int argc, wchar_t* argv[], 
								  ConverterOptions* pConverterOptions,
								  FileOptions* pFileOptions )
{
	int	argCount = 1;

	if( 1 == argc )
	{
		DumpHelpInfo( argv );
		return IFX_E_UNDEFINED;
	}

	//----------------------------------------------
	// Now parse the rest of the input arguments
	//----------------------------------------------
	while( ( argCount < argc ) && ( NULL != argv[argCount] ) )
	{
		int	value = 0;

		// Set the output file name
		if( 0 == wcscmp(L"-output", argv[argCount] ) || 
			0 == wcscmp(L"-o", argv[argCount] ) )
		{
			argCount++;
			IFXCHECKX( pFileOptions->outFile.Assign( (IFXCHAR*)argv[argCount] ) );
			argCount++;
			continue;
		}

		// Get the debug level
		if( 0 == wcscmp(L"-debuglevel", argv[argCount] ) ||
			0 == wcscmp(L"-dl", argv[argCount] ) )
		{
			argCount++;

			swscanf( argv[argCount], L"%d", &value );
			if( value <= 0)
				pFileOptions->debugLevel = 0;
			else if( value >= 1)
				pFileOptions->debugLevel = 1;
			argCount++;
			continue;
		}

		// Get the profile identifier
		if( 0 == wcscmp(L"-profile", argv[argCount] )  ||
			0 == wcscmp(L"-p", argv[argCount] ) )
		{
			argCount++;
			swscanf( argv[argCount], L"%d", &value );
			if( value <= 0)
				pFileOptions->profile = 0;
			else if( value >= 14)
				pFileOptions->profile = 14;
			else
				pFileOptions->profile = (U32)value;
			argCount++;
			continue;
		}

		// Get the scaling factor
		if( 0 == wcscmp(L"-scalingfactor", argv[argCount] ) ||
			0 == wcscmp(L"-sf", argv[argCount] ) )
		{
			argCount++;
			F32 scalingFactor;
			swscanf( argv[argCount], L"%f", &scalingFactor );
			if( scalingFactor == 0.0f )
				pFileOptions->scalingFactor = 1.0f;
			else if( scalingFactor < 0.0f )
				pFileOptions->scalingFactor = -scalingFactor;
			else
				pFileOptions->scalingFactor = scalingFactor;
			argCount++;
			continue;
		}

		// Get the position quality
		if( 0 == wcscmp(L"-pquality", argv[argCount] ) ||
			0 == wcscmp(L"-pq", argv[argCount] ) )
		{
			argCount++;
			swscanf( argv[argCount], L"%d", &value );
			if( value < 0)
				pConverterOptions->positionQuality = 0;
			else if( value > 1000)
				pConverterOptions->positionQuality = 1000;
			else
				pConverterOptions->positionQuality = (U32)value;
			argCount++;
			continue;
		}

		// Get the texture coordinate quality
		if( 0 == wcscmp(L"-tcquality", argv[argCount] ) ||
			0 == wcscmp(L"-tcq", argv[argCount] ) )
		{
			argCount++;
			swscanf( argv[argCount], L"%d", &value );
			if( value < 0)
				pConverterOptions->texCoordQuality = 0;
			else if( value > 1000)
				pConverterOptions->texCoordQuality = 1000;
			else
				pConverterOptions->texCoordQuality = (U32)value;
			argCount++;
			continue;
		}

		// Get the normal quality
		if( 0 == wcscmp(L"-nquality", argv[argCount] ) ||
			0 == wcscmp(L"-nq", argv[argCount] ) )
		{
			argCount++;
			swscanf( argv[argCount], L"%d", &value );
			if( value < 0)
				pConverterOptions->normalQuality = 0;
			else if( value > 1000)
				pConverterOptions->normalQuality = 1000;
			else
				pConverterOptions->normalQuality = (U32)value;
			argCount++;
			continue;
		}

		// Get the diffuse color quality
		if( 0 == wcscmp(L"-dcuality", argv[argCount] ) ||
			0 == wcscmp(L"-dcq", argv[argCount] ) )
		{
			argCount++;
			swscanf( argv[argCount], L"%d", &value );
			if( value < 0)
				pConverterOptions->diffuseQuality = 0;
			else if( value > 1000)
				pConverterOptions->diffuseQuality = 1000;
			else
				pConverterOptions->diffuseQuality = (U32)value;
			argCount++;
			continue;
		}

		// Get the specular color quality
		if( 0 == wcscmp(L"-scquality", argv[argCount] ) ||
			0 == wcscmp(L"-scq", argv[argCount] ) )
		{
			argCount++;
			swscanf( argv[argCount], L"%d", &value );
			if( value < 0)
				pConverterOptions->specularQuality = 0;
			else if( value > 1000)
				pConverterOptions->specularQuality = 1000;
			else
				pConverterOptions->specularQuality = (U32)value;
			argCount++;
			continue;
		}

		// Get the geometry quality
		if( 0 == wcscmp(L"-gquality", argv[argCount] ) ||
			0 == wcscmp(L"-gq", argv[argCount] ) )
		{
			argCount++;
			swscanf( argv[argCount], L"%d", &value );
			if( value < 0)
				pConverterOptions->geoQuality = 0;
			else if( value > 1000)
				pConverterOptions->geoQuality = 1000;
			else
				pConverterOptions->geoQuality = (U32)value;
			argCount++;
			continue;
		}

		// Get the texture quality
		if( 0 == wcscmp(L"-tquality", argv[argCount] ) ||
			0 == wcscmp(L"-tq", argv[argCount] ) )
		{
			argCount++;
			swscanf( argv[argCount], L"%d", &value );
			if( value < 0)
				pConverterOptions->textureQuality = 0;
			else if( value > 100)
				pConverterOptions->textureQuality = 100;
			else
				pConverterOptions->textureQuality = (U32)value;
			argCount++;
			continue;
		}

		// Get the animation quality
		if( 0 == wcscmp(L"-aquality", argv[argCount] ) ||
			0 == wcscmp(L"-aq", argv[argCount] ) )
		{
			argCount++;
			swscanf( argv[argCount], L"%d", &value );
			if( value < 0)
				pConverterOptions->animQuality = 0;
			else if( value > 1000)
				pConverterOptions->animQuality = 1000;
			else
				pConverterOptions->animQuality = (U32)value;
			argCount++;
			continue;
		}

		// Get the zero area faces removal mode
		if( 0 == wcscmp(L"-removezerofaces", argv[argCount] ) ||
			0 == wcscmp(L"-rzf", argv[argCount] ) )
		{
			argCount++;
			swscanf( argv[argCount], L"%d", &value );
			if( value <= 0 )
				pConverterOptions->removeZeroAreaFaces = FALSE;
			else
				pConverterOptions->removeZeroAreaFaces = TRUE;
			argCount++;
			continue;
		}

		// Get the zero area face tolerance
		if( 0 == wcscmp(L"-zerofacetolerance", argv[argCount] ) ||
			0 == wcscmp(L"-zft", argv[argCount] ) )
		{
			argCount++;
			F32 tolerance;
			swscanf( argv[argCount], L"%f", &tolerance );
			if( tolerance < 0 )
				pConverterOptions->zeroAreaFaceTolerance = -tolerance;
			else
				pConverterOptions->zeroAreaFaceTolerance = tolerance;
			argCount++;
			continue;
		}

		// Get the normals exclusion mode
		if( 0 == wcscmp(L"-excludenormals", argv[argCount] ) ||
			0 == wcscmp(L"-en", argv[argCount] ) )
		{
			argCount++;
			swscanf( argv[argCount], L"%d", &value );
			if( value <= 0 )
				pConverterOptions->excludeNormals = FALSE;
			else
				pConverterOptions->excludeNormals = TRUE;
			argCount++;
			continue;
		}

		// Get the export options
		if( 0 == wcscmp(L"-exportoptions", argv[argCount] ) ||
			0 == wcscmp(L"-eo", argv[argCount] ) )
		{
			int rawOption = 0;
			argCount++;
			swscanf( argv[argCount], L"%d", &rawOption );
			if( rawOption < 0)
				rawOption = 0;
			if( rawOption > 65535)
				rawOption = 65535;
			pFileOptions->exportOptions = IFXExportOptions( 
				(rawOption & IFXEXPORT_GEOMETRY) | 
				(rawOption & IFXEXPORT_MATERIALS) |
				(rawOption & IFXEXPORT_TEXTURES) | 
				(rawOption & IFXEXPORT_LIGHTS) |
				(rawOption & IFXEXPORT_ANIMATION) | 
				(rawOption & IFXEXPORT_VIEWS) | 
				(rawOption & IFXEXPORT_FILEREFERENCES) | 
				(rawOption & IFXEXPORT_NODE_HIERARCHY) |
				(rawOption & IFXEXPORT_SHADERS) );
			argCount++;
			continue;
		}

		// Get the texture size limit
		if( 0 == wcscmp(L"-texturelimit", argv[argCount] ) ||
			0 == wcscmp(L"-tl", argv[argCount] ) )
		{
			argCount++;
			swscanf( argv[argCount], L"%d", &value );
			if( value < 0)
				pConverterOptions->textureLimit = 0;
			else if( value > 4096)
				pConverterOptions->textureLimit = 4096;
			else
				pConverterOptions->textureLimit = (U32)value;
			argCount++;
			continue;
		}

		// Read in an input file (multiple "-input" arguments, or none, are legal).
		if( 0 == wcscmp(L"-input", argv[argCount] ) ||
			0 == wcscmp(L"-i", argv[argCount] ) )
		{
			argCount++;
			IFXCHECKX( pFileOptions->inFile.Assign( (IFXCHAR*)argv[argCount] ) );
			argCount++;
			continue;
		}

		// Read in an input file (multiple "-input" arguments, or none, are legal).
		if( 0 == wcscmp(L"-pfile", argv[argCount] ) ||
			0 == wcscmp(L"-pf", argv[argCount] ) )
		{
			argCount++;
			ParseParameterFileX( argv[argCount], pConverterOptions, pFileOptions );
			argCount++;
			continue;
		}

		// Skip keywords that we do not recognize.
		wprintf( L"Did not recognize keyword = %ls\n", argv[argCount] );
		argCount++;
	}

	const IFXCHAR* inFile = pFileOptions->inFile.Raw();
	const IFXCHAR* outFile = pFileOptions->outFile.Raw();
	
	// Note our settings.
	printf("\n");
	wprintf(L"Input file name = %ls\n", inFile );
	wprintf(L"Output file name = %ls\n", outFile );
	
	printf("Profile = %d\n", pFileOptions->profile);
	printf("Scaling factor = %f\n", pFileOptions->scalingFactor);
	printf("Debug level = %d\n", pFileOptions->debugLevel);
	printf("Position Quality = %d\n", pConverterOptions->positionQuality);
	printf("Texture Coordinate Quality = %d\n", pConverterOptions->texCoordQuality);
	printf("Normal Quality = %d\n", pConverterOptions->normalQuality);
	printf("Diffuse Color Quality = %d\n", pConverterOptions->diffuseQuality);
	printf("Specular Color  Quality = %d\n", pConverterOptions->specularQuality);
	printf("Geometry Default Quality = %d\n", pConverterOptions->geoQuality);
	printf("Texture Quality = %d\n", pConverterOptions->textureQuality);
	printf("Animation Quality = %d\n", pConverterOptions->animQuality);
	if( TRUE == pConverterOptions->removeZeroAreaFaces )
	{
		printf("Zero Area Faces Removal = ENABLED\n");
		printf("Zero Area Face Tolerance = %f\n", 
			pConverterOptions->zeroAreaFaceTolerance);
	}
	else
		printf("Zero Area Faces Removal = DISABLED\n");
	printf("Exclude Normals = %s\n", 
		( pConverterOptions->excludeNormals == TRUE ? "TRUE" : "FALSE" ) );
	printf("Export Option Flags = %x\n", pFileOptions->exportOptions );
	printf("Texture size limit = %d\n", pConverterOptions->textureLimit);
	printf("\n");

	return IFX_OK;
}


//***************************************************************************
//  Local functions
//***************************************************************************

void DumpHelpInfo( wchar_t* argv[] )
{
	printf("\n\n");
	wprintf(L"%ls <arguments> <input_file_list>\n", argv[0]);
	printf("Note: argument order is important - what happens depends on\n");
	printf("      what arguments were already parsed\n");
	printf("\n");
	printf("Debugging:\n");
	printf("    -debuglevel <number>\n");
	printf("        0 - no debug dump - silent conversion (default)\n");
	printf("        1 - dump debug information to the file\n");
	printf("\n");
	printf("Export Options:\n");
	printf("    -profile or -p <number>: profile identifier\n");
	printf("    -scalingfactor or -sf <number>: units scaling factor\n");
	printf("    -pquality or -pq <number 0 to 1000>: mesh's position quality\n");
	printf("    -tcquality or -tcq <number 0 to 1000>: mesh's texture coordinate quality\n");
	printf("    -nquality or -nq <number 0 to 1000>: mesh's normal quality\n");
	printf("    -dcquality or -dcq <number 0 to 1000>: mesh's diffuse color quality\n");
	printf("    -scquality or -scq <number 0 to 1000>: mesh's specular color quality\n");
	printf("    -gquality or -gq <number 0 to 1000>: geometry default quality\n");
	printf("    -tquality or -tq <number 0 to 100>: texture quality\n");
	printf("    -aquality or -aq <number 0 to 1000>: animation quality\n");
	printf("    -removezerofaces or -rzf <number 0 or 1>: disable or enable zero area faces removal\n");
	printf("    -zerofacetolerance or -zft <positive float number>: zero area face tolerance\n");
	printf("    -excludenormals or -en <number 0 or 1>: disable or enable normals exclusion\n");
	printf("    -exportoptions or -eo <number>\n");
	printf("        0     - do not export scene\n");
	printf("        1     - export animation\n");
	printf("        2     - export geometry\n");
	printf("        4     - export lights\n");
	printf("        8     - export materials\n");
	printf("        16    - export node hierarchy\n");
	printf("        32    - export shaders\n");
	printf("        64    - export textures\n");
	printf("        65535 - export everything (default)\n");
	printf("    -texturelimit or -tl <number>: limit textures to <number> by <number> in size (0 = None), up to 4096\n");
	printf("\n");
	printf("I/O:\n");
	printf("    -input <filename>\n");
	printf("    -output <filename>\n");
	printf("    -pfile <filename> - Read user options from a parameter file. Overrides command line params\n");
	printf("\n");
}


void ParseParameterFileX( const wchar_t* fileName, 
						  ConverterOptions* pConverterOptions,
						  FileOptions* pFileOptions )
{
	FILE* pFile = NULL;
	IFXCHAR buffer[1024];

	if( !pConverterOptions || !fileName )
	{
		printf("Bad pointers passed to parser\n");
		IFXCHECKX( IFX_E_INVALID_POINTER );
	}

	pFile = IFXOSFileOpen( fileName, L"r" );

	if( !pFile )
	{
		wprintf(L"Unable to open file %ls\n", fileName );
		IFXCHECKX( IFX_E_INVALID_FILE );
	}

	while( fgets( (char*)&buffer, 1024, pFile ) )
	{
		IFXCHAR* pCommand = NULL;
		IFXCHAR* pArgument = NULL;
		IFXCHAR* pQuote = NULL;
		int	  value = 0;

		pQuote = wcschr(buffer, L'\"');

#ifdef WIN32		
		pCommand = wcstok(buffer, L" ");
		
		if( pQuote )
			pArgument = wcstok(NULL, L"\"");
		else
			pArgument = wcstok(NULL, L"\n");
#else
		IFXCHAR* state;
		pCommand = wcstok(buffer, L" ", &state);

		if( pQuote )
			pArgument = wcstok(NULL, L"\"", &state);
		else
			pArgument = wcstok(NULL, L"\n", &state);
#endif

		// Set the output file name
		if( 0 == wcscmp(L"-output", pCommand ) ||
			0 == wcscmp(L"-o", pCommand ) )
		{
			IFXCHECKX( pFileOptions->outFile.Assign( pArgument ) );
			continue;
		}

		// Get the debug level
		if( 0 == wcscmp(L"-debuglevel", pCommand ) || 
			0 == wcscmp(L"-dl", pCommand ) )
		{
			swscanf( pArgument, L"%d", &value );
			if( value <= 0)
				pFileOptions->debugLevel = 0;
			else if( value >= 1)
				pFileOptions->debugLevel = 1;
			continue;
		}

		// Get the profile identifier
		if( 0 == wcscmp(L"-profile", pCommand )  || 
			0 == wcscmp(L"-p", pCommand ) )
		{
			swscanf( pArgument, L"%d", &value );
			if( value <= 0)
				pFileOptions->profile = 0;
			else if( value >= 6)
				pFileOptions->profile = 6;
			continue;
		}

		// Get the scaling factor
		if( 0 == wcscmp(L"-scalingfactor", pCommand ) ||
			0 == wcscmp(L"-sf", pCommand ) )
		{
			F32 scalingFactor;
			swscanf( pArgument, L"%f", &scalingFactor );
			if( scalingFactor == 0.0f )
				pFileOptions->scalingFactor = 1.0f;
			else if( scalingFactor < 0.0f )
				pFileOptions->scalingFactor = -scalingFactor;
			else
				pFileOptions->scalingFactor = scalingFactor;
			continue;
		}

		// Get the position quality
		if( 0 == wcscmp(L"-pquality", pCommand ) ||
			0 == wcscmp(L"-pq", pCommand ) )
		{
			swscanf( pArgument, L"%d", &value);
			if( value < 0)
				pConverterOptions->positionQuality = 0;
			else if( value > 1000)
				pConverterOptions->positionQuality = 1000;
			else
				pConverterOptions->positionQuality = (U32)value;
			continue;
		}

		// Get the texture coordinate quality
		if( 0 == wcscmp(L"-tcquality", pCommand ) ||
			0 == wcscmp(L"-tcq", pCommand ) )
		{
			swscanf( pArgument, L"%d", &value);
			if( value < 0)
				pConverterOptions->texCoordQuality = 0;
			else if( value > 1000)
				pConverterOptions->texCoordQuality = 1000;
			else
				pConverterOptions->texCoordQuality = (U32)value;
			continue;
		}

		// Get the normal quality
		if( 0 == wcscmp(L"-nquality", pCommand ) ||
			0 == wcscmp(L"-nq", pCommand ) )
		{
			swscanf( pArgument, L"%d", &value );
			if( value < 0)
				pConverterOptions->normalQuality = 0;
			else if( value > 1000)
				pConverterOptions->normalQuality = 1000;
			else
				pConverterOptions->normalQuality = (U32)value;
			continue;
		}

		// Get the diffuse color quality
		if( 0 == wcscmp(L"-dcuality", pCommand ) ||
			0 == wcscmp(L"-dcq", pCommand ) )
		{
			swscanf( pArgument, L"%d", &value );
			if( value < 0)
				pConverterOptions->diffuseQuality = 0;
			else if( value > 1000)
				pConverterOptions->diffuseQuality = 1000;
			else
				pConverterOptions->diffuseQuality = (U32)value;
			continue;
		}

		// Get the specular color quality
		if( 0 == wcscmp(L"-scquality", pCommand ) ||
			0 == wcscmp(L"-scq", pCommand ) )
		{
			swscanf( pArgument, L"%d", &value );
			if( value < 0)
				pConverterOptions->specularQuality = 0;
			else if( value > 1000)
				pConverterOptions->specularQuality = 1000;
			else
				pConverterOptions->specularQuality = (U32)value;
			continue;
		}

		// Get the geometry quality
		if( 0 == wcscmp(L"-gquality", pCommand ) || 
			0 == wcscmp(L"-gq", pCommand ) )
		{
			swscanf( pArgument, L"%d", &value );
			if( value < 0)
				pConverterOptions->geoQuality = 0;
			else if( value > 1000)
				pConverterOptions->geoQuality = 1000;
			else
				pConverterOptions->geoQuality = (U32)value;
			continue;
		}

		// Get the texture quality
		if( 0 == wcscmp(L"-tquality", pCommand ) ||
			0 == wcscmp(L"-tq", pCommand ) )
		{
			swscanf( pArgument, L"%d", &value );
			if( value < 0)
				pConverterOptions->textureQuality = 0;
			else if( value > 100)
				pConverterOptions->textureQuality = 100;
			else
				pConverterOptions->textureQuality = (U32)value;
			continue;
		}

		// Get the animation quality
		if( 0 == wcscmp(L"-aquality", pCommand ) ||
			0 == wcscmp(L"-aq", pCommand ) )
		{
			swscanf( pArgument, L"%d", &value );
			if( value < 0)
				pConverterOptions->animQuality = 0;
			else if( value > 1000)
				pConverterOptions->animQuality = 1000;
			else
				pConverterOptions->animQuality = (U32)value;
			continue;
		}

		// Get the zero area faces removal
		if( 0 == wcscmp(L"-removezerofaces", pCommand ) ||
			0 == wcscmp(L"-rzf", pCommand ) )
		{
			swscanf( pArgument, L"%d", &value );
			if( value <= 0 )
				pConverterOptions->removeZeroAreaFaces = FALSE;
			else
				pConverterOptions->removeZeroAreaFaces = TRUE;
			continue;
		}

		// Get the zero area face tolerance
		if( 0 == wcscmp(L"-zerofacetolerance", pCommand ) ||
			0 == wcscmp(L"-zft", pCommand ) )
		{
			F32 tolerance = 0.0f;
			swscanf( pArgument, L"%f", &tolerance );

			if( tolerance < 0 )
				pConverterOptions->zeroAreaFaceTolerance = -tolerance;
			else
				pConverterOptions->zeroAreaFaceTolerance = tolerance;
			continue;
		}

		// Get the normals exclusion mode
		if( 0 == wcscmp(L"-excludenormals", pCommand ) ||
			0 == wcscmp(L"-en", pCommand ) )
		{
			swscanf( pArgument, L"%d", &value );
			if( value <= 0 )
				pConverterOptions->excludeNormals = FALSE;
			else
				pConverterOptions->excludeNormals = TRUE;
			continue;
		}

		// Get the export options
		if( 0 == wcscmp(L"-exportoptions", pCommand ) ||
			0 == wcscmp(L"-eo", pCommand ) )
		{
			int  rawOption = 0;
			swscanf( pArgument, L"%d", &rawOption );

			if( rawOption < 0)
				rawOption = 0;
			if( rawOption > 65535)
				rawOption = 65535;
			pFileOptions->exportOptions = IFXExportOptions( 
				(rawOption & IFXEXPORT_GEOMETRY) | 
				(rawOption & IFXEXPORT_MATERIALS) |
				(rawOption & IFXEXPORT_TEXTURES) | 
				(rawOption & IFXEXPORT_LIGHTS) |
				(rawOption & IFXEXPORT_ANIMATION) | 
				(rawOption & IFXEXPORT_NODE_HIERARCHY) |
				(rawOption & IFXEXPORT_SHADERS) );
			continue;
		}

		// Get the texture size limit
		if( 0 == wcscmp(L"-texturelimit", pCommand ) ||
			0 == wcscmp(L"-tl", pCommand ) )
		{
			swscanf( pArgument, L"%d", &value );
			if( value < 0)
				pConverterOptions->textureLimit = 0;
			else if( value > 4096)
				pConverterOptions->textureLimit = 4096;
			else
				pConverterOptions->textureLimit = (U32)value;
			continue;
		}

		// Read in an input file
		if( 0 == wcscmp(L"-input", pCommand ) ||
			0 == wcscmp(L"-i", pCommand ) )
		{
			IFXCHECKX( pFileOptions->inFile.Assign( pArgument ) );
			continue;
		}

		// Skip keywords that we do not recognize.
		wprintf(L"Did not recognize keyword = %ls\n", pCommand);
	}  // While lines in parameter file

	fclose( pFile );
}

}
