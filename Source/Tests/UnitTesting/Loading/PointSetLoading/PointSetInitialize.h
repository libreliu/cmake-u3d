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

#ifndef _PointSETINITIALIZE_H_
#define _PointSETINITIALIZE_H_

#include "IFXAuthorPointSet.h"
#include "IFXAuthorPointSetResource.h"
#include "IFXTest.h"
#include "IFXException.h"

IFXRESULT InitAuthorPointSet(IFXAuthorPointSet* pPointSet);

IFXRESULT Init_Simplest_AuthorPointSet(IFXAuthorPointSet* pPointSet);

IFXRESULT InitGridAuthorPointSet(IFXAuthorPointSet* pPointSet);

IFXRESULT VerifyRenderableData(IFXAuthorPointSet* pPointSet, IFXMeshGroup* pMeshGroup);

IFXRESULT Init_AuthorPointSet_QUAD(IFXAuthorPointSet* pPointSet);

IFXRESULT InitGrid256AuthorPointSet(IFXAuthorPointSet* pPointSet);

#endif // _PointSETINITIALIZE_H_