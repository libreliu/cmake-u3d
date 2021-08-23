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
@file IFXIOFunc.cpp

IFX file input/output routines.
*/

#include "IFXDataTypes.h"
#include "IFXIOFunc.h"
#include "IFXBitStreamX.h"
#include "IFXString.h"

U64 U3DIFXWrite( U3dStream_cbCtx* pCtx, U8* const pData, U64 size )
{
    U64     writtenByte = 0;
    StreamCtx*  streamCtx   = (StreamCtx*)pCtx;

    if(streamCtx->pBitStream) {
		IFXBitStreamX* pBitStream = (IFXBitStreamX*)streamCtx->pBitStream;
		for( writtenByte = 0; writtenByte < size; writtenByte++ )
			pBitStream->WriteU8X( pData[writtenByte] );
    } else {
        fprintf(stderr, "ERROR: File wasn't opened !!!\n");
    } /* if */

  return size;
}

U64 U3DRead( U3dStream_cbCtx* pCtx, U8** pData, U64 size )
{
    StreamCtx*  streamCtx   = (StreamCtx*)pCtx;
    U64         readByte = 0;

    *pData   = 0;

	if(streamCtx->pData && streamCtx->pBitStream) {
		size = size < streamCtx->fileSize ? size : streamCtx->fileSize;
		IFXBitStreamX* pBitStream = (IFXBitStreamX*)streamCtx->pBitStream;
		for( readByte = 0; readByte < size; readByte++ )
			pBitStream->ReadU8X( streamCtx->pData[readByte] );
		*pData   = streamCtx->pData;
		streamCtx->fileSize -= readByte;
	}
    return readByte;
}
