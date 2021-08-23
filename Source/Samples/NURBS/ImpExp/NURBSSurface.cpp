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
@file NURBSSurface.cpp

Implementation of the NURBSSurface interface.
*/

#include "NURBSGenerator.h"
#include "NURBSSurface.h"
#include "memfunc.h"

NURBSSurface::NURBSSurface()
{
	m_memFunc.pCtx = NULL;
	m_memFunc.alloc = U3DMalloc;
	m_memFunc.free = U3DFree;
	m_memFunc.realloc = U3DRealloc;

	m_pKnotMultU = NULL;
    m_pKnotMultV = NULL;
	m_pKnotU = NULL;
	m_pKnotV = NULL;
	m_pControlPoints = NULL;//only position and vertex colors
	m_pTextureCoords = NULL;
	m_pWeights = NULL;

	m_knotPrec = 1;
	m_weightPrec = 1;
	m_CPPrec = 1;

	m_attrib = 0;
	m_degreeU = 0;
	m_degreeV = 0;
}
NURBSSurface::~NURBSSurface()
{
	Deallocate();
}

IFXRESULT NURBSSurface::Allocate( const NURBSSurfaceDesc* pNURBSSurfaceDesc,
								  U32 freeFormAttributes, U3dMemory* pMemFunc )
{
	IFXRESULT result = IFX_OK;
	if( pMemFunc )
		m_memFunc = *pMemFunc;
	if( freeFormAttributes & NURBS_ATTRIB_DOUBLE_PRECISION_KNOTS )
		m_knotPrec = 2;
	if( freeFormAttributes & NURBS_ATTRIB_DOUBLE_PRECISION_WEIGHTS )
		m_weightPrec = 2;
	if( freeFormAttributes & NURBS_ATTRIB_DOUBLE_PRECISION_CONTROL_POINTS )
		m_CPPrec = 2;
	U3dCb_Alloc U3DAlloc = m_memFunc.alloc;
	U3dMem_cbCtx* pU3DCtx = m_memFunc.pCtx;
	if( pMemFunc )
	{
		U3DAlloc = pMemFunc->alloc;
		pU3DCtx = pMemFunc->pCtx;
	}
	if( pNURBSSurfaceDesc->m_numKnotsU != U3D_UNDEF &&
		!m_pKnotMultU && !m_pKnotU )
	{
		m_NURBSSurfaceDesc.m_numKnotsU = pNURBSSurfaceDesc->m_numKnotsU;
		m_pKnotMultU = (U32*)U3DAlloc
			( pU3DCtx, m_NURBSSurfaceDesc.m_numKnotsU*sizeof( U32 ) );
		m_pKnotU = (F32*)U3DAlloc
			( pU3DCtx, m_NURBSSurfaceDesc.m_numKnotsU*sizeof( F32 )*m_knotPrec);
		if( !m_pKnotMultU || !m_pKnotU )
			result = IFX_E_OUT_OF_MEMORY;
	}
	if( pNURBSSurfaceDesc->m_numKnotsV != U3D_UNDEF && IFXSUCCESS( result ) &&
		!m_pKnotMultV && !m_pKnotV )
	{
		m_NURBSSurfaceDesc.m_numKnotsV = pNURBSSurfaceDesc->m_numKnotsV;
		m_pKnotMultV = (U32*)U3DAlloc
			( pU3DCtx, m_NURBSSurfaceDesc.m_numKnotsV*sizeof( U32 ) );
		m_pKnotV = (F32*)U3DAlloc
			( pU3DCtx, m_NURBSSurfaceDesc.m_numKnotsV*sizeof( F32 )*2 );
		if( !m_pKnotMultV || !m_pKnotV )
			result = IFX_E_OUT_OF_MEMORY;
	}
	if( pNURBSSurfaceDesc->m_numControlPointsU != U3D_UNDEF &&
		pNURBSSurfaceDesc->m_numControlPointsV != U3D_UNDEF &&
		IFXSUCCESS( result ) && !m_pControlPoints && !m_pWeights )
	{
		m_NURBSSurfaceDesc.m_numControlPointsU =
			pNURBSSurfaceDesc->m_numControlPointsU;
		m_NURBSSurfaceDesc.m_numControlPointsV =
			pNURBSSurfaceDesc->m_numControlPointsV;
		m_pControlPoints = (F32*)U3DAlloc
			( pU3DCtx, m_NURBSSurfaceDesc.m_numControlPointsU*
			m_NURBSSurfaceDesc.m_numControlPointsV*CP_STRIDE*sizeof( F32 ) );
		m_pWeights = (F32*)U3DAlloc
			( pU3DCtx, m_NURBSSurfaceDesc.m_numControlPointsU*
			m_NURBSSurfaceDesc.m_numControlPointsV*sizeof( F32 )*m_weightPrec );
		if( !m_pControlPoints || !m_pWeights )
			result = IFX_E_OUT_OF_MEMORY;
	}
	//texture coords cannot be allocated without texture layers and dimensions
	//info. handling of this info is unclear so far
	return result;
}

