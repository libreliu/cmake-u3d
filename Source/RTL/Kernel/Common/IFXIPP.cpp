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
	@file	IFXOSIPP.cpp

			This module defines an IPP integration layer.
*/

//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXIPP.h"
#include "IFXOSLoader.h"
#include "IFXMatrix4x4.h"
#include "IFXTextureImageTools.h"

//***************************************************************************
//	Global data
//***************************************************************************

// Memory functions
#ifdef IPP_MEMORY

IFXHANDLE g_hIPPs = NULL;

typedef void* (IFXAPI_STDCALLTYPE *FippsMalloc_8u)(int len);
typedef void (IFXAPI_STDCALLTYPE *FippsFree)(void* ptr);
typedef int (IFXAPI_STDCALLTYPE *FippsCopy_8u)(const void* pSrc, void* pDst, int len);

FippsMalloc_8u ippsMalloc_8u = NULL;
FippsFree ippsFree = NULL;
FippsCopy_8u ippsCopy_8u = NULL;

#endif

// Matrix functions

IFXHANDLE g_hIPPm = NULL;

typedef int (IFXAPI_STDCALLTYPE *FippmLoadIdentity_ma_32f_S)(const F32* pDst, int dstStride0, int dstStride1, int dstStride2, int dstWidth, int dstHeight, unsigned int count);
typedef int (IFXAPI_STDCALLTYPE *FippmCopy_ma_32f_SS)(const F32* pSrc, int srcStride0, int srcStride1, int srcStride2, F32* pDst, int dstStride0, int dstStride1, int dstStride2, unsigned int width, unsigned int height, unsigned int count);

typedef int (IFXAPI_STDCALLTYPE *FippmMul_vac_32f_S2)(const F32* pSrc, int srcStride0, int srcStride2, F32 val, F32* pDst, int dstStride0, int dstStride2, unsigned int len, unsigned int count);
typedef int (IFXAPI_STDCALLTYPE *FippmDotProduct_vava_32f_S2)(const F32* pSrc1, int src1Stride0, int src1Stride2, const F32* pSrc2, int src2Stride0, int src2Stride2, F32* pDst, unsigned int len, unsigned int count);

typedef int (IFXAPI_STDCALLTYPE *FippmMul_mc_32f)(const F32* pSrc, int srcStride1, F32 val, F32* pDst, int dstStride1, unsigned int width, unsigned int height);
typedef int (IFXAPI_STDCALLTYPE *FippmMul_mv_32f)(const F32* pSrc1, int src1Stride1, unsigned int matrWidth, unsigned int matrHeight, const F32* pSrc2, unsigned int vecLen, F32* pDst);
typedef int (IFXAPI_STDCALLTYPE *FippmMul_mm_32f)(const F32* pSrc1, int src1Stride1, unsigned int src1Width, unsigned int src1Height, const F32* pSrc2, int src2Stride1, unsigned int src2Width, unsigned int src2Height, F32* pDst, int dstStride1);
typedef int (IFXAPI_STDCALLTYPE *FippmTranspose_m_32f)(const F32* pSrc, int srcStride1, unsigned int width, unsigned int height, F32* pDst, int dstStride1);
typedef int (IFXAPI_STDCALLTYPE *FippmInvert_m_32f)(const F32* pSrc, int srcStride1, unsigned int widthHeight, F32* pBuffer, F32* pDst, int dstStride1);
typedef int (IFXAPI_STDCALLTYPE *FippmDet_m_32f)(const F32* pSrc, int srcStride1, unsigned int widthHeight, F32* pBuffer, F32* pDst);

typedef int (IFXAPI_STDCALLTYPE *FippmMul_mc_32f_4x4)(const F32* pSrc, int srcStride1, F32 val, F32* pDst, int dstStride1);
typedef int (IFXAPI_STDCALLTYPE *FippmMul_mv_32f_4x4)(const F32* pSrc1, int src1Stride1, const F32* pSrc2, F32* pDst);
typedef int (IFXAPI_STDCALLTYPE *FippmMul_mm_32f_4x4)(const F32* pSrc1, int src1Stride1, const F32* pSrc2, int src2Stride1, F32* pDst, int dstStride1);
typedef int (IFXAPI_STDCALLTYPE *FippmTranspose_m_32f_4x4)(const F32* pSrc, int srcStride1, F32* pDst, int dstStride1);
typedef int (IFXAPI_STDCALLTYPE *FippmInvert_m_32f_4x4)(const F32* pSrc, int srcStride1, F32* pDst, int dstStride1);
typedef int (IFXAPI_STDCALLTYPE *FippmDet_m_32f_4x4)(const F32* pSrc, int srcStride1, F32* pDst);

