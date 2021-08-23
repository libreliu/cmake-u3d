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
	@file	DllEntry.h

			This module contains DllMain and the required functions that 
			must be exported from the IDTF Exporter DLL.

	@note	Portions of this module are based on the Discreet* 3ds max* SDK 
			sample exporters and may contain materials owned 
			by Autodesk*, Inc.  Copyright 1994 - 2004.

			* Other names and brands may be claimed as the property of 
			  others.
*/

//------------- INCLUDES ----------------------------------------------------

#include "Common.h"
#include <windows.h>
#include "max.h"

#include "ExporterPlugin.h"
#include "IFXDataTypes.h"
#include "resource.h"

//------------- DEFINES -----------------------------------------------------

#ifdef EXPORTER_EXPORTS
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllimport)
#endif

extern ClassDesc* getSceneExportDesc();

/*
* GLOBALS
*/
HINSTANCE g_hInstance;
int       g_controlsInit = FALSE;

//------------- GLOBAL FUNCTIONS --------------------------------------------

/*
* main dll function
*/
BOOL WINAPI DllMain( HANDLE hModule,
					ULONG  ul_reason_for_call,
					LPVOID lpvReserved
					)
{
	// cache the DLL instance handle
	g_hInstance = (HINSTANCE)hModule;

	if( !g_controlsInit )
	{
		g_controlsInit = TRUE;
		InitCustomControls( g_hInstance ); // init MAX custom controls
		InitCommonControls();              // init Win custom controls
	}

	switch( ul_reason_for_call )
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}


/*
* Utility function to get string from DLL string table
* (not exported)
*/
TCHAR *GetString(int id)
{
	static TCHAR buf[256];

	if( g_hInstance )
		return LoadString( g_hInstance, id, buf, sizeof(buf) ) ? buf : NULL;

	return NULL;
}


/*
* This function returns a string that describes the DLL and where the user
* could purchase the DLL if they don't have it.
*/
DLLEXPORT const TCHAR * LibDescription()
{
	return GetString(IDS_LIBDESC);
}


/**
* This function returns the number of plug-in classes this DLL
* @todo: Must change this number when adding a new class
*/
DLLEXPORT int LibNumberClasses()
{
	return 1;
}


/*
* For each plug-in class we claim this DLL provides
* (from LibNumberClasses above) we must have a ClassDesc.
*/
DLLEXPORT ClassDesc* LibClassDesc(int i)
{
	switch (i)
	{
	case 0: return getSceneExportDesc();
	default: return 0;
	}
}


/*
* This function returns a pre-defined constant indicating the version of
* the system under which it was compiled.  It is used to allow the system
* to catch obsolete DLLs.
*/
DLLEXPORT ULONG LibVersion()
{
	return VERSION_3DSMAX;
}


