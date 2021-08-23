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
//  CIFXPrimitiveGenerator.cpp
//
//  DESCRIPTION
//    Implementation file for the primitives generator class
//
//  NOTES
//      None.
//
//***************************************************************************
#include "CIFXPrimitiveGenerator.h"
#include "IFXCoreCIDs.h"
#include "IFXCOM.h"
#include <math.h>

//---------------------------------------------------------------------------
//  CIFXPrimitiveGenerator_Factory
//
//  This is the CIFXPrimitiveGenerator friend component factory function.  The
//  CIFXPrimitiveGenerator component is not a singleton.
//---------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXPrimitiveGenerator_Factory( IFXREFIID interfaceId, void** ppInterface )
{
  IFXRESULT result;

  if ( ppInterface )
  {
    // It doesn't exist, so try to create it.  Note:  The component
    // class sets up gs_pSingleton upon construction and NULLs it
    // upon destruction.
    CIFXPrimitiveGenerator  *pComponent = new CIFXPrimitiveGenerator;

    if ( pComponent )
    {
      // Perform a temporary AddRef for our usage of the component.
      pComponent->AddRef();

      // Attempt to obtain a pointer to the requested interface.
      result = pComponent->QueryInterface( interfaceId, ppInterface );

      // Perform a Release since our usage of the component is now
      // complete.  Note:  If the QI fails, this will cause the
      // component to be destroyed.
      pComponent->Release();
    }
    else
      result = IFX_E_OUT_OF_MEMORY;
  }
  else
    result = IFX_E_INVALID_POINTER;

  return result;
}


//---------------------------------------------------------------------------
//  CIFXPrimitiveGenerator
//
//  This is the CIFXPrimitiveGenerator default constructor.  It sets the
//  reference count of this object to zero
//---------------------------------------------------------------------------
CIFXPrimitiveGenerator::CIFXPrimitiveGenerator()
{
  m_uRefCount = 0;
}

//---------------------------------------------------------------------------
//  ~CIFXPrimitiveGenerator
//
//  This is the CIFXPrimitiveGenerator default destructor.
//---------------------------------------------------------------------------
CIFXPrimitiveGenerator::~CIFXPrimitiveGenerator()
{
  ;
}


// IFXUnknown methods...

//---------------------------------------------------------------------------
//  CIFXPrimitiveGenerator::AddRef
//
//  This method increments the reference count for an interface on a
//  component.  It should be called for every new copy of a pointer to an
//  interface on a given component.  It returns a U32 that contains a value
//  from 1 to 2^32 - 1 that defines the new reference count.  The return
//  value should only be used for debugging purposes.
//---------------------------------------------------------------------------
U32 CIFXPrimitiveGenerator::AddRef(void)
{
  return ++m_uRefCount;
}

//---------------------------------------------------------------------------
//  CIFXPrimitiveGenerator::Release
//
//  This method decrements the reference count for the calling interface on a
//  component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//  that defines the new reference count.  The return value should only be
//  used for debugging purposes.  If the reference count on a component falls
//  to zero, the component is destroyed.
//---------------------------------------------------------------------------
U32 CIFXPrimitiveGenerator::Release(void)
{
  if( 1 == m_uRefCount )
  {
    delete this;
    return 0;
  }

  return --m_uRefCount;
}

