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
//  CIFXCylinderGenerator.cpp
//
//  DESCRIPTION
//
//  NOTES
//
//***************************************************************************
#include "IFXCheckX.h"
#include "IFXShapesCIDs.h"
#include "CIFXCylinderGenerator.h"
#include "IFXEncoderX.h"
#include "IFXException.h"
#include "IFXSceneGraph.h"
#include "IFXModifierDataPacket.h"
#include "IFXDids.h"
#include "IFXCOM.h"
#include "IFXExportingCIDs.h"

CIFXCylinderGenerator::CIFXCylinderGenerator()
{
  m_uRefCount = 0;

  m_uTransformDataElementIndex    = (U32)-1;
  m_uMeshGroupDataElementIndex    = (U32)-1;
  m_uNeighborMeshDataElementIndex = (U32)-1;
  m_uBoundSphereDataElementIndex  = (U32)-1;

  m_pPrimitiveGenerator = NULL;
  m_cylinder.uPrimitiveType = IFX_CYLINDER;
  m_cylinder.fTopRadius = 25.0f;
  m_cylinder.fBottomRadius = 25.0f;
  m_cylinder.fHeight = 50.0f;
  m_cylinder.uNumArcSlices = 2;
  m_cylinder.uSolidNumSurfaceSlices = 22;
  m_cylinder.bSolidCapStart = FALSE;
  m_cylinder.bSolidCapEnd = FALSE;
  m_cylinder.bTopCap = TRUE;
  m_cylinder.bBottomCap = TRUE;
  m_cylinder.fSolidAngleStart = 0.0f;
  m_cylinder.fSolidAngleEnd = 2*IFX_PI;
  m_cylinder.uNormalOrientation = IFXPrimitive::eFrontFacing;
  m_cylinder.ePrimitiveOrientation = eUninitialized;

  m_cylinder.m_pMeshGroup    = NULL;
  m_cylinder.m_pNeighborMesh = NULL;

  m_pBoundSphereDataElement = NULL;

  m_transform.CreateNewElement();
  m_transform[0].Reset();
}


CIFXCylinderGenerator::~CIFXCylinderGenerator()
{
  IFXRELEASE(m_pPrimitiveGenerator);
  IFXRELEASE(m_pBoundSphereDataElement);
  IFXRELEASE(m_cylinder.m_pMeshGroup);
  IFXRELEASE(m_cylinder.m_pNeighborMesh); // Smart pointer crashing on release.
}


