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
@file NURBSSurface.h

Declaration of the NURBSSurface interface.
*/

#ifndef NURBSSURFACE_H
#define NURBSSURFACE_H

#include "IFXResult.h"
#include "IFXAPI.h"
#include "u3ddefs.h"

//this stride is also used for storing trimming control points
#define CP_STRIDE 11

//maximum stride for trimming control point
//(achieved by using double precision)
#define CP_STRIDET 8

typedef struct tagNURBSSurfaceDesc
{
	U32 m_numKnotsU;
	U32 m_numKnotsV;
	U32 m_numControlPointsU;
	U32 m_numControlPointsV;
	U32 m_maxNumTextureLayers;//non-applicable for trim curve segments
	U32 m_maxNumTextureDimensions;//non-applicable for trim curve segments

	tagNURBSSurfaceDesc()
	{
		m_numKnotsU = U3D_UNDEF;
		m_numKnotsV = U3D_UNDEF;
		m_numControlPointsU = U3D_UNDEF;
		m_numControlPointsV = U3D_UNDEF;
		m_maxNumTextureLayers = U3D_UNDEF;
		m_maxNumTextureDimensions = U3D_UNDEF;
	}
}NURBSSurfaceDesc;

enum ENURBSSurfaceAttributes
{
	NURBS_SURFACE_DEFAULT = 0,
	NURBS_SURFACE_NON_RATIONAL = 1,
	NURBS_SURFACE_BEZIER = 1 << 1,
	NURBS_SURFACE_B_SPLINE = 1 << 2
};

enum ENURBSTrimSegmentAttributes
{
	NURBS_SEGMENT_DEFAULT = 0,
	NURBS_SEGMENT_NON_RATIONAL = 1,
	NURBS_SEGMENT_BEZIER = 1 << 1,
	NURBS_SEGMENT_B_SPLINE = 1 << 2,
	NURBS_SEGMENT_MINIMUM_U = 1 << 3,
	NURBS_SEGMENT_MAXIMUM_U = 1 << 4,
	NURBS_SEGMENT_MINIMUM_V = 1 << 5,
	NURBS_SEGMENT_MAXIMUM_V = 1 << 6
};

typedef struct tagNURBSTrimSegmentAdjacency
{
	U32 m_loop1;	//Index of the current loop.
	U32 m_segment1;	//Index of the adjacent segment in this loop.
	U32 m_face;		//Index of the face containing the adjacent segment.
	U32 m_loop2;	//Index of the loop in the face containing adjacent segment.
	U32 m_segment2;	//Index of the adjacent segment in this loop.
}NURBSTrimSegmentAdjacency;

/**
This is the main interface for NURBSSurface.
Provides access to surface properties.
Trim curve can also be considered as a surface, but shading properties
cannot be applied to a trim curve.
*/
class NURBSSurface
{
public:
	NURBSSurface();
	virtual ~NURBSSurface();

	/**
	*  Allocates all the necessary space required by the NURBSSurface.
	* 
	*
	*  @param pNURBSSurfaceDesc  The NURBS data structure describing
	*                 the properties of the surface.
	*  @param freeFormAttributes  The free-form object attributes
	*  @param pMemFunc  The set of memory utilities
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  pNURBSSurfaceDesc is NULL
	*  @retval IFX_E_INVALID_RANGE  Some element of description
	*     is less than one.
	*  @retval IFX_E_OUT_OF_MEMORY  Insufficient memory.
	*/
	virtual IFXRESULT IFXAPI Allocate( const NURBSSurfaceDesc*
		pNURBSSurfaceDesc, U32 freeFormAttributes, U3dMemory* pMemFunc = NULL );

	/**
	*  Frees all data associated with the NURBSSurface.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*/
	virtual IFXRESULT IFXAPI Deallocate() ;

	/**
	*  Returns the current surface description.
	*
	*  @return A pointer to a NURBSSurfaceDesc structure describing the
	*     NURBSSurface.
	*/
	virtual NURBSSurfaceDesc* IFXAPI GetNURBSSurfaceDesc() const;

