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
@file NURBSFace.cpp

Implementation of the NURBSFace interface.
*/

#include "NURBSSurface.h"
#include "NURBSFace.h"
#include "memfunc.h"

NURBSFace::NURBSFace()
{
	m_memFunc.pCtx = NULL;
	m_memFunc.alloc = U3DMalloc;
	m_memFunc.free = U3DFree;
	m_memFunc.realloc = U3DRealloc;

	m_surface = U3D_UNDEF;
	m_pTrimLoops = NULL;
	m_loopCount = 0;
}

NURBSFace::~NURBSFace()
{
	Deallocate();
}

IFXRESULT NURBSFace::SetTrimCurveLoopCount( const U32 count,
										    U3dMemory* pMemFunc )
{
	IFXRESULT result = IFX_OK;
	if( pMemFunc )
		m_memFunc = *pMemFunc;
	if( m_pTrimLoops )
		Deallocate();
	m_loopCount = count;
	m_pTrimLoops = (TrimLoop*)( m_memFunc.alloc( m_memFunc.pCtx,
												 count*sizeof( TrimLoop ) ) );
	if( !m_pTrimLoops )
		result = IFX_E_OUT_OF_MEMORY;
	if( IFXSUCCESS( result ) )
	{
		U32 i = 0;
		for( ; i < count; i++ )
		{
			m_pTrimLoops[i].m_attributes = 0;
			m_pTrimLoops[i].m_segmentCount = 0;
			m_pTrimLoops[i].m_ppSegments = NULL;
		}
	}
	return result;
}

IFXRESULT NURBSFace::GetTrimCurveLoopCount(U32* pCount ) const
{
	if( !pCount )
		return IFX_E_INVALID_POINTER;
	*pCount = m_loopCount;
	return IFX_OK;
}

IFXRESULT NURBSFace::Deallocate() 
{
	if( m_pTrimLoops )
	{
		U32 i = 0, j;
		for( ; i < m_loopCount; i++ )
			if( m_pTrimLoops[i].m_ppSegments )
			{
				for( j = 0; j < m_pTrimLoops[i].m_segmentCount; j++ )
				{
					m_pTrimLoops[i].m_ppSegments[j]->Deallocate();
					m_memFunc.free( m_memFunc.pCtx, m_pTrimLoops[i].m_ppSegments[j] );
				}
				m_memFunc.free( m_memFunc.pCtx, m_pTrimLoops[i].m_ppSegments );
			}
			m_memFunc.free( m_memFunc.pCtx, m_pTrimLoops );
	}
	return IFX_OK;
}

IFXRESULT NURBSFace::GetTrimCurveSegmentsCount(U32 loopInd, U32* pCount ) const
{
	if( !pCount )
		return IFX_E_INVALID_POINTER;
	if( loopInd >= m_loopCount )
		return IFX_E_INVALID_RANGE;
	*pCount = m_pTrimLoops[loopInd].m_segmentCount;
	return IFX_OK;
}

IFXRESULT NURBSFace::SetTrimCurveSegmentsCount(U32 loopInd, const U32 count )
{
	IFXRESULT result = IFX_OK;
	U32 j = 0;
	if( loopInd >= m_loopCount )
		result = IFX_E_INVALID_RANGE;
	if( IFXSUCCESS( result ) )
	{
		m_pTrimLoops[loopInd].m_segmentCount = count;
		if( m_pTrimLoops[loopInd].m_ppSegments )
			for( ; j < m_pTrimLoops[loopInd].m_segmentCount; j++ )
				m_pTrimLoops[loopInd].m_ppSegments[j]->Deallocate();
		m_pTrimLoops[loopInd].m_ppSegments = (NURBSTrimSegment**)
			( m_memFunc.alloc( m_memFunc.pCtx, count*sizeof( NURBSTrimSegment* ) ) );
		if( !m_pTrimLoops[loopInd].m_ppSegments )
			result = IFX_E_OUT_OF_MEMORY;
	}
	return IFX_OK;
}

IFXRESULT NURBSFace::GetTrimLoopAttributes( U32 loopInd, U32* pAttrib ) const
{
	if( !pAttrib )
		return IFX_E_INVALID_POINTER;
	if( loopInd >= m_loopCount )
		return IFX_E_INVALID_RANGE;
	*pAttrib = m_pTrimLoops[loopInd].m_attributes;
	return IFX_OK;
}

IFXRESULT NURBSFace::SetTrimLoopAttributes( U32 loopInd, const U32 attrib )
{
	if( loopInd >= m_loopCount )
		return IFX_E_INVALID_RANGE;
	m_pTrimLoops[loopInd].m_attributes = attrib;
	return IFX_OK;
}

IFXRESULT NURBSFace::GetTrimCurveLoopSegment( U32 loopInd, U32 segmentInd,
											NURBSTrimSegment** ppSegment ) const
{
	if( !ppSegment )
		return IFX_E_INVALID_POINTER;
	if( loopInd >= m_loopCount )
		return IFX_E_INVALID_RANGE;
	if( loopInd >= m_loopCount )
		return IFX_E_INVALID_RANGE;
	if( segmentInd >= m_pTrimLoops[loopInd].m_segmentCount )
		return IFX_E_INVALID_RANGE;
	*ppSegment = m_pTrimLoops[loopInd].m_ppSegments[segmentInd];
	return IFX_OK;
}
IFXRESULT NURBSFace::SetTrimCurveLoopSegment( U32 loopInd, U32 segmentInd,
											  NURBSTrimSegment* pSegment ) const
{
	if( loopInd >= m_loopCount )
		return IFX_E_INVALID_RANGE;
	if( segmentInd >= m_pTrimLoops[loopInd].m_segmentCount )
		return IFX_E_INVALID_RANGE;
	m_pTrimLoops[loopInd].m_ppSegments[segmentInd] = pSegment;
	return IFX_OK;
}

U32 NURBSFace::GetSurface() const
{
	return m_surface;
}
IFXRESULT NURBSFace::SetSurface( const U32 surface )
{
	m_surface = surface;
	return IFX_OK;
}
