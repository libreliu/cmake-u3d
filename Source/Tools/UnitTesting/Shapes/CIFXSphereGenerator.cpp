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
//  CIFXSphereGenerator.cpp
//
//  DESCRIPTION
//
//  NOTES
//
//***************************************************************************

#include "IFXShapesCIDs.h"
#include "CIFXSphereGenerator.h"
#include "IFXCheckX.h"
#include "IFXEncoderX.h"
#include "IFXException.h"
#include "IFXSceneGraph.h"
#include "IFXModifierDataPacket.h"
#include "IFXDids.h"
#include "IFXCOM.h"
#include "IFXExportingCIDs.h"

CIFXSphereGenerator::CIFXSphereGenerator()
{
  m_uRefCount = 0;

  // IFXModifier attributes...
  m_uTransformDataElementIndex    = (U32)-1;
  m_uMeshGroupDataElementIndex    = (U32)-1;
  m_uNeighborMeshDataElementIndex = (U32)-1;
  m_uBoundSphereDataElementIndex  = (U32)-1;
  m_sphere.m_pMeshGroup           = NULL;
  m_sphere.m_pNeighborMesh        = NULL;
  m_pBoundSphereDataElement       = NULL;

  // IFXSphereGenerator attributes...
  m_pPrimitiveGenerator = NULL;

  m_sphere.uPrimitiveType=IFX_SPHERE;
  m_sphere.fRadius=25.0f;
  m_sphere.uNumArcSlices=20;
  m_sphere.fArcAngleStart=0.0f;
  m_sphere.fArcAngleEnd=IFX_PI;
  m_sphere.pArcCoords=NULL;
  m_sphere.pArcNormals=NULL;
  m_sphere.uNormalOrientation=IFXPrimitive::eFrontFacing;
  m_sphere.bTopCap=TRUE;
  m_sphere.bBottomCap=TRUE;
  m_sphere.fSolidAngleStart=0.0f;
  m_sphere.fSolidAngleEnd=2*IFX_PI;
  m_sphere.uSolidNumSurfaceSlices=23;
  m_sphere.ePrimitiveOrientation = eUninitialized;

  m_transform.CreateNewElement();
  m_transform[0].Reset();
}


CIFXSphereGenerator::~CIFXSphereGenerator()
{
  IFXRELEASE(m_pPrimitiveGenerator);
  IFXRELEASE(m_pBoundSphereDataElement);
  IFXRELEASE(m_sphere.m_pMeshGroup);
  IFXRELEASE(m_sphere.m_pNeighborMesh); // Smart pointer crashing on release.
}

void CIFXSphereGenerator::InitAsBound()
{
  m_sphere.uPrimitiveType=IFX_SPHERE;
  m_sphere.fRadius=1;
  m_sphere.uNumArcSlices=12;
  m_sphere.fArcAngleStart=0.0f;
  m_sphere.fArcAngleEnd=IFX_PI;
  m_sphere.bTopCap=FALSE;
  m_sphere.bBottomCap=FALSE;
  m_sphere.bSolidCapStart=FALSE;
  m_sphere.bSolidCapEnd=FALSE;
  m_sphere.uNormalOrientation=IFXPrimitive::eFrontFacing;
  m_sphere.fSolidAngleStart=0.0f;
  m_sphere.fSolidAngleEnd=2*IFX_PI;
  m_sphere.uSolidNumSurfaceSlices=12;
}

