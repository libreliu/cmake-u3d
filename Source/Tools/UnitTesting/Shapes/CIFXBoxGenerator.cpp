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
//  CIFXBoxGenerator.cpp
//
//  DESCRIPTION
//
//  NOTES
//
//***************************************************************************

#include "CIFXBoxGenerator.h"
#include "IFXCheckX.h"
#include "IFXShapesCIDs.h"
#include "IFXExportingCIDs.h"
#include "IFXEncoderX.h"
#include "IFXException.h"
#include "IFXSceneGraph.h"
#include "IFXModifierDataPacket.h"
#include "IFXDids.h"
#include "IFXCOM.h"

CIFXBoxGenerator::CIFXBoxGenerator()
{
  m_uRefCount = 0;

  m_uTransformDataElementIndex    = (U32)-1;
  m_uMeshGroupDataElementIndex    = (U32)-1;
  m_uNeighborMeshDataElementIndex = (U32)-1;
  m_uBoundSphereDataElementIndex  = (U32)-1;

  m_pPrimitiveGenerator = NULL;
  m_box.uPrimitiveType = IFX_BOX;
  m_box.bSolidCapStart = TRUE;
  m_box.bSolidCapEnd = TRUE;
  m_box.bTopCap = TRUE;
  m_box.bBottomCap = TRUE;
  m_box.bRight = TRUE;
  m_box.bBack = TRUE;
  m_box.bLeft = TRUE;
  m_box.bRight = TRUE;
  m_box.fHeight = 50.0f;
  m_box.fWidth = 50.0f;
  m_box.fLength = 50.0f;
  m_box.uHeightVertices = 2;
  m_box.uWidthVertices = 2;
  m_box.uLengthVertices = 2;
  m_box.uNormalOrientation = IFXPrimitive::eFrontFacing;
  m_box.ePrimitiveOrientation = eUninitialized;

  m_box.m_pMeshGroup    = NULL;
  m_box.m_pNeighborMesh = NULL;

  m_pBoundSphereDataElement = NULL;

  m_transform.CreateNewElement();
  m_transform[0].Reset();
}


CIFXBoxGenerator::~CIFXBoxGenerator()
{
  IFXRELEASE(m_pPrimitiveGenerator);
  IFXRELEASE(m_pBoundSphereDataElement);
  IFXRELEASE(m_box.m_pMeshGroup);
  IFXRELEASE(m_box.m_pNeighborMesh); // Smart pointer crashing on release.
}


