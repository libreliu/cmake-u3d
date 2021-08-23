//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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
//	CIFXNewBaseMeshDecoder.cpp
//

#include "CIFXNewBaseMeshDecoder.h"
#include "NewBaseMeshUtils.h"
#include "IFXCheckX.h"
#include "IFXNewBaseMeshPluginCIDs.h"
#include "IFXNewBaseMesh.h"
#include "u3dbitdecoder.h"
#include "IFXBitStreamX.h"
#include "IFXModel.h"

#define getFromBS(what){\
	u3dGetU32( pDecoder, (U32*)&(what.VertexA()) );\
	u3dGetU32( pDecoder, (U32*)&(what.VertexB()) );\
	u3dGetU32( pDecoder, (U32*)&(what.VertexC()) );\
}

CIFXNewBaseMeshDecoder::CIFXNewBaseMeshDecoder():
IFXDEFINEMEMBER(m_pAuthorCLODResource),
IFXDEFINEMEMBER(m_pReconstructedAuthorCLODMesh)
{
	m_unitScale = 1.0f;
}

CIFXNewBaseMeshDecoder::~CIFXNewBaseMeshDecoder()
{
	if(m_pAuthorCLODResource) {
		m_pAuthorCLODResource->SetAuthorMeshFinal(m_pReconstructedAuthorCLODMesh);
	}
}

U32 CIFXNewBaseMeshDecoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXNewBaseMeshDecoder::Release()
{
	if ( 1 == m_uRefCount ) {
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXNewBaseMeshDecoder::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		if ( interfaceId == IID_IFXDecoderX )
		{
			*ppInterface = ( IFXDecoderX* ) this;
			this->AddRef();
		}
		else if ( interfaceId == IID_IFXUnknown ) 
		{
			*ppInterface = ( IFXUnknown* ) this;
			this->AddRef();
		} 
		else if ( interfaceId == IID_IFXExtensionDecoderX ) 
		{
			*ppInterface = ( IFXExtensionDecoderX* ) this;
			this->AddRef();
		} 
		else
		{
			*ppInterface = NULL;
			rc = IFX_E_UNSUPPORTED;
		}
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	return rc;
}

// Initialize and get a reference to the core services object
void CIFXNewBaseMeshDecoder::InitializeX( const IFXLoadConfig &lc )
{
	if( lc.m_units > 0.0f )
		m_unitScale = lc.m_units;
	IFXModifierBaseDecoder::InitializeX(lc);
}

void CIFXNewBaseMeshDecoder::CreateObjectX(IFXDataBlockX &rDataBlockX, IFXREFCID cid)
{
	if (NULL == m_pObject) 
	{
		U32 blockType;
		rDataBlockX.GetBlockTypeX(blockType);

		IFXDECLARELOCAL( IFXBitStreamX, pBitStreamX );
		IFXCHECKX(IFXCreateComponent( 
							CID_IFXBitStreamX, 
							IID_IFXBitStreamX, 
							(void**)&pBitStreamX ));
		pBitStreamX->SetDataBlockX( rDataBlockX );

		pBitStreamX->ReadIFXStringX( m_stringObjectName );
		pBitStreamX->ReadU32X( m_uChainPosition );

		IFXDECLARELOCAL(IFXNameMap, pNameMap);
		m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap);
		IFXCHECKX(pNameMap->Map(m_uLoadId, m_ePalette, m_stringObjectName));

		IFXCHECKX(IFXCreateComponent( cid, IID_IFXUnknown, (void**)&m_pObject ));

		IFXNewBaseMesh* pNewBaseMesh;
		m_pObject->QueryInterface(IID_IFXNewBaseMesh, (void**)&pNewBaseMesh);
		pNewBaseMesh->SetCLODResource(m_pAuthorCLODResource);

		IFXDECLARELOCAL(IFXMarker,pMarker );
		IFXCHECKX(m_pObject->QueryInterface( IID_IFXMarker, (void**)&pMarker ));
		IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
		IFXCHECKX(m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph ));
		IFXCHECKX(pMarker->SetSceneGraph( pSceneGraph ));
		IFXCHECKX(m_pAuthorCLODResource->SetSceneGraph( pSceneGraph ));
		pMarker->SetExternalFlag(m_bExternal);

		ProcessChainX(rDataBlockX);
	}
}

