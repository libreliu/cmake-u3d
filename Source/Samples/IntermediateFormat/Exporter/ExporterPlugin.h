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
	@file	ExporterPlugin.h

			This module contains the IFXExporterPlugin class.

	@note	As a starting point, portions of this module were based on the 
			Discreet* 3ds max* SDK sample exporters and may contain 
			materials owned by Autodesk*, Inc.  Copyright 1994 - 2004.

			* Other names and brands may be claimed as the property of 
			  others.
*/

#ifndef __IFXEXPORTERPLUGIN_H__
#define __IFXEXPORTERPLUGIN_H__

#include "IFXAPI.h"

//------------- DEFINES -----------------------------------------------------
// The ClassID
#define IDTFEXP_CLASSID  Class_ID(0x79d144cc, 0x46a0500a)

//------------- CLASS, STRUCTURE AND TYPES ----------------------------------

/**
This class provides a predefined interface between 3DSMAX
and the IDTF exporter. It is derived from a common base
class called SceneExport.
*/
class IFXExporterPlugin : public SceneExport
{
public:
	// construction/destruction
	virtual ~IFXExporterPlugin() {};
	IFXExporterPlugin() {};

	// methods for supporting file extensions
	virtual int ExtCount();
	virtual const TCHAR * Ext( int n );

	// description methods
	virtual const TCHAR * LongDesc();
	virtual const TCHAR * ShortDesc();
	virtual const TCHAR * AuthorName();
	virtual const TCHAR * CopyrightMessage();
	virtual const TCHAR * OtherMessage1();
	virtual const TCHAR * OtherMessage2();

	// Version number of this exporter
	virtual unsigned int Version();

	// Show an about box
	virtual void ShowAbout( HWND hWnd );

	// Do the actual export
	virtual int DoExport( const TCHAR *pName, ExpInterface *pEi, Interface *pI,
		BOOL suppressPrompts = FALSE, DWORD options = 0 );

	// Returns whether we support the extended exporter options
	virtual BOOL SupportsOptions( int ext, DWORD options );
};

//------------- GLOBAL FUNCTION PROTOTYPES ----------------------------------

extern TCHAR *GetString( int id );
//extern ClassDesc* GetSceneExportDesc();

//------------- GLOBAL DATA -------------------------------------------------

extern HINSTANCE g_hInstance;

#endif


