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

#ifndef _LINESETINITIALIZE_H_
#define _LINESETINITIALIZE_H_

#include "IFXAuthorLineSet.h"
#include "IFXAuthorLineSetResource.h"

IFXRESULT InitAuthorLineSet(IFXAuthorLineSet* pLineSet);

IFXRESULT Init_Simplest_AuthorLineSet(IFXAuthorLineSet* pLineSet);

IFXRESULT InitGridAuthorLineSet(IFXAuthorLineSet* pLineSet);

IFXRESULT VerifyRenderableData(IFXAuthorLineSet* pLineSet, IFXMeshGroup* pMeshGroup);

IFXRESULT Init_AuthorLineSet_QUAD(IFXAuthorLineSet* pLineSet);

IFXRESULT InitGrid256AuthorLineSet(IFXAuthorLineSet* pLineSet);

#endif // _LINESETINITIALIZE_H_