// Provide next block of data to the decoder
void CIFXNewBaseMeshDecoder::PutNextBlockX( IFXDataBlockX &rDataBlockX )
{
	if(NULL == m_pDataBlockQueueX) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	m_pDataBlockQueueX->AppendBlockX( rDataBlockX );

	if (NULL == m_pObject)
	{
		IFXRELEASE(m_pAuthorCLODResource);
		IFXCHECKX(IFXCreateComponent(CID_IFXAuthorCLODResource,IID_IFXAuthorCLODResource,(void**)&m_pAuthorCLODResource));

		U32 uBlockType;
		rDataBlockX.GetBlockTypeX(uBlockType);
		CreateObjectX(rDataBlockX, CID_IFXNewBaseMesh);

#ifdef KEEP_DATA_BLOCKS
		IFXCHECKX(m_pAuthorCLODResource->BuildDataBlockQueue());
#endif
	}

#ifdef KEEP_DATA_BLOCKS
	// Add the data block to the generator. This is used by the generator as a
	// cached copy of itself in encoded form thus saving potential encoding at
	// save or export time.
	IFXDECLARELOCAL(IFXDataBlockQueueX,pGeneratorDataBlockQueueX);
	m_pAuthorCLODResource->GetDataBlockQueueX( pGeneratorDataBlockQueueX );
	if ( pGeneratorDataBlockQueueX )
		pGeneratorDataBlockQueueX->AppendBlockX( rDataBlockX );
#endif
}

void CIFXNewBaseMeshDecoder::TransferX(IFXRESULT &rWarningPartialTransfer)
{
	if(NULL == m_pCoreServices || NULL == m_pObject) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	// For each data block in the list
	BOOL bDone = FALSE;
	while ( !bDone) {
		// Get the next data block
		IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
		m_pDataBlockQueueX->GetNextBlockX( pDataBlockX, bDone);

		if(pDataBlockX) 
		{
			U32 blockType;
			pDataBlockX->GetBlockTypeX( blockType );

			if( blockType == m_blockType )
				ProcessNewBaseMeshBlockX(*pDataBlockX);
			else
				IFXCHECKX( IFX_E_UNSUPPORTED );

			IFXCHECKX(m_pAuthorCLODResource->SetAuthorMesh(m_pReconstructedAuthorCLODMesh));

			IFXDECLARELOCAL(IFXSceneGraph,pSG);
			IFXDECLARELOCAL(IFXPalette,pPalette);
			IFXDECLARELOCAL(IFXPalette,pGP);
			U32 index = 0;
			IFXCHECKX(m_pCoreServices->GetSceneGraph(IID_IFXSceneGraph,(void**)&pSG));
			pSG->GetPalette( IFXSceneGraph::NODE, &pPalette );
			pSG->GetPalette( IFXSceneGraph::GENERATOR, &pGP );
			pGP->Find(m_stringObjectName, &index);
			U32 size = 0;
			pPalette->GetPaletteSize(&size);
			U32 i;
			for ( i = 0; i < size; i++) {
				IFXDECLARELOCAL(IFXModel,pObject);
				pPalette->GetResourcePtr( i, IID_IFXModel, (void**)&pObject );
				if (pObject && (pObject->GetResourceIndex() == index))
					pObject->SetResourceIndex(index);
			}
		}
	}

	rWarningPartialTransfer = IFX_OK;
}

