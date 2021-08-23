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
@file encoder.cpp

NURBS encoding tool.
*/

#include "memfunc.h"
#include "encoder.h"

U3dEncoder* initializeEncoder( StreamCtx& stream, U3dMemory memFunc,
							   U3dCb_Write write )
{
	U3dEncoder* pEncoder = NULL;
	U3dStatus status = u3dEncoderInit( &pEncoder, &memFunc, &stream, write,
									   (U64)-1 );
	return pEncoder;
}

void finalizeEncoder( StreamCtx& stream, U3dEncoder* pEncoder )
{
	U3dStatus status    = u3dEncoderFlush(pEncoder);
	status    = u3dEncoderDelete(pEncoder);
	if(stream.pFile) {
		fclose(stream.pFile);
		stream.pFile  = 0;
	}

}

void encode( NURBSGenerator* pGen, U3dMemory* pMemFunc, U3dEncoder* pEncoder )
{
	if( !pGen | !pMemFunc | !pEncoder ) return;
	U3dMemory memFunc = *pMemFunc;
	U32 i, j, k, l, m, val;

	U32 ffAttrib = 0;
	pGen->GetAttributes( &ffAttrib );
	u3dAddU32( pEncoder, ffAttrib );

	NURBSDesc* pDesc = pGen->GetNURBSDesc();

	u3dAddU32( pEncoder, pDesc->m_numSurfaces );
	u3dAddU32( pEncoder, pDesc->m_numFaces );

	//edge num
	U32 totalEdgeNum = 0;
	for( i = 0; i < pDesc->m_numSurfaces; i++ )
	{
		U32 faceCount;
		pGen->GetFaceCount( i, &faceCount );
		for( j = 0; j < faceCount; j++ )
		{
			NURBSFace* pFace;
			pGen->GetFace( i, j, &pFace );
			U32 loopCount;
			pFace->GetTrimCurveLoopCount( &loopCount );
			for( k = 0; k < loopCount; k++ )
			{
				U32 segCount;
				pFace->GetTrimCurveSegmentsCount( k, &segCount );
				for( l = 0; l < segCount; l++ )
				{
					NURBSTrimSegment* pSeg;
					U32 edgeCount;
					pFace->GetTrimCurveLoopSegment( k, l, &pSeg );
					pSeg->GetEdgeCount( &edgeCount );
					totalEdgeNum += edgeCount;
				}
			}
		}
	}
	u3dAddU32( pEncoder, totalEdgeNum );

	u3dAddU32( pEncoder, pDesc->m_numShadingDescriptions );
	for( i = 0; i < pDesc->m_numShadingDescriptions; i++ )
	{
		pGen->GetNURBSShadingDesc( i, &val );
		u3dAddU32( pEncoder, val );
	}
}