FippmLoadIdentity_ma_32f_S ippmLoadIdentity_ma_32f_S = NULL;
FippmCopy_ma_32f_SS ippmCopy_ma_32f_SS = NULL;

FippmMul_vac_32f_S2 ippmMul_vac_32f_S2 = NULL;
FippmDotProduct_vava_32f_S2 ippmDotProduct_vava_32f_S2 = NULL;

FippmMul_mc_32f ippmMul_mc_32f = NULL;
FippmMul_mv_32f ippmMul_mv_32f = NULL;
FippmMul_mm_32f ippmMul_mm_32f = NULL;
FippmTranspose_m_32f ippmTranspose_m_32f = NULL;
FippmInvert_m_32f ippmInvert_m_32f = NULL;
FippmDet_m_32f ippmDet_m_32f = NULL;

FippmMul_mc_32f_4x4 ippmMul_mc_32f_4x4 = NULL;
FippmMul_mv_32f_4x4 ippmMul_mv_32f_4x4 = NULL;
FippmMul_mm_32f_4x4 ippmMul_mm_32f_4x4 = NULL;
FippmTranspose_m_32f_4x4 ippmTranspose_m_32f_4x4 = NULL;
FippmInvert_m_32f_4x4 ippmInvert_m_32f_4x4 = NULL;
FippmDet_m_32f_4x4 ippmDet_m_32f_4x4 = NULL;

// Image processing functions
#ifdef IPP_IMAGE

IFXHANDLE g_hIPPi = NULL;

typedef struct {
	int width;
	int height;
} IppiSize;

typedef struct {
	int x;
	int y;
	int width;
	int height;
} IppiRect;

enum {
	IPPI_INTER_NN     = 1,
	IPPI_INTER_LINEAR = 2,
	IPPI_INTER_CUBIC  = 4,
	IPPI_INTER_SUPER  = 8,
	IPPI_INTER_LANCZOS = 16,
	IPPI_SMOOTH_EDGE  = (1 << 31)
};

typedef U8* (IFXAPI_STDCALLTYPE *FippiMalloc_8u_AC4)(int widthPixels, int heightPixels, U32* pStepBytes);
typedef void (IFXAPI_STDCALLTYPE *FippiFree)(void* ptr);
typedef int (IFXAPI_STDCALLTYPE *FippiCopy_8u_C3R)(const U8* pSrc, int srcStep, U8* pDst, int dstStep, IppiSize roiSize);
typedef int (IFXAPI_STDCALLTYPE *FippiCopy_8u_C4R)(const U8* pSrc, int srcStep, U8* pDst, int dstStep, IppiSize roiSize);
typedef int (IFXAPI_STDCALLTYPE *FippiCopy_8u_C4CR)(const U8* pSrc, int srcStep, U8* pDst, int dstStep, IppiSize roiSize);
typedef int (IFXAPI_STDCALLTYPE *FippiCopy_8u_AC4R)(const U8* pSrc, int srcStep, U8* pDst, int dstStep, IppiSize roiSize);
typedef int (IFXAPI_STDCALLTYPE *FippiCopy_8u_AC4C3R)(const U8* pSrc, int srcStep, U8* pDst, int dstStep, IppiSize roiSize);
typedef int (IFXAPI_STDCALLTYPE *FippiCopy_8u_C3AC4R)(const U8* pSrc, int srcStep, U8* pDst, int dstStep, IppiSize roiSize);
typedef int (IFXAPI_STDCALLTYPE *FippiSwapChannels_8u_C3IR)(U8* pSrcDst, int srcDstStep, IppiSize roiSize, const int dstOrder[3]);
typedef int (IFXAPI_STDCALLTYPE *FippiSwapChannels_8u_AC4R)(const U8* pSrc, int srcStep, U8* pDst, int dstStep, IppiSize roiSize, const int dstOrder[3]);
typedef int (IFXAPI_STDCALLTYPE *FippiSet_8u_C4CR)(U8 value, U8* pDst, int dstStep, IppiSize roiSize);
typedef int (IFXAPI_STDCALLTYPE *FippiResize_8u_C3R)(const U8* pSrc, IppiSize srcSize, int srcStep, IppiRect srcRoi, U8* pDst, int dstStep, IppiSize dstRoiSize, double xFactor, double yFactor, int interpolation);
typedef int (IFXAPI_STDCALLTYPE *FippiResize_8u_C4R)(const U8* pSrc, IppiSize srcSize, int srcStep, IppiRect srcRoi, U8* pDst, int dstStep, IppiSize dstRoiSize, double xFactor, double yFactor, int interpolation);

