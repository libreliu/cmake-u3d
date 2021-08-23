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

extern U32	cReadCount[];
extern U32	cFastNotMask[];
extern U32	cHalfMask;
extern U32	cNotHalfMask;
extern U32	cQuarterMask;
extern U32	cNotThreeQuarterMask;
extern U8		cSwap8Dec[];

extern U32 c32De[];

/**
 * u3dGetCompressedU8 @brief U3D Bitstream decoder function to get U8 
 * compressed value from bitstream by using static compresed context
 * 
 * @b Purpose \n
 *      Get an U8 data from bitstream by using static compresed context 
 *  @param pCtx (in) the pointer to the decoder context of bitstream that will 
 *         be used to get U8 data using static compressed context
 *  @param range (in) the range of static compressed context 
 *  @param value (out) the value that will be gotten from bitstream 
 * @return
 *      Status of geting compressed value from bitstream operation
 *      @retval u3dStsNoErr if the value was successfully gotten from bitstream.
 *      @retval u3dStsContextMatchErr if pCtx isn't valid
 *      @retval u3dStsNullPtrErr if pCtx is null
 *      @retval u3dStsBadArgErr if range is equal to 0
 */
U3D_METHOD(U3dStatus, u3dGetStaticCompressedU8 ,(    
    U3dDecoder*     pCtx, 
    U16			    range,
    U8              *value))
{
	U3dStatus	status;

    if(pCtx == 0) {
        return u3dStsNullPtrErr;
    } /* if */
    if(pCtx->id != U3DDECODERCTX_ID) {
        return u3dStsContextMatchErr;
    } /* if */
    if(range == 0) {
        return u3dStsBadArgErr;
    } /* if */
    if(pCtx->bitstreamposition == pCtx->bitstreamlastposition) {
        U64		  rez = 1;
        rez = GetStreamData(pCtx);
        if(rez == 0) {
            return u3dStsStreamOpErr;
        } /* if */
    } /* if */
	if(range > 0x3FFF) {
		U3dStatus status = u3dStsNoErr;

		status = intGetU8(pCtx, value);
		return status;
	} else {
		status	= intGetCompressedU8(pCtx, range, value);
	} /* if */
#ifdef U3D_LOGDECODER
	// static context
	fprintf(pCtx->logFile, "%d, %lu, %lu\n", 0, range ^ 0x80000000, *value);
#endif

    return status;
} /* u3dGetCompressedU8 */

INT_METHOD(U3dStatus, intGetCompressedU8 ,(    
    U3dDecoder*     pCtx, 
    U16			    range,
    U8              *value))
{
    U32     valueToRead     = 0;
    U32     escape          = 0;
    U32     cumSymbolFreq   = 0;
    U32     symbolFreq      = 1;
    U32     rangeI          = 1;
    U32     code            = 0;
    U32     bitCount		= 0;
    U32     underflowCount	= 0;
	U64		readNumber		= 1;
    U32     maskedState;
    U32     savedBits;
    U32     state;
	U32     high;
    U32     low;

    high    = pCtx->high;
    low     = pCtx->low;
    rangeI  = high + 1 - low;

    // read code from bitstream
    {
        U8*  stream      = pCtx->bitstreamposition;
        U8   bitshift    = pCtx->bitshift;
	    U32  temp;

        // read 1 bit
		temp    = (((*stream) >> bitshift)&0x01) << 15;
        if(pCtx->underflowCount != 0) {
            // skipp underflow
            bitshift    += (U8)(pCtx->underflowCount);
            stream      += (bitshift>> 3);
            bitshift    &= 0x7;
        } /* if */
		// read 15 bits
        READ_8BITS_FROM_STREAM(stream, *(((U8*)&code) + c32De[0]), bitshift);
        READ_8BITS_FROM_STREAM(stream, *(((U8*)&code) + c32De[1]), bitshift);
		code &= 0xFFFE;
		// swap
		code = (cSwap8Dec[(code >> 12) & 0xf] ) |
			   ((cSwap8Dec[(code >> 8) & 0xf ] << 4 )) |
			   ((cSwap8Dec[(code >> 4) & 0xf] << 8)) |
			   ((cSwap8Dec[code & 0xf])<< 12 );
		code |= temp;
    }

    // Static Compresion Context
    symbolFreq      = 1;
    cumSymbolFreq   = (range * (1 + code - low) - 1) / rangeI;
    valueToRead     = cumSymbolFreq + 1;

    high    = low - 1 + rangeI * (cumSymbolFreq + symbolFreq) / range;
    low     = low + rangeI * cumSymbolFreq / range;
	state	= (low << 16) | high;

	// Fast count the first 4 bits
	bitCount	= cReadCount[(((low >> 12) ^ (high >> 12)) & 0xF)];
	state		&= cFastNotMask[bitCount];
	state		<<= bitCount;
	state		|= ((1 << bitCount) - 1);

	{
		U32     bitCount2 = 0;
		// Static context
		// Fast count the next 4 bits
		bitCount2	= cReadCount[(((state >> 12) ^ (state >> 28)) & 0xF)];
		state		&= cFastNotMask[bitCount2];
		state		<<= bitCount2;
		state		|= ((1 << bitCount2) - 1);
		bitCount	+= bitCount2;
	} /* if */
    // Regular count the rest
	maskedState	= cHalfMask & state;
	while(0 == maskedState || cHalfMask == maskedState) {
		state		= ((cNotHalfMask & state) << 1) | 1;
		maskedState	= cHalfMask & state;
		bitCount++;
	} /* while */
    // Count underflow
	savedBits	= maskedState;
	if(bitCount > 0) {
		bitCount	+= pCtx->underflowCount;
		pCtx->underflowCount	= 0;
	} /* if */
	maskedState	= cQuarterMask & state;
	while(0x40000000 == maskedState) {
		state	&= cNotThreeQuarterMask;
		state	<<= 1;
		state	|= 1;
		maskedState	= cQuarterMask & state;
		underflowCount++;
	} /* while */
    // store the current state
	state				 |= savedBits;
    pCtx->high           = state & 0x0000FFFF;
    pCtx->low            = state >> 16;
    pCtx->underflowCount += underflowCount;
	// shift in bitstream
	SHIFT_NBITS_IN_STREAM(pCtx, (U8)bitCount, readNumber);

    if(valueToRead == 0) {
        U3dStatus   rez = u3dStsNoErr;
        // escape symbol
        rez = intGetU8(pCtx, value);
		return rez;
	} else {
	    *value  = (U8)(valueToRead - 1);
	}/* if */
    
    return u3dStsNoErr;
} /* u3dGetStaticCompressedU8 */