//---------------------------------------------------------------------------
//  CIFXPrimitiveGenerator::QueryInterface
//
//  This method provides access to the various interfaces supported by a
//  component.  Upon success, it increments the component's reference count,
//  hands back a pointer to the specified interface and returns IFX_OK.
//  Otherwise, it returns either IFX_E_INVALID_POINTER or IFX_E_UNSUPPORTED.
//
//  A number of rules must be adhered to by all implementations of
//  QueryInterface.  For a list of such rules, refer to the Microsoft COM
//  description of the IUnknown::QueryInterface method.
//---------------------------------------------------------------------------
IFXRESULT CIFXPrimitiveGenerator::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
  IFXRESULT result  = IFX_OK;

  if ( ppInterface )
  {
    if ( interfaceId == IID_IFXUnknown )
    {
      *ppInterface = ( IFXUnknown* ) this;
    }
    else if ( interfaceId == IID_IFXPrimitiveGenerator )
    {
      *ppInterface = ( IFXPrimitiveGenerator* ) this;
    }
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


// CIFXPrimitiveGenerator methods...

//---------------------------------------------------------------------------
//  CIFXPrimitiveGenerator::GeneratePrimitiveFromArc
//
//  This method creates an IFXmesh from an arc specified in the pPrimitive
//  data structure.
//
//  Upon success, it creates an IFXMeshGroup and returns IFX_OK.  Otherwise
//  it returns the proper error code from the sub-procedures.
//---------------------------------------------------------------------------
IFXRESULT CIFXPrimitiveGenerator::GeneratePrimitiveFromArc(CIFXPrimitiveShape* pPrimitive)
{
  IFXRESULT ifxResult = 0;

  ifxResult = MeshBuilder(pPrimitive);
  if(ifxResult == IFX_OK)
    ifxResult = GenerateMeshGroup(pPrimitive);

  return ifxResult;
}

//---------------------------------------------------------------------------
//  CIFXPrimitiveGenerator::MeshBuilder
//
//  This method runs through all the creation procedures for creating an
//  IFXMesh given a user specified arc to rotate.
//
//  Upon success, it creates an IFXMeshGroup and return IFX_OK.  Otherwise
//  it returns the proper error code from the sub-procedures.
//---------------------------------------------------------------------------
IFXRESULT CIFXPrimitiveGenerator::MeshBuilder(CIFXPrimitiveShape* pPrimitive)
{
  IFXRESULT ifxResult = 0;


  if(pPrimitive->uNormalOrientation == IFXPrimitive::eBackFacing || pPrimitive->uNormalOrientation == IFXPrimitive::eBothFacing)
  {

    ifxResult = MeshBuilder(pPrimitive->uNumArcSlices, pPrimitive->uSolidNumSurfaceSlices,
      pPrimitive->pArcCoords, pPrimitive->pArcNormals,
      &pPrimitive->pPrimitiveMeshInverse, pPrimitive);

    if(ifxResult!=IFX_OK)
      return ifxResult;

    // generate top cap (if specified)
    if(pPrimitive->bTopCap && pPrimitive->uNumTopCapVerts != 0)
    {
      ifxResult = MeshBuilder(pPrimitive->uNumTopCapVerts, pPrimitive->uSolidNumSurfaceSlices,
        pPrimitive->pArcTopCapVerts, pPrimitive->pArcTopCapNormals,
        &pPrimitive->pTopCapMeshInverse, pPrimitive);
      if(ifxResult!=IFX_OK)
        return ifxResult;
    }

    // generate bottom cap (if specified)
    U8 eOrientation = pPrimitive->uNormalOrientation;
    pPrimitive->uNormalOrientation = IFXPrimitive::eFrontFacing;
    if(pPrimitive->bBottomCap && pPrimitive->uNumBottomCapVerts != 0)
    {
      ifxResult = MeshBuilder(pPrimitive->uNumBottomCapVerts, pPrimitive->uSolidNumSurfaceSlices,
        pPrimitive->pArcBottomCapVerts, pPrimitive->pArcBottomCapNormals,
        &pPrimitive->pBottomCapMeshInverse, pPrimitive);
      if(ifxResult!=IFX_OK)
        return ifxResult;
    }
    pPrimitive->uNormalOrientation = eOrientation;

    if(pPrimitive->bSolidCapStart)
    {
      ifxResult = CapBuilder(pPrimitive->fSolidAngleStart, TRUE, &pPrimitive->pSolidCapStartMeshInverse,
            pPrimitive);
      if(ifxResult!=IFX_OK)
        return ifxResult;
    }

    if(pPrimitive->bSolidCapEnd)
    {
      ifxResult = CapBuilder(pPrimitive->fSolidAngleEnd, FALSE, &pPrimitive->pSolidCapEndMeshInverse,
            pPrimitive);
      if(ifxResult!=IFX_OK)
        return ifxResult;
    }

  }

  // do other orientation
  if(pPrimitive->uNormalOrientation == IFXPrimitive::eFrontFacing  || pPrimitive->uNormalOrientation == IFXPrimitive::eBothFacing)
  {
    ifxResult = MeshBuilder(pPrimitive->uNumArcSlices, pPrimitive->uSolidNumSurfaceSlices,
      pPrimitive->pArcCoords, pPrimitive->pArcNormals,
      &pPrimitive->m_pPrimitiveMesh, pPrimitive);
    if(ifxResult!=IFX_OK)
      return ifxResult;

    // generate top cap (if specified)
    if(pPrimitive->bTopCap && pPrimitive->uNumTopCapVerts !=0 )
    {
      ifxResult = MeshBuilder(pPrimitive->uNumTopCapVerts, pPrimitive->uSolidNumSurfaceSlices,
        pPrimitive->pArcTopCapVerts, pPrimitive->pArcTopCapNormals,
        &pPrimitive->pTopCapMesh, pPrimitive);
      if(ifxResult!=IFX_OK)
        return ifxResult;
    }

    // generate bottom cap (if specified)
    U8 eOrientation = pPrimitive->uNormalOrientation;
    pPrimitive->uNormalOrientation = IFXPrimitive::eBackFacing;
    if(pPrimitive->bBottomCap && pPrimitive->uNumBottomCapVerts !=0 )
    {
      ifxResult = MeshBuilder(pPrimitive->uNumBottomCapVerts, pPrimitive->uSolidNumSurfaceSlices,
        pPrimitive->pArcBottomCapVerts, pPrimitive->pArcBottomCapNormals,
        &pPrimitive->pBottomCapMesh, pPrimitive);
      if(ifxResult!=IFX_OK)
        return ifxResult;
    }
    pPrimitive->uNormalOrientation = eOrientation;

    if(pPrimitive->bSolidCapStart)
    {
      ifxResult = CapBuilder(pPrimitive->fSolidAngleStart, TRUE, &pPrimitive->pSolidCapStartMesh,
            pPrimitive);
      if(ifxResult!=IFX_OK)
        return ifxResult;
    }

    if(pPrimitive->bSolidCapEnd)
    {
      ifxResult = CapBuilder(pPrimitive->fSolidAngleEnd, FALSE, &pPrimitive->pSolidCapEndMesh,
            pPrimitive);
      if(ifxResult!=IFX_OK)
        return ifxResult;
    }
  }

  return ifxResult;
}

//---------------------------------------------------------------------------
//  CIFXPrimitiveGenerator::GeneratePrimitive
//
//  This function creates an IFXMesh with the specified primitive.  It first
//  creates the primitives arc to be rotated and stores a pointer to those new
//  vertices in the
//---------------------------------------------------------------------------
IFXRESULT CIFXPrimitiveGenerator::GeneratePrimitive( CIFXPrimitiveShape* pPrimitive )
{
  IFXRESULT ifxResult = 0;
  bool    bBothFacing = FALSE;

  // clear out any existing meshgroups
  U32 i;
  for (i = 0; i < sizeof(pPrimitive->m_pAllMeshes)/sizeof(pPrimitive->m_pAllMeshes[0]); i++ )
  {
    if( pPrimitive->m_pAllMeshes[i] )
    {
      pPrimitive->m_uLastMeshId[i] = pPrimitive->m_pAllMeshes[i]->GetId();
      IFXRELEASE( pPrimitive->m_pAllMeshes[i] );
    }
    else
      pPrimitive->m_uLastMeshId[i] = 0;
  }
//  IFXRELEASE( pPrimitive->m_pPrimitiveMesh );
//  IFXRELEASE( pPrimitive->pTopCapMesh );
//  IFXRELEASE( pPrimitive->pBottomCapMesh );
//  IFXRELEASE( pPrimitive->pPrimitiveMeshInverse );
//  IFXRELEASE( pPrimitive->pTopCapMeshInverse );
//  IFXRELEASE( pPrimitive->pBottomCapMeshInverse );
//  IFXRELEASE( pPrimitive->pFrontMesh );
//  IFXRELEASE( pPrimitive->pFrontMeshInverse );
//  IFXRELEASE( pPrimitive->pBackMesh );
//  IFXRELEASE( pPrimitive->pBackMeshInverse );
//  IFXRELEASE( pPrimitive->pSolidCapStartMesh );
//  IFXRELEASE( pPrimitive->pSolidCapEndMesh );
//  IFXRELEASE( pPrimitive->pSolidCapStartMeshInverse );
//  IFXRELEASE( pPrimitive->pSolidCapEndMeshInverse );

  IFXRELEASE( pPrimitive->m_pNeighborMesh );

  if(pPrimitive->uNormalOrientation == IFXPrimitive::eBothFacing)
  {
    bBothFacing = TRUE;
    pPrimitive->uNormalOrientation = IFXPrimitive::eFrontFacing;
  }

  // Rectalinear primitives are generated differently
  if( pPrimitive->uPrimitiveType == IFX_BOX )
  {
    // box
    if(bBothFacing == TRUE)
      pPrimitive->uNormalOrientation = IFXPrimitive::eBothFacing;

    ifxResult = pPrimitive->GenerateMesh();

    if( ifxResult != IFX_OK )
      return ifxResult;

  }
  else if( pPrimitive->uPrimitiveType == IFX_PLANE )
  {
    // plane
    if( bBothFacing == FALSE )
    {
      ifxResult = pPrimitive->GenerateMesh();
    }
    else
    {
      pPrimitive->uNormalOrientation = IFXPrimitive::eBackFacing;
      ifxResult = pPrimitive->GenerateMesh();

      pPrimitive->uNormalOrientation = IFXPrimitive::eFrontFacing;
      ifxResult = pPrimitive->GenerateMesh();

      pPrimitive->uNormalOrientation = IFXPrimitive::eBothFacing;
    }
  }
  else
  {
    // build the primitive using surface of rotation
    ifxResult = GeneratePrimitiveArc( pPrimitive );
    if(ifxResult != IFX_OK)
      return ifxResult;

    // generate the rotated IFXMesh(es)
    ifxResult = MeshBuilder( pPrimitive );
    if(ifxResult != IFX_OK)
      return ifxResult;

    // if both-facing, do it again with other orientation
    if(bBothFacing)
    {
      pPrimitive->uNormalOrientation = IFXPrimitive::eBackFacing;
      if(pPrimitive->uPrimitiveType != IFX_CUSTOM)
        pPrimitive->ClearImmediateData();

      // generate new arc/normals
      ifxResult = GeneratePrimitiveArc(pPrimitive);
      if(ifxResult!=IFX_OK && pPrimitive->uPrimitiveType!=IFX_CUSTOM)
        return ifxResult;

      // generate other orientation IFXMesh(es)
      ifxResult = MeshBuilder(pPrimitive);
      if(ifxResult != IFX_OK)
        return ifxResult;

      pPrimitive->uNormalOrientation = IFXPrimitive::eBothFacing;
    }
  }

  // build the ending mesh group
  GenerateMeshGroup(pPrimitive);

  // do the final orientation fixes based on the global setting
  U32 uIndex = 0;
  U32 x = 0;
  U32 uMax = 0;
  U32 nVertices = 0;
  IFXMesh* pMesh = NULL;
  IFXVector3Iter v3iVertex;
  IFXVector3Iter v3iNormal;
  IFXVector3* pvVertex;
  IFXVector3* pvNormal;
  F32 fcosX, fsinX, fcosY, fsinY, fcosZ, fsinZ;

  fcosX = 1;  fsinX = 0; // no rotations
  fcosY = 1;  fsinY = 0;
  fcosZ = 1;  fsinZ = 0;

  switch(pPrimitive->ePrimitiveOrientation)
  {
  case IFXPrimitive::enegZFrontposYup:
  case IFXPrimitive::eUninitialized:
    if((pPrimitive->uPrimitiveType == IFX_BOX) || (pPrimitive->uPrimitiveType == IFX_PLANE))
    {
      fcosX = 1;  fsinX = 0;
      fcosY = 1;  fsinY = 0;
      fcosZ = 1;  fsinZ = 0;
    }
    else
    {
      fcosX = 0;  fsinX = -1;   // rotate 270 around x
      fcosY = 1;  fsinY = 0;
      fcosZ = 1;  fsinZ = 0;
    }
    break;

  case IFXPrimitive::eposZFrontposYup:
    if((pPrimitive->uPrimitiveType == IFX_BOX) || (pPrimitive->uPrimitiveType == IFX_PLANE))
    {
      fcosX = 1;  fsinX = 0;
      fcosY = -1; fsinY = 0;    // rotate 180 about y
      fcosZ = 1;  fsinZ = 0;
    }
    else
    {
      fcosX = 0;  fsinX = -1;   // rotate 270 around x
      fcosY = -1; fsinY = 0;    // rotate 180 about y
      fcosZ = 1;  fsinZ = 0;
    }
    break;
  }

  if( pPrimitive )
  {
    if( pPrimitive->m_pMeshGroup )
    {
      uMax = pPrimitive->m_pMeshGroup->GetNumMeshes();

      for( uIndex = 0; uIndex<uMax; uIndex++ )
      {
        pPrimitive->m_pMeshGroup->GetMesh(uIndex, pMesh);
        if(pMesh)
        {
          nVertices = pMesh->GetNumVertices();
          pMesh->GetPositionIter(v3iVertex);
          pMesh->GetNormalIter(v3iNormal);
          for(x = 0; x<nVertices; x++)
          {
            pvVertex = v3iVertex.Next();
            pvNormal = v3iNormal.Next();

            // rotate on x axis
            pvVertex->Set(  pvVertex->X(),
              pvVertex->Y()*fcosX-pvVertex->Z()*fsinX,
              pvVertex->Y()*fsinX+pvVertex->Z()*fcosX);
            pvNormal->Set(  pvNormal->X(),
              pvNormal->Y()*fcosX-pvNormal->Z()*fsinX,
              pvNormal->Y()*fsinX+pvNormal->Z()*fcosX);

            // rotate on y axis
            pvVertex->Set(  pvVertex->Z()*fsinY+pvVertex->X()*fcosY,
              pvVertex->Y(),
              pvVertex->Z()*fcosY-pvVertex->X()*fsinY);
            pvNormal->Set(  pvNormal->Z()*fsinY+pvNormal->X()*fcosY,
              pvNormal->Y(),
              pvNormal->Z()*fcosY-pvNormal->X()*fsinY);

            // rotate on z axis
            pvVertex->Set(  pvVertex->X()*fcosZ - pvVertex->Y()*fsinZ,
              pvVertex->Y()*fsinZ + pvVertex->Y()*fcosZ,
              pvVertex->Z());
            pvNormal->Set(  pvNormal->X()*fcosZ - pvNormal->Y()*fsinZ,
              pvNormal->Y()*fsinZ + pvNormal->Y()*fcosZ,
              pvNormal->Z());
          }
        }

        IFXRELEASE(pMesh);
      }
    }
  }

  return ifxResult;
}


void CIFXPrimitiveGenerator::SetAMesh( CIFXPrimitiveShape *pPrimitiveShape, U32 uIndex, U32 *puIndex )
{
  if( pPrimitiveShape->m_pAllMeshes[uIndex] )
  {
    IFXMesh *pLastMesh = NULL;
    pPrimitiveShape->m_pMeshGroup->GetMesh( *puIndex, pLastMesh );

    U32 uNewId = 0;

    if ( pLastMesh )
    {
      uNewId = pLastMesh->GetId();
      pLastMesh->Release();
    }
    else if ( pPrimitiveShape->m_uLastMeshId[uIndex] )
    {
      uNewId = pPrimitiveShape->m_uLastMeshId[uIndex];
    }

    if ( uNewId )
      pPrimitiveShape->m_pAllMeshes[uIndex]->SetId( uNewId );

    pPrimitiveShape->m_pMeshGroup->SetMesh( *puIndex, pPrimitiveShape->m_pAllMeshes[uIndex] );

    (*puIndex)++;
  }
}

//---------------------------------------------------------------------------
//  CIFXPrimitiveGenerator::GenerateMeshGroup
//
//  This function creates an IFXMeshGroup from the various IFXMeshes that were
//  generated by the GeneratePrimitive/GeneratePrimitiveFromArc functions.  It
//  allocates the IFXMeshGroup structure and stores a pointer to it in the
//  primitive structure.
//---------------------------------------------------------------------------
IFXRESULT CIFXPrimitiveGenerator::GenerateMeshGroup(CIFXPrimitiveShape* pPrimitiveShape)
{
  U32 uNumberOfMeshes = 0;
  U32 uIndex = 0;
  IFXRESULT iResult = IFX_OK;

  if(NULL == pPrimitiveShape)
    iResult = IFX_E_INVALID_POINTER;

  if(IFXSUCCESS(iResult))
  {
    // count the meshes
	U32 i;
    for (i = 0; i < sizeof(pPrimitiveShape->m_pAllMeshes)/sizeof(pPrimitiveShape->m_pAllMeshes[0]); i++ )
    {
      if( pPrimitiveShape->m_pAllMeshes[i] )
        uNumberOfMeshes++;
    }

/*
    if(pPrimitiveShape->pSolidCapStartMesh)
      uNumberOfMeshes++;
    if(pPrimitiveShape->pSolidCapStartMeshInverse)
      uNumberOfMeshes++;
    if(pPrimitiveShape->pSolidCapEndMesh)
      uNumberOfMeshes++;
    if(pPrimitiveShape->pSolidCapEndMeshInverse)
      uNumberOfMeshes++;
    if(pPrimitiveShape->pBottomCapMesh)
      uNumberOfMeshes++;
    if(pPrimitiveShape->pBottomCapMeshInverse)
      uNumberOfMeshes++;
    if(pPrimitiveShape->pTopCapMesh)
      uNumberOfMeshes++;
    if(pPrimitiveShape->pTopCapMeshInverse)
      uNumberOfMeshes++;
    if(pPrimitiveShape->m_pPrimitiveMesh)
      uNumberOfMeshes++;
    if(pPrimitiveShape->pPrimitiveMeshInverse)
      uNumberOfMeshes++;
    if(pPrimitiveShape->pFrontMesh)
      uNumberOfMeshes++;
    if(pPrimitiveShape->pFrontMeshInverse)
      uNumberOfMeshes++;
    if(pPrimitiveShape->pBackMesh)
      uNumberOfMeshes++;
    if(pPrimitiveShape->pBackMeshInverse)
      uNumberOfMeshes++;
*/

    if( uNumberOfMeshes == 0 )
    {
      IFXRELEASE( pPrimitiveShape->m_pMeshGroup );
      return IFX_OK;
    }

    // allocate the mesh group
    if( !pPrimitiveShape->m_pMeshGroup || (uNumberOfMeshes != pPrimitiveShape->m_pMeshGroup->GetNumMeshes() ) )
    {
      IFXRELEASE(pPrimitiveShape->m_pMeshGroup);
      IFXCreateComponent(CID_IFXMeshGroup, IID_IFXMeshGroup, (void**)&pPrimitiveShape->m_pMeshGroup);

      if( pPrimitiveShape->m_pMeshGroup == NULL )
        iResult = IFX_E_OUT_OF_MEMORY;
      else
        pPrimitiveShape->m_pMeshGroup->Allocate( uNumberOfMeshes );
    }

    if (IFXSUCCESS(iResult))
    {
      // We can't use the for loop because it changes the shader index associations with
      // different meshes in the primitive.  They MUST be added to the mesh group in
      // the correct order!
      // assign the mesh groups

      if( (pPrimitiveShape->uPrimitiveType == IFX_BOX))
      {
    //  1 - outside, front
        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->pFrontMesh, &uIndex );
        SetAMesh( pPrimitiveShape, 1, &uIndex );

    //  2 - outside, right
        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->pSolidCapEndMesh, &uIndex );
        SetAMesh( pPrimitiveShape, 2, &uIndex );

    //  3 - outside, back
        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->pBackMesh, &uIndex );
        SetAMesh( pPrimitiveShape, 3, &uIndex );

    // 4 - outside, left
        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->pSolidCapStartMesh, &uIndex );
        SetAMesh( pPrimitiveShape, 4, &uIndex );

    // 5 - outside, top
        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->pTopCapMesh, &uIndex );
        SetAMesh( pPrimitiveShape, 5, &uIndex );

    // 6 - outside, bottom
        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->pBottomCapMesh, &uIndex );
        SetAMesh( pPrimitiveShape, 6, &uIndex );

    //  7 - inside, front
        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->pFrontMeshInverse, &uIndex );
        SetAMesh( pPrimitiveShape, 8, &uIndex );

    //  8 - inside, right
        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->pSolidCapEndMeshInverse, &uIndex );
        SetAMesh( pPrimitiveShape, 9, &uIndex );

    //  9 - inside, back
        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->pBackMeshInverse, &uIndex );
        SetAMesh( pPrimitiveShape, 10, &uIndex );

    //  10 - inside, left
        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->pSolidCapStartMeshInverse, &uIndex );
        SetAMesh( pPrimitiveShape, 11, &uIndex );

    //  11 - inside, top
        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->pTopCapMeshInverse, &uIndex );
        SetAMesh( pPrimitiveShape, 12, &uIndex );

    //  12 - inside, bottom
        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->pBottomCapMeshInverse, &uIndex );
        SetAMesh( pPrimitiveShape, 13, &uIndex );

      }
      else
      {
        // all other primitives use this one
        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->m_pPrimitiveMesh, &uIndex );
        SetAMesh( pPrimitiveShape, 0, &uIndex );

        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->pBackMesh, &uIndex );
        SetAMesh( pPrimitiveShape, 3, &uIndex );

        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->pFrontMesh, &uIndex );
        SetAMesh( pPrimitiveShape, 1, &uIndex );

        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->pTopCapMesh, &uIndex );
        SetAMesh( pPrimitiveShape, 5, &uIndex );

        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->pBottomCapMesh, &uIndex );
        SetAMesh( pPrimitiveShape, 6, &uIndex );

        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->pSolidCapEndMesh, &uIndex );
        SetAMesh( pPrimitiveShape, 2, &uIndex );

        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->pSolidCapStartMesh, &uIndex );
        SetAMesh( pPrimitiveShape, 4, &uIndex );


        // inside pieces
        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->pPrimitiveMeshInverse, &uIndex );
        SetAMesh( pPrimitiveShape, 7, &uIndex );

        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->pBackMeshInverse, &uIndex );
        SetAMesh( pPrimitiveShape, 10, &uIndex );

        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->pFrontMeshInverse, &uIndex );
        SetAMesh( pPrimitiveShape, 8, &uIndex );

        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->pTopCapMeshInverse, &uIndex );
        SetAMesh( pPrimitiveShape, 12, &uIndex );

        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->pBottomCapMeshInverse, &uIndex );
        SetAMesh( pPrimitiveShape, 13, &uIndex );

        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->pSolidCapEndMeshInverse, &uIndex );
        SetAMesh( pPrimitiveShape, 9, &uIndex );

        //SetAMesh( pPrimitiveShape->m_pMeshGroup, pPrimitiveShape->pSolidCapStartMeshInverse, &uIndex );
        SetAMesh( pPrimitiveShape, 11, &uIndex );
      }
    }
  }

  return iResult;
}