FippiMalloc_8u_AC4 ippiMalloc_8u_AC4 = NULL;
FippiFree ippiFree = NULL;
FippiCopy_8u_C3R ippiCopy_8u_C3R = NULL;
FippiCopy_8u_C4R ippiCopy_8u_C4R = NULL;
FippiCopy_8u_C4CR ippiCopy_8u_C4CR = NULL;
FippiCopy_8u_AC4R ippiCopy_8u_AC4R = NULL;
FippiCopy_8u_AC4C3R ippiCopy_8u_AC4C3R = NULL;
FippiCopy_8u_C3AC4R ippiCopy_8u_C3AC4R = NULL;
FippiSwapChannels_8u_C3IR ippiSwapChannels_8u_C3IR = NULL;
FippiSwapChannels_8u_AC4R ippiSwapChannels_8u_AC4R = NULL;
FippiSet_8u_C4CR ippiSet_8u_C4CR = NULL;
FippiResize_8u_C3R ippiResize_8u_C3R = NULL;
FippiResize_8u_C4R ippiResize_8u_C4R = NULL;

#endif

//***************************************************************************
//	Global functions
//***************************************************************************

// Memory functions
#ifdef IPP_MEMORY

#define IPP_MEMORY_MARK 0x21435476
#define IPP_MEMORY_BLOCK 32
#define IPP_MEMORY_FACTOR 2

#define GET_ALIGNED_BYTE_COUNT( byteCount ) \
( byteCount + (IPP_MEMORY_BLOCK - (byteCount % IPP_MEMORY_BLOCK))%IPP_MEMORY_BLOCK )

void* IPPAllocate(size_t byteCount)
{
	// When byteCount == 0 return pointer is not null, 
	// for this case one memory block is allocated.
	if (0 == byteCount) 
		byteCount = IPP_MEMORY_BLOCK;
	else
		byteCount = GET_ALIGNED_BYTE_COUNT( byteCount );

	U8* ptr = (U8*)ippsMalloc_8u(byteCount+32);
    if( ptr )
    {
	    *(U32*)ptr = IPP_MEMORY_MARK;
	    *(U32*)(ptr+4) = byteCount;
        ptr += 32;
    }

    return ptr;
}

void IPPDeallocate(void* pMemory)
{
	if (pMemory) 
    {
		U8* ptr = (U8*)pMemory;
		IFXASSERT((*(U32*)(ptr-32)) == IPP_MEMORY_MARK);
		*(U32*)(ptr-32) = 0;
		ippsFree(ptr-32);
	}
}

void* IPPReallocate(void* pMemory, size_t byteCount)
{
    U8* ptr;

	if (!pMemory) 
    {
        ptr = (U8*)IPPAllocate(byteCount);
    }
    else
    {
	    ptr = (U8*)pMemory;
	    IFXASSERT((*(U32*)(ptr-32)) == IPP_MEMORY_MARK);
	    size_t oldsize = *(U32*)(ptr-28);
	    	
		if (byteCount > oldsize)
		{
			U8* newptr = (U8*)IPPAllocate(byteCount);
		    ippsCopy_8u(ptr, newptr, oldsize);
		    IPPDeallocate(ptr);
		    ptr = newptr;
		}
		else if (byteCount*IPP_MEMORY_FACTOR <= oldsize)
        {
		    IPPDeallocate(ptr);
			ptr = (U8*)IPPAllocate(byteCount);
		}
    }
	return ptr;
}
#endif

// Matrix functions