IFXRESULT NURBSSurface::Deallocate()
{
	U3dCb_Free U3DFree = m_memFunc.free;
	U3dMem_cbCtx* pU3DCtx = m_memFunc.pCtx;
	if( m_pKnotMultU ) U3DFree( pU3DCtx, m_pKnotMultU );
	if( m_pKnotMultV ) U3DFree( pU3DCtx, m_pKnotMultV );
	if( m_pKnotU ) U3DFree( pU3DCtx, m_pKnotU );
	if( m_pKnotV ) U3DFree( pU3DCtx, m_pKnotV );
	if( m_pControlPoints ) U3DFree( pU3DCtx, m_pControlPoints );
	if( m_pTextureCoords ) U3DFree( pU3DCtx, m_pTextureCoords );
	if( m_pWeights ) U3DFree( pU3DCtx, m_pWeights );
	return IFX_OK;
}

NURBSSurfaceDesc* NURBSSurface::GetNURBSSurfaceDesc() const
{
	return (NURBSSurfaceDesc*)&m_NURBSSurfaceDesc;
}
IFXRESULT NURBSSurface::SetNURBSSurfaceDesc( const NURBSSurfaceDesc*
											 pNURBSSurfaceDesc )
{
	IFXRESULT result = IFX_OK;
	if( !pNURBSSurfaceDesc )
		result = IFX_E_INVALID_POINTER;
	if( IFXSUCCESS( result ) )
	{
		if( pNURBSSurfaceDesc->m_numKnotsU != U3D_UNDEF )
		{
			if( pNURBSSurfaceDesc->m_numKnotsU > m_NURBSSurfaceDesc.m_numKnotsU)
				result = IFX_E_INVALID_RANGE;
			else
				m_NURBSSurfaceDesc.m_numKnotsU = pNURBSSurfaceDesc->m_numKnotsU;
		}
		if( pNURBSSurfaceDesc->m_numKnotsV != U3D_UNDEF && IFXSUCCESS( result ))
		{
			if( pNURBSSurfaceDesc->m_numKnotsV > m_NURBSSurfaceDesc.m_numKnotsV)
				result = IFX_E_INVALID_RANGE;
			else
				m_NURBSSurfaceDesc.m_numKnotsV = pNURBSSurfaceDesc->m_numKnotsV;
		}
		if( pNURBSSurfaceDesc->m_numControlPointsU != U3D_UNDEF &&
			IFXSUCCESS( result ) )
		{
			if( pNURBSSurfaceDesc->m_numControlPointsU >
				m_NURBSSurfaceDesc.m_numControlPointsU )
				result = IFX_E_INVALID_RANGE;
			else
				m_NURBSSurfaceDesc.m_numControlPointsU =
				pNURBSSurfaceDesc->m_numControlPointsU;
		}
		if( pNURBSSurfaceDesc->m_numControlPointsV != U3D_UNDEF &&
			IFXSUCCESS( result ) )
		{
			if( pNURBSSurfaceDesc->m_numControlPointsV >
				m_NURBSSurfaceDesc.m_numControlPointsV )
				result = IFX_E_INVALID_RANGE;
			else
				m_NURBSSurfaceDesc.m_numControlPointsV =
				pNURBSSurfaceDesc->m_numControlPointsV;
		}
		//texture parameters handling is unclear so far
	}
	return result;
}
IFXRESULT NURBSSurface::GetAttributes( U32* pAttrib ) const
{
	if( !pAttrib )
		return IFX_E_INVALID_POINTER;
	*pAttrib = m_attrib;
	return IFX_OK;
}

IFXRESULT NURBSSurface::SetAttributes( const U32 attrib )
{
	m_attrib = attrib;
	return IFX_OK;
}

IFXRESULT NURBSSurface::GetDegreeU( U32* pDegree ) const
{
	if( !pDegree )
		return IFX_E_INVALID_POINTER;
	*pDegree = m_degreeU;
	return IFX_OK;
}

