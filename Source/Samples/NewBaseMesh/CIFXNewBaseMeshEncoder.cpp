//***************************************************************************
//
//  Copyright (c) 2002 - 2006 Intel Corporation
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
//	CIFXNewBaseMeshEncoder.cpp
//
//	DESCRIPTION:
//		Implementation of the CIFXNewBaseMeshEncoder.
//		The CIFXNewBaseMeshEncoder contains Base Mesh compression modifier
//		encoding functionality that is used by the write manager.
//
//*****************************************************************************

#include "IFXCoreCIDs.h"
#include "IFXExportingCIDs.h"
#include "IFXCheckX.h"
#include "IFXAuthorGeomCompiler.h"
#include "qcomp\vqapi.h"
#include "u3dbitencoder.h"
#include "CIFXNewBaseMeshEncoder.h"
#include "NewBaseMeshUtils.h"
#include "IFXModifierChain.h"
#include "IFXNewBaseMeshPluginCIDs.h"
#include <malloc.h>


// add vertex to bitstream
#define ADDTOBS(what){\
	u3dAddU32( pEncoder, *((U32*)&(what.VertexA())) );\
	u3dAddU32( pEncoder, *((U32*)&(what.VertexB())) );\
	u3dAddU32( pEncoder, *((U32*)&(what.VertexC())) );\
}

// constructor
CIFXNewBaseMeshEncoder::CIFXNewBaseMeshEncoder() :
IFXDEFINEMEMBER(m_pModifier),
IFXDEFINEMEMBER(m_pAuthorCLODResource),
IFXDEFINEMEMBER(m_pOriginalAuthorCLODMesh)
{
	m_uRefCount = 0;

	m_bInitialized = FALSE;
	m_pObject = NULL;
	m_uRefCount = 0;
	m_pName.size = 0;
	m_pName.string = 0;
	m_blockType = 0;
	m_pCoreServices = 0;

	m_uPriorityIncrement = 0;
	m_uPriorityCurrent = 0;

	m_unitScale = 1.0f;
}

// destructor
CIFXNewBaseMeshEncoder::~CIFXNewBaseMeshEncoder()
{
	IFXRELEASE( m_pObject );
	if (m_pName.string && m_pName.size)
		streamFree(0, m_pName.string);
}


// IFXUnknown
U32 CIFXNewBaseMeshEncoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXNewBaseMeshEncoder::Release()
{
	if ( !( --m_uRefCount ) )
	{
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return m_uRefCount;
}

IFXRESULT CIFXNewBaseMeshEncoder::QueryInterface( IFXREFIID interfaceId,
												  void**	ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXEncoderX )
		{
			*ppInterface = ( IFXEncoderX* ) this;
			this->AddRef();
		}
		else if ( interfaceId == IID_IFXExtensionEncoderX )
		{
			*ppInterface = ( IFXExtensionEncoderX* ) this;
			this->AddRef();
		}
		else if ( interfaceId == IID_IFXUnknown )
		{
			*ppInterface = ( IFXUnknown* ) this;
			this->AddRef();
		}
		else
		{
			*ppInterface = NULL;
			rc = IFX_E_UNSUPPORTED;
		}
	}
	else
		rc = IFX_E_INVALID_POINTER;

	IFXRETURN(rc);
}


// IFXEncoderX

void CIFXNewBaseMeshEncoder::GetExtensionDataX( IFXExtensionData &rExtData )
{
	// This CID will be used for writing into a file,
	// so it should be a decoder CID. Later it would be
	// possible to create the decoder for data reading.
	rExtData.m_cid = CID_IFXNewBaseMeshDecoder;

	IFXString extName( L"New Base Mesh" );
	IFXString vendorName( L"Intel Corporation" );
	IFXString extInfo( L"New base mesh storage extension" );
	m_urls[0] = L"http://www.intel.com";
	m_urls[1] = L"http://www.intel.com";
	rExtData.m_extensionName.Assign( &extName );
	rExtData.m_vendorName.Assign( &vendorName );
	rExtData.m_continuationBlockTypesCount = 0;
	rExtData.m_extensionURLCount = 2;
	rExtData.m_extensionURLs = m_urls;
	rExtData.m_extensionInfo.Assign( &extInfo );
	rExtData.m_modifierType = IFXModifierChain::RESOURCE;
}