void IPPMatrix4x4_Multiply(F32* m_data, const F32* m1, const F32* m2)
{
	ippmMul_mm_32f_4x4(m1, 16, m2, 16, m_data, 16);
}

void IPPMatrix4x4_Multiply3x4(F32* m_data, const F32* m1, const F32* m2)
{
	ippmMul_mm_32f_4x4(m1, 16, m2, 16, m_data, 16);
}

void IPPMatrix4x4_MakeIdentity(F32* m_data)
{
	ippmLoadIdentity_ma_32f_S(m_data, 0, 16, 4, 4, 4, 1);
}

void IPPMatrix4x4_Scale(F32* m_data, const F32* v)
{
	F32 buffer[16];
	ippmCopy_ma_32f_SS(m_data, 0, 16, 4, buffer, 0, 16, 4, 4, 4, 1);
	ippmDotProduct_vava_32f_S2(buffer, 16, 4, v, 4, 0, m_data, 4, 4);
}

void IPPMatrix4x4_Transpose(F32* m_data)
{
	F32 buffer[16];
	ippmCopy_ma_32f_SS(m_data, 0, 16, 4, buffer, 0, 16, 4, 4, 4, 1);
	ippmTranspose_m_32f_4x4(buffer, 16, m_data, 16);
}

F32 IPPMatrix4x4_CalcDeterminant3x3(const F32* m_data)
{
	static F32 buffer[16];
	F32 result;
	ippmDet_m_32f(m_data, 16, 4, buffer, &result);
	return result;
}

F32 IPPMatrix4x4_CalcDeterminant(const F32* m_data)
{
	F32 result;
	ippmDet_m_32f_4x4(m_data, 16, &result);
	return result;
}

IFXRESULT IPPMatrix4x4_Invert3x4(F32* m_data, const F32* m)
{
	ippmInvert_m_32f_4x4(m, 16, m_data, 16);
	return IFX_OK;
}

IFXRESULT IPPMatrix4x4_Invert(F32* m_data, const F32* m)
{
	ippmInvert_m_32f_4x4(m, 16, m_data, 16);
	return IFX_OK;
}

// Image processing functions
#ifdef IPP_IMAGE

extern "C"
IFXRESULT IFXAPI_CALLTYPE IPPTextureImageTools_ReformatImage(U8* pSrc, U8* pDst, U8 uPixelSize, IFXenum eSrcFormat, IFXenum eSrcOrder, IFXenum eDstFormat, IFXenum eDstOrder, U32 uSrcX, U32 uSrcY, U32 uSrcPitch, U32 uDstX, U32 uDstY, U32 uDstPitch, U32 uWidth, U32 uHeight)
{
	if (!((eSrcFormat == IFX_RGBA_8888) || (eSrcFormat == IFX_RGBA_8880)) || !((eDstFormat == IFX_RGBA_8888) || (eDstFormat == IFX_RGBA_8880)))
		return IFX_E_UNSUPPORTED;
	if (!pSrc || !pDst)
		return IFX_E_INVALID_POINTER;

	int spp = (eSrcFormat == IFX_RGBA_8888) ? 4 : 3, dpp = (eDstFormat == IFX_RGBA_8888) ? 4 : 3;
	IppiSize roi = { uWidth, uHeight };
	U8* _pDst = pDst;
	U32 _uDstPitch = uDstPitch;
	if ((eSrcOrder != eDstOrder) && (eDstFormat == IFX_RGBA_8888))
		pDst = ippiMalloc_8u_AC4(uWidth, uHeight, &uDstPitch);

	if (eSrcFormat == IFX_RGBA_8888) {
		if (eDstFormat == IFX_RGBA_8888)
			ippiCopy_8u_C4R(pSrc+uSrcY*uSrcPitch+uSrcX*spp, uSrcPitch, pDst+uDstY*uDstPitch+uDstX*dpp, uDstPitch, roi);
		else
			ippiCopy_8u_AC4C3R(pSrc+uSrcY*uSrcPitch+uSrcX*spp, uSrcPitch, pDst+uDstY*uDstPitch+uDstX*dpp, uDstPitch, roi);
	} else {
		if (eDstFormat == IFX_RGBA_8888) {
			ippiCopy_8u_C3AC4R(pSrc+uSrcY*uSrcPitch+uSrcX*spp, uSrcPitch, pDst+uDstY*uDstPitch+uDstX*dpp, uDstPitch, roi);
			ippiSet_8u_C4CR(0xFF, pDst+uDstY*uDstPitch+uDstX*dpp+3, uDstPitch, roi);
		} else
			ippiCopy_8u_C3R(pSrc+uSrcY*uSrcPitch+uSrcX*spp, uSrcPitch, pDst+uDstY*uDstPitch+uDstX*dpp, uDstPitch, roi);
	}

	if (eSrcOrder != eDstOrder) {
		int order[3] = {2,1,0};
		if (eDstFormat == IFX_RGBA_8888) {
			ippiSwapChannels_8u_AC4R(pDst, uDstPitch, _pDst, _uDstPitch, roi, order);
			ippiCopy_8u_C4CR(pDst+3, uDstPitch, _pDst+3, _uDstPitch, roi);
			ippiFree(pDst);
		} else
			ippiSwapChannels_8u_C3IR(pDst+uDstY*uDstPitch+uDstX*dpp, uDstPitch, roi, order);
	}

	return IFX_OK;
}

