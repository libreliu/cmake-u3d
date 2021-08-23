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


#include "IFXModifierChain.h"
#include "IFXModifierDataPacket.h"

#include "IFXSceneGraph.h"
#include "IFXModel.h"
#include "IFXNode.h"
#include "IFXView.h"
#include "IFXPalette.h"
#include "IFXMatrix4x4.h"
#include "IFXQuaternion.h"

#include "U3DCameraNavigation.h"

//-------------------------------------------------------------------------------------------------
/**
@brief		Translates camera along X and/or Y axis.

@param		F32				fInX				Translation along X axis.
@param		F32				fInY				Translation along Y axis.
@param		F32				filmPlaneDistance	How far the camera is from the film plane
@param		IFXVector3		pivotPoint			The pivot point for our world
@param		IFXMatrix4x4	*pViewMatrix	The new view matrix


@return	void
**/
//-------------------------------------------------------------------------------------------------

IFXRESULT PanView( F32			fInX, 
				  F32			fInY,
				  IFXMatrix4x4	*pViewMatrix)
{
	IFXRESULT		result = IFX_OK;

	if (pViewMatrix) {
		IFXVector3 panVector( -fInX, fInY, 0.0f);
		pViewMatrix->Translate3x4( panVector );
	} else 
		result = IFX_E_NOT_INITIALIZED;

	return result;
}

//-------------------------------------------------------------------------------------------------
/**
@brief		Rotates camera.

@param		F32 fInX		Rotation (in radians) about X axis.
@param	    F32 fInY		Rotation (in radians) about Y axis.
@param	    BOOL mod		Modifier to switch to rotation about Z axis
@param		IFXVector3		pivotPoint			The pivot point for our world
@param		IFXMatrix4x4	*pViewMatrix		The new view matrix

@return	void
**/
//-------------------------------------------------------------------------------------------------

IFXRESULT RotateView( F32 fInX, 
				F32 fInY,
				BOOL mod,
				BOOL yKeyDown,
				IFXVector3		pivotPoint,
				IFXMatrix4x4	*pViewMatrix)
{
	IFXRESULT result = IFX_OK;
	F32 radians;

	if (pViewMatrix) {
		if ( mod )
		{
			if ( fInX != 0 )
			{
				// Roll the camera in camera space
				radians = ToRadians(fInX * 0.5f);
				pViewMatrix->Rotate3x4( radians, IFX_Z_AXIS );
			}
		}
		else
		{
			if ( fInY != 0 )
			{
				F32 *rawMatrix=pViewMatrix->Raw();

				// Tilt camera in world space about pivot
				radians = ToRadians(-fInY * 0.5f);

				IFXVector3 right(rawMatrix[0], rawMatrix[1], rawMatrix[2]);
				right.Normalize();

				// Build axis-angle rotation matrix
				IFXQuaternion	quat;
				quat.MakeRotation(radians, right);

				IFXMatrix4x4	mAngleAxisRot = quat;
				IFXMatrix4x4	mIntermediate(rawMatrix);

				// rather than actually compositing these matrices, build
				// the translation component of TRT^-1 directly (faster).

				rawMatrix[12] = -(pivotPoint.X()*rawMatrix[0] +
					pivotPoint.Y()*rawMatrix[4] +
					pivotPoint.Z()*rawMatrix[8]) + pivotPoint.X();
				rawMatrix[13] = -(pivotPoint.X()*rawMatrix[1] +
					pivotPoint.Y()*rawMatrix[5] +
					pivotPoint.Z()*rawMatrix[9]) + pivotPoint.Y();
				rawMatrix[14] = -(pivotPoint.X()*rawMatrix[2] +
					pivotPoint.Y()*rawMatrix[6] +
					pivotPoint.Z()*rawMatrix[10]) + pivotPoint.Z();


				// Now apply this rotation about the pivot point
				pViewMatrix->Multiply3x4(mAngleAxisRot, mIntermediate);		
			}

			if ( fInX != 0 )
			{
				F32 *rawMatrix=pViewMatrix->Raw();

				// compute up vector
				IFXVector3 up(0.0f, 0.0f, 0.0f);
				if ( yKeyDown )
					up.Y() = 1.0f;		// Y is UP
				else
					up.Z() = 1.0f;		// Z is UP

				// compute rotation angle
				radians = ToRadians(-fInX * 0.5f);

				// Rotate camera about Up axis in world space about pivot

				// Build axis-angle rotation matrix
				IFXQuaternion	quat;
				quat.MakeRotation(radians, up);

				IFXMatrix4x4	mAngleAxisRot = quat;
				IFXMatrix4x4	mIntermediate(rawMatrix);

				// rather than actually compositing these matrices, build
				// the translation component of TRT^-1 directly (faster).

				rawMatrix[12] = -(pivotPoint.X()*rawMatrix[0] +
					pivotPoint.Y()*rawMatrix[4] +
					pivotPoint.Z()*rawMatrix[8]) + pivotPoint.X();
				rawMatrix[13] = -(pivotPoint.X()*rawMatrix[1] +
					pivotPoint.Y()*rawMatrix[5] +
					pivotPoint.Z()*rawMatrix[9]) + pivotPoint.Y();
				rawMatrix[14] = -(pivotPoint.X()*rawMatrix[2] +
					pivotPoint.Y()*rawMatrix[6] +
					pivotPoint.Z()*rawMatrix[10]) + pivotPoint.Z();


				// Now apply this rotation about the pivot point
				pViewMatrix->Multiply3x4(mAngleAxisRot, mIntermediate);
			}
		}
	} else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}





