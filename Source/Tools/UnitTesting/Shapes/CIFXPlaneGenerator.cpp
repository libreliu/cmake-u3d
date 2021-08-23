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
//
//  CIFXPlaneGenerator.cpp
//
//  DESCRIPTION
//
//  NOTES
//
//***************************************************************************

#include "IFXShapesCIDs.h"
#include "CIFXPlaneGenerator.h"
#include "IFXCheckX.h"
#include "IFXException.h"
#include "IFXEncoderX.h"
#include "IFXSceneGraph.h"
#include "IFXModifierDataPacket.h"
#include "IFXExportingCIDs.h"
#include "IFXDids.h"
#include "IFXCOM.h"

CIFXPlaneGenerator::CIFXPlaneGenerator()
{
  m_uRefCount = 0;

  m_uTransformDataElementIndex    = (U32)-1;
  m_uMeshGroupDataElementIndex    = (U32)-1;
  m_uNeighborMeshDataElementIndex = (U32)-1;
  m_uBoundSphereDataElementIndex  = (U32)-1;

  m_pPrimitiveGenerator = NULL;
  m_plane.uPrimitiveType  = IFX_PLANE;
  m_plane.fLength = 1.0f;
  m_plane.fWidth = 1.0f;
  m_plane.uLengthVertices = 2;
  m_plane.uWidthVertices = 2;
  m_plane.uNormalOrientation= IFXPrimitive::eBothFacing;
  m_plane.ePrimitiveOrientation = eUninitialized;

  m_plane.m_pMeshGroup  = NULL;
  m_plane.m_pNeighborMesh = NULL;

  m_pBoundSphereDataElement = NULL;

  m_transform.CreateNewElement();
  m_transform[0].Reset();
}


CIFXPlaneGenerator::~CIFXPlaneGenerator()
{
  IFXRELEASE(m_pPrimitiveGenerator);
  IFXRELEASE(m_pBoundSphereDataElement);
  IFXRELEASE(m_plane.m_pMeshGroup);
  IFXRELEASE(m_plane.m_pNeighborMesh); // Smart pointer crashing on release.
}


IFXRESULT IFXAPI_CALLTYPE CIFXPlaneGenerator_Factory( IFXREFIID iid, void** ppv )
{
  IFXRESULT result;

  if (ppv)
  {
    // Create the CIFXClassName component.
    CIFXPlaneGenerator *pGenerator = new CIFXPlaneGenerator;

    if (pGenerator)
    {
      // Perform a temporary AddRef for our usage of the component.
      pGenerator->AddRef();

      // Attempt to obtain a pointer to the requested interface.
      result = pGenerator->QueryInterface( iid, ppv );

      // Perform a Release since our usage of the component is now
      // complete.  Note:  If the QI fails, this will cause the
      // component to be destroyed.
      pGenerator->Release();
    }
    else
      result = IFX_E_OUT_OF_MEMORY;
  }
  else
    result = IFX_E_INVALID_POINTER;

  return result;
}


// IFXUnknown
U32 CIFXPlaneGenerator::AddRef()
{

  return ++m_uRefCount;
}


U32 CIFXPlaneGenerator::Release()
{

  if( 1 == m_uRefCount )
  {
    CIFXModifier::PreDestruct();
    delete this;
    return 0;
  }

  return --m_uRefCount;
}


IFXRESULT CIFXPlaneGenerator::QueryInterface( IFXREFIID riid, void **ppv )
{
  IFXRESULT result = IFX_OK;

  if ( ppv )
  {
    if ( riid == IID_IFXUnknown )
      *ppv = (IFXUnknown*)this;
    else if ( riid == IID_IFXPlaneGenerator   )
      *ppv = (IFXPlaneGenerator*)this;
    else if ( riid == IID_IFXPrimitive )
      *ppv = (IFXPrimitive*)this;
    else if ( riid == IID_IFXMarker )
      *ppv = (IFXMarker*)this;
    else if ( riid == IID_IFXMarkerX )
      *ppv = (IFXMarkerX*)this;
    else if ( riid == IID_IFXModifier )
      *ppv = (IFXModifier*)this;
    else if ( riid == IID_IFXSubject )
      *ppv = (IFXSubject*)this;
    else if ( riid == IID_IFXGenerator)
      *ppv = (IFXGenerator*)this;
    else
    {
      *ppv = NULL;
      result = IFX_E_UNSUPPORTED;
    }

    if( IFXSUCCESS(result) )
      AddRef();
  }
  else result = IFX_E_INVALID_POINTER;

  return result;
}