extern "C"
IFXRESULT IFXAPI_CALLTYPE IPPTextureImageTools_ResizeImage(U8* pSrc, U8* pDst, U8 uPixelSize, BOOL bHasAlpha, U32 uSrcWidth, U32 uSrcHeight, U32 uDstWidth, U32 uDstHeight)
{
	if ((uPixelSize != 3) && (uPixelSize != 4))
		return IFX_E_UNSUPPORTED;
	if (!pSrc || !pDst)
		return IFX_E_INVALID_POINTER;

	IppiSize srcSize = { uSrcWidth, uSrcHeight };
	IppiRect roi = { 0, 0, uSrcWidth, uSrcHeight };
	IppiSize dstSize = { uSrcWidth, uSrcHeight };
	if (uPixelSize == 3)
		ippiResize_8u_C3R(pSrc, srcSize, uSrcWidth*uPixelSize, roi, pDst, uDstWidth*uPixelSize, dstSize, (double)uDstWidth/uSrcWidth, (double)uDstHeight/uSrcHeight, IPPI_INTER_LANCZOS);
	else
		ippiResize_8u_C4R(pSrc, srcSize, uSrcWidth*uPixelSize, roi, pDst, uDstWidth*uPixelSize, dstSize, (double)uDstWidth/uSrcWidth, (double)uDstHeight/uSrcHeight, IPPI_INTER_LANCZOS);
	return IFX_OK;
}

#endif

// Another part of variable definitions

#ifdef IPP_MEMORY

pFunction* g_IPPs_vars[] = { (pFunction*)&gs_pAllocateFunction, (pFunction*)&gs_pDeallocateFunction, (pFunction*)&gs_pReallocateFunction };
const U32 IPPs_count = sizeof(g_IPPs_vars) / sizeof(void*);
pFunction g_IPPs_new[IPPs_count] = { (pFunction)IPPAllocate, (pFunction)IPPDeallocate, (pFunction)IPPReallocate };
pFunction g_IPPs_old[IPPs_count];

#endif

pFunction* g_IPPm_vars[] = { (pFunction*)&_IFXMatrix4x4_Multiply, (pFunction*)&_IFXMatrix4x4_Multiply3x4, (pFunction*)&_IFXMatrix4x4_MakeIdentity,
							 (pFunction*)&_IFXMatrix4x4_Scale, (pFunction*)&_IFXMatrix4x4_Transpose, (pFunction*)&_IFXMatrix4x4_CalcDeterminant3x3,
							 (pFunction*)&_IFXMatrix4x4_CalcDeterminant, (pFunction*)&_IFXMatrix4x4_Invert3x4, (pFunction*)&_IFXMatrix4x4_Invert };
const U32 IPPm_count = U32( sizeof(g_IPPm_vars) / sizeof(void*) );
pFunction g_IPPm_new[IPPm_count] = { (pFunction)IPPMatrix4x4_Multiply, (pFunction)IPPMatrix4x4_Multiply3x4, (pFunction)IPPMatrix4x4_MakeIdentity,
									 (pFunction)IPPMatrix4x4_Scale, (pFunction)IPPMatrix4x4_Transpose, (pFunction)IPPMatrix4x4_CalcDeterminant3x3,
									 (pFunction)IPPMatrix4x4_CalcDeterminant, (pFunction)IPPMatrix4x4_Invert3x4, (pFunction)IPPMatrix4x4_Invert };
