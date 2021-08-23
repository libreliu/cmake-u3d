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
@file NURBSGenerator.h

Declaration of the NURBSGenerator interface.
*/

#ifndef NURBSGENERATOR_H
#define NURBSGENERATOR_H

#include "IFXResult.h"
#include "IFXAPI.h"
#include "u3ddefs.h"

#include "NURBSSurface.h"
#include "NURBSFace.h"

typedef struct tagNURBSDesc
{
	U32 m_numSurfaces;
	U32 m_numFaces;
	U32 m_numEdges;
	U32 m_numShadingDescriptions;

	tagNURBSDesc()
	{
		m_numSurfaces = U3D_UNDEF;
		m_numFaces = U3D_UNDEF;
		m_numEdges = U3D_UNDEF;
		m_numShadingDescriptions = U3D_UNDEF;
	}
}NURBSDesc;

enum ENURBSShading
{
	NURBS_SHADING_DIFFUSE = 1,
	NURBS_SHADING_SPECULAR = 1 << 1,
	NURBS_SHADING_TEXTURE = 1 << 2
};

enum ENURBSAttributes
{
	NURBS_ATTRIB_DEFAULT = 0,
	NURBS_ATTRIB_DOUBLE_PRECISION_KNOTS = 1,
	NURBS_ATTRIB_DOUBLE_PRECISION_WEIGHTS = 1 << 1,
	NURBS_ATTRIB_DOUBLE_PRECISION_CONTROL_POINTS = 1 << 2
};

/**
This is the main interface for NURBSGenerator.
Provides access to NURBS description and associated surfaces.
*/
class NURBSGenerator
{
public:
	NURBSGenerator();
	virtual ~NURBSGenerator();
	/**
	*  Allocates all the necessary space required by the NURBSGenerator.
	*
	*  @param pNURBSDesc  The NURBS data structure describing
	*                 the properties of the NURBS.
	*  @param pMemFunc  The set of memory utilities
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  pNURBSDesc is NULL
	*  @retval IFX_E_INVALID_RANGE  Some element of description
	*     is less than one.
	*  @retval IFX_E_OUT_OF_MEMORY  Insufficient memory.
	*/
	virtual IFXRESULT IFXAPI Allocate( const NURBSDesc* pNURBSDesc,
									   U3dMemory* pMemFunc = NULL);

	/**
	*  Frees all data associated with the NURBSGenerator.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*/
	virtual IFXRESULT IFXAPI Deallocate();

	/**
	*  Returns the current NURBSGenerator description.
	*
	*  @return A pointer to a NURBSDesc structure describing the
	*     NURBSGenerator.
	*/
	virtual NURBSDesc* IFXAPI GetNURBSDesc() const;

	/**
	*  Returns the value of NURBS attributes.
	*
	*  @param  pAttrib  a pointer to the value of attributes.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  pAttrib is NULL
	*/
    virtual IFXRESULT IFXAPI GetAttributes( U32* pAttrib ) const;

	/**
	*  Sets the value of NURBS attributes.
	*
	*  @param  attrib  the value of attributes.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*/
    virtual IFXRESULT IFXAPI SetAttributes( const U32 attrib );

	/**
	*  Sets the shading description of a control point.
	*
	*  @param shadingIdx  Index of desired shading description.
	*  @param NURBSShadingDesc  A combination of ENURBSShading values
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_RANGE  shadingIdx is greater than number of
	*	shading descriptions in NURBSDesc
	*/
	virtual IFXRESULT IFXAPI SetNURBSShadingDesc( U32 shadingIdx,
												  const U32 NURBSShadingDesc );

	/**
	*  Returns the shading description of a control point.
	*
	*  @param shadingIdx  Index of desired shading description.
	*  @param pNURBSShadingDesc  A pointer to a set of ENURBSShading values
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_POINTER  pNURBSShadingDesc is NULL
	*  @retval IFX_E_INVALID_RANGE  shadingIdx is greater than number of
	*	shading descriptions in NURBSDesc
	*/
	virtual IFXRESULT IFXAPI GetNURBSShadingDesc( U32 shadingIdx,
												  U32* pNURBSShadingDesc );

	/**
	*  Finds a free-form surface which is a part of NURBS.
	*
	*  @param  surfInd  Index of desired surface.
	*  @param  ppSurface  a pointer to the surface.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_RANGE surfInd is negative or greater than
	*     surface count.
	*  @retval IFX_E_INVALID_POINTER  ppSurface is NULL
	*/
    virtual IFXRESULT IFXAPI GetSurface( U32 surfInd,
										 NURBSShadedSurface** ppSurface ) const;
	/**
	*  Sets a free-form surface as a part of NURBS.
	*
	*  @param  surfInd  Index of desired surface.
	*  @param  pSurface  a pointer to the surface.
	*  @param  shaderID  Index of shading description of surface.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_RANGE surfInd is negative or greater than
	*     surface count.
	*/
	virtual IFXRESULT IFXAPI SetSurface( U32 surfInd, NURBSShadedSurface*
										 pSurface, U32 shaderID );

	/**
	*  Finds a trimming sequence cutting a face from a free-form surface.
	*
	*  @param  surfInd  Index of surface associated with the desired face.
	*  @param  faceInd  Index of the desired face in the surface.
	*  @param  ppFace  a pointer to the face.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_RANGE surfInd is negative or greater than
	*     surface count and/or facefInd is negative or greater than
	*     face count in the surface.
	*  @retval IFX_E_INVALID_POINTER  ppFace is NULL
	*/
    virtual IFXRESULT IFXAPI GetFace( U32 surfIdx, U32 faceIdx,
									  NURBSFace** ppFace ) const;
	/**
	*  Sets a trimming sequence for a free-form surface.
	*
	*  @param  surfInd  Index of surface associated with the face being created.
	*  @param  faceInd  Index of the face in the surface.
	*  @param  pFace  a pointer to the face.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_RANGE surfInd is negative or greater than
	*     surface count and/or facefInd is negative or greater than
	*     face count in the surface.
	*/
	virtual IFXRESULT IFXAPI SetFace( U32 surfIdx, U32 faceIdx,
									  NURBSFace* pFace );
	/**
	*  Returns a number of faces associated with a certain surface.
	*
	*  @param  surfInd  Index of surface.
	*  @param  pFaceCount  a pointer to the face count value.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVALID_RANGE surfInd is negative or greater than
	*     surface count
	*  @retval IFX_E_INVALID_POINTER  pFaceCount is NULL
	*/
    virtual IFXRESULT IFXAPI GetFaceCount( U32 surfIdx, U32* pFaceCount ) const;

private:
	U3dMemory m_memFunc;
	NURBSDesc m_NURBSDesc;
	NURBSShadedSurface** m_ppSurfaces;
	NURBSFace** m_ppFaces;
	U32* m_pShadingDescriptions;
	U32 m_attrib;
	U32 m_lastAllocFace;
};

#endif