// IFXMarker
IFXRESULT CIFXPlaneGenerator::InitializeObject()
{
  IFXRESULT result = IFX_OK;

  if ( !m_pBoundSphereDataElement )
  {
    result = IFXCreateComponent(CID_IFXBoundSphereDataElement, IID_IFXBoundSphereDataElement, (void**) &m_pBoundSphereDataElement);

	if ( IFXSUCCESS(result) )
		result = IFXCreateComponent(CID_IFXPrimitiveGenerator, IID_IFXPrimitiveGenerator, (void**) &m_pPrimitiveGenerator);

	if ( IFXSUCCESS(result) )
		m_plane.ePrimitiveOrientation = enegZFrontposYup;
  }

  return result;
}


// IFXMarkerX interface...
void CIFXPlaneGenerator::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
//  CIFXMarker::GetEncoderX(CID_IFXPlaneGeneratorEncoder, rpEncoderX);
}


// IFXModifier
const IFXGUID* CIFXPlaneGenerator::m_scpOutputDIDs[] =
{
  &DID_IFXTransform,
  &DID_IFXRenderableGroup,
  &DID_IFXNeighborMesh,
  &DID_IFXRenderableGroupBounds
};

const IFXGUID* CIFXPlaneGenerator::m_scpNeighborMeshDeps[] =
{
  &DID_IFXRenderableGroup
};




IFXRESULT CIFXPlaneGenerator::GetOutputs( IFXGUID**& rpOutOutputs,
                                             U32&       rOutNumberOfOutputs,
                                             U32*&      rpOutOutputDepAttrs )
{
  rOutNumberOfOutputs = sizeof(m_scpOutputDIDs)/ sizeof(IFXGUID*);
  rpOutOutputDepAttrs = NULL;
  rpOutOutputs = (IFXGUID**)&m_scpOutputDIDs;
  return IFX_OK;
}


IFXRESULT CIFXPlaneGenerator::GetDependencies(
                                IFXGUID*   pInOutputDID,
                                IFXGUID**& rppOutInputDependencies,
                                  U32&       rOutNumberInputDependencies,
                                  IFXGUID**& rppOutOutputDependencies,
                                U32&       rOutNumberOfOutputDependencies,
                  U32*&    rpOutOutputDepAttrs )
{
  IFXRESULT result = IFX_OK;

  if (pInOutputDID == &DID_IFXNeighborMesh)
  {
    rppOutInputDependencies = NULL;
    rOutNumberInputDependencies = 0;
    rppOutOutputDependencies = (IFXGUID**)m_scpNeighborMeshDeps;
    rOutNumberOfOutputDependencies = sizeof(m_scpNeighborMeshDeps) / sizeof(IFXGUID*);
    rpOutOutputDepAttrs = NULL;
  }
  else if ( (pInOutputDID == &DID_IFXRenderableGroup)
         || (pInOutputDID == &DID_IFXTransform)
         || (pInOutputDID == &DID_IFXRenderableGroupBounds) )
  {
    rppOutInputDependencies = NULL;
    rOutNumberInputDependencies = 0;
    rppOutOutputDependencies = NULL;
    rOutNumberOfOutputDependencies = 0;
    rpOutOutputDepAttrs = NULL;
  }
  else
  {
    IFXASSERTBOX( 1, "CIFXPlaneGenerator::GetDependencies() called with unknown output!" );
    result = IFX_E_UNDEFINED;
  }

  return result;
}


