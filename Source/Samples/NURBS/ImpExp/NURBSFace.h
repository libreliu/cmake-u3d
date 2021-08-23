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
@file NURBSFace.h

Declaration of the NURBSFace interface.
*/

#ifndef NURBSFACE_H
#define NURBSFACE_H

#include "IFXResult.h"
#include "IFXAPI.h"
#include "u3ddefs.h"

#include "NURBSSurface.h"

/**
This is the main interface for NURBSFace.
Provides access to trim curves.
*/
class NURBSFace
{
public:
	NURBSFace();
	virtual ~NURBSFace();

	/**
	*  Allocates/reallocates all the necessary space required by the NURBSFace
	*
	*  @param count  Number of trim curve loops in the face.
	*  @param pMemory  The set of memory utilities
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_RANGE  Count is less than one.
	*  @retval IFX_E_OUT_OF_MEMORY  Insufficient memory.
	*/
	IFXRESULT SetTrimCurveLoopCount( const U32 count,
									 U3dMemory* pMemFunc = NULL );

	/**
	*  Returns the number of trim curve loops in the face.
	*
	*  @param pCount  A pointer to the number of trim curve loops in the face.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  pCount is NULL
	*/
	IFXRESULT GetTrimCurveLoopCount(U32* pCount ) const;

	/**
	*  Frees all data associated with the NURBSFace.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*/
	IFXRESULT Deallocate() ;

	/**
	*  Returns the number of trim curve segments in the loop.
	*
	*  @param  loopInd  Index of the desired loop.
	*  @param pCount A pointer to the number of trim curve segments in the loop.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  pCount is NULL
	*/
	IFXRESULT GetTrimCurveSegmentsCount(U32 loopInd, U32* pCount ) const;
	/**
	*  Allocates/reallocates all the necessary space required by the TrimLoop
	*
	*  @param  loopInd  Index of the desired loop.
	*  @param count  Number of trim curve segments in the loop.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_RANGE  Count is less than one.
	*  @retval IFX_E_OUT_OF_MEMORY  Insufficient memory.
	*/
	IFXRESULT SetTrimCurveSegmentsCount(U32 loopInd, const U32 count );

	/**
	*  Returns the value of trim loop attributes.
	*
	*  @param  loopInd  Index of the desired loop.
	*  @param  pAttrib  a pointer to the value of attributes.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  pAttrib is NULL
	*/
	virtual IFXRESULT IFXAPI GetTrimLoopAttributes( U32 loopInd,
													U32* pAttrib ) const;

	/**
	*  Sets the value of trim loop attributes.
	*
	*  @param  loopInd  Index of the desired loop.
	*  @param  attrib  the value of attributes.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*/
    virtual IFXRESULT IFXAPI SetTrimLoopAttributes( U32 loopInd,
													const U32 attrib );

	/**
	*  Returns the trim curve loop segment.
	*
	*  @param  loopInd  Index of the desired loop.
	*  @param  segmentInd  Index of the desired segment.
	*  @param ppSegment  A pointer to segment.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  ppSegment is NULL
	*/
	IFXRESULT GetTrimCurveLoopSegment( U32 loopInd, U32 segmentInd,
									   NURBSTrimSegment** ppSegment ) const;
	/**
	*  Sets the trim curve loop segment.
	*
	*  @param  loopInd  Index of the desired loop.
	*  @param  segmentInd  Index of the desired segment.
	*  @param pSegment  A pointer to segment.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*/
	IFXRESULT SetTrimCurveLoopSegment( U32 loopInd, U32 segmentInd,
									   NURBSTrimSegment* pSegment ) const;

	/**
	*  Returns the corresponding surface index.
	*
	*  @return Surface index.
	*/
	U32 GetSurface() const;
	/**
	*  Sets the corresponding surface index.
	*
	*  @param surface  Surface index.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*/
	IFXRESULT SetSurface( const U32 surface );
private:
	typedef struct tagTrimLoop
	{
		U32 m_attributes;
		U32 m_segmentCount;
		NURBSTrimSegment** m_ppSegments;
	}TrimLoop;

	U3dMemory m_memFunc;

	U32 m_surface; //index of corresponding surface
	TrimLoop* m_pTrimLoops;
	U32 m_loopCount;
};

#endif
