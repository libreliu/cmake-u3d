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
#ifndef __COMMON_H__
#define __COMMON_H__

//------------- INCLUDES ----------------------------------------------------

#include "max.h"
#include "IFXDataTypes.h"
#include "IFXResult.h"

namespace U3D_IDTF{}
using namespace U3D_IDTF;
//------------- DEFINES -----------------------------------------------------

// The extension for the type of file we export --
// change as needed
#define EXPORTER_EXT "idtf"

// Exporter version reported to MAX (We keep this in-synch with
// version resource manually)
#define EXPORTER_VER 160

// IDTF format version
#define IDTF_FORMAT_VER 100

#define MAX_TOKEN_LEN 128
#define DEFAULT_PRECISION 6
#define EPSILON 0.00001f
#define MAX_BONE_NUM 512
#define ONE_DEGREE_RAD    0.0174532f

#if _MSC_VER > 1000
#pragma once
#endif

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers

//------------- GLOBAL FUNCTION PROTOTYPES ----------------------------------

#endif