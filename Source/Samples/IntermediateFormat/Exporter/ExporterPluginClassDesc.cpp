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
	@file	ExporterPluginClassDesc.cpp

			This module contains the class descriptor for the IDTF Exporter.

	@note	As a starting point, portions of this module were based on the 
			Discreet* 3ds max* SDK sample exporters and may contain 
			materials owned by Autodesk*, Inc.  Copyright 1994 - 2004.

			* Other names and brands may be claimed as the property of 
			  others.
*/

//------------- INCLUDES ----------------------------------------------------

#include "Common.h"
#include "max.h"
#include "ExporterPlugin.h" // for ExporterPlugin
#include "resource.h" // for IDS_CLASSNAME

//------------- PUBLIC METHODS ----------------------------------------------

/**
*  Class descriptor for exporter dll.
*/
class ExporterPluginClassDesc : public ClassDesc
{
public:
	virtual int      IsPublic()
	{ return TRUE; }

	virtual void*    Create( BOOL loading )
	{ return new IFXExporterPlugin; }

	virtual const TCHAR* ClassName()
	{ return GetString( IDS_CLASSNAME ); }

	virtual SClass_ID  SuperClassID()
	{ return SCENE_EXPORT_CLASS_ID; }

	virtual Class_ID   ClassID()
	{ return IDTFEXP_CLASSID; }

	virtual const TCHAR* Category()
	{ return _T(""); }

	// Deletes exporter plugin
	void     DeleteThis()
	{ delete this; }
};

//------------- GLOBAL FUNCTIONS ---------------------------------------------

static ExporterPluginClassDesc theExporterClassDesc;

ClassDesc* getSceneExportDesc()
{
	return &theExporterClassDesc;
}