IFXRESULT NURBSSurface::SetDegreeU( const U32 degree )
{
	m_degreeU = degree;
	return IFX_OK;
}

IFXRESULT NURBSSurface::GetDegreeV( U32* pDegree ) const
{
	if( !pDegree )
		return IFX_E_INVALID_POINTER;
	*pDegree = m_degreeV;
	return IFX_OK;
}

IFXRESULT NURBSSurface::SetDegreeV( const U32 degree )
{
	m_degreeV = degree;
	return IFX_OK;
}

IFXRESULT NURBSSurface::GetKnotMultiplicityArrayU( U32** ppKnotMultU ) const
{
	IFXRESULT result = IFX_OK;
	if( !ppKnotMultU )
		result = IFX_E_INVALID_POINTER;
	if( IFXSUCCESS( result ) )
		*ppKnotMultU = m_pKnotMultU;
	return result;
}

IFXRESULT NURBSSurface::SetKnotMultiplicityArrayU( U32* pKnotMultU )
{
	IFXRESULT result = IFX_OK;
	U32 i = 0;
	for( ; i < m_NURBSSurfaceDesc.m_numKnotsU; i++ )
		m_pKnotMultU[i] = pKnotMultU[i];
	return result;
}

IFXRESULT NURBSSurface::GetKnotArrayU( F32** ppKnotU ) const
{
	IFXRESULT result = IFX_OK;
	if( !ppKnotU )
		result = IFX_E_INVALID_POINTER;
	if( IFXSUCCESS( result ) )
		*ppKnotU = m_pKnotU;
	return result;
}

IFXRESULT NURBSSurface::SetKnotArrayU( F32* pKnotU )
{
	IFXRESULT result = IFX_OK;
	U32 i = 0;
	if( m_knotPrec == 1 )
		for( ; i < m_NURBSSurfaceDesc.m_numKnotsU; i++ )
			m_pKnotU[i] = pKnotU[i];
	else
		for( ; i < m_NURBSSurfaceDesc.m_numKnotsU; i++ )
			((F64*)m_pKnotU)[i] = ((F64*)pKnotU)[i];
	return result;
}

IFXRESULT NURBSSurface::GetKnotMultiplicityArrayV( U32** ppKnotMultV ) const
{
	IFXRESULT result = IFX_OK;
	if( !ppKnotMultV )
		result = IFX_E_INVALID_POINTER;
	if( IFXSUCCESS( result ) )
		*ppKnotMultV = m_pKnotMultV;
	return result;
}

IFXRESULT NURBSSurface::SetKnotMultiplicityArrayV( U32* pKnotMultV )
{
	IFXRESULT result = IFX_OK;
	U32 i = 0;
	for( ; i < m_NURBSSurfaceDesc.m_numKnotsV; i++ )
		m_pKnotMultV[i] = pKnotMultV[i];
	return result;
}

IFXRESULT NURBSSurface::GetKnotArrayV( F32** ppKnotV ) const
{
	IFXRESULT result = IFX_OK;
	if( !ppKnotV )
		result = IFX_E_INVALID_POINTER;
	if( IFXSUCCESS( result ) )
		*ppKnotV = m_pKnotV;
	return result;
}

IFXRESULT NURBSSurface::SetKnotArrayV( F32* pKnotV )
{
	IFXRESULT result = IFX_OK;
	U32 i = 0;
	if( m_knotPrec == 1 )        
		for( ; i < m_NURBSSurfaceDesc.m_numKnotsV; i++ )
			m_pKnotV[i] = pKnotV[i];
	else
		for( ; i < m_NURBSSurfaceDesc.m_numKnotsV; i++ )
			((F64*)m_pKnotV)[i] = ((F64*)pKnotV)[i];
	return result;
}

IFXRESULT NURBSSurface::GetControlPointsArray( F32** ppCP, U32 stride ) const
{
	IFXRESULT result = IFX_OK;
	if( !ppCP )
		result = IFX_E_INVALID_POINTER;
	if( IFXSUCCESS( result ) )
		*ppCP = m_pControlPoints;
	return result;
}

IFXRESULT NURBSSurface::SetControlPointsArray( F32* pCP, U32 stride )
{
	IFXRESULT result = IFX_OK;
	if( !pCP )
		result = IFX_E_INVALID_POINTER;
	if( IFXSUCCESS( result ) )
	{
		U32 i = 0, j;
		F32* pInCP = m_pControlPoints;
		for( ; i < m_NURBSSurfaceDesc.m_numControlPointsU*
			 m_NURBSSurfaceDesc.m_numControlPointsV; i++ )
		{
			for( j = 0; j < stride; j++ )
				pInCP[j] = pCP[j];
			pInCP += CP_STRIDE; //default stride for internal CP representation
			pCP += stride; //custom stride for output array
		}
	}
	return result;
}

