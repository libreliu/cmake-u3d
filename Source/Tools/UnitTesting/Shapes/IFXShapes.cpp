//***************************************************************************
//
//  Copyright (c) 2004 - 2006 Intel Corporation
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
  @file		IFXShapes.cpp

			This module contains the plug-in specific functions required to
			be exported from the DL by the IFXCOM component system.  In
			addition, it also provides common cross-platform startup and
			shutdown functionality for the DL.
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXPlugin.h"
#include "IFXShapesCIDs.h"
#include "IFXMemory.h"
#include "IFXOSUtilities.h"
#include "IFXIPP.h"

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
//  Global function prototypes
//***************************************************************************

// Shapes
extern IFXRESULT IFXAPI_CALLTYPE CIFXSphereGenerator_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXBoxGenerator_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXCylinderGenerator_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXPlaneGenerator_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXPrimitiveGenerator_Factory( IFXREFIID interfaceId, void** ppInterface );

extern IFXRESULT IFXAPI_CALLTYPE CIFXCLODModifier2Decoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXCLODModifier2Encoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXCLODModifier2_Factory( IFXREFIID interfaceId, void** ppInterface );

//***************************************************************************
//  Global data
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
//  Local data
//***************************************************************************


/**
  List of ComponentDescriptor structures for each IFXCOM component
  exposed by the plug-in module.

  @note Look at IFXPlugin.h for IFXComponentDescriptor declaration.
*/
static IFXComponentDescriptor gs_componentDescriptorList[] =
{
	{
		&CID_IFXBoxGenerator,
		{CIFXBoxGenerator_Factory},
		1
	},
	{
		&CID_IFXCylinderGenerator,
		{CIFXCylinderGenerator_Factory},
		1
	},
	{
		&CID_IFXPlaneGenerator,
		{CIFXPlaneGenerator_Factory},
		1
	},
	{
		&CID_IFXSphereGenerator,
		{CIFXSphereGenerator_Factory},
		1
	},
	{
		&CID_IFXPrimitiveGenerator,
		{CIFXPrimitiveGenerator_Factory},
		1
	},
	{
		&CID_IFXCLODModifier2Decoder,
		{CIFXCLODModifier2Decoder_Factory},
		1
	},
	{
		&CID_IFXCLODModifier2Encoder,
		{CIFXCLODModifier2Encoder_Factory},
		1
	},
	{
		&CID_IFXCLODModifier2,
		{CIFXCLODModifier2_Factory},
		1
	}
};

static IFXDID gs_DIDsList[] =
{
	DID_IFXShapesTest
};

//***************************************************************************
//  Local function prototypes
//***************************************************************************


//***************************************************************************
//  Global functions
//***************************************************************************


//---------------------------------------------------------------------------
/**
  This function should be invoked by dynamic library initialization
  function.

  @note For Windows this is the DLL_PROCESS_ATTACH section of DllMain.
*/
IFXRESULT IFXAPI_CALLTYPE IFXShapesStartup()
{
	IFXRESULT result = IFX_OK;

	// Initialize persistent global data.

	/// @todo: put your code for initialization here

	return result;
}

//---------------------------------------------------------------------------
/**
  This function should be invoked by dynamic library uninitialization
  function.

  @note For Windows this is the DLL_PROCESS_DETACH section of DllMain.
*/
IFXRESULT IFXAPI_CALLTYPE IFXShapesShutdown()
{
	IFXRESULT result = IFX_OK;

	// Dispose of persistent global data

	/// @todo: put your code for uninitialization here

	return result;
}


//***************************************************************************
//  Local functions
//***************************************************************************


//***************************************************************************
// Exported functions
//***************************************************************************


//---------------------------------------------------------------------------
/**
  This function provides registration information about plug-in components
  to IFXCOM component system.

  @param  pComponentNumber  Pointer to the number of components in
                component descriptor list.

  @param  ppComponentDescriptorList
                Pointer to the component descriptor list
                array (or pointer to the first element pointer
                of component descriptor list).

  @return Upon success, it returns the value IFX_OK. If input pointers
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

/**
	This function is used to register DIDs specific to this plugin.
*/
extern "C"
IFXRESULT IFXAPI IFXPluginRegisterDids(
									   U32 *pDidsCount,
									   IFXDID **ppDidsList )
{
	IFXRESULT result = IFX_OK;

	if( 0 != pDidsCount && 0 != ppDidsList )
	{
		*pDidsCount = 1;
		*ppDidsList = gs_DIDsList;
	}
	else
		result = IFX_E_PARAMETER_NOT_INITIALIZED;

	return result;
}

//---------------------------------------------------------------------------
/**
  This function is used to let component system know if plug-in can be
  unloaded now or not.

  @return If plug-in can be unloaded, it will return the value IFX_OK.
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
