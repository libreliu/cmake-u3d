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
#ifndef __IFXLIGHTEXPORTER_H__
#define __IFXLIGHTEXPORTER_H__

//------------- INCLUDES ----------------------------------------------------

#include "ObjectExporter.h"

//------------- DEFINES -----------------------------------------------------

//------------- CONSTANTS ---------------------------------------------------

//------------- ENUMERATIONS ------------------------------------------------

//------------- CLASS, STRUCTURE AND TYPES ----------------------------------

/**
*  IFXLightExporter class is used to export Lights
*/
class IFXLightExporter : public IFXObjectExporter
{
public:
	virtual ~IFXLightExporter();
	IFXLightExporter( const IFXNodeExporter* );

	IFXRESULT IFXAPI exportNode();
	IFXRESULT IFXAPI exportResource( const IFXNodeData* pData );
	virtual const IFXCHAR* getNodeType() const { return IDTF_LIGHT; }

private:
	IFXLightExporter() {};
	IFXLightExporter( const IFXLightExporter& ) {};
	IFXLightExporter& operator= ( const IFXLightExporter& ) {};

	GenLight* m_pLight;
}; 


//------------- GLOBAL FUNCTION PROTOTYPES ----------------------------------

//------------- GLOBAL DATA -------------------------------------------------

#endif