void encodeCont( NURBSGenerator* pGen, U3dMemory* pMemFunc, U3dEncoder* pEncoder )
{
	if( !pGen | !pMemFunc | !pEncoder ) return;
	U3dMemory memFunc = *pMemFunc;
	U32 i, j, k, l, m, val;
	U32 ffAttrib = 0;
	NURBSDesc* pDesc = pGen->GetNURBSDesc();

	u3dAddU32( pEncoder, 0 );//surface start index	
	u3dAddU32( pEncoder, pDesc->m_numSurfaces );//surface end index

	NURBSShadedSurface* pSurface = NULL;
	NURBSSurfaceDesc* pSurfDesc = NULL;
	U32 surfAttrib;
	F32* pKnots = NULL;
	U32* pKnotMult = NULL;
	F32* pCP = NULL;
	F32* pWeights = NULL;

	NURBSFace* pFace = NULL;
	U32 faceCnt, trimLoopCnt, trimSegmentCnt;
	NURBSTrimSegment* pTrimSegment = NULL;
	for( i = 0; i < pDesc->m_numSurfaces; i++ )
	{
		pGen->GetSurface( i, &pSurface );
		pSurface->GetAttributes( &surfAttrib );
		u3dAddU32( pEncoder, surfAttrib );
		pSurface->GetShadingDescription( &val );
		u3dAddU32( pEncoder, val );
		pSurface->GetDegreeU( &val );
		u3dAddU32( pEncoder, val );
		pSurface->GetDegreeV( &val );
		u3dAddU32( pEncoder, val );

		//what is the case in spec when knots can be omitted?
		pSurfDesc = pSurface->GetNURBSSurfaceDesc();
		u3dAddU32( pEncoder, pSurfDesc->m_numKnotsU );
		pSurface->GetKnotArrayU( &pKnots );
		pSurface->GetKnotMultiplicityArrayU( &pKnotMult );
		for( j = 0; j < pSurfDesc->m_numKnotsU; j++ )
		{
			u3dAddU32( pEncoder, pKnotMult[j] );
			if( ffAttrib & NURBS_ATTRIB_DOUBLE_PRECISION_KNOTS )
				u3dAddU64( pEncoder, ((U64*)pKnots)[j] );
			else
				u3dAddU32( pEncoder, ((U32*)pKnots)[j] );
		}
		u3dAddU32( pEncoder, pSurfDesc->m_numKnotsV );
		pSurface->GetKnotArrayV( &pKnots );
		pSurface->GetKnotMultiplicityArrayV( &pKnotMult );
		for( j = 0; j < pSurfDesc->m_numKnotsV; j++ )
		{
			u3dAddU32( pEncoder, pKnotMult[j] );
			if( ffAttrib & NURBS_ATTRIB_DOUBLE_PRECISION_KNOTS )
				u3dAddU64( pEncoder, ((U64*)pKnots)[j] );
			else
				u3dAddU32( pEncoder, ((U32*)pKnots)[j] );
		}
		u3dAddU32( pEncoder, pSurfDesc->m_numControlPointsU );
		u3dAddU32( pEncoder, pSurfDesc->m_numControlPointsV );
		pSurface->GetControlPointsArray( &pCP );
		pSurface->GetWeights( &pWeights );
		for( j = 0; j < pSurfDesc->m_numControlPointsU*
			 pSurfDesc->m_numControlPointsV; j++ )
		{
			u3dAddU32( pEncoder, ((U32*)pCP)[j*CP_STRIDE] );
			u3dAddU32( pEncoder, ((U32*)pCP)[j*CP_STRIDE+1] );
			u3dAddU32( pEncoder, ((U32*)pCP)[j*CP_STRIDE+2] );

			//if shading description contains diffuse colors
			u3dAddU32( pEncoder, ((U32*)pCP)[j*CP_STRIDE+3] );
			u3dAddU32( pEncoder, ((U32*)pCP)[j*CP_STRIDE+4] );
			u3dAddU32( pEncoder, ((U32*)pCP)[j*CP_STRIDE+5] );
			u3dAddU32( pEncoder, ((U32*)pCP)[j*CP_STRIDE+6] );

			//if shading description contains specular colors
			u3dAddU32( pEncoder, ((U32*)pCP)[j*CP_STRIDE+7] );
			u3dAddU32( pEncoder, ((U32*)pCP)[j*CP_STRIDE+8] );
			u3dAddU32( pEncoder, ((U32*)pCP)[j*CP_STRIDE+9] );
			u3dAddU32( pEncoder, ((U32*)pCP)[j*CP_STRIDE+10] );

			//here should be texture coords

			if(  !surfAttrib & NURBS_SURFACE_NON_RATIONAL )
			{
				if( ffAttrib & NURBS_ATTRIB_DOUBLE_PRECISION_WEIGHTS )
					u3dAddU64( pEncoder, ((U64*)pWeights)[j] );
				else
					u3dAddU32( pEncoder, ((U32*)pWeights)[j] );
			}
		}
		pGen->GetFaceCount( i, &faceCnt );
		u3dAddU32( pEncoder, faceCnt );
		for( j = 0; j < faceCnt; j++ )
		{
			pGen->GetFace( i, j, &pFace );
			pFace->GetTrimCurveLoopCount( &trimLoopCnt );
			u3dAddU32( pEncoder, trimLoopCnt );
			for( k = 0; k < trimLoopCnt; k++ )
			{
				pFace->GetTrimLoopAttributes( k, &val );
				u3dAddU32( pEncoder, val );
				pFace->GetTrimCurveSegmentsCount( k, &trimSegmentCnt );
				u3dAddU32( pEncoder, trimSegmentCnt );
				for( l = 0; l < trimSegmentCnt; l++ )
				{
					pFace->GetTrimCurveLoopSegment( k, l, &pTrimSegment );
					pTrimSegment->GetAttributes( &val );
					u3dAddU32( pEncoder, val );
					pTrimSegment->GetDegreeU( &val );
					u3dAddU32( pEncoder, val );
					pSurfDesc = pTrimSegment->GetNURBSSurfaceDesc();
					u3dAddU32( pEncoder, pSurfDesc->m_numKnotsU );
					pTrimSegment->GetKnotArrayU( &pKnots );
					pTrimSegment->GetKnotMultiplicityArrayU( &pKnotMult );
					for( m = 0; m < pSurfDesc->m_numKnotsU; m++ )
					{
						u3dAddU32( pEncoder, pKnotMult[m] );
						if( ffAttrib & NURBS_ATTRIB_DOUBLE_PRECISION_KNOTS )
							u3dAddU64( pEncoder, ((U64*)pKnots)[m] );
						else
							u3dAddU32( pEncoder, ((U32*)pKnots)[m] );
					}

					u3dAddU32( pEncoder, pSurfDesc->m_numControlPointsU );
					//number of CPs in the V dimension is 1
					pTrimSegment->GetControlPointsArray( &pCP );
					pTrimSegment->GetWeights( &pWeights );
					for( m = 0; m < pSurfDesc->m_numControlPointsU; m++ )
					{
						if( ffAttrib &
							NURBS_ATTRIB_DOUBLE_PRECISION_CONTROL_POINTS )
						{
							//CP-U position
							u3dAddU64( pEncoder, ((U64*)pCP)[m*CP_STRIDET] );
							u3dAddU64( pEncoder, ((U64*)pCP)[m*CP_STRIDET+1] );
							u3dAddU64( pEncoder, ((U64*)pCP)[m*CP_STRIDET+2] );

							//CP-V position
							u3dAddU64( pEncoder, ((U64*)pCP)[m*CP_STRIDET+3] );
							u3dAddU64( pEncoder, ((U64*)pCP)[m*CP_STRIDET+4] );
							u3dAddU64( pEncoder, ((U64*)pCP)[m*CP_STRIDET+5] );
						}
						else
						{
							//CP-U position
							u3dAddU32( pEncoder, ((U32*)pCP)[m*CP_STRIDET] );
							u3dAddU32( pEncoder, ((U32*)pCP)[m*CP_STRIDET+1] );
							u3dAddU32( pEncoder, ((U32*)pCP)[m*CP_STRIDET+2] );

							//CP-V position
							u3dAddU32( pEncoder, ((U32*)pCP)[m*CP_STRIDET+3] );
							u3dAddU32( pEncoder, ((U32*)pCP)[m*CP_STRIDET+4] );
							u3dAddU32( pEncoder, ((U32*)pCP)[m*CP_STRIDET+5] );
						}

						if(  !surfAttrib & NURBS_SURFACE_NON_RATIONAL )
						{
							if( ffAttrib &
								NURBS_ATTRIB_DOUBLE_PRECISION_WEIGHTS )
								u3dAddU64( pEncoder, ((U64*)pWeights)[m] );
							else
								u3dAddU32( pEncoder, *((U32*)pWeights+m) );
						}
					}

					U32 edgeCount;
					NURBSTrimSegmentAdjacency adj;
					pTrimSegment->GetEdgeCount( &edgeCount );
					u3dAddU32( pEncoder, edgeCount );//number of edges
					for( m = 0; m < edgeCount; m++ )
					{
						pTrimSegment->GetAdjacent( m, &adj );
						u3dAddU32( pEncoder, adj.m_loop1 );
						u3dAddU32( pEncoder, adj.m_segment1 );
						u3dAddU32( pEncoder, adj.m_face );
						u3dAddU32( pEncoder, adj.m_loop2 );
						u3dAddU32( pEncoder, adj.m_segment2 );
					}
				}
			}
		}
	}
}