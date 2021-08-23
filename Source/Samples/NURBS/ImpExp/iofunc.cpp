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
@file iofunc.cpp

Default file input/output routines.
*/

#include "iofunc.h"

U64 U3DWrite( U3dStream_cbCtx* pCtx, U8* const pData, U64 size )
{
    U64     writtenByte = 0;
    StreamCtx*  pStreamCtx   = (StreamCtx*)pCtx;

    if( pStreamCtx && pStreamCtx->pFile ) {
        writtenByte = (U64)fwrite(pData, sizeof(U8), (size_t)size,
                  pStreamCtx->pFile);
    }
	return writtenByte;
}

U64 U3DRead( U3dStream_cbCtx* pCtx, U8** pData, U64 size )
{
    StreamCtx*  pStreamCtx   = (StreamCtx*)pCtx;
    U64         readByte = 0;

    *pData   = 0;

    if( pStreamCtx && pStreamCtx->pFile ) {
        if( pData && pStreamCtx->pData ) {
            size     = size < pStreamCtx->fileSize ? size : pStreamCtx->fileSize;
            readByte = fread(pStreamCtx->pData, sizeof(U8), (size_t)size,
                             pStreamCtx->pFile);
            *pData   = pStreamCtx->pData;
            pStreamCtx->fileSize -= readByte;
        }
    }
    return readByte;
}
