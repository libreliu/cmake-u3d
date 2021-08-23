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
#ifndef __IFXNODEEXPORTER_H__
#define __IFXNODEEXPORTER_H__

//------------- INCLUDES ----------------------------------------------------

#include "max.h"
#include "IFXCOM.h"
#include "ShaderExporter.h"

//------------- DEFINES -----------------------------------------------------

//------------- CONSTANTS ---------------------------------------------------

//------------- ENUMERATIONS ------------------------------------------------
enum EIFXNodeType
{
	IFXNodeTypeUnknown,
	IFXNodeTypeModel,
	IFXNodeTypeLight,
	IFXNodeTypeView,
	IFXNodeTypeGroup
};

//------------- CLASS, STRUCTURE AND TYPES ----------------------------------

struct IFXNodeData
{
	INode*          pNode;
	IFXWriter*         pWriter;
	IFXShaderExporter* pShaderExporter;
	Interval*       pAnimInterval;
};

/**
*  IFXNodeExporter class is used to export Nodes
*/
class IFXNodeExporter
{
public:
	~IFXNodeExporter();
	IFXNodeExporter( IFXNodeData* );

	IFXRESULT exportNode( U32 node );
	IFXRESULT exportResource( U32 node );
	EIFXNodeType getEnumNodeType();
	IFXRESULT exportShadingModifier();

	IFXWriter* getWriter() const { return m_pData->pWriter; }
	INode*  getNode() const   { return m_pData->pNode; }

protected:
	IFXNodeExporter();
	IFXNodeExporter( const IFXNodeExporter& ) {};
	IFXNodeExporter& operator= ( const IFXNodeExporter& ) {};

private:
	IFXRESULT exportTM();
	IFXRESULT exportNodeHeader();

	IFXNodeData* m_pData;
	TCHAR* m_pName;
	TCHAR* m_pParentName;
	TCHAR* m_pType;
}; 

//------------- GLOBAL FUNCTION PROTOTYPES ----------------------------------

//------------- GLOBAL DATA -------------------------------------------------

#endif