//---------------------------------------------------------------------------
//  CIFXPrimitiveGenerator::GeneratePrimitiveArc
//
//  This function calls the GenerateArc() function on the specified primtive.
//  That function creates an array of IFXVector3 points.  It then fills them
//  with the points which define the arc that is to be rotated at a later
//  step.  A pointer to this vector list is stored in the structure.
//---------------------------------------------------------------------------
IFXRESULT CIFXPrimitiveGenerator::GeneratePrimitiveArc(CIFXPrimitiveShape* pPrimitive)
{
  if(pPrimitive == NULL)
    return IFX_E_INVALID_POINTER;

  return pPrimitive->GenerateArc();
}

//---------------------------------------------------------------------------
//  CIFXPrimitiveGenerator::MeshBuilder
//
//  This function takes the arc, parameters, and builds the actual IFXMesh.
//  It rotates the given arc by the amounts specified in the primitiveShape
//  data structure.  It stores the IFXMesh pointer in the structure as well.
//---------------------------------------------------------------------------
IFXRESULT CIFXPrimitiveGenerator::MeshBuilder(U32 uNumSourceArcVertices, U32 uNumSurfaceSlices,
                      IFXVector3* pArcVertices, IFXVector3* pArcNormals,
                      IFXMesh** pDestMesh, CIFXPrimitiveShape* pPrimitive)
{
  // booleans for easier variable checking
  bool  bCounterClockWise = TRUE;   // default orientation (outwards normals)
  bool  bTopPointisAxis = FALSE;
  bool  bBottomPointisAxis = FALSE;

  // IFXMesh parameters - iterators
  IFXMesh*  pDestinationMesh = NULL;
  IFXVector3Iter v3iVertex;
  IFXVector3Iter v3iNormal;
  IFXVector2Iter v2iTexCoord;
  IFXFaceIter fiFace;
  IFXVector3* pvVertex;
  IFXVector3* pvNormal;
  IFXVector2* pvTexCoord;
  IFXFace*  pfFace;

  // indexing and tracking parameters
  IFXRESULT ifxResult;
  U32 uTopAxisVertex;
  U32 uBottomAxisVertex;
  U32 uIndex;
  U32 uPreviousRowVertexIndex;
  U32 uCurrentVertexIndex;
  U32 i, j;

  // calculation parameters
  U32 uFinalMeshVerticeCount;
  U32 uFinalMeshFaceCount;
  F32 fTheta;
  F32 fThetaDelta;
  F32 cosTheta;
  F32 sinTheta;
  F32 fUDelta;      // u texture coordinate delta
  F32 fVDelta;      // v texture coordinate delta
  F32 fUTextureCoord;
  F32 fVTextureCoord;

  // parameter checking
  if( uNumSourceArcVertices == 0 )
    return IFX_E_INVALID_RANGE;
  if(uNumSurfaceSlices == 0)
    return IFX_E_INVALID_RANGE;
  if(pArcVertices == NULL)
    return IFX_E_INVALID_POINTER;
  if(pArcNormals == NULL)
    return IFX_E_INVALID_POINTER;
  if(pDestMesh == NULL)
    return IFX_E_INVALID_POINTER;
  if(pPrimitive == NULL)
    return IFX_E_INVALID_POINTER;

  // Initialize all variables
  if(pPrimitive->uNormalOrientation == IFXPrimitive::eBackFacing)
    bCounterClockWise = FALSE;

  if(pArcVertices[0].X()==0.0 && pArcVertices[0].Y()==0.0)
    bTopPointisAxis = TRUE;

  if(pArcVertices[uNumSourceArcVertices].X()==0.0 && pArcVertices[uNumSourceArcVertices].Y()==0.0)
    bBottomPointisAxis = TRUE;

  uTopAxisVertex = 0;
  uBottomAxisVertex = uNumSourceArcVertices;

  fTheta = pPrimitive->fSolidAngleStart;
  fThetaDelta=(pPrimitive->fSolidAngleEnd - pPrimitive->fSolidAngleStart) / uNumSurfaceSlices;

  // calculate the texture coordinate generation parameters
  {
    F32 fTemp;
    fTemp = (F32) uNumSurfaceSlices;
    fUDelta = (1/fTemp);
    fTemp = (F32) uNumSourceArcVertices;
    fVDelta = (1/fTemp);
  }

  // calculate the number of faces and vertices for IFXMesh data structure
  if(bTopPointisAxis && bBottomPointisAxis)
  {
    uFinalMeshVerticeCount = (uNumSourceArcVertices + 1) * (uNumSurfaceSlices + 1);
    uFinalMeshFaceCount = ( (( (uNumSourceArcVertices+1) - 3)*2) + 2 ) * (uNumSurfaceSlices);

  }
  else if((!bTopPointisAxis && bBottomPointisAxis) || (bTopPointisAxis && !bBottomPointisAxis))
  {
    uFinalMeshVerticeCount = (uNumSourceArcVertices + 1) * (uNumSurfaceSlices + 1);
    uFinalMeshFaceCount = ( (( (uNumSourceArcVertices+1) - 3)*2) + 3 ) * (uNumSurfaceSlices);

  }
  else if(!bTopPointisAxis && !bBottomPointisAxis)
  {
    uFinalMeshVerticeCount = (uNumSourceArcVertices + 1) * (uNumSurfaceSlices + 1);
    uFinalMeshFaceCount = ( (( (uNumSourceArcVertices+1) - 3)*2) + 4 ) * (uNumSurfaceSlices);

  }

  // create the IFXMesh with the correct size
  IFXCreateComponent(CID_IFXMesh, IID_IFXMesh, (void**)&pDestinationMesh);
  if( !pDestinationMesh )
    return IFX_E_OUT_OF_MEMORY;

  IFXVertexAttributes vertexAttributes;
  // Defaults for vertexAttributes are good.

  ifxResult = pDestinationMesh->Allocate(vertexAttributes,
    uFinalMeshVerticeCount,
    uFinalMeshFaceCount);
  if(ifxResult!=IFX_OK)
    return ifxResult;

  // set up iterators
  pDestinationMesh->GetPositionIter(v3iVertex);
  pDestinationMesh->GetNormalIter(v3iNormal);
  pDestinationMesh->GetFaceIter(fiFace);
  pDestinationMesh->GetTexCoordIter(v2iTexCoord);

  cosTheta=(F32)cos(fTheta);
  sinTheta=(F32)sin(fTheta);

  if(!bCounterClockWise)
    fUTextureCoord = PRIMITIVE_TEXTURECOORDINATE_START;
  else
    fUTextureCoord = PRIMITIVE_TEXTURECOORDINATE_END;

  fVTextureCoord = PRIMITIVE_TEXTURECOORDINATE_END;

  // add the initial strip of verticies
  for(uIndex = 0; uIndex<=uNumSourceArcVertices; uIndex++)
  {
    pvVertex = v3iVertex.Next();
    pvNormal = v3iNormal.Next();
    pvTexCoord = v2iTexCoord.Next();

    pvVertex->Set(pArcVertices[uIndex].X()*cosTheta-pArcVertices[uIndex].Y()*sinTheta,
      pArcVertices[uIndex].X()*sinTheta-pArcVertices[uIndex].Y()*cosTheta,
      pArcVertices[uIndex].Z()
      );
    pvNormal->Set(pArcNormals[uIndex].X()*cosTheta-pArcNormals[uIndex].Y()*sinTheta,
      pArcNormals[uIndex].X()*sinTheta-pArcNormals[uIndex].Y()*cosTheta,
      pArcNormals[uIndex].Z()
      );
    pvTexCoord->Set(fUTextureCoord,fVTextureCoord);
    fVTextureCoord -= fVDelta;
    if (fVTextureCoord < PRIMITIVE_TEXTURECOORDINATE_START)
      fVTextureCoord = PRIMITIVE_TEXTURECOORDINATE_START;
  }

  // prepare counters/indicies for main loop
  uPreviousRowVertexIndex = uTopAxisVertex;
  uCurrentVertexIndex = uNumSourceArcVertices+1;

  for(i = 0; i<uNumSurfaceSlices; i++)
  {

    // rotate the arc to the new angle for this slice and add points to mesh
    fTheta+=fThetaDelta;
    if(!bCounterClockWise)
    {
      fUTextureCoord+=fUDelta;
      if(fUTextureCoord > PRIMITIVE_TEXTURECOORDINATE_END)
        fUTextureCoord = PRIMITIVE_TEXTURECOORDINATE_END;
    }
    else
    {
      fUTextureCoord-=fUDelta;
      if(fUTextureCoord < PRIMITIVE_TEXTURECOORDINATE_START)
        fUTextureCoord = PRIMITIVE_TEXTURECOORDINATE_START;
    }

    fVTextureCoord = PRIMITIVE_TEXTURECOORDINATE_END;

    cosTheta=(F32)cos(fTheta);
    sinTheta=(F32)sin(fTheta);
    if(i==(uNumSurfaceSlices-1) && pPrimitive->fSolidAngleEnd == 2*IFX_PI)
    {
      cosTheta=(F32)cos(0.0f);
      sinTheta=(F32)sin(0.0f);
    }

    // add the new stripe of verticies
    for(uIndex = 0; uIndex<=uNumSourceArcVertices; uIndex++)
    {
      pvVertex = v3iVertex.Next();
      pvNormal = v3iNormal.Next();
      pvTexCoord = v2iTexCoord.Next();

      pvVertex->Set(pArcVertices[uIndex].X()*cosTheta-pArcVertices[uIndex].Y()*sinTheta,
        pArcVertices[uIndex].X()*sinTheta-pArcVertices[uIndex].Y()*cosTheta,
        pArcVertices[uIndex].Z()
        );
      pvNormal->Set(pArcNormals[uIndex].X()*cosTheta-pArcNormals[uIndex].Y()*sinTheta,
        pArcNormals[uIndex].X()*sinTheta-pArcNormals[uIndex].Y()*cosTheta,
        pArcNormals[uIndex].Z()
        );
      pvTexCoord->Set(fUTextureCoord,fVTextureCoord);
      fVTextureCoord-=fVDelta;
      if(fVTextureCoord < PRIMITIVE_TEXTURECOORDINATE_START)
        fVTextureCoord = PRIMITIVE_TEXTURECOORDINATE_START;
    }

    // stitch up the faces
    for(j = 0; j<uNumSourceArcVertices; j++)
    {
      if(j != 0 || !bTopPointisAxis)
      {
        pfFace = fiFace.Next();
        if(bCounterClockWise)
          pfFace->Set(uCurrentVertexIndex, uPreviousRowVertexIndex+1, uPreviousRowVertexIndex);
        else
          pfFace->Set(uCurrentVertexIndex, uPreviousRowVertexIndex, uPreviousRowVertexIndex+1);
      }

      if(j != (uNumSourceArcVertices-1) || !bBottomPointisAxis)
      {
        pfFace = fiFace.Next();
        if(bCounterClockWise)
          pfFace->Set(uCurrentVertexIndex, uCurrentVertexIndex+1, uPreviousRowVertexIndex+1);
        else
          pfFace->Set(uCurrentVertexIndex, uPreviousRowVertexIndex+1, uCurrentVertexIndex+1);
      }
      uCurrentVertexIndex++;
      uPreviousRowVertexIndex++;
    }

    uCurrentVertexIndex++;
    uPreviousRowVertexIndex++;
    if(uPreviousRowVertexIndex == uNumSourceArcVertices)
      uPreviousRowVertexIndex++;
  }

  // parameters that need to be passed back
  *pDestMesh = pDestinationMesh;

  return IFX_OK;
}

