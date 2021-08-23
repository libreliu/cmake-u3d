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
@file decoder.cpp

NURBS decoding tool.
*/

#include "NURBSGenerator.h"
#include "memfunc.h"
#include "decoder.h"

U3dDecoder* initializeDecoder( StreamCtx& stream, U3dMemory memFunc,
							   U3dCb_Map read )
{
	if(stream.pData == 0)
		stream.pData = (U8*)memFunc.alloc( memFunc.pCtx,
						  ((size_t)stream.fileSize) );
	U3dDecoder* pDecoder = NULL;
	if( stream.pData ) U3dStatus status  = u3dDecoderInit(&pDecoder, &memFunc, &stream, read);
	return pDecoder;
}

void finalizeDecoder( StreamCtx& stream, U3dDecoder* pDecoder,
					  U3dMemory memFunc )
{
	U3dStatus status = u3dDecoderDelete(pDecoder);

	if(stream.pData) memFunc.free( memFunc.pCtx, stream.pData );
	if(stream.pFile) {
		fclose(stream.pFile);
		stream.pFile  = 0;
	}
}

void decode( NURBSGenerator* pGen, U3dMemory* pMemFunc, U3dDecoder* pDecoder )
{
	if( !pGen | !pMemFunc | !pDecoder ) return;
	U3dMemory memFunc = *pMemFunc;
	U32 i, j, k, l, m, val, val2;

	U32 ffAttrib = 0;
	u3dGetU32( pDecoder, &ffAttrib );
	pGen->SetAttributes( ffAttrib );

	NURBSDesc desc;

	u3dGetU32( pDecoder, &(desc.m_numSurfaces) );
	u3dGetU32( pDecoder, &(desc.m_numFaces) );

	//edge num - not used
	u3dGetU32( pDecoder, &(desc.m_numEdges) );

	u3dGetU32( pDecoder, &desc.m_numShadingDescriptions );
	pGen->Allocate( &desc, &memFunc );

	for( i = 0; i < desc.m_numShadingDescriptions; i++ )
	{
		u3dGetU32( pDecoder, &val );
		pGen->SetNURBSShadingDesc( i, val );
	}
}