IFXRESULT IFXAPI_CALLTYPE CIFXCylinderGenerator_Factory( IFXREFIID iid, void** ppv )
{
  IFXRESULT result;

  if (ppv)
  {
    // Create the CIFXClassName component.
    CIFXCylinderGenerator *pGenerator = new CIFXCylinderGenerator;

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
U32 CIFXCylinderGenerator::AddRef()
{
  return ++m_uRefCount;
}


U32 CIFXCylinderGenerator::Release()
{
  if( 1 == m_uRefCount )
  {
    CIFXModifier::PreDestruct();
    delete this;
    return 0;
  }

  return --m_uRefCount;
}


IFXRESULT CIFXCylinderGenerator::QueryInterface( IFXREFIID riid, void **ppv )
{
  IFXRESULT result = IFX_OK;

  if ( ppv )
  {
    if ( riid == IID_IFXUnknown )
      *ppv = (IFXUnknown*)this;
    else if ( riid == IID_IFXCylinderGenerator   )
      *ppv = (IFXCylinderGenerator*)this;
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
    else if ( IID_IFXMetaDataX == riid )
    {
      *ppv = ( IFXMetaDataX* ) this;
    }
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
IFXRESULT CIFXCylinderGenerator::InitializeObject()
{
  IFXRESULT result = IFX_OK;

  if ( !m_pBoundSphereDataElement )
    result = IFXCreateComponent(CID_IFXBoundSphereDataElement, IID_IFXBoundSphereDataElement, (void**) &m_pBoundSphereDataElement);

  if ( IFXSUCCESS(result) )
    result = IFXCreateComponent(CID_IFXPrimitiveGenerator, IID_IFXPrimitiveGenerator, (void**) &m_pPrimitiveGenerator);

  if ( IFXSUCCESS(result) )
  {
  m_cylinder.ePrimitiveOrientation = enegZFrontposYup;
  }

  return result;
}

// IFXMarkerX interface...
void CIFXCylinderGenerator::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
//  CIFXMarker::GetEncoderX(CID_IFXCylinderGeneratorEncoder, rpEncoderX);
}


// IFXModifier
const IFXGUID* CIFXCylinderGenerator::m_scpOutputDIDs[] =
{
  &DID_IFXTransform,
  &DID_IFXRenderableGroup,
  &DID_IFXNeighborMesh,
  &DID_IFXRenderableGroupBounds
};

const IFXGUID* CIFXCylinderGenerator::m_scpNeighborMeshDeps[] =
{
  &DID_IFXRenderableGroup
};




IFXRESULT CIFXCylinderGenerator::GetOutputs( IFXGUID**& rpOutOutputs,
                                             U32&       rOutNumberOfOutputs,
                                             U32*&      rpOutOutputDepAttrs )
{
  rOutNumberOfOutputs = sizeof(m_scpOutputDIDs)/ sizeof(IFXGUID*);
  rpOutOutputDepAttrs = NULL;
  rpOutOutputs = (IFXGUID**)&m_scpOutputDIDs;
  return IFX_OK;
}


IFXRESULT CIFXCylinderGenerator::GetDependencies(
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
    IFXASSERTBOX( 1, "CIFXCylinderGenerator::GetDependencies() called with unknown output!" );
    result = IFX_E_UNDEFINED;
  }

  return result;
}


IFXRESULT CIFXCylinderGenerator::GenerateOutput( U32    inOutputDataElementIndex,
                                               void*& rpOutData, BOOL& rNeedRelease )
{
  IFXRESULT result = IFX_OK;

  if ( inOutputDataElementIndex == m_uTransformDataElementIndex )
    rpOutData = &m_transform;
  else if ( inOutputDataElementIndex == m_uMeshGroupDataElementIndex )
  {
    m_pPrimitiveGenerator->GeneratePrimitive(&m_cylinder);
    m_cylinder.m_pMeshGroup->QueryInterface( IID_IFXUnknown, &rpOutData );
//    m_cylinder.m_pMeshGroup->Release();
  rNeedRelease = TRUE;
  }
  else if ( inOutputDataElementIndex == m_uBoundSphereDataElementIndex )
  {
    IFXVector3 v(m_cylinder.fHeight * 0.5f,
      IFXMAX(m_cylinder.fTopRadius, m_cylinder.fBottomRadius),
      0);
    m_pBoundSphereDataElement->Bound().Radius() = v.CalcMagnitude();
    m_pBoundSphereDataElement->QueryInterface( IID_IFXUnknown, &rpOutData );
//    m_pBoundSphereDataElement->Release();
  rNeedRelease = TRUE;
  }
  else if ( inOutputDataElementIndex == m_uNeighborMeshDataElementIndex )
  {
    if ( m_cylinder.m_pNeighborMesh == NULL )
      IFXCreateComponent(CID_IFXNeighborMesh,
                 IID_IFXNeighborMesh,
                 (void**)&(m_cylinder.m_pNeighborMesh));
    if (!m_cylinder.m_pNeighborMesh)
      result = IFX_E_OUT_OF_MEMORY;
    if (IFXSUCCESS(result))
      result = m_cylinder.m_pNeighborMesh->Allocate(*(m_cylinder.m_pMeshGroup));
    if (IFXSUCCESS(result))
      result = m_cylinder.m_pNeighborMesh->Build(*(m_cylinder.m_pMeshGroup), NULL);
    if (IFXSUCCESS(result))
      result = m_cylinder.m_pNeighborMesh->QueryInterface( IID_IFXUnknown, &rpOutData );
//    if (IFXSUCCESS(result))
//      m_cylinder.m_pNeighborMesh->Release();
  rNeedRelease = TRUE;
  }
  else
  {
    IFXASSERTBOX( 1, "CIFXCylinderGenerator::GetOutput() not implemented!" );
    result = IFX_E_UNDEFINED;
  }

  return result;
}


IFXRESULT CIFXCylinderGenerator::SetDataPacket(
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


IFXRESULT CIFXCylinderGenerator::Notify( IFXModifierMessage eInMessage,
                                      void*               pMessageContext )
{
  return IFX_OK;
}


// IFXPrimitive
IFXRESULT CIFXCylinderGenerator::SetFacing(EIPrimitiveFacing eFacing) {
  IFXRESULT iResult = IFX_OK;

  if(IFXSUCCESS(iResult))
  {
    m_cylinder.uNormalOrientation = eFacing;
    if(m_pModifierDataPacket)
      m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
  }

  return iResult;
}


IFXRESULT CIFXCylinderGenerator::GetFacing(EIPrimitiveFacing* pFacing) {
  IFXRESULT iResult = IFX_OK;

  if(NULL == pFacing)
    iResult = IFX_E_INVALID_POINTER;
  else
    *pFacing = (EIPrimitiveFacing)m_cylinder.uNormalOrientation;

  return iResult;
}


// IFXCylinderPrimitive
BOOL CIFXCylinderGenerator::GetTopCap( void )
{
  return m_cylinder.bTopCap;
}


IFXRESULT CIFXCylinderGenerator::SetTopCap( BOOL bInTopCap )
{
  if ( bInTopCap != (BOOL) m_cylinder.bTopCap )
  {
    m_cylinder.bTopCap = bInTopCap;
    if(m_pModifierDataPacket)
      m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
  }
  return IFX_OK;
}


BOOL CIFXCylinderGenerator::GetBottomCap( void )
{
  return m_cylinder.bBottomCap;
}


IFXRESULT CIFXCylinderGenerator::SetBottomCap( BOOL bInBottomCap )
{
  if ( bInBottomCap != (BOOL) m_cylinder.bBottomCap )
  {
    m_cylinder.bBottomCap = bInBottomCap;
    if(m_pModifierDataPacket)
      m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
  }
  return IFX_OK;
}


F32 CIFXCylinderGenerator::GetTopRadius( void )
{
  return m_cylinder.fTopRadius;
}


IFXRESULT CIFXCylinderGenerator::SetTopRadius( F32 fInRadius )
{
  if ( fInRadius != m_cylinder.fTopRadius )
  {
    if ( fInRadius >= 0 )
    {
      m_cylinder.fTopRadius = fInRadius;
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


F32 CIFXCylinderGenerator::GetBottomRadius( void )
{
  return m_cylinder.fBottomRadius;
}


IFXRESULT CIFXCylinderGenerator::SetBottomRadius( F32 fInRadius )
{
  if ( fInRadius != m_cylinder.fBottomRadius )
  {
    if ( fInRadius >= 0 )
    {
      m_cylinder.fBottomRadius = fInRadius;
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


F32 CIFXCylinderGenerator::GetHeight( void )
{
  return m_cylinder.fHeight;
}


IFXRESULT CIFXCylinderGenerator::SetHeight ( F32 fInHeight )
{
  if ( fInHeight != m_cylinder.fHeight )
  {
    if ( fInHeight >= 0 )
    {
      m_cylinder.fHeight = fInHeight;
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


F32 CIFXCylinderGenerator::GetStartAngle( void )
{
  F32 fAngle=(180/IFX_PI)*m_cylinder.fSolidAngleStart;
  return fAngle;
}


IFXRESULT CIFXCylinderGenerator::SetStartAngle ( F32 fInStartAngle )
{
  IFXRESULT iResult = IFX_OK;

  if ( fInStartAngle != m_cylinder.fSolidAngleStart )
  {
    if( ((IFX_PI/180)*fInStartAngle) > m_cylinder.fSolidAngleEnd)
      iResult = IFX_E_INVALID_RANGE;
    if( ((IFX_PI/180)*fInStartAngle) > 360.0f || ((IFX_PI/180)*fInStartAngle) < 0.0f)
      iResult = IFX_E_INVALID_RANGE;

    if(IFXSUCCESS(iResult)) {
      m_cylinder.fSolidAngleStart = (IFX_PI/180)*fInStartAngle;
      if(m_pModifierDataPacket)
        m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
    }
  }
  return iResult;
}


F32 CIFXCylinderGenerator::GetEndAngle( void )
{
  F32 fAngle=(180/IFX_PI)*m_cylinder.fSolidAngleEnd;
  return fAngle;
}


IFXRESULT CIFXCylinderGenerator::SetEndAngle ( F32 fInEndAngle )
{
  IFXRESULT iResult = IFX_OK;

  if ( fInEndAngle != m_cylinder.fSolidAngleEnd )
  {
    if(((IFX_PI/180)*fInEndAngle) < m_cylinder.fSolidAngleStart)
      iResult = IFX_E_INVALID_RANGE;
    if( ((IFX_PI/180)*fInEndAngle) > 360.0f || ((IFX_PI/180)*fInEndAngle) < 0.0f)
      iResult = IFX_E_INVALID_RANGE;

    if(IFXSUCCESS(iResult)) {
      m_cylinder.fSolidAngleEnd = (IFX_PI/180)*fInEndAngle;
      if(m_pModifierDataPacket)
        m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
    }
  }
  return iResult;
}


U32 CIFXCylinderGenerator::GetNumSegments( void )
{
  // uNumArcSlices is a misnomer. It corresponds to the number
  // of segments along the axis of the cylinder instead of the
  // more apt number of segments along the circumference.
  return m_cylinder.uNumArcSlices;
}


IFXRESULT CIFXCylinderGenerator::SetNumSegments ( U32 iInNumSegments )
{
  if(iInNumSegments<1)
    return IFX_E_INVALID_RANGE;

  if ( iInNumSegments != m_cylinder.uNumArcSlices )
  {
    // uNumArcSlices is a misnomer. It corresponds to the number
    // of segments along the axis of the cylinder instead of the
    // more apt number of segments along the circumference.
    // The code in IFXGeneratePrimitive has uNumArcSlices
    // and uSolidNumSurfaceSlices reversed.
    //
    m_cylinder.uNumArcSlices = iInNumSegments;
    if(m_pModifierDataPacket)
      m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
  }
  return IFX_OK;
}


U32 CIFXCylinderGenerator::GetResolution( void )
{
  // uSolidNumSurfaceSlices is poorly name, IMHO.
  // It corresponds to the number of segments along the
  // circumference of the cylinder. The code in IFXGeneratePrimitive
  // has uNumArcSlices and uSolidNumSurfaceSlices reversed.
  //
  return (m_cylinder.uSolidNumSurfaceSlices-2);
}


IFXRESULT CIFXCylinderGenerator::SetResolution ( U32 iInResolution )
{
  if(iInResolution<1)
    return IFX_E_INVALID_RANGE;

  if ( iInResolution != m_cylinder.uSolidNumSurfaceSlices )
  {
    m_cylinder.uSolidNumSurfaceSlices = iInResolution+2;
    if(m_pModifierDataPacket)
      m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
  }
  return IFX_OK;
}


IFXRESULT CIFXCylinderGenerator::SetBoundingSphere(const IFXVector4& vInBoundingSphere)
{
  m_pBoundSphereDataElement->Bound() = vInBoundingSphere;
  return m_pModifierDataPacket->InvalidateDataElement(m_uBoundSphereDataElementIndex);
}


IFXRESULT CIFXCylinderGenerator::SetTransform(const IFXMatrix4x4& tInTransform)
{
  m_transform[0] = tInTransform;
  return m_pModifierDataPacket->InvalidateDataElement(m_uTransformDataElementIndex);
}


IFXRESULT CIFXCylinderGenerator::InvalidateTransform()
{
  if(m_pModifierDataPacket)
  {
    m_pModifierDataPacket->InvalidateDataElement(m_uTransformDataElementIndex);
  }
  return IFX_OK;
}