//---------------------------------------------------------------------------
//  CIFXPrimitiveGenerator::CapBuilder
//
//  This function takes the arc, parameters, and builds a cap for IFXMesh.
//  It rotates the given arc by the amounts specified in the primitiveShape
//  data structure.  It stores the IFXMesh pointer in the structure as well.
//---------------------------------------------------------------------------
IFXRESULT CIFXPrimitiveGenerator::CapBuilder(F32 fTheta, bool bStart, IFXMesh** pDestMesh,
                       CIFXPrimitiveShape* pPrimitive)
{

  // booleans for easier checking
  bool  bTopPointisAxis = FALSE;
  bool  bBottomPointisAxis = FALSE;
  bool  bCounterClockWise = TRUE;

  // mesh variables
  IFXMesh*  pDestinationMesh = NULL;
  IFXVector3Iter v3iVertex;
  IFXVector3Iter v3iNormal;
  IFXFaceIter fiFace;
  IFXVector3* pvVertex;
  IFXVector3* pvNormal;
  IFXFace*  pfFace;

  // counting variables
  U32 uIndex;
  U32 i;
  U32 uCurrentVertexIndex;
  U32 uAxisRowIndex;
  U32 uAxisVertexCount;
  U32 uFinalMeshFaceCount;
  U32 uFinalMeshVerticeCount;
  U32 uNumSourceArcVertices;

  // math variables
  F32 cosTheta;
  F32 sinTheta;
  F32 fNormalOrientation = 1;

  // pointers to make life easier
  IFXVector3* pArcVertices;
  IFXVector3* pArcNormals;

  IFXRESULT ifxResult;

  // parameter checking
  if(pDestMesh == NULL)
    return IFX_E_INVALID_POINTER;
  if(pPrimitive == NULL)
    return IFX_E_INVALID_POINTER;

  // Calculate the size of the mesh
  // Initialize all variables
  uFinalMeshVerticeCount = pPrimitive->uNumArcSlices*2+2;
  uFinalMeshFaceCount = pPrimitive->uNumArcSlices*2;
  uAxisVertexCount = pPrimitive->uNumArcSlices+1;
  uIndex = 0;
  pArcVertices = pPrimitive->pArcCoords;
  pArcNormals = pPrimitive->pArcNormals;
  uNumSourceArcVertices = pPrimitive->uNumArcSlices;

  // get orientation of normals right...
  if(bStart == TRUE)
  {
    if(pPrimitive->uNormalOrientation == IFXPrimitive::eBackFacing)
    {
      bCounterClockWise = FALSE;
      fNormalOrientation*=1;
    }
    else
    {
      bCounterClockWise = TRUE;
      fNormalOrientation*=-1;
    }
  }
  if(bStart == FALSE)
  {
    if(pPrimitive->uNormalOrientation == IFXPrimitive::eBackFacing)
    {
      bCounterClockWise = TRUE;
      fNormalOrientation*=-1;
    }
    else
    {
      bCounterClockWise = FALSE;
      fNormalOrientation*=1;
    }
  }

  if(pArcVertices[0].X()==0.0 && pArcVertices[0].Y()==0.0)
  {
    bTopPointisAxis = TRUE;
    uFinalMeshFaceCount--;
    uFinalMeshVerticeCount--;
    uAxisVertexCount--;
  }
  if(pArcVertices[uNumSourceArcVertices].X()==0.0 && pArcVertices[uNumSourceArcVertices].Y()==0.0)
  {
    bBottomPointisAxis = TRUE;
    uFinalMeshFaceCount--;
    uFinalMeshVerticeCount--;
    uAxisVertexCount--;
  }

  // create the IFXMesh with the correct size
  IFXCreateComponent(CID_IFXMesh, IID_IFXMesh, (void**)&pDestinationMesh);
  if(pDestinationMesh == NULL)
    return IFX_E_OUT_OF_MEMORY;

  IFXVertexAttributes vertexAttributes;
  vertexAttributes.m_uData.m_uNumTexCoordLayers = 0;

  ifxResult = pDestinationMesh->Allocate(vertexAttributes,
                      uFinalMeshVerticeCount,
                      uFinalMeshFaceCount);
  if(ifxResult!=IFX_OK)
    return ifxResult;
  *pDestMesh = pDestinationMesh;

  // set up iterators
  pDestinationMesh->GetPositionIter(v3iVertex);
  pDestinationMesh->GetNormalIter(v3iNormal);
  pDestinationMesh->GetFaceIter(fiFace);

  cosTheta=(F32)cos(fTheta);
  sinTheta=(F32)sin(fTheta);

  // add the initial arc of vertices - rotating normals/coords by fTheta
  for(i = 0; i<=uNumSourceArcVertices; i++)
  {
    pvVertex = v3iVertex.Next();
    pvNormal = v3iNormal.Next();

    pvVertex->Set(pArcVertices[uIndex].X()*cosTheta-pArcVertices[uIndex].Y()*sinTheta,
      pArcVertices[uIndex].X()*sinTheta-pArcVertices[uIndex].Y()*cosTheta,
      pArcVertices[uIndex].Z()
      );
    pvNormal->Set((fNormalOrientation*-1*cosTheta),
            (fNormalOrientation*sinTheta),
            0.0f
      );

    uIndex++;
  }
  uIndex = 0;
  if(bTopPointisAxis)
    uIndex++;

  // project the arc onto the x/y axis and rotate points
  for(i = 0; i<uAxisVertexCount; i++)
  {
    pvVertex = v3iVertex.Next();
    pvNormal = v3iNormal.Next();

    pvVertex->Set(0.0f,
      0.0f,
      pArcVertices[uIndex].Z()
      );
    pvNormal->Set((fNormalOrientation*-1*cosTheta),
      (fNormalOrientation*sinTheta),
      0.0f
      );
    uIndex++;
  }

  uCurrentVertexIndex = 0;
  uAxisRowIndex = pPrimitive->uNumArcSlices+1;

  // build top and bottom triangles if they're on an axis/shared point
  if(bTopPointisAxis)
  {
    pfFace = fiFace.Next();
    if(bCounterClockWise)
      pfFace->Set(uAxisRowIndex, 1, 0);
    else
      pfFace->Set(uAxisRowIndex, 0, 1);
    uCurrentVertexIndex++;

  }

  for(i = 1; i<uAxisVertexCount; i++)
  {
    pfFace = fiFace.Next();
    if(bCounterClockWise)
      pfFace->Set(uCurrentVertexIndex, uAxisRowIndex+1, uAxisRowIndex);
    else
      pfFace->Set(uCurrentVertexIndex, uAxisRowIndex, uAxisRowIndex+1);


    pfFace = fiFace.Next();
    if(bCounterClockWise)
      pfFace->Set(uCurrentVertexIndex, uCurrentVertexIndex+1, uAxisRowIndex+1);
    else
      pfFace->Set(uCurrentVertexIndex, uAxisRowIndex+1, uCurrentVertexIndex+1);


    uCurrentVertexIndex++;
    uAxisRowIndex++;

  }

  if(bBottomPointisAxis)
  {
    pfFace = fiFace.Next();
    if(bCounterClockWise)
      pfFace->Set(pPrimitive->uNumArcSlices, uAxisRowIndex, pPrimitive->uNumArcSlices-1);
    else
      pfFace->Set(pPrimitive->uNumArcSlices, pPrimitive->uNumArcSlices-1, uAxisRowIndex);

  }

  return IFX_OK;
}