	/**
	*  Sets the current surface description.
	*
	*  @param pNURBSSurfaceDesc  The new surface description
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  pNURBSSurfaceDesc is NULL
	*  @retval IFX_E_INVALID_RANGE One of the values is
	*     larger than the description used to allocate the surface.
	*/
	virtual IFXRESULT IFXAPI SetNURBSSurfaceDesc( const NURBSSurfaceDesc*
												  pNURBSSurfaceDesc );

	/**
	*  Returns the value of surface attributes.
	*
	*  @param  pAttrib  a pointer to the value of attributes.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  pAttrib is NULL
	*/
    virtual IFXRESULT IFXAPI GetAttributes( U32* pAttrib ) const;

	/**
	*  Sets the value of surface attributes.
	*
	*  @param  attrib  the value of attributes.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*/
    virtual IFXRESULT IFXAPI SetAttributes( const U32 attrib );

	/**
	*  Returns the U-degree of the surface (sometimes also called "order").
	*
	*  @param  pDegree  a pointer to the degree.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  pDegree is NULL
	*/
    virtual IFXRESULT IFXAPI GetDegreeU( U32* pDegree ) const;

	/**
	*  Sets the U-degree of the surface.
	*
	*  @param  degree the index.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*/
    virtual IFXRESULT IFXAPI SetDegreeU( const U32 degree );

	/**
	*  Returns the V-degree of the surface (sometimes also called "order").
	*
	*  @param  pDegree  a pointer to the degree.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  pDegree is NULL
	*/
    virtual IFXRESULT IFXAPI GetDegreeV( U32* pDegree ) const;

	/**
	*  Sets the V-degree of the surface.
	*
	*  @param  degree the index.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*/
    virtual IFXRESULT IFXAPI SetDegreeV( const U32 degree );

	/**
	*  Returns the array of knot multiplicity in the U dimension.
	*
	*  @param  ppKnotMultU  array of knot multiplicity.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  ppKnotMultU is NULL
	*/
    virtual IFXRESULT IFXAPI GetKnotMultiplicityArrayU( U32**
														ppKnotMultU ) const;

	/**
	*  Sets the array of knot multiplicity in the U dimension.
	*
	*  @param  pKnotMultU  array of knot multiplicity.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  pKnotMultU is NULL
	*/
    virtual IFXRESULT IFXAPI SetKnotMultiplicityArrayU( U32* pKnotMultU );

	/**
	*  Returns the array of knot values in the U dimension.
	*
	*  @param  ppKnotU  array of knot values.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  ppKnotU is NULL
	*/
    virtual IFXRESULT IFXAPI GetKnotArrayU( F32** ppKnotU ) const;

	/**
	*  Sets the array of knot values in the U dimension.
	*
	*  @param  pKnotU  array of knot values.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  pKnotU is NULL
	*/
    virtual IFXRESULT IFXAPI SetKnotArrayU( F32* pKnotU );

	/**
	*  Returns the array of knot multiplicity in the V dimension.
	*
	*  @param  ppKnotMultV  array of knot multiplicity.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  ppKnotMultV is NULL
	*/
    virtual IFXRESULT IFXAPI GetKnotMultiplicityArrayV( U32**
														ppKnotMultV ) const;

	/**
	*  Sets the array of knot multiplicity in the V dimension.
	*
	*  @param  pKnotMultV  array of knot multiplicity.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  pKnotMultV is NULL
	*/
    virtual IFXRESULT IFXAPI SetKnotMultiplicityArrayV( U32* pKnotMultV );

	/**
	*  Returns the array of knot values in the V dimension.
	*
	*  @param  ppKnotV  array of knot values.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  ppKnotV is NULL
	*/
    virtual IFXRESULT IFXAPI GetKnotArrayV( F32** pKnotV ) const;

	/**
	*  Sets the array of knot values in the V dimension.
	*
	*  @param  pKnotV  array of knot values.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  pKnotV is NULL
	*/
    virtual IFXRESULT IFXAPI SetKnotArrayV( F32* pKnotV );

	/**
	*  Returns the array of control points.
	*
	*  @param  ppCP  array of control points.
	*  @param  stride  expected stride between adjoining points in the array.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  ppCP is NULL
	*/
    virtual IFXRESULT IFXAPI GetControlPointsArray( F32** ppCP,
		U32 stride = CP_STRIDE ) const;

