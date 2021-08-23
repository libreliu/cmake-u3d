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
@file encoder.h

NURBS high level output.
*/

#ifndef ENCODER_H
#define ENCODER_H

#include "NURBSGenerator.h"
#include "u3dbitencoder.h"
#include "iofunc.h"

U3dEncoder* initializeEncoder
			( StreamCtx& stream, U3dMemory memFunc, U3dCb_Write write );
void finalizeEncoder( StreamCtx& stream, U3dEncoder* pEncoder );
//encode declaration block
void encode( NURBSGenerator* pGen, U3dMemory* pMemFunc, U3dEncoder* pEncoder );
//encode continuation block
void encodeCont( NURBSGenerator* pGen, U3dMemory* pMemFunc, U3dEncoder* pEncoder );

#endif