IFXRESULT CIFXPlaneGenerator::GenerateOutput( U32    inOutputDataElementIndex,
                                            void*& rpOutData, BOOL& rNeedRelease )
{
  IFXRESULT result = IFX_OK;

  if ( inOutputDataElementIndex == m_uTransformDataElementIndex )
    rpOutData = &m_transform;
  else if ( inOutputDataElementIndex == m_uMeshGroupDataElementIndex )
  {
    m_pPrimitiveGenerator->GeneratePrimitive(&m_plane);
    m_plane.m_pMeshGroup->QueryInterface( IID_IFXUnknown, &rpOutData );
//    m_plane.m_pMeshGroup->Release();
  rNeedRelease = TRUE;

    // Map the shader sets on to the plane
    IFXRenderable* pRndr = NULL;
    m_plane.m_pMeshGroup->QueryInterface( IID_IFXRenderable, (void**) &pRndr );

    IFXRELEASE(pRndr);

  }
  else if ( inOutputDataElementIndex == m_uBoundSphereDataElementIndex )
  {
    // The following should be simplified using the appropriate implicit function.

    IFXVector3 v(m_plane.fWidth* 0.5f, m_plane.fLength* 0.5f, 0.0f);
    m_pBoundSphereDataElement->Bound().Radius() = v.CalcMagnitude();
    m_pBoundSphereDataElement->QueryInterface( IID_IFXUnknown, &rpOutData );
//    m_pBoundSphereDataElement->Release();
  rNeedRelease = TRUE;
  }
  else if ( inOutputDataElementIndex == m_uNeighborMeshDataElementIndex )
  {
    if ( m_plane.m_pNeighborMesh == NULL )
      IFXCreateComponent(CID_IFXNeighborMesh,
                 IID_IFXNeighborMesh,
                 (void**)&(m_plane.m_pNeighborMesh));
    if (!m_plane.m_pNeighborMesh)
      result = IFX_E_OUT_OF_MEMORY;
    if (IFXSUCCESS(result))
      result = m_plane.m_pNeighborMesh->Allocate(*(m_plane.m_pMeshGroup));
    if (IFXSUCCESS(result))
      result = m_plane.m_pNeighborMesh->Build(*(m_plane.m_pMeshGroup), NULL);
    if (IFXSUCCESS(result))
      result = m_plane.m_pNeighborMesh->QueryInterface( IID_IFXUnknown, &rpOutData );
//    if (IFXSUCCESS(result))
//      m_plane.m_pNeighborMesh->Release();
  rNeedRelease = TRUE;
  }
  else
  {
    IFXASSERTBOX( 1, "CIFXPlaneGenerator::GetOutput() not implemented!" );
    result = IFX_E_UNDEFINED;
  }

  return result;
}


IFXRESULT CIFXPlaneGenerator::SetDataPacket(
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
  }
  else
  {
    result = IFX_E_INVALID_POINTER;
  }

  if( IFXSUCCESS(result) )
    result = pInDataPacket->GetDataElementIndex(
      DID_IFXTransform,
      m_uTransformDataElementIndex );

  if( IFXSUCCESS(result) )
    result = pInDataPacket->GetDataElementIndex(
      DID_IFXRenderableGroup,
      m_uMeshGroupDataElementIndex );

  if( IFXSUCCESS(result) )
    m_pBoundSphereDataElement->RenderableIndex() =
      m_uMeshGroupDataElementIndex;

  if( IFXSUCCESS(result) )
    result = pInDataPacket->GetDataElementIndex(
      DID_IFXNeighborMesh,
      m_uNeighborMeshDataElementIndex );

  if( IFXSUCCESS(result) )
    result = pInDataPacket->GetDataElementIndex(
      DID_IFXRenderableGroupBounds,
      m_uBoundSphereDataElementIndex );

  return result;
}


IFXRESULT CIFXPlaneGenerator::Notify( IFXModifierMessage eInMessage,
                                      void*               pMessageContext )

{
  return IFX_OK;
}


IFXRESULT CIFXPlaneGenerator::VerifyInvalidation(
                                                IFXGUID*   pInDependentDID,
                                                void*      pInDependentValue,
                                                IFXGUID*   pInDependencyDID,
                                                void*      pInDependencyValue )
{
  return IFX_OK;
}


