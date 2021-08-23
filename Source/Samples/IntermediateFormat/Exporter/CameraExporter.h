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
#ifndef __CAMERAEXPORTER_H__
#define __CAMERAEXPORTER_H__

//------------- INCLUDES ----------------------------------------------------

#include "ObjectExporter.h"

//------------- DEFINES -----------------------------------------------------

//------------- CONSTANTS ---------------------------------------------------

//------------- ENUMERATIONS ------------------------------------------------

//------------- CLASS, STRUCTURE AND TYPES ----------------------------------
/**
*  CameraExporter is used to export CameraObjects 
*/
class IFXCameraExporter : public IFXObjectExporter
{
public:
	virtual ~IFXCameraExporter();
	IFXCameraExporter( const IFXNodeExporter* );

	IFXRESULT IFXAPI exportNode();
	IFXRESULT IFXAPI exportResource( const IFXNodeData* pData );
	virtual const IFXCHAR* getNodeType() const { return IDTF_VIEW; }

private:
	IFXCameraExporter( const IFXCameraExporter& ) {};
	IFXCameraExporter& operator= ( const IFXCameraExporter& ) {};
	IFXCameraExporter() {};

	CameraObject* m_pCamera;
}; 


//------------- GLOBAL FUNCTION PROTOTYPES ----------------------------------

//------------- GLOBAL DATA -------------------------------------------------

#endif