void CIFXNewBaseMeshEncoder::SetBlockTypesX( U32* pBlockTypes, const U32 blockTypeCount )
{
	// we know how many blocks this encoder supports, so we can check if input data is correct
	if ( blockTypeCount != 1 )
		throw IFXException( IFX_E_INVALID_RANGE );
	
	m_blockType  = pBlockTypes[0];
}

U64 myWrite(U3dStream_cbCtx* pCtx, U8* const pData, U64 size)
{
	if (!pCtx)
		return 0;
	U64         writtenByte = 0;
	U3DStream*  stream = (U3DStream*)pCtx;

	if(stream) {
		if (size + stream->m_written > stream->m_size) {
			// too small buffer, reallocate
			U8* pmem = 0;
			pmem = (U8*)realloc(stream->m_stream, size + stream->m_written);
			if (!pmem) {
				// not enought memory
				writtenByte = stream->m_size - stream->m_written;
			} else {
				stream->m_stream = pmem;
				stream->m_curPos = stream->m_stream + stream->m_written;
				stream->m_size = size + stream->m_written;
				writtenByte = size;
			}
		} else {
			writtenByte = size;
		}
		memcpy(stream->m_curPos, pData, writtenByte);
		stream->m_curPos+=writtenByte;
		stream->m_written+=writtenByte;
	}/* if */

	return writtenByte;
}

