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
//

#include "decoderctx.h"

extern U32 c64De[];

/*
 * U3D Bitstream decoder functions u3dGetU64/U32/U16/U8
 * Purpose
 *      Get an U32/U16/U8 data from bitstream.
 *  Parameters
 *  (in)pCtx - the pointer to the decoder context of bitstream that will be 
 *             used to get U64/U32/U16/U8 data
 *  (out)value - the value that should be gotten from bitstream 
 *  Return
 *      u3dStsNoErr if the value was successfully added to bitstream.
 *      u3dStsContextMatchErr if pCtx isn't valid
 *      u3dStsNullPtrErr if pCtx is null
 */
U3D_METHOD(U3dStatus, u3dGetU64 ,(U3dDecoder* pCtx, U64  *value))
{
	U3dStatus status = u3dStsNoErr;
	U8*		  value_t = (U8*)value;

    if(pCtx == 0) {
        return u3dStsNullPtrErr;
    } /* if */
    if(pCtx->id != U3DDECODERCTX_ID) {
        return u3dStsContextMatchErr;
    } /* if */

	status = intGetU8(pCtx, value_t + c64De[0]);
	if(status != u3dStsNoErr) {
		return status;
	}
	status = intGetU8(pCtx, value_t + c64De[1]);
	if(status != u3dStsNoErr) {
		return status;
	}
	status = intGetU8(pCtx, value_t + c64De[2]);
	if(status != u3dStsNoErr) {
		return status;
	}
	status = intGetU8(pCtx, value_t + c64De[3]);
	if(status != u3dStsNoErr) {
		return status;
	}
	status = intGetU8(pCtx, value_t + c64De[4]);
	if(status != u3dStsNoErr) {
		return status;
	}
	status = intGetU8(pCtx, value_t + c64De[5]);
	if(status != u3dStsNoErr) {
		return status;
	}
	status = intGetU8(pCtx, value_t + c64De[6]);
	if(status != u3dStsNoErr) {
		return status;
	}
	status = intGetU8(pCtx, value_t + c64De[7]);
	return status;

#ifdef U3D_LOGDECODER
	fprintf(pCtx->logFile, "%d, %lu, %lu\n", 3, 0, *value);
#endif

} /* u3dGetU64 */