IFXRESULT IFXAPI_CALLTYPE CIFXBoxGenerator_Factory( IFXREFIID iid, void** ppv )
{
  IFXRESULT result;

  if (ppv)
  {
    // Create the CIFXClassName component.
    CIFXBoxGenerator *pGenerator = new CIFXBoxGenerator;

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
U32 CIFXBoxGenerator::AddRef()
{
  return ++m_uRefCount;
}


U32 CIFXBoxGenerator::Release()
{
  if( 1 == m_uRefCount )
  {
    CIFXModifier::PreDestruct();
    delete this;
    return 0;
  }

  return --m_uRefCount;
}


IFXRESULT CIFXBoxGenerator::QueryInterface( IFXREFIID riid, void **ppv )
{
  IFXRESULT result = IFX_OK;

  if ( ppv )
  {
    if ( riid == IID_IFXUnknown )
      *ppv = (IFXUnknown*)this;
    else if ( riid == IID_IFXBoxGenerator   )
      *ppv = (IFXBoxGenerator*)this;
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
IFXRESULT CIFXBoxGenerator::InitializeObject( )
{
  IFXRESULT result = IFX_OK;

  if ( !m_pBoundSphereDataElement )
    result = IFXCreateComponent(CID_IFXBoundSphereDataElement, IID_IFXBoundSphereDataElement, (void**) &m_pBoundSphereDataElement);

  if ( IFXSUCCESS(result) )
    result = IFXCreateComponent(CID_IFXPrimitiveGenerator, IID_IFXPrimitiveGenerator, (void**) &m_pPrimitiveGenerator);

  if ( IFXSUCCESS(result) )
  {
  m_box.ePrimitiveOrientation = enegZFrontposYup;
  }

  return result;
}



// IFXMarkerX interface...
void CIFXBoxGenerator::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
//  CIFXMarker::GetEncoderX(CID_IFXBoxGeneratorEncoder, rpEncoderX);
}

// IFXModifier
const IFXGUID* CIFXBoxGenerator::m_scpOutputDIDs[] =
{
  &DID_IFXTransform,
  &DID_IFXRenderableGroup,
  &DID_IFXNeighborMesh,
  &DID_IFXRenderableGroupBounds
};

const IFXGUID* CIFXBoxGenerator::m_scpNeighborMeshDeps[] =
{
  &DID_IFXRenderableGroup
};


IFXRESULT CIFXBoxGenerator::GetOutputs( IFXGUID**& rpOutOutputs,
                                      U32&       rOutNumberOfOutputs,
                    U32*&      rpOutOutputDepAttrs )
{
  rOutNumberOfOutputs = sizeof(m_scpOutputDIDs)/ sizeof(IFXGUID*);
  rpOutOutputDepAttrs = NULL;
  rpOutOutputs = (IFXGUID**)&m_scpOutputDIDs;
  return IFX_OK;
}


IFXRESULT CIFXBoxGenerator::GetDependencies(
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
    IFXASSERTBOX( 1, "CIFXBoxGenerator::GetDependencies() called with unknown output!" );
    result = IFX_E_UNDEFINED;
  }

  return result;
}

IFXRESULT CIFXBoxGenerator::GenerateOutput( U32    inOutputDataElementIndex,
                                            void*& rpOutData, BOOL& rNeedRelease )
{
  IFXRESULT result = IFX_OK;

  if ( inOutputDataElementIndex == m_uTransformDataElementIndex )
    rpOutData = &m_transform;
  else if ( inOutputDataElementIndex == m_uMeshGroupDataElementIndex )
  {
    m_pPrimitiveGenerator->GeneratePrimitive(&m_box);
    if (!m_box.m_pMeshGroup)
    {
      if (!m_DegenerateMeshGroup.GetPointerNR())
        m_DegenerateMeshGroup.Create(CID_IFXMeshGroup, IID_IFXMeshGroup);
      m_box.m_pMeshGroup = m_DegenerateMeshGroup.GetPointerAR();
    }
    m_box.m_pMeshGroup->QueryInterface( IID_IFXUnknown, &rpOutData );
//    m_box.m_pMeshGroup->Release();
  rNeedRelease = TRUE;
  }
  else if ( inOutputDataElementIndex == m_uBoundSphereDataElementIndex )
  {
    IFXVector3 v(m_box.fHeight * 0.5f, m_box.fWidth * 0.5f, m_box.fLength * 0.5f);
    m_pBoundSphereDataElement->Bound().Radius() = v.CalcMagnitude();
    m_pBoundSphereDataElement->QueryInterface( IID_IFXUnknown, &rpOutData );
//    m_pBoundSphereDataElement->Release();
  rNeedRelease = TRUE;
  }
  else if ( inOutputDataElementIndex == m_uNeighborMeshDataElementIndex )
  {
    if ( m_box.m_pNeighborMesh == NULL )
      IFXCreateComponent(CID_IFXNeighborMesh,
                 IID_IFXNeighborMesh,
                 (void**)&(m_box.m_pNeighborMesh));
    if (!m_box.m_pNeighborMesh)
      result = IFX_E_OUT_OF_MEMORY;
    if (IFXSUCCESS(result))
      result = m_box.m_pNeighborMesh->Allocate(*(m_box.m_pMeshGroup));
    if (IFXSUCCESS(result))
      result = m_box.m_pNeighborMesh->Build(*(m_box.m_pMeshGroup), NULL);
    if (IFXSUCCESS(result))
      result = m_box.m_pNeighborMesh->QueryInterface( IID_IFXUnknown, &rpOutData );
//    if (IFXSUCCESS(result))
//      m_box.m_pNeighborMesh->Release();
  rNeedRelease = TRUE;
  }
  else
  {
    IFXASSERTBOX( 1, "CIFXBoxGenerator::GetOutput() not implemented!" );
    result = IFX_E_UNDEFINED;
  }

  return result;
}


IFXRESULT CIFXBoxGenerator::SetDataPacket(
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

    result |= pInDataPacket->GetDataElementIndex(
      DID_IFXTransform,
      m_uTransformDataElementIndex );

    result |= pInDataPacket->GetDataElementIndex(
      DID_IFXRenderableGroup,
      m_uMeshGroupDataElementIndex );

    m_pBoundSphereDataElement->RenderableIndex() =
      m_uMeshGroupDataElementIndex;

    result |= pInDataPacket->GetDataElementIndex(
      DID_IFXNeighborMesh,
      m_uNeighborMeshDataElementIndex );

    result |= pInDataPacket->GetDataElementIndex(
      DID_IFXRenderableGroupBounds,
      m_uBoundSphereDataElementIndex );
  }

  return result;
}

IFXRESULT CIFXBoxGenerator::Notify( IFXModifierMessage eInMessage,
                                      void*               pMessageContext )
{
  return IFX_OK;
}


// IFXPrimitive
IFXRESULT CIFXBoxGenerator::SetFacing(EIPrimitiveFacing eFacing)
{
  if ( m_box.uNormalOrientation != eFacing )
  {
    m_box.uNormalOrientation = eFacing;
    if(m_pModifierDataPacket)
    {
      m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
    }
  }

  return IFX_OK;
}


IFXRESULT CIFXBoxGenerator::GetFacing(EIPrimitiveFacing* pFacing) {
  IFXRESULT iResult = IFX_OK;

  if(NULL == pFacing)
    iResult = IFX_E_INVALID_POINTER;
  else
    *pFacing = (EIPrimitiveFacing)m_box.uNormalOrientation;

  return iResult;
}


// IFXBoxPrimitive
BOOL CIFXBoxGenerator::GetTop( void )
{
  return m_box.bTopCap;
}


IFXRESULT CIFXBoxGenerator::SetTop( BOOL bInTop )
{
  if ( bInTop != (BOOL) m_box.bTopCap )
  {
    m_box.bTopCap = bInTop;
    if(m_pModifierDataPacket)
      m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
  }
  return IFX_OK;
}


BOOL CIFXBoxGenerator::GetBottom( void )
{
  return m_box.bBottomCap;
}


IFXRESULT CIFXBoxGenerator::SetBottom( BOOL bInBottom )
{
  if ( bInBottom != (BOOL) m_box.bBottomCap )
  {
    m_box.bBottomCap = bInBottom;
    if(m_pModifierDataPacket)
      m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
  }
  return IFX_OK;
}


BOOL CIFXBoxGenerator::GetFront( void )
{
  return m_box.bFront;
}


IFXRESULT CIFXBoxGenerator::SetFront( BOOL bInFront )
{
  if ( bInFront != (BOOL) m_box.bFront )
  {
    m_box.bFront = bInFront;
    if(m_pModifierDataPacket)
      m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
  }
  return IFX_OK;
}


BOOL CIFXBoxGenerator::GetBack( void )
{
  return m_box.bBack;
}


IFXRESULT CIFXBoxGenerator::SetBack( BOOL bInBack )
{
  if ( bInBack != (BOOL) m_box.bBack )
  {
    m_box.bBack = bInBack;
    if(m_pModifierDataPacket)
      m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
  }
  return IFX_OK;
}


BOOL CIFXBoxGenerator::GetLeft( void )
{
  return m_box.bLeft;
}


IFXRESULT CIFXBoxGenerator::SetLeft( BOOL bInLeft )
{
  if ( bInLeft != (BOOL) m_box.bLeft )
  {
    m_box.bLeft = bInLeft;
    if(m_pModifierDataPacket)
      m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
  }
  return IFX_OK;
}


BOOL CIFXBoxGenerator::GetRight( void )
{
  return m_box.bRight;
}


IFXRESULT CIFXBoxGenerator::SetRight( BOOL bInRight )
{
  if ( bInRight != (BOOL) m_box.bRight )
  {
    m_box.bRight = bInRight;
    if(m_pModifierDataPacket)
      m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
  }
  return IFX_OK;
}


F32 CIFXBoxGenerator::GetHeight( void )
{
  return m_box.fHeight;
}


IFXRESULT CIFXBoxGenerator::SetHeight( F32 fInHeight )
{
  if ( fInHeight != m_box.fHeight )
  {
    m_box.fHeight = fInHeight;
    if(m_pModifierDataPacket)
    {
      m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
      m_pModifierDataPacket->InvalidateDataElement( m_uBoundSphereDataElementIndex );
    }
  }
  return IFX_OK;
}


F32 CIFXBoxGenerator::GetWidth( void )
{
  return m_box.fWidth;
}


IFXRESULT CIFXBoxGenerator::SetWidth( F32 fInWidth )
{
  if ( fInWidth != m_box.fWidth )
  {
    m_box.fWidth = fInWidth;
    if(m_pModifierDataPacket)
    {
      m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
      m_pModifierDataPacket->InvalidateDataElement( m_uBoundSphereDataElementIndex );
    }
  }
  return IFX_OK;
}


F32 CIFXBoxGenerator::GetLength( void )
{
  return m_box.fLength;
}


IFXRESULT CIFXBoxGenerator::SetLength( F32 fInLength )
{
  if ( fInLength != m_box.fLength )
  {
    m_box.fLength = fInLength;
    if(m_pModifierDataPacket)
    {
      m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
      m_pModifierDataPacket->InvalidateDataElement( m_uBoundSphereDataElementIndex );
    }
  }
  return IFX_OK;
}


U32 CIFXBoxGenerator::GetNumHeightVertices( void )
{
  return m_box.uHeightVertices;
}


IFXRESULT CIFXBoxGenerator::SetNumHeightVertices( U32 iInNumHeightVertices )
{
  if(iInNumHeightVertices < 2)
    return IFX_E_INVALID_RANGE;

  if ( iInNumHeightVertices != m_box.uHeightVertices )
  {
    m_box.uHeightVertices = iInNumHeightVertices;
    if(m_pModifierDataPacket)
    {
      m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
    }
  }
  return IFX_OK;
}


U32 CIFXBoxGenerator::GetNumWidthVertices( void )
{
  return m_box.uWidthVertices;
}


IFXRESULT CIFXBoxGenerator::SetNumWidthVertices( U32 iInNumWidthVertices )
{
  if(iInNumWidthVertices < 2)
    return IFX_E_INVALID_RANGE;

  if ( iInNumWidthVertices != m_box.uWidthVertices )
  {
    m_box.uWidthVertices = iInNumWidthVertices;
    if(m_pModifierDataPacket)
    {
      m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
    }
  }
  return IFX_OK;
}


U32 CIFXBoxGenerator::GetNumLengthVertices( void )
{
  return m_box.uLengthVertices;
}


IFXRESULT CIFXBoxGenerator::SetNumLengthVertices( U32 iInNumLengthVertices )
{
  if(iInNumLengthVertices < 2)
    return IFX_E_INVALID_RANGE;

  if ( iInNumLengthVertices != m_box.uLengthVertices )
  {
    m_box.uLengthVertices = iInNumLengthVertices;
    if(m_pModifierDataPacket)
      m_pModifierDataPacket->InvalidateDataElement( m_uMeshGroupDataElementIndex );
  }
  return IFX_OK;
}


IFXRESULT CIFXBoxGenerator::SetBoundingSphere(const IFXVector4& vInBoundingSphere)
{
  m_pBoundSphereDataElement->Bound() = vInBoundingSphere;
  return m_pModifierDataPacket->InvalidateDataElement(m_uBoundSphereDataElementIndex);
}


IFXRESULT CIFXBoxGenerator::SetTransform(const IFXMatrix4x4& tInTransform)
{
  m_transform[0] = tInTransform;
  return m_pModifierDataPacket->InvalidateDataElement(m_uTransformDataElementIndex);
}


IFXRESULT CIFXBoxGenerator::InvalidateTransform()
{
  if(m_pModifierDataPacket)
  {
    m_pModifierDataPacket->InvalidateDataElement(m_uTransformDataElementIndex);
  }
  return IFX_OK;
}
