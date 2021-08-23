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
@file decoder.h

NURBS high level input.
*/

#ifndef DECODER_H
#define DECODER_H

#include "u3ddefs.h"
#include "u3dbitdecoder.h"
#include "iofunc.h"
#include "NURBSGenerator.h"

U3dDecoder* initializeDecoder
			( StreamCtx& stream, U3dMemory memFunc, U3dCb_Map read );
void finalizeDecoder
	 ( StreamCtx& stream, U3dDecoder* pDecoder, U3dMemory memFunc );
//decode declaration block
void decode( NURBSGenerator* pGen, U3dMemory* pMemFunc, U3dDecoder* pDecoder );
//decode continuation block
void decodeCont( NURBSGenerator* pGen, U3dMemory* pMemFunc, U3dDecoder* pDecoder );

#endif