IFXRESULT NURBSSurface::GetTexCoords( U32* pTexCoords, U32* pStride ) const
{
	return IFX_OK;
}

IFXRESULT NURBSSurface::SetTexCoords( U32* pTexCoords, U32 stride )
	//layer count = 8??? where is it defined?
{
	return IFX_OK;
}

IFXRESULT NURBSSurface::GetWeights( F32** ppWeights ) const
{
	IFXRESULT result = IFX_OK;
	if( !ppWeights )
		result = IFX_E_INVALID_POINTER;
	if( IFXSUCCESS( result ) )
		*ppWeights = m_pWeights;
	return result;
}

IFXRESULT NURBSSurface::SetWeights( F32* pWeights )
{
	IFXRESULT result = IFX_OK;
	U32 wnum = m_NURBSSurfaceDesc.m_numControlPointsU*
		m_NURBSSurfaceDesc.m_numControlPointsV;
	U32 i = 0;
	if( m_weightPrec == 1 )        
		for( ; i < wnum; i++ )
			m_pWeights[i] = pWeights[i];
	else
		for( ; i < wnum; i++ )
			((F64*)m_pWeights)[i] = ((F64*)pWeights)[i];
	return result;
}



NURBSShadedSurface::NURBSShadedSurface() : NURBSSurface()
{
	m_shader = 0;
}

IFXRESULT NURBSShadedSurface::GetShadingDescription( U32* pDesc ) const
{
	if( !pDesc )
		return IFX_E_INVALID_POINTER;
	*pDesc = m_shader;
	return IFX_OK;
}

IFXRESULT NURBSShadedSurface::SetShadingDescription( const U32 desc )
{
	m_shader = desc;
	return IFX_OK;
}

NURBSTrimSegment::NURBSTrimSegment()
{
	m_pAdjacent = NULL;
}

NURBSTrimSegment::~NURBSTrimSegment()
{
	Deallocate();
}

IFXRESULT NURBSTrimSegment::Deallocate()
{
	NURBSSurface::Deallocate();
	if( m_pAdjacent ) m_memFunc.free( m_memFunc.pCtx, m_pAdjacent );
	return IFX_OK;
}

IFXRESULT NURBSTrimSegment::GetEdgeCount( U32* pCount )
{
	*pCount = m_edgeCount;
	return IFX_OK;
}

IFXRESULT NURBSTrimSegment::SetEdgeCount( U32 count )
{
	IFXRESULT result = IFX_OK;
	m_edgeCount = count;
	if( m_pAdjacent ) m_memFunc.free( m_memFunc.pCtx, m_pAdjacent );
	m_pAdjacent = (NURBSTrimSegmentAdjacency*)m_memFunc.alloc
		( m_memFunc.pCtx, m_edgeCount*sizeof(NURBSTrimSegmentAdjacency) );
	if( !m_pAdjacent )
		result = IFX_E_OUT_OF_MEMORY;
	return result;
}

IFXRESULT NURBSTrimSegment::GetAdjacent( U32 i,
NURBSTrimSegmentAdjacency* pAdj ) const
{
	if( !pAdj )
		return IFX_E_INVALID_POINTER;
	if( i >= m_edgeCount )
		return IFX_E_INVALID_RANGE;
	pAdj->m_loop1 = m_pAdjacent[i].m_loop1;
	pAdj->m_segment1 = m_pAdjacent[i].m_segment1;
	pAdj->m_face = m_pAdjacent[i].m_face;
	pAdj->m_loop2 = m_pAdjacent[i].m_loop2;
	pAdj->m_segment2 = m_pAdjacent[i].m_segment2;
	return IFX_OK;
}

IFXRESULT NURBSTrimSegment::SetAdjacent( U32 i, NURBSTrimSegmentAdjacency adj )
{
	if( i >= m_edgeCount )
		return IFX_E_INVALID_RANGE;
	m_pAdjacent[i].m_loop1 = adj.m_loop1;
	m_pAdjacent[i].m_segment1 = adj.m_segment1;
	m_pAdjacent[i].m_face = adj.m_face;
	m_pAdjacent[i].m_loop2 = adj.m_loop2;
	m_pAdjacent[i].m_segment2 = adj.m_segment2;
	return IFX_OK;
}
