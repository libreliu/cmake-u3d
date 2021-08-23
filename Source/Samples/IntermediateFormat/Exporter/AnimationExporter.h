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
#ifndef __ANIMATIONEXPORTER_H__
#define __ANIMATIONEXPORTER_H__

//------------- INCLUDES ----------------------------------------------------

#include "Max.h"
#include "NodeExporter.h"
#include "BonesExporter.h"
#include "BonesUtil.h"
#include "Writer.h"

//------------- DEFINES -----------------------------------------------------

//------------- CONSTANTS ---------------------------------------------------

//------------- ENUMERATIONS ------------------------------------------------

//------------- CLASS, STRUCTURE AND TYPES ----------------------------------

/**
 *  IFXAnimationExporter class is used to export Animation Modifier
 */
class IFXAnimationExporter
{
public:
    ~IFXAnimationExporter();
    
    IFXAnimationExporter( IFXNodeData* );
    
    void export();

    IFXWriter*     getWriter() const       { return m_pData->pWriter; }
    INode*      getNode() const         { return m_pData->pNode; }
    Interval*   getAnimRange() const    { return m_pData->pAnimInterval; }
    int         getKeyFrameStep() const { return 10; }

	IFXRESULT exportAnimationModifier();
    BOOL checkForAnimation();

protected:
    IFXAnimationExporter();

private:
	void InitBoneAnimExport();
	void TermBoneAnimExport();
    BOOL checkForGeneralAnimation();
    BOOL checkForBonesAnimation();
    BOOL isKnownController( Control* );

    // data
    IFXNodeData* m_pData;
	U32 m_boneCount;
	INode** m_ppBoneList;
	IFXBonesExporter* m_pBoneInfo;
	BipedUtil bipedUtil;
}; 

//------------- GLOBAL FUNCTION PROTOTYPES ----------------------------------

//------------- GLOBAL DATA -------------------------------------------------

#endif
