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
@file  CIFXCLODModifier2.cpp

The implementation file of the CIFXCLODModifier2 component.      
*/

#include "CIFXCLODModifier2.h"
#include "IFXMeshGroup.h"
#include "IFXModifierDataPacket.h"
#include "IFXCoreCIDs.h"
#include "IFXDids.h"
#include "IFXEnums.h"
#include "IFXFrustum.h"
#include "IFXBoundSphereDataElement.h"
#include "IFXArray.h"
#include "IFXMatrix4x4.h"

#include "IFXShapesCIDs.h"

const U32 IFX_MESHGROUP_CB_FACECOUNT  	= 1<<1;
const U32 IFX_MESHGROUP_CB_VERTEXCOUNT	= 1<<2;
const U32 IFX_MESHGROUP_CB_FACES	  	= 1<<16;
const U32 IFX_MESHGROUP_CB_ALL			= 0x00FFFFFF;

CIFXCLODModifier2::CIFXCLODModifier2()
{
	m_uRefCount = 0;

	m_uMeshGroupIndex = (U32)-1;
	m_uMeshGroupChangeCount = 0;

	m_uCLODManagerItfcIndex = (U32)-1;
	m_uCLODManagerItfcCount = 0;

	m_uFrustrumIndex = (U32)-1;
	m_uViewSizeIndex = (U32)-1;
	m_uTransformIndex = (U32)-1;
	m_uMeshGroupBoundsIndex = (U32)-1;

	// IFXCLODModifier
	m_bCLODScreenSpaceMetricOn = TRUE;
	m_fCLODScreenSpaceBias = 1.0f;
	m_fCLODLevel = 1.0f;

	m_pMeshGroup = NULL;
	m_pCLODManager = NULL;
	m_pModifierDataPacket = NULL;
}


CIFXCLODModifier2::~CIFXCLODModifier2()
{
	IFXRELEASE(m_pMeshGroup);
	IFXRELEASE(m_pCLODManager);
}


U32 CIFXCLODModifier2::AddRef(void)
{
	return ++m_uRefCount;
}


U32 CIFXCLODModifier2::Release(void)
{
	if ( 1 == m_uRefCount )
	{
		PreDestruct();
		delete this;
		return 0;
	}

	return --m_uRefCount;
}