void decodeCont( NURBSGenerator* pGen, U3dMemory* pMemFunc, U3dDecoder* pDecoder )
{
	if( !pGen | !pMemFunc | !pDecoder ) return;
	U3dMemory memFunc = *pMemFunc;
	U32 i, j, k, l, m, val, val2;
	U32 ffAttrib = 0;
	NURBSDesc desc;

	U32 surfStart, surfEnd;
	u3dGetU32( pDecoder, &surfStart );//surface start index. 0 by default
	u3dGetU32( pDecoder, &surfEnd );//surface end index

	U32 surfAttrib;

	U32 faceCnt, trimLoopCnt, trimSegmentCnt;
	for( i = surfStart; i < surfEnd; i++ )
	{
		NURBSShadedSurface* pSurface =(NURBSShadedSurface*)
			memFunc.alloc( memFunc.pCtx, sizeof( NURBSShadedSurface ) );
		pSurface->NURBSShadedSurface::NURBSShadedSurface();
		NURBSSurfaceDesc* pSurfDesc = pSurface->GetNURBSSurfaceDesc();
		F32* pKnots = NULL; //not F32 due to the bitstream limitation
		U32* pKnotMult = NULL;
		F32* pCP = NULL;
		F32* pWeights = NULL;

		pGen->GetNURBSShadingDesc( i, &val );
		pGen->SetSurface( i, pSurface, val );

		u3dGetU32( pDecoder, &surfAttrib );
		pSurface->SetAttributes( surfAttrib );
		u3dGetU32( pDecoder, &val );
		pSurface->SetShadingDescription( val );
		u3dGetU32( pDecoder, &val );
		pSurface->SetDegreeU( val );
		u3dGetU32( pDecoder, &val );
		pSurface->SetDegreeV( val );

		//what is the case in the spec when knots can be omitted?
		u3dGetU32( pDecoder, &val );//m_numKnotsU
		NURBSSurfaceDesc allocKU;
		allocKU.m_numKnotsU = val;
		pSurface->Allocate( &allocKU, ffAttrib, &memFunc );
		pKnots = new F32[pSurfDesc->m_numKnotsU];
		pKnotMult = new U32[pSurfDesc->m_numKnotsU];
		for( j = 0; j < pSurfDesc->m_numKnotsU; j++ )
		{
			u3dGetU32( pDecoder, pKnotMult+j );
			if( ffAttrib & NURBS_ATTRIB_DOUBLE_PRECISION_KNOTS )
				u3dGetU64( pDecoder, (U64*)pKnots+j );
			else
				u3dGetU32( pDecoder, (U32*)pKnots+j );
		}
		pSurface->SetKnotArrayU( pKnots );
		pSurface->SetKnotMultiplicityArrayU( pKnotMult );
		delete pKnots;
		delete pKnotMult;
		u3dGetU32( pDecoder, &val );//m_numKnotsV
		NURBSSurfaceDesc allocKV;
		allocKV.m_numKnotsV = val;
		pSurface->Allocate( &allocKV, ffAttrib, &memFunc );
		pKnots = new F32[pSurfDesc->m_numKnotsV];
		pKnotMult = new U32[pSurfDesc->m_numKnotsV];
		for( j = 0; j < pSurfDesc->m_numKnotsV; j++ )
		{
			u3dGetU32( pDecoder, pKnotMult+j );
			if( ffAttrib & NURBS_ATTRIB_DOUBLE_PRECISION_KNOTS )
				u3dGetU64( pDecoder, (U64*)pKnots+j );
			else
				u3dGetU32( pDecoder, (U32*)pKnots+j );
		}
		pSurface->SetKnotArrayV( pKnots );
		pSurface->SetKnotMultiplicityArrayV( pKnotMult );
		delete pKnots;
		delete pKnotMult;

		u3dGetU32( pDecoder, &val );//m_numControlPoints
		u3dGetU32( pDecoder, &val2 );
		NURBSSurfaceDesc allocCP;
		allocCP.m_numControlPointsU = val;
		allocCP.m_numControlPointsV = val2;
		pSurface->Allocate( &allocCP, ffAttrib, &memFunc );
		pCP = new F32[val*val2*CP_STRIDE];
		pWeights = new F32[val*val2];
		for( j = 0; j < pSurfDesc->m_numControlPointsU*
			 pSurfDesc->m_numControlPointsV; j++ )
		{
			u3dGetU32( pDecoder, (U32*)pCP+j*CP_STRIDE );
			u3dGetU32( pDecoder, (U32*)pCP+j*CP_STRIDE+1 );
			u3dGetU32( pDecoder, (U32*)pCP+j*CP_STRIDE+2 );

			//if shading description contains diffuse colors
			u3dGetU32( pDecoder, (U32*)pCP+j*CP_STRIDE+3 );
			u3dGetU32( pDecoder, (U32*)pCP+j*CP_STRIDE+4 );
			u3dGetU32( pDecoder, (U32*)pCP+j*CP_STRIDE+5 );
			u3dGetU32( pDecoder, (U32*)pCP+j*CP_STRIDE+6 );

			//if shading description contains specular colors
			u3dGetU32( pDecoder, (U32*)pCP+j*CP_STRIDE+7 );
			u3dGetU32( pDecoder, (U32*)pCP+j*CP_STRIDE+8 );
			u3dGetU32( pDecoder, (U32*)pCP+j*CP_STRIDE+9 );
			u3dGetU32( pDecoder, (U32*)pCP+j*CP_STRIDE+10 );

			//here should be texture coords

			if(  !surfAttrib & NURBS_SURFACE_NON_RATIONAL )
			{
				if( ffAttrib & NURBS_ATTRIB_DOUBLE_PRECISION_WEIGHTS )
					u3dGetU64( pDecoder, (U64*)pWeights+j );
				else
					u3dGetU32( pDecoder, (U32*)pWeights+j );
			}
		}
		pSurface->SetControlPointsArray( pCP );
		pSurface->SetWeights( pWeights );
		delete pCP;
		delete pWeights;

		u3dGetU32( pDecoder, &faceCnt );
		for( j = 0; j < faceCnt; j++ )
		{
			NURBSFace* pFace = (NURBSFace*)memFunc.alloc
							   ( memFunc.pCtx, sizeof( NURBSFace ) );
			pFace->NURBSFace::NURBSFace();
			pGen->SetFace( i, j, pFace );
			u3dGetU32( pDecoder, &trimLoopCnt );
			pFace->SetTrimCurveLoopCount( trimLoopCnt );
			for( k = 0; k < trimLoopCnt; k++ )
			{
				u3dGetU32( pDecoder, &val );
				pFace->SetTrimLoopAttributes( k, val );
				u3dGetU32( pDecoder, &trimSegmentCnt );
				pFace->SetTrimCurveSegmentsCount( k, trimSegmentCnt );
				for( l = 0; l < trimSegmentCnt; l++ )
				{
					NURBSTrimSegment* pTrimSegment = (NURBSTrimSegment*)
					  memFunc.alloc( memFunc.pCtx, sizeof( NURBSTrimSegment ) );
					pTrimSegment->NURBSTrimSegment::NURBSTrimSegment();
					pFace->SetTrimCurveLoopSegment( k, l, pTrimSegment );
					u3dGetU32( pDecoder, &val );
					pTrimSegment->SetAttributes( val );
					u3dGetU32( pDecoder, &val );
					pTrimSegment->SetDegreeU( val );

					u3dGetU32( pDecoder, &val );//m_numKnotsT
					NURBSSurfaceDesc allocKT;
					allocKT.m_numKnotsU = val;
					pTrimSegment->Allocate( &allocKT, ffAttrib, &memFunc );
					pSurfDesc = pTrimSegment->GetNURBSSurfaceDesc();
					pKnots = new F32[pSurfDesc->m_numKnotsU];
					pKnotMult = new U32[pSurfDesc->m_numKnotsU];
					for( m = 0; m < pSurfDesc->m_numKnotsU; m++ )
					{
						u3dGetU32( pDecoder, pKnotMult+m );
						if( ffAttrib & NURBS_ATTRIB_DOUBLE_PRECISION_KNOTS )
							u3dGetU64( pDecoder, (U64*)pKnots+m );
						else
							u3dGetU32( pDecoder, (U32*)pKnots+m );
					}
					pTrimSegment->SetKnotArrayU( pKnots );
					pTrimSegment->SetKnotMultiplicityArrayU( pKnotMult );
					delete pKnots;
					delete pKnotMult;

					u3dGetU32( pDecoder, &val );//m_numControlPointsU
					NURBSSurfaceDesc allocCP;
					allocCP.m_numControlPointsU = val;
					allocCP.m_numControlPointsV = 1;
					pTrimSegment->Allocate( &allocCP, ffAttrib, &memFunc );
					pCP = new F32[val*CP_STRIDET];
					pWeights = new F32[val];
					for( m = 0; m < pSurfDesc->m_numControlPointsU; m++ )
					{
						if( ffAttrib &
							NURBS_ATTRIB_DOUBLE_PRECISION_CONTROL_POINTS )
						{
							//CP-U position
							u3dGetU64( pDecoder, (U64*)pCP+m*CP_STRIDET );
							u3dGetU64( pDecoder, (U64*)pCP+m*CP_STRIDET+1 );
							u3dGetU64( pDecoder, (U64*)pCP+m*CP_STRIDET+2 );

							//CP-V position
							u3dGetU64( pDecoder, (U64*)pCP+m*CP_STRIDET+3 );
							u3dGetU64( pDecoder, (U64*)pCP+m*CP_STRIDET+4 );
							u3dGetU64( pDecoder, (U64*)pCP+m*CP_STRIDET+5 );
						}
						else
						{
							//CP-U position
							u3dGetU32( pDecoder, (U32*)pCP+m*CP_STRIDET );
							u3dGetU32( pDecoder, (U32*)pCP+m*CP_STRIDET+1 );
							u3dGetU32( pDecoder, (U32*)pCP+m*CP_STRIDET+2 );

							//CP-V position
							u3dGetU32( pDecoder, (U32*)pCP+m*CP_STRIDET+3 );
							u3dGetU32( pDecoder, (U32*)pCP+m*CP_STRIDET+4 );
							u3dGetU32( pDecoder, (U32*)pCP+m*CP_STRIDET+5 );
						}

						if(  !surfAttrib & NURBS_SURFACE_NON_RATIONAL )
						{
							if( ffAttrib &
								NURBS_ATTRIB_DOUBLE_PRECISION_WEIGHTS )
								u3dGetU64( pDecoder, (U64*)pWeights+m );
							else
								u3dGetU32( pDecoder, (U32*)pWeights+m );
						}
					}
					pTrimSegment->SetControlPointsArray( pCP );
					pTrimSegment->SetWeights( pWeights );
					delete pCP;
					delete pWeights;

					U32 edgeCount;
					NURBSTrimSegmentAdjacency adj;
					u3dGetU32( pDecoder, &edgeCount );//number of edges
					pTrimSegment->SetEdgeCount( edgeCount );
					for( m = 0; m < edgeCount; m++ )
					{
						u3dGetU32( pDecoder, &(adj.m_loop1) );
						u3dGetU32( pDecoder, &(adj.m_segment1) );
						u3dGetU32( pDecoder, &(adj.m_face) );
						u3dGetU32( pDecoder, &(adj.m_loop2) );
						u3dGetU32( pDecoder, &(adj.m_segment2) );
						pTrimSegment->SetAdjacent( m, adj );
					}
				}
			}
		}
	}
}