// IFXPrimitive
IFXRESULT CIFXPlaneGenerator::SetFacing(EIPrimitiveFacing eFacing)
{
  if ( m_plane.uNormalOrientation != eFacing )
  {
    m_plane.uNormalOrientation = eFacing;

    if(m_pModifierDataPacket)
      m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
  }

  return IFX_OK;
}


IFXRESULT CIFXPlaneGenerator::GetFacing(EIPrimitiveFacing* pFacing) {
  IFXRESULT iResult = IFX_OK;

  if(NULL == pFacing)
    iResult = IFX_E_INVALID_POINTER;
  else
    *pFacing = (EIPrimitiveFacing)m_plane.uNormalOrientation;

  return iResult;
}


// IFXPlanePrimitive
F32 CIFXPlaneGenerator::GetLength( void )
{
  return m_plane.fLength;
}


IFXRESULT CIFXPlaneGenerator::SetLength( F32 fInLength )
{
  if ( fInLength != m_plane.fLength )
  {
    if ( fInLength > 0 )
    {
      m_plane.fLength = fInLength;
      if(m_pModifierDataPacket)
      {
        m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
        m_pModifierDataPacket->InvalidateDataElement( m_uBoundSphereDataElementIndex );
      }
    }
    else
      return IFX_E_INVALID_RANGE;
  }
  return IFX_OK;
}


F32 CIFXPlaneGenerator::GetWidth( void )
{
  return m_plane.fWidth;
}


IFXRESULT CIFXPlaneGenerator::SetWidth( F32 fInWidth )
{
  if ( fInWidth != m_plane.fWidth )
  {
    if ( fInWidth > 0 )
    {
      m_plane.fWidth = fInWidth;
      if(m_pModifierDataPacket)
      {
        m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
        m_pModifierDataPacket->InvalidateDataElement( m_uBoundSphereDataElementIndex );
      }
    }
    else
      return IFX_E_INVALID_RANGE;
  }
  return IFX_OK;
}

U32 CIFXPlaneGenerator::GetLengthVertices( void )
{
  return m_plane.uLengthVertices;
}

IFXRESULT CIFXPlaneGenerator::SetLengthVertices( U32 iInLengthVertices )
{
  if ( iInLengthVertices != m_plane.uLengthVertices)
  {
    if ( iInLengthVertices >= 2 )
    {
      m_plane.uLengthVertices = iInLengthVertices;
      if(m_pModifierDataPacket)
      {
        m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
        m_pModifierDataPacket->InvalidateDataElement( m_uBoundSphereDataElementIndex );
      }
    }
    else
      return IFX_E_INVALID_RANGE;
  }
  return IFX_OK;
}

U32 CIFXPlaneGenerator::GetWidthVertices( void )
{
  return m_plane.uWidthVertices;
}


IFXRESULT CIFXPlaneGenerator::SetWidthVertices( U32 iInWidthVertices )
{
  if ( iInWidthVertices != m_plane.uWidthVertices )
  {
    if ( iInWidthVertices >= 2 )
    {
      m_plane.uWidthVertices = iInWidthVertices;
      if(m_pModifierDataPacket)
      {
        m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
        m_pModifierDataPacket->InvalidateDataElement( m_uBoundSphereDataElementIndex );
      }
    }
    else
      return IFX_E_INVALID_RANGE;
  }
  return IFX_OK;
}


IFXRESULT CIFXPlaneGenerator::SetBoundingSphere(const IFXVector4& vInBoundingSphere)
{
  m_pBoundSphereDataElement->Bound() = vInBoundingSphere;
  return m_pModifierDataPacket->InvalidateDataElement(m_uBoundSphereDataElementIndex);
}


IFXRESULT CIFXPlaneGenerator::SetTransform(const IFXMatrix4x4& tInTransform)
{
  m_transform[0] = tInTransform;
  return m_pModifierDataPacket->InvalidateDataElement(m_uTransformDataElementIndex);
}


IFXRESULT CIFXPlaneGenerator::InvalidateTransform()
{
  if(m_pModifierDataPacket)
  {
    m_pModifierDataPacket->InvalidateDataElement(m_uTransformDataElementIndex);
  }
  return IFX_OK;
}
