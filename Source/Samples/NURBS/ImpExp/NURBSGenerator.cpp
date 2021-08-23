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
@file NURBSGenerator.cpp

Implementation of the NURBSGenerator interface.
*/

#include "NURBSGenerator.h"
#include "NURBSFace.h"
#include "memfunc.h"
#include "memory.h"

NURBSGenerator::NURBSGenerator()
{
	m_memFunc.pCtx = NULL;
	m_memFunc.alloc = U3DMalloc;
	m_memFunc.free = U3DFree;
	m_memFunc.realloc = U3DRealloc;

	m_ppSurfaces = NULL;
	m_ppFaces = NULL;
	m_pShadingDescriptions = NULL;
	m_attrib = 0;
	m_lastAllocFace = (U32)-1;
}
NURBSGenerator::~NURBSGenerator()
{
	Deallocate();
}

IFXRESULT NURBSGenerator::Allocate( const NURBSDesc* pNURBSDesc,
								   U3dMemory* pMemFunc)
{
	IFXRESULT result = IFX_OK;
	if( pMemFunc )
		m_memFunc = *pMemFunc;
	U3dCb_Alloc U3DAlloc = m_memFunc.alloc;
	U3dMem_cbCtx* pU3DCtx = m_memFunc.pCtx;
	if( pNURBSDesc->m_numSurfaces != U3D_UNDEF && !m_ppSurfaces )
	{
		m_NURBSDesc.m_numSurfaces = pNURBSDesc->m_numSurfaces;
		if( pNURBSDesc->m_numSurfaces > 0 )
		{
			m_ppSurfaces = (NURBSShadedSurface**)U3DAlloc( pU3DCtx,
				m_NURBSDesc.m_numSurfaces*sizeof( NURBSShadedSurface* ) );
			if( !m_ppSurfaces )
				result = IFX_E_OUT_OF_MEMORY;
			else
				memset( m_ppSurfaces, 0,
				m_NURBSDesc.m_numSurfaces*sizeof( NURBSShadedSurface* ) );
		}
	}
	if( pNURBSDesc->m_numFaces != U3D_UNDEF && IFXSUCCESS(result) && !m_ppFaces)
	{
		m_NURBSDesc.m_numFaces = pNURBSDesc->m_numFaces;
		if( pNURBSDesc->m_numFaces > 0 )
		{
			m_ppFaces = (NURBSFace**)U3DAlloc
				( pU3DCtx, m_NURBSDesc.m_numFaces*sizeof( NURBSFace* ) );
			if( !m_ppFaces )
				result = IFX_E_OUT_OF_MEMORY;
			else
				memset( m_ppFaces, 0,
				m_NURBSDesc.m_numFaces*sizeof( NURBSFace* ) );
		}
	}
	if( pNURBSDesc->m_numShadingDescriptions != U3D_UNDEF &&
		IFXSUCCESS( result ) && !m_pShadingDescriptions )
	{
		m_NURBSDesc.m_numShadingDescriptions =
			pNURBSDesc->m_numShadingDescriptions;
		if( pNURBSDesc->m_numShadingDescriptions > 0 )
		{
			m_pShadingDescriptions = (U32*)U3DAlloc( pU3DCtx,
				m_NURBSDesc.m_numShadingDescriptions*sizeof( U32 ) );
			if( !m_pShadingDescriptions )
				result = IFX_E_OUT_OF_MEMORY;
		}
	}
	if( pNURBSDesc->m_numEdges != U3D_UNDEF && IFXSUCCESS( result ) )
	{
		m_NURBSDesc.m_numEdges = pNURBSDesc->m_numEdges;
	}
	return result;
}

IFXRESULT NURBSGenerator::Deallocate()
{
	U3dCb_Free U3DFree = m_memFunc.free;
	U3dMem_cbCtx* pU3DCtx = m_memFunc.pCtx;
	U32 i = 0;
	if( m_NURBSDesc.m_numSurfaces != U3D_UNDEF && m_ppSurfaces )
	{
		for( ; i < m_NURBSDesc.m_numSurfaces; i++ )
			if( m_ppSurfaces[i] )
			{
				m_ppSurfaces[i]->Deallocate();
				U3DFree( pU3DCtx, m_ppSurfaces[i] );
			}
		U3DFree( pU3DCtx, m_ppSurfaces );
	}
	if( m_NURBSDesc.m_numFaces != U3D_UNDEF && m_ppFaces )
	{
		for( i = 0; i < m_NURBSDesc.m_numFaces; i++ )
			if( m_ppFaces[i] )
			{
				m_ppFaces[i]->Deallocate();
				U3DFree( pU3DCtx, m_ppFaces[i] );
			}
		U3DFree( pU3DCtx, m_ppFaces );
	}
	if( m_pShadingDescriptions ) U3DFree( pU3DCtx, m_pShadingDescriptions );
	return IFX_OK;
}

NURBSDesc* NURBSGenerator::GetNURBSDesc() const
{
	return (NURBSDesc*)&m_NURBSDesc;
}

IFXRESULT NURBSGenerator::GetAttributes( U32* pAttrib ) const
{
	if( !pAttrib )
		return IFX_E_INVALID_POINTER;
	*pAttrib = m_attrib;
	return IFX_OK;
}

