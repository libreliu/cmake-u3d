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
#ifndef __OBJECTEXPORTER_H__
#define __OBJECTEXPORTER_H__

//------------- INCLUDES ----------------------------------------------------

#include "max.h" // INode
#include "IFXCOM.h" // error code
#include "Writer.h"
#include "Common.h"
#include "Tokens.h"
#include "NodeExporter.h"

//------------- DEFINES -----------------------------------------------------

//------------- CONSTANTS ---------------------------------------------------

//------------- ENUMERATIONS ------------------------------------------------

//------------- CLASS, STRUCTURE AND TYPES ----------------------------------

/**
*  ObjectExporter class is used to export Objects
*/
class IFXObjectExporter
{
public:
	virtual ~IFXObjectExporter();
	IFXObjectExporter( const IFXNodeExporter* );

	virtual const IFXCHAR* getNodeType() const=0;

	virtual IFXRESULT IFXAPI exportNode()=0;
	virtual IFXRESULT IFXAPI exportResource( const IFXNodeData* data )=0;

protected:
	IFXObjectExporter() {};

	INode* getNode() { return m_pNodeExp->getNode(); }
	ObjectState& getState() { return m_pState; }
	IFXWriter* m_pWriter;

private:
	const IFXNodeExporter* m_pNodeExp;
	ObjectState   m_pState;
}; 


//------------- GLOBAL FUNCTION PROTOTYPES ----------------------------------

//------------- GLOBAL DATA -------------------------------------------------

#endif
