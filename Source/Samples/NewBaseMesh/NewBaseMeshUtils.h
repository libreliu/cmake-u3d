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
	@file	NewBaseMeshUtils.h

			This module defines the common stream functions.
*/


#ifndef NEWBASEMESHUTILS_H
#define NEWBASEMESHUTILS_H

#include "u3ddefs.h"

void* streamMalloc(void* memCtx, U32 size);

void streamFree(void* memCtx, void* buf);

struct U3DStream {
	U8* m_stream;   // stream pointer
	U8* m_curPos;   // current position in the stream
	U64 m_size;     // stream size
	U64 m_read;     // bytes read
	U64 m_written;  // bytes written
};

#endif