IFXRESULT IFXAPI_CALLTYPE CIFXSphereGenerator_Factory( IFXREFIID iid, void** ppv )
{
  IFXRESULT result;

  if (ppv)
  {
    // Create the CIFXClassName component.
    CIFXSphereGenerator *pGenerator = new CIFXSphereGenerator;

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

IFXRESULT CIFXSphereGenerator::GetMesh( IFXMesh** pMesh )
{
    IFXRESULT res = IFX_OK;

    IFXPrimitiveGenerator *pPrimGen = NULL;
    res = IFXCreateComponent( CID_IFXPrimitiveGenerator, IID_IFXPrimitiveGenerator, (void**)&pPrimGen );

    if( IFXSUCCESS( res ) )
    {
        pPrimGen->GeneratePrimitive( &m_sphere );
        m_sphere.m_pMeshGroup->GetMesh( 0, *pMesh );
        if( NULL == pMesh )
        {
            res = IFX_E_OUT_OF_MEMORY;
        }
    }

    IFXRELEASE( pPrimGen );

    return res;
}

// IFXUnknown
U32 CIFXSphereGenerator::AddRef()
{
  return ++m_uRefCount;
}


U32 CIFXSphereGenerator::Release()
{
  if( 1 == m_uRefCount )
  {
    CIFXModifier::PreDestruct();
    delete this;
    return 0;
  }

  return --m_uRefCount;
}


IFXRESULT CIFXSphereGenerator::QueryInterface( IFXREFIID riid, void **ppv )
{
  IFXRESULT result = IFX_OK;

  if ( ppv )
  {
    if ( riid == IID_IFXUnknown )
      *ppv = (IFXUnknown*)this;
    else if ( riid == IID_IFXSphereGenerator   )
      *ppv = (IFXSphereGenerator*)this;
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
IFXRESULT CIFXSphereGenerator::InitializeObject()
{
  IFXRESULT result = IFX_OK;

  if ( !m_pBoundSphereDataElement )
      result = IFXCreateComponent(CID_IFXBoundSphereDataElement, IID_IFXBoundSphereDataElement, (void**) &m_pBoundSphereDataElement);

  if ( IFXSUCCESS(result) )
    result = IFXCreateComponent(CID_IFXPrimitiveGenerator, IID_IFXPrimitiveGenerator, (void**) &m_pPrimitiveGenerator);

  if ( IFXSUCCESS(result) )
  {
  m_sphere.ePrimitiveOrientation = enegZFrontposYup;
  }

  return result;
}

// IFXMarkerX interface...
void CIFXSphereGenerator::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
//  CIFXMarker::GetEncoderX(CID_IFXSphereGeneratorEncoder, rpEncoderX );
}

// IFXModifier
const IFXGUID* CIFXSphereGenerator::m_scpOutputDIDs[] =
{
  &DID_IFXTransform,
  &DID_IFXRenderableGroup,
  &DID_IFXNeighborMesh,
  &DID_IFXRenderableGroupBounds
};


const IFXGUID* CIFXSphereGenerator::m_scpNeighborMeshDeps[] =
{
  &DID_IFXRenderableGroup
};


IFXRESULT CIFXSphereGenerator::GetOutputs( IFXGUID**& rpOutOutputs,
                                         U32&       rOutNumberOfOutputs,
                                           U32*&      rpOutOutputDepAttrs )
{
  rOutNumberOfOutputs = sizeof(m_scpOutputDIDs)/ sizeof(IFXGUID*);
  rpOutOutputDepAttrs = NULL;
  rpOutOutputs = (IFXGUID**)&m_scpOutputDIDs;
  return IFX_OK;
}


IFXRESULT CIFXSphereGenerator::GetDependencies(
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
    IFXASSERTBOX( 1, "CIFXSphereGenerator::GetDependencies() called with unknown output!" );
    result = IFX_E_UNDEFINED;
  }

  return result;
}


IFXRESULT CIFXSphereGenerator::GenerateOutput( U32    inOutputDataElementIndex,
                                               void*& rpOutData, BOOL& rNeedRelease )
{
  IFXRESULT result = IFX_OK;

  if ( inOutputDataElementIndex == m_uTransformDataElementIndex )
    rpOutData = &m_transform;
  else if ( inOutputDataElementIndex == m_uMeshGroupDataElementIndex )
  {
    IFXRELEASE(m_sphere.m_pMeshGroup);
    m_pPrimitiveGenerator->GeneratePrimitive(&m_sphere);
    m_sphere.m_pMeshGroup->QueryInterface( IID_IFXUnknown, &rpOutData );
    //m_sphere.m_pMeshGroup->Release();
  rNeedRelease = TRUE;
  }
  else if ( inOutputDataElementIndex == m_uBoundSphereDataElementIndex )
  {
    m_pBoundSphereDataElement->Bound().Radius() = m_sphere.fRadius;
    m_pBoundSphereDataElement->QueryInterface( IID_IFXUnknown, &rpOutData );
    //m_pBoundSphereDataElement->Release();
  rNeedRelease = TRUE;
  }
  else if ( inOutputDataElementIndex == m_uNeighborMeshDataElementIndex )
  {
    IFXRELEASE( m_sphere.m_pNeighborMesh );
    if ( m_sphere.m_pNeighborMesh == NULL )
      IFXCreateComponent(CID_IFXNeighborMesh,
                         IID_IFXNeighborMesh,
                         (void**)&(m_sphere.m_pNeighborMesh));
    if (!m_sphere.m_pNeighborMesh)
      result = IFX_E_OUT_OF_MEMORY;
    if (IFXSUCCESS(result))
      result = m_sphere.m_pNeighborMesh->Allocate(*(m_sphere.m_pMeshGroup));
    if (IFXSUCCESS(result))
      result = m_sphere.m_pNeighborMesh->Build(*(m_sphere.m_pMeshGroup), NULL);
    if (IFXSUCCESS(result))
      result = m_sphere.m_pNeighborMesh->QueryInterface( IID_IFXUnknown, &rpOutData );
  rNeedRelease = TRUE;
//    if (IFXSUCCESS(result))
//      m_sphere.m_pNeighborMesh->Release();
  }
  else
  {
    IFXASSERTBOX( 1, "CIFXSphereGenerator::GetOutput() not implemented!" );
    result = IFX_E_UNDEFINED;
  }

  return result;
}


IFXRESULT CIFXSphereGenerator::SetDataPacket(
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

IFXRESULT CIFXSphereGenerator::Notify( IFXModifierMessage eInMessage,
                                       void*               pMessageContext )

{
  return IFX_OK;
}


// IFXPrimitive
IFXRESULT CIFXSphereGenerator::SetFacing(EIPrimitiveFacing eFacing) {
  IFXRESULT iResult = IFX_OK;

  if(IFXSUCCESS(iResult)) {
    m_sphere.uNormalOrientation = eFacing;
    if(m_pModifierDataPacket)
      m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
  }

  return iResult;
}

IFXRESULT CIFXSphereGenerator::GetFacing(EIPrimitiveFacing* pFacing) {
  IFXRESULT iResult = IFX_OK;

  if(NULL == pFacing)
    iResult = IFX_E_INVALID_POINTER;
  else
    *pFacing = (EIPrimitiveFacing)m_sphere.uNormalOrientation;

  return iResult;
}


// IFXSpherePrimitive
F32 CIFXSphereGenerator::GetRadius( void )
{
  return m_sphere.fRadius;
}


IFXRESULT CIFXSphereGenerator::SetRadius( F32 fInRadius )
{
  if ( fInRadius != m_sphere.fRadius )
  {
    if ( fInRadius > 0 )
    {
      m_sphere.fRadius = fInRadius;
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


F32 CIFXSphereGenerator::GetStartAngle( void )
{
  F32 fAngle= RadToDeg( m_sphere.fSolidAngleStart );
  return fAngle;
}


IFXRESULT CIFXSphereGenerator::SetStartAngle( F32 fInStartAngle )
{
  IFXRESULT iResult = IFX_OK;

  fInStartAngle = DegToRad( fInStartAngle );

  if (fInStartAngle < 0.0f || fInStartAngle > 360.0f)
    iResult = IFX_E_INVALID_RANGE;

  if (fInStartAngle > m_sphere.fSolidAngleEnd)
    iResult = IFX_E_INVALID_RANGE;

  if (IFXSUCCESS(iResult) && fInStartAngle != m_sphere.fSolidAngleStart)
  {
    m_sphere.fSolidAngleStart = fInStartAngle;
    if (m_pModifierDataPacket)
      m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
  }

  return iResult;
}


F32 CIFXSphereGenerator::GetEndAngle( void )
{
  return RadToDeg( m_sphere.fSolidAngleEnd );
}


IFXRESULT CIFXSphereGenerator::SetEndAngle( F32 fInEndAngle )
{
  IFXRESULT iResult = IFX_OK;

  fInEndAngle = DegToRad( fInEndAngle );

  if(fInEndAngle < 0.0f || fInEndAngle > 360.0f)
    iResult = IFX_E_INVALID_RANGE;

  if(fInEndAngle < m_sphere.fSolidAngleStart)
    iResult = IFX_E_INVALID_RANGE;

  if(IFXSUCCESS(iResult))
  {
    m_sphere.fSolidAngleEnd = fInEndAngle;
    if (m_pModifierDataPacket)
      m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
  }

  return iResult;
}

U32 CIFXSphereGenerator::GetResolution( void )
{
  return (m_sphere.uSolidNumSurfaceSlices-3);
}


IFXRESULT CIFXSphereGenerator::SetResolution( U32 uResolution )
{
  IFXRESULT iResult = IFX_OK;
  if(uResolution < 1)
    iResult = IFX_E_INVALID_RANGE;

  if ( IFXSUCCESS(iResult) && (uResolution != m_sphere.uSolidNumSurfaceSlices) )
  {
    uResolution+=3;   // adjustment so that this parameter can be 0 based
    m_sphere.uSolidNumSurfaceSlices = uResolution;
    m_sphere.uNumArcSlices = ( (2*uResolution) /3);
    if(m_pModifierDataPacket)
      m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
  }
  return iResult;
}


IFXRESULT CIFXSphereGenerator::SetBoundingSphere(const IFXVector4& vInBoundingSphere)
{
  m_pBoundSphereDataElement->Bound() = vInBoundingSphere;
  return m_pModifierDataPacket->InvalidateDataElement(m_uBoundSphereDataElementIndex);
}


IFXRESULT CIFXSphereGenerator::SetTransform(const IFXMatrix4x4& tInTransform)
{
  m_transform[0] = tInTransform;
  return m_pModifierDataPacket->InvalidateDataElement(m_uTransformDataElementIndex);
}


IFXRESULT CIFXSphereGenerator::InvalidateTransform()
{
  if(m_pModifierDataPacket)
  {
    m_pModifierDataPacket->InvalidateDataElement(m_uTransformDataElementIndex);
  }
  return IFX_OK;
}