pFunction g_IPPm_old[IPPm_count];

#ifdef IPP_IMAGE

pFunction* g_IPPi_vars[] = { (pFunction*)&_IFXTextureImageTools_ReformatImage, (pFunction*)&_IFXTextureImageTools_ResizeImage };
const U32 IPPi_count = U32( sizeof(g_IPPi_vars) / sizeof(void*) );
pFunction g_IPPi_new[IPPi_count] = { (pFunction)IPPTextureImageTools_ReformatImage, (pFunction)IPPTextureImageTools_ResizeImage };
pFunction g_IPPi_old[IPPi_count];

#endif

// Initialization functions

void IFXIPPInitialize()
{
	U32 i = 0;
#ifdef IPP_MEMORY
#ifdef LINUX
	g_hIPPs = IFXLoadLibrary(L"libipps20.so");
#else
	g_hIPPs = IFXLoadLibrary(L"ipps20");
#endif
	if (g_hIPPs) {
		ippsMalloc_8u = (FippsMalloc_8u)IFXGetAddress(g_hIPPs, "ippsMalloc_8u");
		ippsFree = (FippsFree)IFXGetAddress(g_hIPPs, "ippsFree");
		ippsCopy_8u = (FippsCopy_8u)IFXGetAddress(g_hIPPs, "ippsCopy_8u");

		for (i = 0; i < IPPs_count; i++) {
			g_IPPs_old[i] = *g_IPPs_vars[i];
			*g_IPPs_vars[i] = g_IPPs_new[i];
		}
	}
#endif
#ifdef LINUX
	g_hIPPm = IFXLoadLibrary(L"libippm20.so");
#else
	g_hIPPm = IFXLoadLibrary(L"ippm20");
#endif
	if (g_hIPPm) {
		ippmLoadIdentity_ma_32f_S = (FippmLoadIdentity_ma_32f_S)IFXGetAddress(g_hIPPm, "ippmLoadIdentity_ma_32f_S");
		ippmCopy_ma_32f_SS = (FippmCopy_ma_32f_SS)IFXGetAddress(g_hIPPm, "ippmCopy_ma_32f_SS");
		ippmMul_vac_32f_S2 = (FippmMul_vac_32f_S2)IFXGetAddress(g_hIPPm, "ippmMul_vac_32f_S2");
		ippmDotProduct_vava_32f_S2 = (FippmDotProduct_vava_32f_S2)IFXGetAddress(g_hIPPm, "ippmDotProduct_vava_32f_S2");

		ippmMul_mc_32f = (FippmMul_mc_32f)IFXGetAddress(g_hIPPm, "ippmMul_mc_32f");
		ippmMul_mv_32f = (FippmMul_mv_32f)IFXGetAddress(g_hIPPm, "ippmMul_mv_32f");
		ippmMul_mm_32f = (FippmMul_mm_32f)IFXGetAddress(g_hIPPm, "ippmMul_mm_32f");
		ippmTranspose_m_32f = (FippmTranspose_m_32f)IFXGetAddress(g_hIPPm, "ippmTranspose_m_32f");
		ippmInvert_m_32f = (FippmInvert_m_32f)IFXGetAddress(g_hIPPm, "ippmInvert_m_32f");
		ippmDet_m_32f = (FippmDet_m_32f)IFXGetAddress(g_hIPPm, "ippmDet_m_32f");

		ippmMul_mc_32f_4x4 = (FippmMul_mc_32f_4x4)IFXGetAddress(g_hIPPm, "ippmMul_mc_32f_4x4");
		ippmMul_mv_32f_4x4 = (FippmMul_mv_32f_4x4)IFXGetAddress(g_hIPPm, "ippmMul_mv_32f_4x4");
		ippmMul_mm_32f_4x4 = (FippmMul_mm_32f_4x4)IFXGetAddress(g_hIPPm, "ippmMul_mm_32f_4x4");
		ippmTranspose_m_32f_4x4 = (FippmTranspose_m_32f_4x4)IFXGetAddress(g_hIPPm, "ippmTranspose_m_32f_4x4");
		ippmInvert_m_32f_4x4 = (FippmInvert_m_32f_4x4)IFXGetAddress(g_hIPPm, "ippmInvert_m_32f_4x4");
		ippmDet_m_32f_4x4 = (FippmDet_m_32f_4x4)IFXGetAddress(g_hIPPm, "ippmDet_m_32f_4x4");

		for (i = 0; i < IPPm_count; i++) {
			g_IPPm_old[i] = *g_IPPm_vars[i];
			*g_IPPm_vars[i] = g_IPPm_new[i];
		}
	}
#ifdef IPP_IMAGE
#ifdef LINUX
	g_hIPPi = IFXLoadLibrary(L"libippi20.so");
#else
	g_hIPPi = IFXLoadLibrary(L"ippi20");
#endif
	if (g_hIPPi) {
		ippiMalloc_8u_AC4 = (FippiMalloc_8u_AC4)IFXGetAddress(g_hIPPi, "ippiMalloc_8u_AC4");
		ippiFree = (FippiFree)IFXGetAddress(g_hIPPi, "ippiFree");
		ippiCopy_8u_C3R = (FippiCopy_8u_C3R)IFXGetAddress(g_hIPPi, "ippiCopy_8u_C3R");
		ippiCopy_8u_C4R = (FippiCopy_8u_C4R)IFXGetAddress(g_hIPPi, "ippiCopy_8u_C4R");
		ippiCopy_8u_C4CR = (FippiCopy_8u_C4CR)IFXGetAddress(g_hIPPi, "ippiCopy_8u_C4CR");
		ippiCopy_8u_AC4R = (FippiCopy_8u_AC4R)IFXGetAddress(g_hIPPi, "ippiCopy_8u_AC4R");
		ippiCopy_8u_AC4C3R = (FippiCopy_8u_AC4C3R)IFXGetAddress(g_hIPPi, "ippiCopy_8u_AC4C3R");
		ippiCopy_8u_C3AC4R = (FippiCopy_8u_C3AC4R)IFXGetAddress(g_hIPPi, "ippiCopy_8u_C3AC4R");
		ippiSwapChannels_8u_C3IR = (FippiSwapChannels_8u_C3IR)IFXGetAddress(g_hIPPi, "ippiSwapChannels_8u_C3IR");
		ippiSwapChannels_8u_AC4R = (FippiSwapChannels_8u_AC4R)IFXGetAddress(g_hIPPi, "ippiSwapChannels_8u_AC4R");
		ippiSet_8u_C4CR = (FippiSet_8u_C4CR)IFXGetAddress(g_hIPPi, "ippiSet_8u_C4CR");
		ippiResize_8u_C3R = (FippiResize_8u_C3R)IFXGetAddress(g_hIPPi, "ippiResize_8u_C3R");
		ippiResize_8u_C4R = (FippiResize_8u_C4R)IFXGetAddress(g_hIPPi, "ippiResize_8u_C4R");

		for (i = 0; i < IPPi_count; i++) {
			g_IPPi_old[i] = *g_IPPi_vars[i];
			*g_IPPi_vars[i] = g_IPPi_new[i];
		}
	}
#endif
}

void IFXIPPUninitialize()
{
	U32 i = 0;
#ifdef IPP_MEMORY
	if (g_hIPPs) {
		for (i = 0; i < IPPs_count; i++) {
			*g_IPPs_vars[i] = g_IPPs_old[i];
			g_IPPs_old[i] = NULL;
		}
		IFXReleaseLibrary(g_hIPPs);
		g_hIPPs = NULL;
	}
#endif
	if (g_hIPPm) {
		for (i = 0; i < IPPm_count; i++) {
			*g_IPPm_vars[i] = g_IPPm_old[i];
			g_IPPm_old[i] = NULL;
		}
		IFXReleaseLibrary(g_hIPPm);
		g_hIPPm = NULL;
	}
#ifdef IPP_IMAGE
	if (g_hIPPi) {
		for (i = 0; i < IPPi_count; i++) {
			*g_IPPi_vars[i] = g_IPPi_old[i];
			g_IPPi_old[i] = NULL;
		}
		IFXReleaseLibrary(g_hIPPi);
		g_hIPPi = NULL;
	}
#endif
}
