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
@file memfunc.cpp

Default memory routines.
*/

#include "memfunc.h"
#include <malloc.h>

void* U3DMalloc(void* pMemCtx, U32 size)
{
    return malloc(size);
}

void U3DFree(void* pMemCtx, void* pBuf)
{
    free(pBuf);
    return;
}

void* U3DRealloc(void* pMemCtx, void* pBuf, U32 size)
{
    return realloc(pBuf, size);
}