IFXRESULT NURBSGenerator::SetAttributes( const U32 attrib )
{
	m_attrib = attrib;
	return IFX_OK;
}

IFXRESULT NURBSGenerator::SetNURBSShadingDesc( U32 shadingIdx,
											  const U32 NURBSShadingDesc )
{
	IFXRESULT result = IFX_OK;
	if( shadingIdx >= m_NURBSDesc.m_numShadingDescriptions )
		result = IFX_E_INVALID_RANGE;
	if( IFXSUCCESS( result ) )
		m_pShadingDescriptions[shadingIdx] = NURBSShadingDesc;
	return result;
}

IFXRESULT NURBSGenerator::GetNURBSShadingDesc( U32 shadingIdx,
											  U32* pNURBSShadingDesc )
{
	IFXRESULT result = IFX_OK;
	if( shadingIdx >= m_NURBSDesc.m_numShadingDescriptions )
		result = IFX_E_INVALID_RANGE;
	if( !pNURBSShadingDesc )
		result = IFX_E_INVALID_POINTER;
	if( IFXSUCCESS( result ) )
		*pNURBSShadingDesc = m_pShadingDescriptions[shadingIdx];
	return result;
}

IFXRESULT NURBSGenerator::GetSurface( U32 surfInd,
									 NURBSShadedSurface** ppSurface ) const
{
	IFXRESULT result = IFX_OK;
	if( surfInd >= m_NURBSDesc.m_numSurfaces )
		result = IFX_E_INVALID_RANGE;
	if( !ppSurface )
		result = IFX_E_INVALID_POINTER;
	if( IFXSUCCESS( result ) )
		*ppSurface = m_ppSurfaces[surfInd];
	return result;
}

IFXRESULT NURBSGenerator::SetSurface( U32 surfInd, NURBSShadedSurface* pSurface,
									 U32 shaderID )
{
	IFXRESULT result = IFX_OK;
	if( !pSurface )
		result = IFX_E_INVALID_POINTER;
	if( surfInd >= m_NURBSDesc.m_numSurfaces )
		result = IFX_E_INVALID_RANGE;
	if( IFXSUCCESS( result ) )
	{
		pSurface->SetShadingDescription( shaderID );
		m_ppSurfaces[surfInd] = pSurface;
	}
	return result;
}

IFXRESULT NURBSGenerator::GetFace( U32 surfIdx, U32 faceIdx,
								  NURBSFace** ppFace ) const
{
	IFXRESULT result = IFX_OK;
	if( surfIdx >= m_NURBSDesc.m_numSurfaces ||
		faceIdx >= m_NURBSDesc.m_numFaces )
		result = IFX_E_INVALID_RANGE;
	if( !ppFace )
		result = IFX_E_INVALID_POINTER;
	if( IFXSUCCESS( result ) )
	{
		U32 i = 0;
		NURBSFace* pFace = NULL;
		U32 faceCnt = 0;
		BOOL searching = IFX_TRUE;
		for( ; i < m_NURBSDesc.m_numFaces && searching; i++ )
		{
			pFace = m_ppFaces[i];
			if( pFace && pFace->GetSurface() == surfIdx )
			{
				if( faceCnt == faceIdx )
					searching = IFX_FALSE;
				faceCnt++;
			}
		}
		if( searching )
			*ppFace = NULL;
		else
			*ppFace = pFace;
	}
	return result;
}

IFXRESULT NURBSGenerator::SetFace( U32 surfIdx, U32 faceIdx, NURBSFace* pFace )
{
	IFXRESULT result = IFX_OK;
	if( surfIdx >= m_NURBSDesc.m_numSurfaces ||
		faceIdx >= m_NURBSDesc.m_numFaces )
		result = IFX_E_INVALID_RANGE;
	if( !pFace )
		result = IFX_E_INVALID_POINTER;
	if( IFXSUCCESS( result ) )
	{
		U32 i = 0;
		NURBSFace* pCurFace = NULL;
		U32 faceCnt = 0;
		BOOL searching = IFX_TRUE;
		for( ; i < m_NURBSDesc.m_numFaces && searching; i++ )
		{
			pCurFace = m_ppFaces[i];
			if( pCurFace && pCurFace->GetSurface() == surfIdx )
			{
				if( faceCnt == faceIdx )
					searching = IFX_FALSE;
				faceCnt++;
			}
		}
		if( searching ) //adding new face
		{
			m_ppFaces[++m_lastAllocFace] = pFace;
			pFace->SetSurface( surfIdx );
		}
		else
			m_ppFaces[i] = pFace;
	}
	return result;
}


IFXRESULT NURBSGenerator::GetFaceCount( U32 surfIdx, U32* pFaceCount ) const
{
	IFXRESULT result = IFX_OK;
	if( surfIdx >= m_NURBSDesc.m_numSurfaces )
		result = IFX_E_INVALID_RANGE;
	if( !pFaceCount )
		result = IFX_E_INVALID_POINTER;
	if( IFXSUCCESS( result ) )
	{
		U32 i = 0;
		NURBSFace* pFace = NULL;
		U32 faceCnt = 0;
		for( ; i < m_NURBSDesc.m_numFaces; i++ )
		{
			pFace = m_ppFaces[i];
			if( pFace && pFace->GetSurface() == surfIdx )
				faceCnt++;
		}
		*pFaceCount = faceCnt;
	}
	return result;
}
