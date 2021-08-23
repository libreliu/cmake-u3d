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
	@file	IFXNewBaseMeshPlugin.cpp

			This module contains the plug-in specific functions required to
			be exported from the DL by the IFXCOM component system. In
			addition, it also provides common cross-platform startup and
			shutdown functionality for the DL.
*/


//***************************************************************************
//	Includes
//***************************************************************************


#include "IFXPlugin.h"
#include "IFXNewBaseMeshPluginCIDs.h"
#include "IFXMemory.h"
#include "IFXOSUtilities.h"


//***************************************************************************
//	Defines
//***************************************************************************


//***************************************************************************
//	Constants
//***************************************************************************


//***************************************************************************
//	Enumerations
//***************************************************************************


//***************************************************************************
//	Classes, structures and types
//***************************************************************************


//***************************************************************************
//	Global function prototypes
//***************************************************************************


extern IFXRESULT IFXAPI_CALLTYPE CIFXNewBaseMesh_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXNewBaseMeshDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXNewBaseMeshEncoder_Factory( IFXREFIID interfaceId, void** ppInterface );


//***************************************************************************
//	Global data
//***************************************************************************


/**
	Count of active objects.
	This counter is used by IFXPluginCanUnloadNow and defined in the 
	IFXCorePluginStatic.cpp.
	If this counter equals 0 it means there is no active plug-in objects
	and plug-in can be successfully unloaded.
*/
extern U32 g_countActiveObjects;


//***************************************************************************
//	Local data
//***************************************************************************


/**
	List of ComponentDescriptor structures for each IFXCOM component
	exposed by the plug-in module.

	@note	Look at IFXPlugin.h for IFXComponentDescriptor declaration.
*/
static IFXComponentDescriptor gs_componentDescriptorList[] =
{
	{
		&CID_IFXNewBaseMesh,
		{CIFXNewBaseMesh_Factory},
		1
	},
	{
		&CID_IFXNewBaseMeshDecoder,
		{CIFXNewBaseMeshDecoder_Factory},
		1
	},
	{
		&CID_IFXNewBaseMeshEncoder,
		{CIFXNewBaseMeshEncoder_Factory},
		1
	}
};


//***************************************************************************
//	Global functions
//***************************************************************************


//---------------------------------------------------------------------------
/**
	This function should be invoked by dynamic library initialization
	function.

	@note	For Windows this is the DLL_PROCESS_ATTACH section of DllMain.
 */
IFXRESULT IFXAPI_CALLTYPE IFXNewBaseMeshPluginStartup()
{
	IFXRESULT result = IFX_OK;

	IFXOSInitialize();

	// Initialize persistent global data.

	// TODO: put your code for initialization here

	return result;
}

//---------------------------------------------------------------------------
/**
	This function should be invoked by dynamic library uninitialization
	function.

	@note	For Windows this is the DLL_PROCESS_DETACH section of DllMain.
 */
IFXRESULT IFXAPI_CALLTYPE IFXNewBaseMeshPluginShutdown()
{
	IFXRESULT result = IFX_OK;

	// Dispose of persistent global data

	// TODO: put your code for uninitialization here

	IFXOSUninitialize();

	return result;
}


//***************************************************************************
// Exported functions
//***************************************************************************


//---------------------------------------------------------------------------
/**
	This function provides registration information about plug-in components
	to IFXCOM component system.

	@param	pComponentNumber	Pointer to the number of components in
								component descriptor list.

	@param	ppComponentDescriptorList
								Pointer to the component descriptor list
								array (or pointer to the first element pointer
								of component descriptor list).
	@return	Upon success, it returns the value IFX_OK. If input pointers
	were not defined, it will return the value IFX_E_PARAMETER_NOT_INITIALIZED.
*/
extern "C"
IFXRESULT IFXAPI IFXPluginRegister(
					U32* pComponentNumber,
					IFXComponentDescriptor** ppComponentDescriptorList )
{
	IFXRESULT result = IFX_OK;

	if( 0 != pComponentNumber && 0 != ppComponentDescriptorList )
	{
		*pComponentNumber =
			sizeof(gs_componentDescriptorList)/sizeof(IFXComponentDescriptor);
		*ppComponentDescriptorList = gs_componentDescriptorList;
	}
	else
		result = IFX_E_PARAMETER_NOT_INITIALIZED;

	return result;
}

//---------------------------------------------------------------------------
/**
	This function is used to let component system know if plug-in can be
	unloaded now or not.

	@return	If plug-in can be unloaded, it will return the value IFX_OK.
	Otherwise, it will return the value IFX_E_NOT_DONE.
*/
extern "C"
IFXRESULT IFXAPI IFXPluginCanUnloadNow()
{
	IFXRESULT result = IFX_OK;

	if( 0 != g_countActiveObjects )
		result = IFX_E_NOT_DONE;

	return result;
}
