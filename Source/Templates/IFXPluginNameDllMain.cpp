TODO:  Replace PluginName usages with the mixed-case version of your plug-in name.
TODO:  Address other TODOs.
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
	@file	IFXPluginNameDllMain.cpp

			This module defines for IFXPluginName the main DL entry point.
*/


//***************************************************************************
//	Includes
//***************************************************************************

// TODO: use appropriate code for DL entry point.
#ifdef WIN32

#include <windows.h>

#elif LINUX

#ifndef STRICT
#define STRICT  1
#endif

#include "IFXDebug.h"

#else

// TODO: add includes for other platforms.

#endif

#include "IFXAPI.h"
#include "IFXResult.h"


//***************************************************************************
//  Global function prototypes (not exposed to the client)
//***************************************************************************


extern IFXRESULT IFXAPI_CALLTYPE IFXPluginNameStartup();
extern IFXRESULT IFXAPI_CALLTYPE IFXPluginNameShutdown();


//***************************************************************************
//	Global functions
//***************************************************************************

// TODO: use appropriate code for DL entry point.
#ifdef WIN32
/**
	This is the main DLL entry point.  It performs DLL startup and shutdown
	activities.

	@return	Upon success, TRUE is returned.  Otherwise, FALSE is returned.  
			The Win32 OS only acts upon the return value if the reason is 
			DLL_PROCESS_ATTACH.
*/
extern "C"
BOOL WINAPI DllMain(	HINSTANCE	hInstance,
                        DWORD		reason,
                        LPVOID		pReserved )
{
	IFXRESULT	result	= IFX_OK;

    if ( reason == DLL_PROCESS_ATTACH )
    {
		DisableThreadLibraryCalls( hInstance );

		result = IFXPluginNameStartup();
    }
    else if ( reason == DLL_PROCESS_DETACH )
    {
		result = IFXPluginNameShutdown();
    }

	return ( IFXSUCCESS( result ) ? TRUE : FALSE );
}

#elif LINUX

/**
	This is the main DLL entry point.  It performs DL startup and shutdown
	activities.
*/
void __attribute((constructor)) PluginStartUp( void )
{
    IFXRESULT result = IFXExportingStartup();
	
	if( IFXFAILURE( result ) )
		IFXTRACE_GENERIC(L"IFXExporting failed to start up successfully\n");
}

void __attribute((destructor)) PluginShutDown( void )
{
    IFXRESULT result = IFXExportingShutdown();
	
	if( IFXFAILURE( result ) )
		IFXTRACE_GENERIC(L"IFXExporting failed to shut down successfully\n");
}

#else

// TODO: add code for other platforms.

#endif