	/**
	*  Sets the array of control points.
	*
	*  @param  pCP  array of control points.
	*  @param  stride  expected stride between adjoining points in the array.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  pCP is NULL
	*/
    virtual IFXRESULT IFXAPI SetControlPointsArray( F32* pCP,
													U32 stride = CP_STRIDE );

	/**
	*  Returns the array of texture coords of control points.
	*
	*  @param  pTexCoords  array of texture coords of control points.
	*  @param  pStride  expected stride between adjoining points in the array.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  pTexCoords or pStride is NULL
	*/
    virtual IFXRESULT IFXAPI GetTexCoords( U32* pTexCoords, U32* pStride )const;

	/**
	*  Returns the array of texture coords of control points.
	*
	*  @param  pTexCoords  array of texture coords of control points.
	*  @param  pStride  expected stride between adjoining points in the array.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  pTexCoords is NULL
	*/
    virtual IFXRESULT IFXAPI SetTexCoords( U32* pTexCoords, U32 stride=32*4*8 );
	//layer count = 8??? where is it defined?

	/**
	*  Returns the array of weights of control points.
	*
	*  @param  ppWeights  array of weights of control points.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  pWeights or pStride is NULL
	*/
    virtual IFXRESULT IFXAPI GetWeights( F32** ppWeights ) const;

	/**
	*  Sets the array of weights of control points.
	*
	*  @param  pWeights  array of weights of control points.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  pWeights is NULL
	*/
    virtual IFXRESULT IFXAPI SetWeights( F32* pWeights );
protected:
	U3dMemory m_memFunc;
private:
	NURBSSurfaceDesc m_NURBSSurfaceDesc;
    U32* m_pKnotMultU;
    U32* m_pKnotMultV;
	F32* m_pKnotU;
	F32* m_pKnotV;
	F32* m_pControlPoints;//only position and vertex colors
	F32* m_pTextureCoords;
	F32* m_pWeights;

	U32 m_knotPrec;
	U32 m_weightPrec;
	U32 m_CPPrec;

	U32 m_attrib;
	U32 m_degreeU;
	U32 m_degreeV;
};

class NURBSShadedSurface : public NURBSSurface
{
public:
	NURBSShadedSurface();
	virtual ~NURBSShadedSurface() {}

	/**
	*  Returns the shading description index referring to
	*  the NURBS generator shading description array.
	*
	*  @param  pDesc  a pointer to the index.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  ppDesc is NULL
	*/
    virtual IFXRESULT IFXAPI GetShadingDescription( U32* pDesc ) const;

	/**
	*  Sets the shading description index.
	*
	*  @param  desc the index.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*/
    virtual IFXRESULT IFXAPI SetShadingDescription( const U32 desc );
private:
	U32 m_shader;//shading description
};

class NURBSTrimSegment : public NURBSSurface
{
public:
	NURBSTrimSegment();
	virtual ~NURBSTrimSegment();

	/**
	*  Frees all data associated with the NURBSTrimSegment.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*/
	virtual IFXRESULT IFXAPI Deallocate() ;

	/**
	*  Returns the number of faces adjacent to *this.
	*
	*  @param  pCount	 Edge count
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*/
	virtual IFXRESULT IFXAPI GetEdgeCount( U32* pCount );

	/**
	*  Sets the number of faces adjacent to *this.
	*
	*  @param  count	 Edge count
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_OUT_OF_MEMORY  Not enough memory
	*/
	virtual IFXRESULT IFXAPI SetEdgeCount( U32 count );

	/**
	*  Returns the index of ith trim segment adjacent to *this.
	*
	*  @param  i		Index of the adjacent segment in the array.
	*  @param  pAdj		Adjacency structure
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  pAdj is NULL
	*/
	virtual IFXRESULT IFXAPI GetAdjacent( U32 i, NURBSTrimSegmentAdjacency*
										  pAdj ) const;

	/**
	*  Sets the index of ith trim segment adjacent to *this.
	*
	*  @param  i		Index of the adjacent segment in the array.
	*  @param  adj		Adjacency structure
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*/
    virtual IFXRESULT IFXAPI SetAdjacent( U32 i, NURBSTrimSegmentAdjacency adj);
private:
	NURBSTrimSegmentAdjacency* m_pAdjacent;
	U32 m_edgeCount;
};
#endif