void CIFXNewBaseMeshEncoder::EncodeX( IFXString& rName, IFXDataBlockQueueX& rDataBlockQueue, F64 units )
{
	try
	{
		// check for initialization
		if ( FALSE == m_bInitialized )
			throw IFXException( IFX_E_NOT_INITIALIZED );
		if ( NULL == m_pObject )
			throw IFXException( IFX_E_CANNOT_FIND );

		IFXASSERT(m_pAuthorCLODResource);

		// Get priority increment
		{
			IFXDECLARELOCAL(IFXMarkerX,pMarker);
			IFXCHECKX(m_pAuthorCLODResource->QueryInterface(IID_IFXMarkerX,(void**)&pMarker));
			m_uPriorityIncrement = pMarker->GetPriority();
		}

		if( units > 0.0f )
			m_unitScale = units;

		// Get the original author mesh from the author CLOD resource
		IFXVector3 scale;
		const IFXMatrix4x4 &matrix = m_pAuthorCLODResource->GetTransform();
		matrix.CalcTRS( NULL, NULL, &scale );
		IFXCHECKX(m_pAuthorCLODResource->GetAuthorMesh(m_pOriginalAuthorCLODMesh));
		// GetAuthorMesh returns success even if there's no mesh, so
		// validate that there really is one.
		IFXCHECKX_RESULT( m_pOriginalAuthorCLODMesh, IFX_E_INVALID_POINTER );

		// Apply scale from Author CLOD Resource transform to the mesh geometry. Do it
		// only if scale != (1.0f, 1.0f, 1.0f) or m_unitScale != 1.0f
		if( scale.X() != 1.0f || scale.Y() != 1.0f || scale.Z() != 1.0f || m_unitScale != 1.0f )
		{
			m_pOriginalAuthorCLODMesh->Lock();

			IFXVector3* pPositions = NULL;
			const IFXAuthorMeshDesc* pMaxMeshDesc = NULL;

			pMaxMeshDesc = m_pOriginalAuthorCLODMesh->GetMaxMeshDesc();
			if( NULL == pMaxMeshDesc )
				IFXCHECKX( IFX_E_NOT_INITIALIZED );

			U32 i, positionCnt = pMaxMeshDesc->NumPositions;

			IFXCHECKX( m_pOriginalAuthorCLODMesh->GetPositions( &pPositions ) );

			if( m_unitScale == 1.0f )
			{
				for( i = 0; i < positionCnt; i++ )
				{
					pPositions[i].Multiply( scale );
				}
			}
			else
			{
				F32 invScale = 1.0f/(F32)m_unitScale;

				if( scale.X() != 1.0f || scale.Y() != 1.0f || scale.Z() != 1.0f )
				{
					for( i = 0; i < positionCnt; i++ )
					{
						pPositions[i].Multiply( scale );
						pPositions[i].Scale( invScale );
					}
				}
				else
				{
					for( i = 0; i < positionCnt; i++ )
					{
						pPositions[i].Scale( invScale );
					}
				}
			}

			m_pOriginalAuthorCLODMesh->Unlock();
		}

		IFXDECLARELOCAL(IFXMeshMap,pMeshMap);
		m_pAuthorCLODResource->GetAuthorMeshMap( &pMeshMap );
		//    IFXCHECKX_RESULT( pMeshMap, IFX_E_INVALID_POINTER );

		if( NULL == pMeshMap )
		{
			// Create Author Geom Compiler to create Author Mesh Map
			IFXDECLARELOCAL(IFXAuthorGeomCompiler,pAGC);
			IFXDECLARELOCAL(IFXAuthorCLODResource,pACR);
			IFXCHECKX(IFXCreateComponent( CID_IFXAuthorGeomCompiler, IID_IFXAuthorGeomCompiler, (void **)&pAGC ));
			IFXDECLARELOCAL(IFXSceneGraph,pSG);
			IFXCHECKX( m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSG ) );
			IFXCHECKX( pAGC->SetSceneGraph( pSG ) );
			IFXCHECKX( pAGC->Compile( rName, m_pOriginalAuthorCLODMesh, &pACR, FALSE ) );

			// Get the MeshMap from the author CLOD resource
			IFXCHECKX( pACR->GetAuthorMeshMap( &pMeshMap ) );
		}

		// Make the static geometry block
		MakeAuthorCLODStaticGeometryBlockX(rName, rDataBlockQueue, *pMeshMap);

		// Set the newly-created mesh map into the AuthorCLODResource
		IFXCHECKX(m_pAuthorCLODResource->SetAuthorMeshMap( pMeshMap ));

		// Revert scale applied to mesh during encoding
		if( scale.X() != 1.0f || scale.Y() != 1.0f || scale.Z() != 1.0f || m_unitScale != 1.0f )
		{
			m_pOriginalAuthorCLODMesh->Lock();

			if( scale.X() == 0.0f || scale.Y() == 0.0f || scale.Z() == 0.0f )
				IFXCHECKX( IFX_E_BAD_PARAM );

			IFXVector3 invScale = scale.Reciprocate();
			IFXVector3* pPositions = NULL;
			const IFXAuthorMeshDesc* pMaxMeshDesc = NULL;

			pMaxMeshDesc = m_pOriginalAuthorCLODMesh->GetMaxMeshDesc();
			if( NULL == pMaxMeshDesc )
				IFXCHECKX( IFX_E_NOT_INITIALIZED );

			U32 i, positionCnt = pMaxMeshDesc->NumPositions;

			IFXCHECKX( m_pOriginalAuthorCLODMesh->GetPositions( &pPositions ) );

			if( m_unitScale == 1.0f )
			{
				for( i = 0; i < positionCnt; i++ )
				{
					pPositions[i].Multiply( invScale );
				}
			}
			else if( scale.X() != 1.0f || scale.Y() != 1.0f || scale.Z() != 1.0f )
			{
				for( i = 0; i < positionCnt; i++ )
				{
					pPositions[i].Multiply( invScale );
					pPositions[i].Scale( (F32)m_unitScale );
				}
			}
			else
			{
				for( i = 0; i < positionCnt; i++ )
				{
					pPositions[i].Scale( (F32)m_unitScale );
				}
			}

			m_pOriginalAuthorCLODMesh->Unlock();
		}

		//IFXRELEASE( pMeshMap );
	}
	catch ( ... )
	{
		throw;
	}
}

void CIFXNewBaseMeshEncoder::InitializeX( IFXCoreServices& rCoreServices )
{
	try
	{
		// Release any previous core services pointer
		// and store the new core services reference
		rCoreServices.AddRef();
		IFXRELEASE(m_pCoreServices);
		m_pCoreServices = &rCoreServices;
		m_bInitialized = TRUE;
	}
	catch ( ... )
	{
		IFXRELEASE( m_pCoreServices );

		throw;
	}
}

void CIFXNewBaseMeshEncoder::SetObjectX( IFXUnknown& rObject )
{
	IFXModifier* pModifier= NULL;
	IFXNewBaseMesh* pNewBaseMesh= NULL;

	try
	{
		// set the object
		IFXRELEASE( m_pObject );
		m_pObject = &rObject;
		m_pObject->AddRef();

		m_pObject->QueryInterface( IID_IFXModifier, (void**)&pModifier );

		pModifier->AddRef();
		IFXRELEASE( m_pModifier );
		m_pModifier = pModifier;

		m_pObject->QueryInterface( IID_IFXNewBaseMesh, (void**)&pNewBaseMesh );
		pNewBaseMesh->GetCLODResource( &m_pAuthorCLODResource );

		m_pAuthorCLODResource->GetAuthorMesh(m_pOriginalAuthorCLODMesh);

		IFXRELEASE( pNewBaseMesh );
		IFXRELEASE( pModifier );
	}
	catch ( ... )
	{
		IFXRELEASE( m_pOriginalAuthorCLODMesh );
		IFXRELEASE( m_pAuthorCLODResource );
		IFXRELEASE( pNewBaseMesh );
		IFXRELEASE( pModifier );
		IFXRELEASE( m_pObject ); // release the member variable, not the input parameter

		throw;
	}
}