//-------------------------------------------------------------------------------------------------
/**
@brief		Translates camera along X and/or Z axis.

@param		F32 fInX		Translation along X axis.
@param	    F32 fInY		Translation along Z axis.
@param		BOOL mod		Scales zoom amount.
@param		IFXMatrix4x4	*pViewMatrix		The new view matrix

@return	void
**/
//-------------------------------------------------------------------------------------------------

IFXRESULT ZoomView( F32	fInX, 
			  F32	fInY,
			  BOOL	mod,
			  IFXMatrix4x4	*pViewMatrix)
{
	IFXRESULT result = IFX_OK;

	if (pViewMatrix) {
		// Get world space camera position
		IFXVector3 camPt(0.0f, 0.0f, 0.0f);
		F32 scaleFactor = (mod) ? 0.01f : 0.001f;

		camPt = pViewMatrix->TranslationConst();

		// Compute an amount to move along the Z axis in camera space.
		// This is a function of the distance of the camera to the origin.
		F32 distance = camPt.CalcMagnitude() * fInY * scaleFactor;
		if ( fabs(distance) < fabs(fInY) ) 
		{
			distance = fInY;
		}

		IFXVector3 transVector(0.0f, 0.0f, distance);

		// translate in camera space
		pViewMatrix->Translate3x4(transVector);
	} else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}



//-------------------------------------------------------------------------------------------------
/**
@brief		Computes a pivot point.

@param		IFXPalette	*pNodePalette		IFX Node palette.
@param		IFXVector3	*pPivotPoint	The pivot point for our world

@return	void
**/
//-------------------------------------------------------------------------------------------------

IFXRESULT ComputePivot( IFXPalette * pNodePalette, 
					   IFXVector3 *pPivotPoint )
{
	IFXRESULT	result = IFX_OK;
	IFXVector3	center(0.0f, 0.0f, 0.0f);
	U32			palSize = 0;
	U32			nodeID  = 0;
	U32			count   = 0;
	U32			modelCount = 0;

	// check inputs
	if ( !pNodePalette )
		return IFX_E_INVALID_POINTER;

	// Get the first node in the palette
	result = pNodePalette->GetPaletteSize( &palSize );

	if ( IFXSUCCESS(result) ) 
	{
		result = pNodePalette->First(&nodeID);
	}

	if ( IFXSUCCESS(result) )
	{
		for (count = 0; count < palSize; ++count)
		{
			IFXUnknown *pUnknown = NULL;

			if ( IFXSUCCESS(result) && (count > 0) )
				result = pNodePalette->Next(&nodeID);

			// Get the resource for the node at this palette ID
			if ( IFXSUCCESS(result) )
				result = pNodePalette->GetResourcePtr(nodeID, &pUnknown);

			if (IFX_E_PALETTE_NULL_RESOURCE_POINTER == result)
				result = IFX_OK;  // All palettes have a "null" entry in
			// them.  This is OK.

			// is it a model?
			if ( pUnknown && IFXSUCCESS(result) )
			{
				IFXNode		*pNode = NULL;
				IFXModel	*pModel = NULL;

				if ( IFX_OK == pUnknown->QueryInterface( IID_IFXModel,
					(void**)&pModel))
				{
					IFXModifierChain			*pModifierChain = NULL;
					IFXModifierDataPacket		*pMDP = NULL;
					IFXBoundSphereDataElement	*pBoundSphere = NULL;
					U32							uBoundsElementIndex = 0;

					IFXRELEASE(pModel);

					result = pUnknown->QueryInterface( IID_IFXNode,
						(void**)&pNode);

					if (pNode && IFXSUCCESS(result))
					{
						result = pNode->GetModifierChain( &pModifierChain );
					}

					if (pModifierChain && IFXSUCCESS(result))
					{
						result = pModifierChain->GetDataPacket( pMDP );
					}

					if (pMDP  && IFXSUCCESS(result))
					{
						IFXUnknown	*pUnknown = NULL;

						if ((IFX_OK == pMDP->GetDataElementIndex( 
							DID_IFXRenderableGroupBounds,
							uBoundsElementIndex )))
						{
							if (pMDP  && IFXSUCCESS(result))
							{
								result = pMDP->GetDataElement( uBoundsElementIndex,
									(void**)&pUnknown );   // Does not AddRef
							}

							if (pUnknown && IFXSUCCESS(result))
							{
								result = pUnknown->QueryInterface(IID_IFXBoundSphereDataElement,
									(void **)&pBoundSphere);
							}

							if (pBoundSphere && IFXSUCCESS(result))
							{
								center.X() += pBoundSphere->Bound().X();
								center.Y() += pBoundSphere->Bound().Y();
								center.Z() += pBoundSphere->Bound().Z();
							}

							IFXRELEASE(pBoundSphere);

							++modelCount;
						}  // We have a mesh group bounding sphere

						IFXRELEASE(pMDP);
					}

					IFXRELEASE(pModifierChain);
					IFXRELEASE(pNode);
				}  // we have a model node!

				IFXRELEASE( pUnknown );
			} // we got an entry from the palette OK

		}  // for all nodes in the node palette

		// Now take the average of the bounding sphere centers.
		// This will be the pivot point.
		if ( modelCount > 1 ) 
		{
			F32 f = 1.0f / (F32)modelCount;
			center.X() *= f;
			center.Y() *= f;
			center.Z() *= f;
		}
	}  // if success

	if ( IFXSUCCESS(result) )
		pPivotPoint->Set(center.X(), center.Y(), center.Z());

	return result;
}
