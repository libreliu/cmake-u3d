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

#ifndef _CLODINITIALIZE_H_
#define _CLODINITIALIZE_H_

#include "IFXAuthorMesh.h"
#include "IFXAuthorCLODResource.h"
//#include "IFXMeshGenerator.h"


IFXRESULT InitAuthorClod(IFXAuthorCLODMesh* pClodMesh);

IFXRESULT Init_Simplest_AuthorClod(IFXAuthorCLODMesh* pClodMesh);
IFXRESULT InitAuthorClod(IFXAuthorCLODMesh* pClodMesh);

//IFXRESULT InitGridAuthorLineSet(IFXAuthorLineSet* pLineSet);

//IFXRESULT VerifyRenderableData(IFXAuthorLineSet* pLineSet, IFXMeshGroup* pMeshGroup);

#endif // _CLODINITIALIZE_H_