// Factory friend
IFXRESULT IFXAPI_CALLTYPE CIFXNewBaseMeshEncoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;


	if ( ppInterface )
	{
		// Create the CIFXNewBaseMeshEncoder component.
		CIFXNewBaseMeshEncoder *pComponent = new CIFXNewBaseMeshEncoder;

		if ( pComponent )
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			rc = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		}

		else
			rc = IFX_E_OUT_OF_MEMORY;
	}

	else
		rc = IFX_E_INVALID_POINTER;


	IFXRETURN( rc );
}


void CIFXNewBaseMeshEncoder::MakeAuthorCLODStaticGeometryBlockX(IFXString &rName, IFXDataBlockQueueX &rDataBlockQueue, IFXMeshMap &rMeshMap)
{
	IFXASSERT(m_pOriginalAuthorCLODMesh);
	IFXASSERT(m_pAuthorCLODResource);

	U32 uMinResolution = m_pOriginalAuthorCLODMesh->GetMinResolution();
	U32 uMaxResolution = m_pOriginalAuthorCLODMesh->GetFinalMaxResolution();
	const IFXAuthorMeshDesc* pMinMeshDescription = m_pOriginalAuthorCLODMesh->GetMeshDesc();
	const IFXAuthorMeshDesc* pMaxMeshDescription = m_pOriginalAuthorCLODMesh->GetMaxMeshDesc();
	const IFXAuthorMeshDesc* pMeshDescription = pMaxMeshDescription;

	m_pOriginalAuthorCLODMesh->SetResolution(uMinResolution);

	BOOL bExcludeNormals = FALSE;
	m_pAuthorCLODResource->GetExcludeNormals(bExcludeNormals);

	// Create a bitstream
	U3dEncoder* pEncoder;
	U3dMemory	memfunc = {0, streamMalloc, streamFree, 0};
	U64         estimatedBlockSize = (pMeshDescription->NumPositions*3*sizeof(F32))+ 
							pMeshDescription->NumFaces*3*sizeof(U32);
	U3DStream  stream;

	stream.m_stream = (U8*)streamMalloc(0, estimatedBlockSize);
	stream.m_size = estimatedBlockSize;
	stream.m_curPos = stream.m_stream;
	stream.m_written = 0;
    
	u3dEncoderInit(&pEncoder, &memfunc, &stream, myWrite, (U64)-1);

	IFXCHECKX(m_pOriginalAuthorCLODMesh->Lock());

	if (m_pName.size && m_pName.string) {
		streamFree (0, m_pName.string);
		m_pName.size = 0; m_pName.string = 0;
	}
	m_pName.size = rName.LengthU8();
	m_pName.string = (char*)streamMalloc(0, (m_pName.size+1)*sizeof(char));
	rName.ConvertToRawU8((U8*)m_pName.string, m_pName.size);

	// 1. Name
	u3dAddString(pEncoder, m_pName);

	// 2. Chain index
	U32 uTemp = 0;
	IFXCHECKX( m_pModifier->GetModifierChainIndex( uTemp ) );
	u3dAddU32(pEncoder, uTemp);

	// prepare data for tristrip making
	U32 numVertex = pMeshDescription->NumPositions;
	U32 numIndex = pMeshDescription->NumFaces*3;
	U32 numStrips, numElements, newSize;
	U32* indexes = (U32*)streamMalloc(0, numIndex * sizeof(U32));
	U32* indexres = (U32*)streamMalloc(0, (numIndex>numVertex)?numIndex:numVertex*(sizeof(U32)));
	U8* newindexes = (U8*)streamMalloc(0, numIndex * sizeof(U32) + sizeof(U8)*2);

	// copy indices to our array
	IFXVector3* pPos;
	IFXAuthorFace face, faceNorm, faceDiff, faceSpec, faceTexCoord;
	m_pOriginalAuthorCLODMesh->GetPositions(&pPos);
	for( U32 i = 0; i < numIndex; i+=3 )
	{
		m_pOriginalAuthorCLODMesh->GetPositionFace( i/3, &face );
		indexes[i] = face.VertexA();
		indexes[i+1] = face.VertexB();
		indexes[i+2] = face.VertexC();
	}

	// make triangle strips
	makeTriStrips(indexes, indexres, numIndex, newindexes, &newSize, &numStrips, &numElements);

	// save vertices
	u3dAddU32(pEncoder, numVertex);
	// save indices
	u3dAddU32(pEncoder, numIndex/3);
	u3dAddU32(pEncoder, numElements);
	u3dAddU32(pEncoder, (bExcludeNormals)?0:pMeshDescription->NumNormals);
	u3dAddU32(pEncoder, pMeshDescription->NumDiffuseColors);
	u3dAddU32(pEncoder, pMeshDescription->NumSpecularColors);
	u3dAddU32(pEncoder, pMeshDescription->NumTexCoords);
	u3dAddU32(pEncoder, pMeshDescription->NumMaterials);

	//    3.8 Shaders Descriptions
	{
		IFXAuthorMaterial* pAuthorMeshMaterial = NULL;
		IFXCHECKX(m_pOriginalAuthorCLODMesh->GetMaterials(&pAuthorMeshMaterial));
		U32 i = 0;

		for(i=0; i < pMaxMeshDescription->NumMaterials; i++) {
			//  3.8.1 U32 Shading Attributes (BOOL m_uDiffuseColors; BOOL m_uSpecularColors;)
			U32 uMaterialAttributes = 0;
			if(pAuthorMeshMaterial[i].m_uDiffuseColors) {
				uMaterialAttributes |= 1;
			}
			if(pAuthorMeshMaterial[i].m_uSpecularColors) {
				uMaterialAttributes |= 2;
			}
			u3dAddU32(pEncoder, uMaterialAttributes);
			//  3.8.2 U32 m_uNumTextureLayers;
			u3dAddU32(pEncoder, pAuthorMeshMaterial[i].m_uNumTextureLayers);
			//  3.8.3 U32 m_uNumTextureLayers[m_uNumTextureLayers]; (IFX_MAX_TEXUNITS == 8)
			IFXASSERT(IFX_MAX_TEXUNITS >= pAuthorMeshMaterial[i].m_uNumTextureLayers);
			U32 j = 0;
			for(j = 0; j < pAuthorMeshMaterial[i].m_uNumTextureLayers; j++) {
				u3dAddU32(pEncoder, pAuthorMeshMaterial[i].m_uTexCoordDimensions[j]);
			}
			//  3.8.4 U32 OriginalShadingID;  // the mesh scrubber may have removed unused materials an repacked them.
			u3dAddU32(pEncoder, pAuthorMeshMaterial[i].m_uOriginalMaterialID);
		}
	}

	U32 uMat;
	IFXAuthorMaterial *pMeshMat;
	m_pOriginalAuthorCLODMesh->GetMaterials( &pMeshMat );

	// save connectivity data
	bool isTri = false;     // true - triangle list is started
	U8 faceCounter; // number of vertices collected. If < 3 - dont process face info saving
	U8* bkup_newindexes = newindexes;
	U32 faceIndex = 0;
	U32 newv, oldv, medv;
	newv = oldv = medv = (U32)(-1);
	for (U32 indexStrip = 0; indexStrip < numElements; indexStrip++) {
		unsigned char op = (*newindexes);
		newindexes+=sizeof(char); newindexes+=sizeof(char);
		U32 indx = *((U32*)newindexes);
		newindexes+=sizeof(U32);

		// write tristrip header if needed
		switch (op) {
			case 0: // triangle vertex
				faceCounter++;
				if (faceCounter > 3) faceCounter = 1;
				break;
			case 1: // tristrip vertex
				faceCounter++;
				break;
			case 2: // start triangle list
				// traingle list - save zero as number of vertices
				u3dAddU8(pEncoder, 0);
				isTri = true;
				faceCounter = 1;
				break;
			default:    // start of triangle strip
				u3dAddU8(pEncoder, (U8)op);
				faceCounter = 1;
				break;
		}

		u3dAddU32(pEncoder, indx);
		oldv = medv; medv = newv; newv = indx;

		if ((isTri && faceCounter == 3) || (!isTri && faceCounter > 2 )) {
			// save face info
			U32 j = 0;
			m_pOriginalAuthorCLODMesh->GetFaceMaterial( indexes[faceIndex], &uMat );
			u3dAddU32(pEncoder, uMat);

			if(!bExcludeNormals) 
				m_pOriginalAuthorCLODMesh->GetNormalFace( indexes[faceIndex], &faceNorm );
			if( pMeshMat[ uMat ].m_uDiffuseColors ) 
				m_pOriginalAuthorCLODMesh->GetDiffuseFace( indexes[faceIndex], &faceDiff );
			if( pMeshMat[ uMat ].m_uSpecularColors ) 
				m_pOriginalAuthorCLODMesh->GetSpecularFace( indexes[faceIndex], &faceSpec );

			// Reorder information according to real order of vertices in the triangle
			U32 res;
			m_pOriginalAuthorCLODMesh->GetPositionFace(indexes[faceIndex], &face);
			U32 first_old = face.VertexA();
			U32 second_old = face.VertexB();
			U32 third_old = face.VertexC();

			U32 first_new = oldv;
			U32 second_new = (!isTri && !(faceCounter%2))?newv:medv;
			U32 third_new = (!isTri && !(faceCounter%2))?medv:newv;

			if (first_new == first_old) {
				// do nothing
				ADDTOBS(faceNorm);
			} else if (first_new == second_old) {
				// B->A, C->B, A->C
				if(!bExcludeNormals) {
					res = faceNorm.VertexA();
					faceNorm.VertexA() = faceNorm.VertexB();
					faceNorm.VertexB() = faceNorm.VertexC();
					faceNorm.VertexC() = res;
					ADDTOBS(faceNorm);
				}
				if( pMeshMat[ uMat ].m_uDiffuseColors ) {
					res = faceDiff.VertexA();
					faceDiff.VertexA() = faceDiff.VertexB();
					faceDiff.VertexB() = faceDiff.VertexC();
					faceDiff.VertexC() = res;
					ADDTOBS(faceDiff);
				}
				if( pMeshMat[ uMat ].m_uSpecularColors ) {
					res = faceSpec.VertexA();
					faceSpec.VertexA() = faceSpec.VertexB();
					faceSpec.VertexB() = faceSpec.VertexC();
					faceSpec.VertexC() = res;
					ADDTOBS(faceSpec);
				}
		   } else {
				// C->A, A->B, B->C
				if(!bExcludeNormals) {
					res = faceNorm.VertexA();
					faceNorm.VertexA() = faceNorm.VertexC();
					faceNorm.VertexC() = faceNorm.VertexB();
					faceNorm.VertexB() = res;
					ADDTOBS(faceNorm);
				}
				if( pMeshMat[ uMat ].m_uDiffuseColors ) {
					res = faceDiff.VertexA();
					faceDiff.VertexA() = faceDiff.VertexC();
					faceDiff.VertexC() = faceDiff.VertexB();
					faceDiff.VertexB() = res;
					ADDTOBS(faceDiff);
				}
				if( pMeshMat[ uMat ].m_uSpecularColors ) {
					res = faceSpec.VertexA();
					faceSpec.VertexA() = faceSpec.VertexC();
					faceSpec.VertexC() = faceSpec.VertexB();
					faceSpec.VertexB() = res;
					ADDTOBS(faceSpec);
				}
			}

			for( j = 0; j < pMeshMat[ uMat ].m_uNumTextureLayers; j++ )
			{
				m_pOriginalAuthorCLODMesh->GetTexFace( j, indexes[faceIndex], &faceTexCoord );
				if (first_new == first_old) {
					// do nothing
				} else if (first_new == second_old) {
					// B->A, C->B, A->C
					res = faceTexCoord.VertexA();
					faceTexCoord.VertexA() = faceTexCoord.VertexB();
					faceTexCoord.VertexB() = faceTexCoord.VertexC();
					faceTexCoord.VertexC() = res;
				} else {
					res = faceTexCoord.VertexA();
					faceTexCoord.VertexA() = faceTexCoord.VertexC();
					faceTexCoord.VertexC() = faceTexCoord.VertexB();
					faceTexCoord.VertexB() = res;
				}
				ADDTOBS(faceTexCoord);
			}
			faceIndex++;
		}
	}
	streamFree(0, indexes); 
	streamFree(0, indexres); 
	streamFree(0, bkup_newindexes); 

	// in case no plane triangles exists add 0 as number of plain triangles
	if (!isTri)
		u3dAddU8(pEncoder, 0);

	for( U32 i = 0; i < numVertex; i++ )
	{
		u3dAddU32( pEncoder, *((U32*)&(pPos[i].X())));
		u3dAddU32( pEncoder, *((U32*)&(pPos[i].Y())));
		u3dAddU32( pEncoder, *((U32*)&(pPos[i].Z())));
	}

	if(!bExcludeNormals) 
	{
		// If normals are not excluded
		// 4.2
		IFXVector3* pNorm;
		m_pOriginalAuthorCLODMesh->GetNormals( &pNorm );
		for( i = 0; i < pMeshDescription->NumNormals; i++ )
		{
			u3dAddU32(pEncoder, *((U32*)(&pNorm[ i ].X())));
			u3dAddU32(pEncoder, *((U32*)(&pNorm[ i ].Y())));
			u3dAddU32(pEncoder, *((U32*)(&pNorm[ i ].Z())));
		}
	}

	// 4.3
	IFXVector4* pDiffColors;
	// We will use IFXVector4 to convert color from U32 BGRA format in which
	// they are stored in AuthorCLODMesh; and then write
	// it in R, G, B, A order
	IFXVector4 color;
	m_pOriginalAuthorCLODMesh->GetDiffuseColors( &pDiffColors );
	for( i = 0; i < pMeshDescription->NumDiffuseColors; i++ )
	{
		color = pDiffColors[ i ];
		u3dAddU32(pEncoder, *((U32*)(&color.R())));
		u3dAddU32(pEncoder, *((U32*)(&color.G())));
		u3dAddU32(pEncoder, *((U32*)(&color.B())));
		u3dAddU32(pEncoder, *((U32*)(&color.A())));
	}

	// 4.4
	IFXVector4* pSpecColors;
	m_pOriginalAuthorCLODMesh->GetSpecularColors( &pSpecColors );
	for( i = 0; i < pMeshDescription->NumSpecularColors; i++ )
	{
		color = pSpecColors[ i ];
		u3dAddU32(pEncoder, *((U32*)(&color.R())));
		u3dAddU32(pEncoder, *((U32*)(&color.G())));
		u3dAddU32(pEncoder, *((U32*)(&color.B())));
		u3dAddU32(pEncoder, *((U32*)(&color.A())));
	}

	// 4.5
	IFXVector4* pTexCoords;
	m_pOriginalAuthorCLODMesh->GetTexCoords( &pTexCoords );
	for( i = 0; i < pMeshDescription->NumTexCoords; i++ )
	{
		u3dAddU32(pEncoder, *((U32*)(&pTexCoords[ i ].U())));
		u3dAddU32(pEncoder, *((U32*)(&pTexCoords[ i ].V())));
		u3dAddU32(pEncoder, *((U32*)(&pTexCoords[ i ].W())));
		u3dAddU32(pEncoder, *((U32*)(&pTexCoords[ i ].T())));
	}

	IFXCHECKX(m_pOriginalAuthorCLODMesh->Unlock());

	u3dAddU32(pEncoder, 0);

	u3dEncoderFlush(pEncoder);
    
	IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
	IFXCHECKX(IFXCreateComponent(CID_IFXDataBlockX, IID_IFXDataBlockX, (void**)&pDataBlockX));
	pDataBlockX->SetSizeX(stream.m_written);
	U8* pData = NULL;
	pDataBlockX->GetPointerX(pData);
	memcpy(pData,stream.m_stream,stream.m_written);
	streamFree(0, stream.m_stream);
	stream.m_written = 0;
	stream.m_curPos = 0;
	stream.m_size = 0;

	// Set block type
	pDataBlockX->SetBlockTypeX( m_blockType );

	// Set block priority
	pDataBlockX->SetPriorityX(m_uPriorityCurrent);
	m_uPriorityCurrent += m_uPriorityIncrement;

	// set metadata
	IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
	IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
	pDataBlockX->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
	m_pModifier->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
	pBlockMD->AppendX(pObjectMD);

	// Put the data block on the list
	rDataBlockQueue.AppendBlockX(*pDataBlockX);

	// clean up
	IFXRELEASE( pDataBlockX );
	u3dEncoderDelete(pEncoder);
}