IFXRESULT CIFXCLODModifier2::QueryInterface( IFXREFIID interfaceId,
										   void**    ppInterface )
{
	IFXRESULT result = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else if ( interfaceId == IID_IFXCLODModifier )
			*ppInterface = ( IFXCLODModifier* ) this;
		else if ( interfaceId == IID_IFXMarker )
			*ppInterface = ( IFXMarker* ) this;
		else if ( interfaceId == IID_IFXMarkerX )
			*ppInterface = ( IFXMarkerX* ) this;
		else if ( interfaceId == IID_IFXModifier)
			*ppInterface = ( IFXModifier* ) this;
		else if ( interfaceId == IID_IFXSubject )
			*ppInterface = ( IFXSubject* ) this;
		else if ( interfaceId == IID_IFXMetaDataX )
			*ppInterface = ( IFXMetaDataX* ) this;
		else
		{
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( result ) )
			AddRef();
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


IFXRESULT IFXAPI_CALLTYPE CIFXCLODModifier2_Factory(IFXREFIID riid, void **ppv)
{
	IFXRESULT result;

	if ( ppv )
	{
		// Create the CIFXClassName component.
		CIFXCLODModifier2 *pCLODModifier = new CIFXCLODModifier2;

		if ( pCLODModifier )
		{
			// Perform a temporary AddRef for our usage of the component.
			pCLODModifier->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pCLODModifier->QueryInterface( riid, ppv );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pCLODModifier->Release();
		}
		else
			result = IFX_E_OUT_OF_MEMORY;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


// IFXMarkerX
void CIFXCLODModifier2::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
	/// @todo: Consider renaming CID_IFXCLODGeneratorParamEncoder to something
	/// that is more indicative of its functionality (i.e. it actually encodes
	/// CLOD modifiers not CLOD mod params).
	CIFXMarker::GetEncoderX(CID_IFXCLODModifier2Encoder, rpEncoderX);
}

// IFXModifier
static const IFXGUID* s_pOutputDIDs[] =
{
	&DID_IFXRenderableGroup,
	&DID_IFXCLODController
};

static const U32 s_pMeshGroupUnchangedAttrs[] =
{
	IFX_MESHGROUP_CB_ALL ^
		(IFX_MESHGROUP_CB_FACES | IFX_MESHGROUP_CB_FACECOUNT | IFX_MESHGROUP_CB_VERTEXCOUNT),
		0
};

static const IFXGUID* s_pMeshGroupInputDIDs[] =
{
	&DID_IFXRenderableGroup,
	&DID_IFXCLODController,
	&DID_IFXFrustum,
	&DID_IFXViewSize,
	&DID_IFXTransform,
	&DID_IFXRenderableGroupBounds
};

static const IFXGUID* s_pMeshGroupNoViewInputDIDs[] =
{
	&DID_IFXRenderableGroup,
	&DID_IFXCLODController,
	&DID_IFXTransform,
	&DID_IFXRenderableGroupBounds
};

static const IFXGUID* s_pCLODInputDIDs[] =
{
	&DID_IFXCLODController
};


static const IFXGUID* s_pCLODIntraDIDs[] =
{
	&DID_IFXRenderableGroup
};



static const U32 s_pCLODIntraAttrs[] =
{
	//  IFX_MESHGROUP_CB_ALL ^
	//    (IFX_MESHGROUP_CB_FACES | IFX_MESHGROUP_CB_FACECOUNT | IFX_MESHGROUP_CB_VERTEXCOUNT)

	IFX_MESHGROUP_CB_FACES | IFX_MESHGROUP_CB_FACECOUNT | IFX_MESHGROUP_CB_VERTEXCOUNT
};

IFXRESULT CIFXCLODModifier2::GetOutputs( IFXGUID**& rpOutOutputs,
									   U32&       rOutNumberOfOutputs,
									   U32*&      rpOutOutputDepAttrs )
{
	rOutNumberOfOutputs = sizeof(s_pOutputDIDs)/ sizeof(IFXGUID*);
	rpOutOutputs = (IFXGUID**)&s_pOutputDIDs;
	rpOutOutputDepAttrs = (U32*)s_pMeshGroupUnchangedAttrs;
	return IFX_OK;
}


IFXRESULT CIFXCLODModifier2::GetDependencies(
	IFXGUID*   pInOutputDID,
	IFXGUID**& rppOutInputDependencies,
	U32&       rOutNumberInputDependencies,
	IFXGUID**& rppOutOutputDependencies,
	U32&       rOutNumberOfOutputDependencies,
	U32*&      rpOutOutputDepAttrs )
{
	rOutNumberInputDependencies = 0;
	rOutNumberOfOutputDependencies = 0;
	rpOutOutputDepAttrs = 0;

	if ( *pInOutputDID == DID_IFXRenderableGroup )
	{
		// IFXMeshGroup
		if (m_bCLODScreenSpaceMetricOn) {
			rppOutInputDependencies = (IFXDID**) s_pMeshGroupInputDIDs;
			rOutNumberInputDependencies = sizeof(s_pMeshGroupInputDIDs)/sizeof(IFXGUID*);
		} else {
			rppOutInputDependencies = (IFXDID**) s_pMeshGroupNoViewInputDIDs;
			rOutNumberInputDependencies = sizeof(s_pMeshGroupNoViewInputDIDs)/sizeof(IFXGUID*);
		}
	}
	else if ( *pInOutputDID == DID_IFXCLODController )
	{   // IFXCLODManager
		rppOutInputDependencies = (IFXDID**) s_pCLODInputDIDs;
		rOutNumberInputDependencies = sizeof(s_pCLODInputDIDs)/sizeof(IFXGUID*);
		rppOutOutputDependencies = (IFXDID**) s_pCLODIntraDIDs;
		rOutNumberOfOutputDependencies = sizeof(s_pCLODIntraDIDs)/sizeof(IFXGUID*);
		rpOutOutputDepAttrs = (U32*)s_pCLODIntraAttrs;
	}
	else
	{
		return IFX_E_UNSUPPORTED;
	}

	return IFX_OK;
}


IFXRESULT CIFXCLODModifier2::GenerateOutput( U32    inOutputDataElementIndex,
										   void*& rpOutData, BOOL& rNeedRelease )
{
	IFXRESULT result = IFX_OK;

	if ( inOutputDataElementIndex == m_uMeshGroupIndex )
	{
		result = ChangeMeshResolution();

		m_pMeshGroup->QueryInterface( IID_IFXUnknown, &rpOutData );
		rNeedRelease = TRUE;
		return IFX_OK;
	}
	else if(inOutputDataElementIndex == m_uCLODManagerItfcIndex)
	{
		result = ChangeMeshResolution();
		if(IFXSUCCESS(result))
		{
			m_pCLODManager->QueryInterface(IID_IFXUnknown, &rpOutData);
			rNeedRelease = TRUE;
		}
	}

	return result;
}


IFXRESULT CIFXCLODModifier2::SetDataPacket(
	IFXModifierDataPacket* pInInputDataPacket,
	IFXModifierDataPacket* pInDataPacket )
{
	IFXRESULT result = IFX_OK;

	IFXRELEASE(m_pModifierDataPacket);
	IFXRELEASE(m_pInputDataPacket);

	if (pInDataPacket && pInInputDataPacket)
	{
		pInDataPacket->AddRef();
		m_pModifierDataPacket = pInDataPacket;
		pInInputDataPacket->AddRef();
		m_pInputDataPacket = pInInputDataPacket;

		if( IFXSUCCESS(result) )
		{
			result = pInDataPacket->GetDataElementIndex( DID_IFXRenderableGroup,
				m_uMeshGroupIndex );
		}

		if( IFXSUCCESS(result) )
		{
			result = pInDataPacket->GetDataElementIndex( DID_IFXCLODController,
				m_uCLODManagerItfcIndex );
		}

		if (m_bCLODScreenSpaceMetricOn) {
			if( IFXSUCCESS(result) )
			{
				result = pInDataPacket->GetDataElementIndex( DID_IFXFrustum,
					m_uFrustrumIndex );
			}

			if( IFXSUCCESS(result) )
			{
				result = pInDataPacket->GetDataElementIndex( DID_IFXViewSize,
					m_uViewSizeIndex );
			}
		}

		if( IFXSUCCESS(result) )
		{
			result = pInDataPacket->GetDataElementIndex( DID_IFXTransform,
				m_uTransformIndex );
		}

		if( IFXSUCCESS(result) )
		{
			result = pInDataPacket->GetDataElementIndex( DID_IFXRenderableGroupBounds,
				m_uMeshGroupBoundsIndex);
		}
	} else
		result = IFX_E_INVALID_POINTER;

	return result;
}

IFXRESULT CIFXCLODModifier2::Notify( IFXModifierMessage eInMessage,
								   void*               pMessageContext )

{
	return IFX_OK;
}



// IFXCLODModifier
IFXRESULT
CIFXCLODModifier2::GetCLODScreenSpaceControllerState(BOOL* pbOutOn)
{
	IFXRESULT result = IFX_OK;

	if (pbOutOn)
		*pbOutOn = m_bCLODScreenSpaceMetricOn;
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


IFXRESULT
CIFXCLODModifier2::SetCLODScreenSpaceControllerState(BOOL bInOn)
{
	if(m_bCLODScreenSpaceMetricOn != bInOn)
	{
		m_bCLODScreenSpaceMetricOn = bInOn;

		if(m_pModifierDataPacket )
		{
			m_pModifierDataPacket->InvalidateDataElement(m_uMeshGroupIndex);
		}
	}

	return IFX_OK;
}


IFXRESULT CIFXCLODModifier2::GetLODBias(F32* pfOutCLODScreenSpaceBias)
{
	IFXRESULT result = IFX_OK;

	if (pfOutCLODScreenSpaceBias)
		*pfOutCLODScreenSpaceBias = m_fCLODScreenSpaceBias;
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


IFXRESULT CIFXCLODModifier2::SetLODBias(F32 fInCLODScreenSpaceBias)
{
	if ( fInCLODScreenSpaceBias < 0 )
		return IFX_E_INVALID_RANGE;

	m_fCLODScreenSpaceBias = fInCLODScreenSpaceBias;

	if( m_pModifierDataPacket)
	{
		m_pModifierDataPacket->InvalidateDataElement(m_uMeshGroupIndex);
	}

	return IFX_OK;
}


IFXRESULT CIFXCLODModifier2::GetCLODLevel(F32* pfOutCLODLevel)
{
	IFXRESULT result = IFX_OK;

	if (pfOutCLODLevel)
		*pfOutCLODLevel = m_fCLODLevel;
	else
		result = IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(result) && m_pModifierDataPacket)
	{
		m_pModifierDataPacket->InvalidateDataElement(m_uMeshGroupIndex);
	}

	return result;
}


IFXRESULT CIFXCLODModifier2::SetCLODLevel(F32 fInCLODLevel)
{
	if (( fInCLODLevel < 0 )||( fInCLODLevel > 1 ))
		return IFX_E_INVALID_RANGE;

	m_fCLODLevel = fInCLODLevel;
	if(m_pModifierDataPacket)
	{
		m_pModifierDataPacket->InvalidateDataElement(m_uMeshGroupIndex);
	}
	return IFX_OK;
}

#define LOD_PERSPECTIVE_SCALE_FACTOR  2000
#define LOD_ORTHO_SCALE_FACTOR        500
//extern F32 g_fCLODMultiplier;

IFXRESULT CIFXCLODModifier2::ChangeMeshResolution()
{
	IFXRESULT result = IFX_OK;

	U32 CurMeshGroupChangeCount = (U32)-1;
	m_pInputDataPacket->GetDataElementChangeCount(m_uMeshGroupIndex, CurMeshGroupChangeCount);
	if(m_uMeshGroupChangeCount != CurMeshGroupChangeCount)
	{
		IFXMeshGroup* pMg = NULL;
		IFXUnknown* pUnk = NULL;
		m_pInputDataPacket->GetDataElement(m_uMeshGroupIndex, (void**) &pUnk);
		pUnk->QueryInterface(IID_IFXMeshGroup, (void**)&pMg);
		IFXRELEASE(pUnk);

		if(!m_pMeshGroup)
		{
			IFXCreateComponent(CID_IFXMeshGroup, IID_IFXMeshGroup, (void**) & m_pMeshGroup );
		}

		result = m_pMeshGroup->TransferData(*pMg, ~IFXMeshAttributes() ^ IFXMeshAttributes(IFX_MESH_FACE), IFXMeshAttributes(IFX_MESH_FACE), FALSE);

		IFXRELEASE(pMg);
		m_uMeshGroupChangeCount = CurMeshGroupChangeCount;
	};

	if(!m_pCLODManager)
	{
		IFXCreateComponent(CID_IFXCLODManager, IID_IFXCLODManager, (void**)&m_pCLODManager);
		if(!m_pCLODManager)
		{
			result = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(result))
	{
		U32 CurCLODChangeCount = (U32)-1;
		m_pInputDataPacket->GetDataElementChangeCount(m_uMeshGroupIndex, CurCLODChangeCount);
		if(m_uCLODManagerItfcCount != CurCLODChangeCount)
		{
			IFXUnknown* pUnk = NULL;
			IFXCLODManager* pCLODManager = NULL;
			result = m_pInputDataPacket->GetDataElement(m_uCLODManagerItfcIndex, (void**)&pUnk);
			if(IFXSUCCESS(result))
				pUnk->QueryInterface(IID_IFXCLODManager, (void**)&pCLODManager);
			IFXRELEASE(pUnk);
			if(IFXSUCCESS(result))
			{
				m_pCLODManager->Initialize(pCLODManager, m_pMeshGroup);
				m_uCLODManagerItfcCount = CurCLODChangeCount;
				pCLODManager->Release();
			}
		}
	}

	if(IFXSUCCESS(result))
	{
		if(m_bCLODScreenSpaceMetricOn)
		{
			F32 fNewRes = 1.0f;
			F32 fNewResMax = 0.0f; // init this variable with minimal possible value
			U32 i;
			U32 numElem = 0;

			// Get the current size
			F32 ViewportHeight;
			U32 viewSize = 0;
			IFXArray<IFXMatrix4x4> *pAModeltransform;
			IFXMatrix4x4* pModeltransform = NULL;
			IFXFrustum* pFrustrum = NULL;
			IFXBoundSphereDataElement* pBounds = NULL;

			result = m_pInputDataPacket->GetDataElement(m_uViewSizeIndex, (void**)&viewSize);
			if (IFXSUCCESS(result))
				result = m_pInputDataPacket->GetDataElement(m_uTransformIndex, (void**)&pAModeltransform);
			if (IFXSUCCESS(result))
				result = m_pInputDataPacket->GetDataElement(m_uFrustrumIndex, (void**)&pFrustrum);
			if (IFXSUCCESS(result))
				result = m_pInputDataPacket->GetDataElement(m_uMeshGroupBoundsIndex, IID_IFXBoundSphereDataElement, (void**)&pBounds);

			if (IFXSUCCESS(result)) {
				numElem = pAModeltransform->GetNumberElements();

				for (i = 0; i < numElem; i++) {
					pModeltransform = &pAModeltransform->GetElement(i);

					// Get the vector from the view to the model.
					IFXVector3 viewToModel;
					viewToModel=pModeltransform->TranslationConst();
					IFXVector3 viewTrans;
					viewTrans=pFrustrum->GetWorldTransform().TranslationConst();
					viewToModel.Subtract(viewTrans);

					ViewportHeight = (F32)(viewSize & 0x0000FFFF);

					// Determine the resolution:
					if ( pFrustrum->GetProjectionMode() == IFX_FRUSTUM_PERSPECTIVE_PROJECTION )
			  {
				  F32 ProjectionDegrees = pFrustrum->GetFOV();
				  fNewRes = (pBounds->Bound().Radius() * ViewportHeight
					  * m_fCLODScreenSpaceBias * LOD_PERSPECTIVE_SCALE_FACTOR)
					  / (sqrtf(viewToModel.DotProduct(viewToModel)) * ProjectionDegrees);
			  }
					else
			  {
				  F32 OrthoHeight = pFrustrum->GetPlane(IFX_FRUSTUM_TOP).position.CalcDistanceFrom(pFrustrum->GetPlane(IFX_FRUSTUM_BOTTOM).position);
				  fNewRes = (pBounds->Bound().Radius() * ViewportHeight
					  * m_fCLODScreenSpaceBias * LOD_ORTHO_SCALE_FACTOR / OrthoHeight);
			  }
			  fNewResMax = IFXMAX(fNewResMax, fNewRes);
				}
			}

			// Clamp to top end
			F32 fMaxRes = (F32)m_pCLODManager->GetMaxResolution();
			if (fNewResMax > fMaxRes)
				fNewResMax = fMaxRes;

			// Store the new settings
			m_fCLODLevel = fNewResMax/fMaxRes;
			m_pCLODManager->SetResolution((U32)fNewResMax);

			IFXRELEASE(pBounds);
		}
		else
		{
			m_pCLODManager->SetResolution((U32)(m_fCLODLevel * m_pCLODManager->GetMaxResolution()));
		}

	}

	return result;
}
