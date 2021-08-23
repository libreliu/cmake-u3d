//***************************************************************************
//
//  Copyright (c) 2002 - 2006 Intel Corporation
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
#ifndef __BONEWEIGHTMODIFIEREXPORTER_H__
#define __BONEWEIGHTMODIFIEREXPORTER_H__

//------------- INCLUDES ----------------------------------------------------

#include "Max.h"
#include "NodeExporter.h"
#include "Writer.h"

//------------- DEFINES -----------------------------------------------------

//------------- CONSTANTS ---------------------------------------------------

//------------- ENUMERATIONS ------------------------------------------------

//------------- CLASS, STRUCTURE AND TYPES ----------------------------------

/**
*  IFXBoneWeightModifierExporter class is used to export Bone Weight Modifier
*/
class IFXBoneWeightModifierExporter
{
public:
	~IFXBoneWeightModifierExporter();
	IFXBoneWeightModifierExporter( IFXNodeData* );

	IFXWriter* getWriter() const { return m_pData->pWriter; }
	INode* getNode() const { return m_pData->pNode; }

	IFXRESULT export();

protected:
	IFXBoneWeightModifierExporter();

private:

	// data
	IFXNodeData* m_pData;
};

//------------- GLOBAL FUNCTION PROTOTYPES ----------------------------------

//------------- GLOBAL DATA -------------------------------------------------

#endif
