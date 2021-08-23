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
#ifndef __BONESUTIL_H__
#define __BONESUTIL_H__

//------------- INCLUDES ----------------------------------------------------

#include "max.h"
#include "IFXDataTypes.h"
#include "IFXResult.h"
#include "bipexp.h"
#include "iparamb2.h"
#include "iskin.h"

//------------- DEFINES -----------------------------------------------------

//------------- CONSTANTS ---------------------------------------------------

//------------- ENUMERATIONS ------------------------------------------------

//------------- CLASSES, STRUCTURES AND TYPES -------------------------------

class BipedUtil
{
public:
	BipedUtil();
	~BipedUtil(){}
	bool FindBipedRoot(INode **pMAXNodeList, int in_Cnt);
	bool BipedIntoReferenceMode();
	bool BipedOutOfReferenceMode();
	bool BipedRemoveNonUniformScale();
	bool BipedReturnNonUniformScale();
	bool BipedExportComplete();
	bool GetBipInterface();
private:
	INode* m_pBipedRoot;
	IBipedExport*   m_pBipedInterface;
};

//------------- GLOBAL FUNCTION PROTOTYPES ----------------------------------

bool CheckID(INode *pNode, Class_ID ClassID);
bool CheckControlID(INode* pNode, Class_ID ClassID);
bool IsBiped( INode* pNode );
bool IsRootBone( INode* pNode );
bool HasSkeleton( INode* pNode );
Modifier *GetModifier(INode * pNode, int scid, Class_ID cid);
IFXRESULT CollectBones( INode* pNode, INode** ppBoneList, U32* pBoneCount );

#endif