U64 myRead(U3dStream_cbCtx* pCtx, U8** pData, U64 size)
{
	U64         readByte = 0;
	U3DStream*  stream = (U3DStream*)pCtx;

	if(stream) {
		(size + stream->m_read <= stream->m_size)?
			readByte = size:readByte = stream->m_size-stream->m_read;
		*pData = stream->m_curPos;
		stream->m_curPos+=readByte;
		stream->m_read+=readByte;
	}/* if */
	return readByte;
}


void CIFXNewBaseMeshDecoder::ProcessNewBaseMeshBlockX( IFXDataBlockX &rDataBlockX )
{
	// This version of Texture Modifier has only hardcoded functionality, so
	// it doesn't write/read any data except some common values like name and 
	// chain index. These elements are common to all Modifier blocks and are
	// decoded in DecodeCommonElements(), so we can skip them since we decode
	// nothing after them. In case if any information specific to Texture Mod
	// are added then uncomment the following block and add specific data
	// decoding after it.

	// set metadata
	IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
	IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
	rDataBlockX.QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
	m_pObject->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
	pObjectMD->AppendX(pBlockMD);

	U3DStream streamCtx;
	U3dDecoder* pDecoder;
	U3dMemory	memfunc = {0, streamMalloc, streamFree, 0};
	U32 size;
	rDataBlockX.GetSizeX(size);
	streamCtx.m_size = size;
	rDataBlockX.GetPointerX(streamCtx.m_stream);
	streamCtx.m_curPos = streamCtx.m_stream;
	streamCtx.m_read = 0;
    
	// Create the bitstream for reading from the data block
	U3dStatus status  = u3dDecoderInit(&pDecoder, &memfunc, &streamCtx, myRead);

	// 1. Model Name
	U3dString   name;
	u3dGetU16(pDecoder, &name.size);
	name.string = (char*)streamMalloc(0, (name.size+1)*sizeof(char));
	name.string[name.size] = 0;
	for (unsigned long i = 0; i < name.size; i++)
		u3dGetU8(pDecoder, (U8*)&(name.string[i]));
	streamFree(0, name.string); 

	// 2. Index for modifier chain
	u3dGetU32(pDecoder, &m_uChainPosition);

	if( m_unitScale > 0.0f && m_unitScale != 1.0f )
	{
		IFXMatrix4x4 matrix = m_pAuthorCLODResource->GetTransform();
		IFXVector3 scale( (F32)m_unitScale, (F32)m_unitScale, (F32)m_unitScale );
		matrix.Scale( scale );
		m_pAuthorCLODResource->SetTransform( matrix );
	}

	//  3. Max Mesh Description
	IFXAuthorMeshDesc samdMinMeshDescription;
	IFXAuthorMeshDesc* pMinMeshDescription = &samdMinMeshDescription;
	IFXASSERT(pMinMeshDescription);

	U32 numElements, numVertex;
	u3dGetU32(pDecoder, &pMinMeshDescription->NumPositions);
	numVertex = pMinMeshDescription->NumPositions;
	//    3.1 U32 NumFaces;
	u3dGetU32(pDecoder, &pMinMeshDescription->NumFaces);
	u3dGetU32(pDecoder, &numElements);
	//    3.3 U32 NumNormals;
	u3dGetU32(pDecoder, &pMinMeshDescription->NumNormals);
	//    3.4 U32 NumDiffuseColors;
	u3dGetU32(pDecoder, &pMinMeshDescription->NumDiffuseColors);
	//    3.5 U32 NumSpecularColors;
	u3dGetU32(pDecoder, &pMinMeshDescription->NumSpecularColors);
	//    3.6 U32 NumTexCoords;
	u3dGetU32(pDecoder, &pMinMeshDescription->NumTexCoords);
	//    3.7 U32 NumShaders;
	u3dGetU32(pDecoder, &pMinMeshDescription->NumMaterials);

	bool bExcludeNormals = (pMinMeshDescription->NumNormals)?0:1;
	IFXRELEASE(m_pReconstructedAuthorCLODMesh);
	IFXCHECKX(IFXCreateComponent(CID_IFXAuthorMesh,IID_IFXAuthorCLODMesh,(void**)&m_pReconstructedAuthorCLODMesh));
	IFXASSERT(m_pReconstructedAuthorCLODMesh);
	IFXCHECKX(m_pReconstructedAuthorCLODMesh->Allocate(pMinMeshDescription));
	m_pReconstructedAuthorCLODMesh->SetMeshDesc( pMinMeshDescription );

	//    3.8 Shaders Descriptions
	{
		IFXAuthorMaterial* pAuthorMeshMaterial = NULL;
		IFXCHECKX(m_pReconstructedAuthorCLODMesh->Lock());
		IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetMaterials(&pAuthorMeshMaterial));
		U32 i = 0;

		for(i=0; i < pMinMeshDescription->NumMaterials; i++) {
			//  3.8.1 Shading Attributes (BOOL m_uDiffuseColors; BOOL m_uSpecularColors;)
			U32 uMaterialAttributes = 0;
			u3dGetU32(pDecoder, &uMaterialAttributes);
			if(uMaterialAttributes & 1) {
				pAuthorMeshMaterial[i].m_uDiffuseColors = TRUE;
			} else {
				pAuthorMeshMaterial[i].m_uDiffuseColors = FALSE;
			}
			if(uMaterialAttributes & 2) {
				pAuthorMeshMaterial[i].m_uSpecularColors = TRUE;
			} else {
				pAuthorMeshMaterial[i].m_uSpecularColors = FALSE;
			}
			if(bExcludeNormals) {
				pAuthorMeshMaterial[i].m_uNormals = FALSE;
			} else {
				pAuthorMeshMaterial[i].m_uNormals = TRUE;
			}

			//  3.8.2 U32 m_uNumTextureLayers;
			u3dGetU32(pDecoder, &pAuthorMeshMaterial[i].m_uNumTextureLayers);
			//  3.8.3 U32 m_uNumTextureLayers[m_uNumTextureLayers]; (IFXMAX_TEX_COORD_LAYERS == 8)
			IFXASSERT(IFX_MAX_TEXUNITS >= pAuthorMeshMaterial[i].m_uNumTextureLayers);
			U32 j = 0;
			for(j = 0; j < pAuthorMeshMaterial[i].m_uNumTextureLayers; j++) {
				u3dGetU32(pDecoder, &pAuthorMeshMaterial[i].m_uTexCoordDimensions[j]);
			}
			//  3.8.4 U32 OriginalShadingID;  // the mesh scrubber may have removed unused materials an repacked them.
			u3dGetU32(pDecoder, &pAuthorMeshMaterial[i].m_uOriginalMaterialID);
		}
		IFXCHECKX(m_pReconstructedAuthorCLODMesh->Unlock());
	}

	m_pAuthorCLODResource->SetExcludeNormals(bExcludeNormals);

	m_pReconstructedAuthorCLODMesh->SetFinalMaxResolution(pMinMeshDescription->NumPositions);
	m_pReconstructedAuthorCLODMesh->SetMaxResolution(pMinMeshDescription->NumPositions);
	m_pReconstructedAuthorCLODMesh->SetMinResolution(pMinMeshDescription->NumPositions);

	// process triangle strips
	U8 elCount;
	U32 nVRead = 0;
	U32 uFaceMaterial = 0;

	IFXAuthorMaterial* pAuthorMeshMaterial = NULL;
	IFXCHECKX(m_pReconstructedAuthorCLODMesh->Lock());
	IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetMaterials(&pAuthorMeshMaterial));

	U32 uNumFaceTypes;
	IFXAuthorFace *faces;

	IFXVector3* pReconstructedPositions = NULL;
	IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetPositions(&pReconstructedPositions));
	IFXVector3* pReconstructedNormals = NULL;
	IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetNormals(&pReconstructedNormals));
	U32 faceIndex = 0;
	U32 isTri = false;     // true - triangle list is started
	U8 faceCounter; // number of vertices collected. If < 3 - dont process face info saving
	U32 numElementsRead = 0;
	U32 oldv, newv, medv, indx;
	oldv = newv = medv = (U32)(-1);
	// process traingle strips
	do {
		faceCounter = 0;
		// read element count;
		u3dGetU8(pDecoder, &elCount);
		if (elCount) {
			// tristrip is started
		} else {
			// plain triangles are started
			isTri = true; elCount = numElements - numElementsRead;
		}
		for (U32 i = 0; i < elCount; i++) {
			faceCounter++;
			if (isTri && faceCounter > 3)
				faceCounter = 1;
			u3dGetU32(pDecoder, &indx);
			oldv = medv; medv = newv; newv = indx;
			
			numElementsRead++;
			if ((isTri && faceCounter == 3) || (!isTri && faceCounter > 2 )) {
				// read shaders and add triangle to the mesh
				//  4.6.1 Face Shaders
				uFaceMaterial = 0;
				u3dGetU32(pDecoder, &uFaceMaterial);
				m_pReconstructedAuthorCLODMesh->SetFaceMaterial(faceIndex, uFaceMaterial);

				// allocate a memory for whole bunch of data for this face
				uNumFaceTypes = 2 + pAuthorMeshMaterial[uFaceMaterial].m_uNumTextureLayers;
				if( pAuthorMeshMaterial[uFaceMaterial].m_uDiffuseColors )
					uNumFaceTypes++;
				if( pAuthorMeshMaterial[uFaceMaterial].m_uSpecularColors )
					uNumFaceTypes++;
				faces = (IFXAuthorFace*)streamMalloc(0, sizeof(IFXAuthorFace)*uNumFaceTypes);

				faces[0].VertexA() = oldv;
				U8 count = 2;

				U32 second = medv;
				U32 third = newv;
				if (!isTri && !(faceCounter%2)) {
					// swap
					second = newv;
					third = medv;
				}
				faces[0].VertexA() = oldv;
				faces[0].VertexB() = second;
				faces[0].VertexC() = third;
				// set up faces
				m_pReconstructedAuthorCLODMesh->SetPositionFace( faceIndex, &faces[0] );
				if(!bExcludeNormals) {
					getFromBS(faces[1]);
					m_pReconstructedAuthorCLODMesh->SetNormalFace( faceIndex, &faces[1] );
				}
				if(pAuthorMeshMaterial[uFaceMaterial].m_uDiffuseColors)
				{
					getFromBS(faces[count]);
					m_pReconstructedAuthorCLODMesh->SetDiffuseFace( faceIndex, &faces[count] );
					count++;
				}
				if(pAuthorMeshMaterial[uFaceMaterial].m_uSpecularColors)
				{
					getFromBS(faces[count]);
					m_pReconstructedAuthorCLODMesh->SetSpecularFace( faceIndex, &faces[count] );
					count++;
				}
				for( U32 j=0; j < pAuthorMeshMaterial[uFaceMaterial].m_uNumTextureLayers; j++)
				{
					getFromBS(faces[count]);
					m_pReconstructedAuthorCLODMesh->SetTexFace( j, faceIndex, &faces[count] );
					count++;
				}

				streamFree(0, faces);

				faceIndex++;
			}
		}
	} while (!isTri);

	IFXVector3* pReconstructedPos = NULL;
	IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetPositions(&pReconstructedPos));
	for(i = 0; i < pMinMeshDescription->NumPositions; i++)
	{
		F32 fX = 0;
		F32 fY = 0;
		F32 fZ = 0;
		u3dGetU32(pDecoder, (U32*)&fX);
		u3dGetU32(pDecoder, (U32*)&fY);
		u3dGetU32(pDecoder, (U32*)&fZ);
		pReconstructedPos[i].X() = fX;
		pReconstructedPos[i].Y() = fY;
		pReconstructedPos[i].Z() = fZ;
	}
	if( pMinMeshDescription->NumNormals )
	{
		IFXVector3* pReconstructedNormals = NULL;
		IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetNormals(&pReconstructedNormals));
		for(i = 0; i < pMinMeshDescription->NumNormals; i++)
		{
			F32 fX = 0;
			F32 fY = 0;
			F32 fZ = 0;
			u3dGetU32(pDecoder, (U32*)&fX);
			u3dGetU32(pDecoder, (U32*)&fY);
			u3dGetU32(pDecoder, (U32*)&fZ);
			pReconstructedNormals[i].X() = fX;
			pReconstructedNormals[i].Y() = fY;
			pReconstructedNormals[i].Z() = fZ;
		}
	}
	// load colors etc
	IFXVector4 color;
	IFXVector4* pReconstructedColors = NULL;

	//  4.3 Diffuse Colors [NumDiffuseColors]
	if( pMinMeshDescription->NumDiffuseColors )
	{
		IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetDiffuseColors(&pReconstructedColors));
		for(i = 0; i < pMinMeshDescription->NumDiffuseColors; i++)
		{
			F32 R = 0, G = 0, B = 0, A = 0;
			u3dGetU32(pDecoder, (U32*)&R);
			u3dGetU32(pDecoder, (U32*)&G);
			u3dGetU32(pDecoder, (U32*)&B);
			u3dGetU32(pDecoder, (U32*)&A);
			color.Set( R, G, B, A );
			pReconstructedColors[i] = color;
		}
	}

	//  4.4 Specular Colors [NumSpecularColors]
	if( pMinMeshDescription->NumSpecularColors )
	{
		IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetSpecularColors(&pReconstructedColors));
		for(i = 0; i < pMinMeshDescription->NumSpecularColors; i++)
		{
			F32 R, G, B, A;
			u3dGetU32(pDecoder, (U32*)&R);
			u3dGetU32(pDecoder, (U32*)&G);
			u3dGetU32(pDecoder, (U32*)&B);
			u3dGetU32(pDecoder, (U32*)&A);
			color.Set( R, G, B, A );
			pReconstructedColors[i] = color;
		}
	}

	//  4.5 Tex Coords [NumTexCoords]
	if( pMinMeshDescription->NumTexCoords )
	{
		IFXVector4* pReconstructedTexCoords = NULL;
		/// @todo: When using IFXVector4 as a Texture Coordinate,
		// the short cuts R(), G(), B(), A() do not make sense.
		// Change to something more meaningful.
		IFXCHECKX(m_pReconstructedAuthorCLODMesh->GetTexCoords(&pReconstructedTexCoords));
		for(i = 0; i < pMinMeshDescription->NumTexCoords; i++)
		{
			F32 fU = 0;
			F32 fV = 0;
			F32 fW = 0;
			F32 fT = 0;
			u3dGetU32(pDecoder, (U32*)&fU);
			u3dGetU32(pDecoder, (U32*)&fV);
			u3dGetU32(pDecoder, (U32*)&fW);
			u3dGetU32(pDecoder, (U32*)&fT);
			pReconstructedTexCoords[i].U() = fU;
			pReconstructedTexCoords[i].V() = fV;
			pReconstructedTexCoords[i].W() = fW;
			pReconstructedTexCoords[i].T() = fT;
		}
	}

	// process triangles
	IFXCHECKX(m_pReconstructedAuthorCLODMesh->Unlock());
	u3dDecoderDelete(pDecoder);
}

IFXRESULT IFXAPI_CALLTYPE CIFXNewBaseMeshDecoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		CIFXNewBaseMeshDecoder *pComponent = new CIFXNewBaseMeshDecoder;

		if ( pComponent ) {
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			rc = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		} else {
			rc = IFX_E_OUT_OF_MEMORY;
		}
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}

void CIFXNewBaseMeshDecoder::SetContBlockTypes( U32 *blockTypes, U32 typesCount )
{
	IFXASSERT( typesCount == 1 );

	m_blockType = blockTypes[0];
}

