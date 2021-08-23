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

/**
@file IFXIOFunc.h

IFX file input/output routines.
*/

#ifndef IFXIOFUNC_H
#define IFXIOFUNC_H

#include "iofunc.h"

U64 U3DIFXWrite( U3dStream_cbCtx* pCtx, U8* const pData, U64 size );
U64 U3DIFXRead( U3dStream_cbCtx* pCtx, U8** pData, U64 size );

#endif
