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

//------------- INCLUDES ----------------------------------------------------

#include "GeomObjectExporter.h"
#include "iparamm2.h"

//------------- DEFINES -----------------------------------------------------

//------------- CONSTANTS ---------------------------------------------------

//------------- ENUMERATIONS ------------------------------------------------

//------------- CLASSES, STRUCTURES AND TYPES -------------------------------

//------------- LOCAL FUNCTION PROTOTYPES -----------------------------------

BOOL getTMNegParity( Matrix3& );
Point3 getVertexNormal( Mesh*, U32, U32 );

//------------- PUBLIC METHODS ----------------------------------------------

IFXGeomObjectExporter::IFXGeomObjectExporter( const IFXNodeExporter* pNodeExp )
: IFXObjectExporter( pNodeExp )
{
	m_pShaderMapTable = new U32[MAX_MESHMAPS];
	m_pShaderUseTable = new U32[MAX_MESHMAPS];
	m_isModelTextured = false;
	m_pObj = IFXObjectExporter::getState().obj;
	m_boneCount = 0;
	m_ppBoneList = new INode*[MAX_BONE_NUM];
	m_pBoneInfo = NULL;
}

IFXGeomObjectExporter::~IFXGeomObjectExporter()
{
	delete m_pShaderMapTable;
	delete m_pShaderUseTable;
	delete[] m_ppBoneList;
}

//------------- PROTECTED METHODS -------------------------------------------

IFXRESULT IFXGeomObjectExporter::exportNode()
{
	IFXRESULT ec = IFX_OK;
	return ec;
}

IFXRESULT IFXGeomObjectExporter::exportResource( const IFXNodeData* pData )
{
	IFXRESULT ec = IFX_OK;

	U32     vo[3]; // vertex order
	Class_ID cid(TRIOBJ_CLASS_ID, 0);
	TimeValue t = 0; /// @todo: t might not be 0!
	INode*  pNode = IFXObjectExporter::getNode();
	Mtl*    pMaterial = pNode->GetMtl();
	Matrix3 tm = pNode->GetObjTMAfterWSM(t);
	BOOL    negScale = getTMNegParity(tm);
	BOOL    isNeedDel = FALSE;
	TriObject* pTri = NULL; // triangle mesh object
	Mesh*   pMesh = NULL; // mesh

	// Set a pointer to a TriObject given an INode or set it to NULL
	// if the node cannot be converted to a TriObject
	if( m_pObj->CanConvertToType( cid ) )
	{
		pTri = (TriObject *)m_pObj->ConvertToType( t, cid );
		// Note that the TriObject should only be deleted
		// if the pointer to it is not equal to the object
		// pointer that called ConvertToType()
		if( m_pObj != pTri )
			isNeedDel = TRUE;
	}
	else
	{
		pTri = NULL;
		ec  = IFX_E_BAD_PARAM;
	}

	if( IFXSUCCESS( ec ) )
	{
		// Order of the vertices. Get 'em counter clockwise if the objects is
		// negatively scaled.
		if( negScale )
		{
			vo[0] = 2; vo[1] = 1; vo[2] = 0;
		}
		else
		{
			vo[0] = 0; vo[1] = 1; vo[2] = 2;
		}

		// set mesh and build normals
		pMesh = &pTri->GetMesh();

		if( pMesh != NULL )
		{
			BipedUtil bipedUtil;
			bipedUtil.FindBipedRoot( m_ppBoneList, m_boneCount );
			bipedUtil.BipedIntoReferenceMode();
			CollectBones( pNode, m_ppBoneList, &m_boneCount );
			pMesh->buildNormals();
			m_pWriter->output( IDTF_MODEL_TYPE, IDTF_MESH );
			m_pWriter->begin( IDTF_MESH );
			exportParams( pMesh, pMaterial );
			exportShadingDescription( pMesh );
			exportFacePositionList( pMesh, vo );
			exportFaceNormalList( pMesh );
			exportMeshFaceShadingList( pMesh );
			if( m_isModelTextured )
				exportMeshFaceTextureLayerList( pMesh, vo );
			exportMeshFaceDiffuseColorList( pMesh, vo );
			exportMeshFaceSpecularColorList( pMesh );
			exportMeshPositionList( pMesh, tm );
			exportMeshNormalList( pMesh, vo );
			exportMeshDiffuseColorList( pMesh );
			exportMeshSpecularColorList( pMesh );
			if( m_isModelTextured )
				exportModelTextureCoordList( pMaterial, pMesh );
			exportModelSkeleton( &bipedUtil );
			m_pWriter->end();
		}
	}

	return ec;
}

//------------- PRIVATE METHODS ---------------------------------------------


void IFXGeomObjectExporter::exportParams( Mesh* pMesh, Mtl* pMtl )
{
	if( !pMesh )
		return;
	U32 i;
	for( i = 0; i < MAX_MESHMAPS; i++ )
	{
		m_pShaderMapTable[i] = 0;
		m_pShaderUseTable[i] = 0;
	}

	U32 numTexCoords = 0;
	m_pWriter->output( IDTF_FACE_COUNT, pMesh->getNumFaces() );
	m_pWriter->output( IDTF_MODEL_POSITION_COUNT, pMesh->getNumVerts() );
	m_pWriter->output( IDTF_MODEL_NORMAL_COUNT, pMesh->getNumFaces() * 3 );
	m_pWriter->output( IDTF_MODEL_DIFFUSE_COLOR_COUNT, pMesh->numCVerts );
	m_pWriter->output( IDTF_MODEL_SPECULAR_COLOR_COUNT, 0 );

/*	for( U32 i = 0; i < getNode()->GetMtl()->NumSubMtls(); i++ )
	{
		RecordTotalShdrMapUsage( getNode()->GetMtl()->GetSubMtl( i ), m_pShaderMapTable, m_pShaderUseTable );
		for (U32 i = 0; i < MAX_MESHMAPS; i++)
		{			
			if (pMesh->mapSupport(i) && m_pShaderUseTable[i] != 0)
				numTexCoords += pMesh->getNumMapVerts(i);
		}
	}*/
	RecordTotalShdrMapUsage( pMtl, m_pShaderMapTable, m_pShaderUseTable );
	for (U32 i = 0; i < MAX_MESHMAPS; i++)
	{
		if (pMesh->mapSupport(i) && m_pShaderUseTable[i] != 0)
			numTexCoords += pMesh->getNumMapVerts(i);
	}
	if( numTexCoords > 0 )
		m_isModelTextured = true;

	m_pWriter->output( IDTF_MODEL_TEXTURE_COORD_COUNT, numTexCoords );
	m_pWriter->output( IDTF_MODEL_BONE_COUNT, m_boneCount );
	if( pMtl && pMtl->NumSubMtls() > 0 )
		m_pWriter->output( IDTF_MODEL_SHADING_COUNT, pMtl->NumSubMtls() );
	else
		m_pWriter->output( IDTF_MODEL_SHADING_COUNT, 1 );
}

/*
export vertex list
*/
void IFXGeomObjectExporter::exportMeshPositionList( Mesh* pMesh, Matrix3& tm )
{
	if( !pMesh )
		return;
	m_pWriter->begin( IDTF_MODEL_POSITION_LIST );
	for( U32 i = 0; i < pMesh->getNumVerts(); i++ )
	{
		Point3 v = pMesh->verts[i];
		v[0] = v[0];
		v[1] = v[1];
		v[2] = v[2];
		m_pWriter->output( L"", &v );
	}
	m_pWriter->end();
}

/*
export faces
*/
void IFXGeomObjectExporter::exportFacePositionList( Mesh* pMesh, U32* vo )
{
	if( !pMesh || !vo )
		return;
	U32 vi[3];

	m_pWriter->begin( IDTF_MESH_FACE_POSITION_LIST );

	// To determine visibility of a face, get the vertices in clockwise order.
	// If the objects has a negative scaling, we must compensate for that by
	// taking the vertices counter clockwise
	for( U32 i = 0; i < pMesh->getNumFaces(); i++ )
	{
		vi[0] = pMesh->faces[i].getVert( vo[0] );
		vi[1] = pMesh->faces[i].getVert( vo[1] );
		vi[2] = pMesh->faces[i].getVert( vo[2] );
		m_pWriter->output( L"", vi );

		// write out smoothing group
		//    writer->output( "", (unsigned int)pMesh->faces[i].getSmGroup(), false );

		// This is the material ID for the face.
		// Note: If you use this you should make sure that the material ID
		// is not larger than the number of sub materials in the material.
		// The standard approach is to use a modulus function to bring down
		// the material ID.
		//    writer->output( "", pMesh->faces[i].getMatID(), false );
	}

	m_pWriter->end(); // End face list
}

/*
export normals
*/
void IFXGeomObjectExporter::exportMeshNormalList( Mesh* pMesh, U32* vo )
{
	if( !pMesh || !vo )
		return;
	Point3 fn;  // Face normal
	Point3 vn;  // Vertex normal
	U32    vert;
	Face*  pF;

	// Export mesh (face + vertex) normals
	m_pWriter->begin( IDTF_MODEL_NORMAL_LIST );

	// Face and vertex normals.
	// In MAX a vertex can have more than one normal (but doesn't always have it).
	// This is depending on the face you are accessing the vertex through.
	// To get all information we need to export all three vertex normals
	// for every face.
	for( U32 i = 0; i < pMesh->getNumFaces(); i++ )
	{
		pF = &pMesh->faces[i];
		//		fn = pMesh->getFaceNormal( i );
		//		m_pWriter->output( "", &fn );
		vert = pF->getVert( vo[0] );
		vn = getVertexNormal( pMesh, i, vert );
		m_pWriter->output( L"", &vn );

		vert = pF->getVert( vo[1] );
		vn = getVertexNormal( pMesh, i, vert );
		m_pWriter->output( L"", &vn );

		vert = pF->getVert( vo[2] );
		vn = getVertexNormal( pMesh, i, vert );
		m_pWriter->output( L"", &vn );
	}

	m_pWriter->end();
}

void IFXGeomObjectExporter::exportShadingDescription( Mesh* pMesh )
{
	if( !pMesh )
		return;
	U32 texLayerCount;
	m_pWriter->begin( IDTF_MODEL_SHADING_DESCRIPTION_LIST );
	U32 numShadingDescriptions = 1;
	Mtl* pMtl = getNode()->GetMtl();
	if( pMtl && pMtl->NumSubMtls() > 0 )
		numShadingDescriptions = pMtl->NumSubMtls();
	for( U32 i = 0; i < numShadingDescriptions; i++ )
	{
		m_pWriter->begin( IDTF_SHADING_DESCRIPTION, i );
		if( pMtl && pMtl->NumSubMtls() > 0 )
		{
			RecordTotalShdrMapUsage( pMtl->GetSubMtl( i ), m_pShaderMapTable, m_pShaderUseTable );
			texLayerCount = getActiveTextureCount( pMtl->GetSubMtl( i ) );
		}
		else
		{
			RecordTotalShdrMapUsage( pMtl, m_pShaderMapTable, m_pShaderUseTable );
			texLayerCount = getActiveTextureCount( pMtl );
		}
		m_pWriter->output( IDTF_TEXTURE_LAYER_COUNT, texLayerCount );
		if( texLayerCount > 0 )
		{
			m_pWriter->begin( IDTF_TEXTURE_COORD_DIMENSION_LIST );
			for( U32 j = 0; j < texLayerCount; j++ )
			{
				m_pWriter->output( IDTF_TEXTURE_LAYER, j );
				m_pWriter->setPlain( true );
				m_pWriter->output( IDTF_TEXTURE_LAYER_DIMENSION, 2 );//temp
				m_pWriter->setPlain( false );
			}
			m_pWriter->end();
		}
		m_pWriter->output( IDTF_SHADER_ID, i );
		m_pWriter->end();
	}
	m_pWriter->end();
	if( pMtl && pMtl->NumSubMtls() > 0 )
		RecordTotalShdrMapUsage( pMtl, m_pShaderMapTable, m_pShaderUseTable );
}
void IFXGeomObjectExporter::exportFaceNormalList( Mesh* pMesh )
{
	if( !pMesh )
		return;
	U32 fn[3];  // Face normal
	U32    vert = 0;

	m_pWriter->begin( IDTF_MESH_FACE_NORMAL_LIST );

	// Face normals.
	for( U32 i = 0; i < pMesh->getNumFaces(); i++ )
	{
		fn[0] = 3*i;
		fn[1] = 3*i+1;
		fn[2] = 3*i+2;
		m_pWriter->output( L"", fn );
	}

	m_pWriter->end();
}
void IFXGeomObjectExporter::exportMeshFaceShadingList( Mesh* pMesh )
{
	if( !pMesh )
		return;
	// Export shading list
	m_pWriter->begin( IDTF_MESH_FACE_SHADING_LIST );
	if( getNode()->GetMtl() && getNode()->GetMtl()->NumSubMtls() > 0 )
	{
		for( U32 i = 0; i < pMesh->getNumFaces(); i++ )
			m_pWriter->output( L"", pMesh->getFaceMtlIndex( i ) );
	}
	else
	{
		for( U32 i = 0; i < pMesh->getNumFaces(); i++ )
			m_pWriter->output( L"", 0 );
	}

	m_pWriter->end();
}
void IFXGeomObjectExporter::exportMeshFaceTextureLayerList( Mesh* pMesh, U32* vo )
{
	if( !pMesh || !vo )
		return;
	Mtl* pMtl = getNode()->GetMtl();
	U32 i,j;
	U32 layerCount = 0;
	U32 coordCount = 0;
	U32 layer[3];
	TVFace  *pTVFace = NULL;
	m_pWriter->begin( IDTF_MESH_FACE_TEXTURE_COORD_LIST );
	for (j = 0; j < pMesh->getNumFaces(); j++)
	{
		Mtl* pSubMtl = NULL;
		if( pMtl )
			pSubMtl = pMtl->GetSubMtl( pMesh->getFaceMtlIndex(j) );
		if( !pSubMtl )
			pSubMtl = pMtl;
		m_pWriter->begin( IDTF_MESH_FACE, j );
		layerCount = 0;
		U32 num = pSubMtl->NumSubTexmaps();
		// sub-texture maps
		for( i = 0; i < num; ++i )
		{
			Texmap* pSubTex = pSubMtl->GetSubTexmap(i);
			if( pSubTex != NULL && pSubTex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00) &&
				( pSubMtl->ClassID() != Class_ID(DMTL_CLASS_ID, 0) || ((StdMat*)pSubMtl)->MapEnabled(i) ) )
			{
				pTVFace = pMesh->mapFaces(i);
				layer[0] = pTVFace[j].t[vo[0]];
				layer[1] = pTVFace[j].t[vo[1]];
				layer[2] = pTVFace[j].t[vo[2]];
				m_pWriter->output( IDTF_TEXTURE_LAYER, layerCount );
				m_pWriter->setPlain( true );
				m_pWriter->output( IDTF_LINE_TEX_COORD, layer );
				m_pWriter->setPlain( false );
				coordCount += pMesh->getNumMapVerts(i);
				layerCount++;
			}
		}
		m_pWriter->end();
	}
	m_pWriter->end();
}
void IFXGeomObjectExporter::exportMeshDiffuseColorList( Mesh* pMesh )
{
	if( !pMesh )
		return;
	U32 numCVx = pMesh->numCVerts;

	if( numCVx )
	{
		U32 i;

		// begin vertex colors list
		m_pWriter->begin( IDTF_MODEL_DIFFUSE_COLOR_LIST );
		for( i = 0; i < numCVx; i++ )
		{
			Point3 vc = pMesh->vertCol[i]; // typedef Point3 VertColor
			m_pWriter->output( L"", &vc );
		}
		m_pWriter->end(); // end vertex color list

	}
}
void IFXGeomObjectExporter::exportMeshFaceSpecularColorList( Mesh* pMesh )
{
}
void IFXGeomObjectExporter::exportMeshFaceDiffuseColorList( Mesh* pMesh, U32* vo )
{
	if( !pMesh )
		return;
	U32 numCVx = pMesh->numCVerts;

	if( numCVx )
	{
		U32 i;
		U32 vi[3];

		// begin color per vertex faces list
		m_pWriter->begin( IDTF_MESH_FACE_DIFFUSE_COLOR_LIST );
		for( i = 0; i < pMesh->getNumFaces(); i++)
		{
			vi[0] = pMesh->vcFace[i].t[vo[0]];
			vi[1] = pMesh->vcFace[i].t[vo[1]];
			vi[2] = pMesh->vcFace[i].t[vo[2]];
			m_pWriter->output( L"", vi );
		}
		m_pWriter->end(); // end vertex colors faces list
	}
}
void IFXGeomObjectExporter::exportMeshSpecularColorList( Mesh* pMesh )
{
}
void IFXGeomObjectExporter::exportModelTextureCoordList( Mtl* pMtl, Mesh* pMesh )
{
	if( !pMesh || !pMtl )
		return;
	m_pWriter->begin( IDTF_MODEL_TEXTURE_COORD_LIST );
	U32 i,j;

	for (i = 0; i < MAX_MESHMAPS; i++)
	{
		if (pMesh->mapSupport(i) && (m_pShaderUseTable[i] != 0))
		{
			UVVert* pUVVerts = NULL;
			pUVVerts = pMesh->mapVerts(i);
			for (j = 0; j < pMesh->getNumMapVerts(i); j++)
			{
				m_pWriter->output( L"", (F32)pUVVerts[j][0] );
				m_pWriter->setPlain( true );
				m_pWriter->output( L"", (F32)pUVVerts[j][1] );
				m_pWriter->output( L"", (F32)pUVVerts[j][2] );
				m_pWriter->output( L"", (F32)0.0 );
				m_pWriter->setPlain( false );
			}
		}
	}
	m_pWriter->end();
}

void IFXGeomObjectExporter::exportModelSkeleton( BipedUtil* pBipedUtil )
{
	if( !pBipedUtil )
		return;
	pBipedUtil->BipedRemoveNonUniformScale();

	if (m_boneCount > 0)
		m_pBoneInfo = new IFXBonesExporter[m_boneCount];
	else
		m_pBoneInfo = NULL;


	// iterate the bones in the list and initialize the bones conversion info. 
	unsigned int i, j; 
	for(i = 0; i < m_boneCount; ++i )
	{
		m_pBoneInfo[i].Init(i, m_ppBoneList[i], m_ppBoneList[0]);
	}

	// a n^2 pass to set up the Bone parent 
	for(i = 0; i < m_boneCount; ++i )
	{
		// don't search for the parent on bones with the 
		// scene root for the parent
		if(m_pBoneInfo[i].GetParentNode())
		{
			for(j = 0; j < m_boneCount; ++j )
			{
				if(m_pBoneInfo[j].GetNode() == m_pBoneInfo[i].GetParentNode())
				{
					m_pBoneInfo[i].SetParent(j, &m_pBoneInfo[j]);
					break;
				}
			}
		}
	}

	for(i = 0; i < m_boneCount; ++i )
		m_pBoneInfo[i].ComputeBoneInfo();

	for(i = 0; i < m_boneCount; ++i )
		m_pBoneInfo[i].ComputeInitialTransform();

	if( m_boneCount > 0 )
		m_pWriter->begin( IDTF_MODEL_SKELETON );
	for(i = 0; i < m_boneCount; ++i )
	{
		m_pWriter->begin( IDTF_BONE, i );
		m_pWriter->output( IDTF_BONE_NAME, m_pBoneInfo[i].GetNode()->GetName() );
		if( !IsRootBone( m_pBoneInfo[i].GetNode() ) )
			m_pWriter->output( IDTF_PARENT_BONE_NAME, m_pBoneInfo[i].GetParentNode()->GetName() );
		else
			m_pWriter->output( IDTF_PARENT_BONE_NAME, "<unnamed>" );
		F32 len = 0;
		Point3 disp(0, 0, 0);
		Quat rot(0.0, 0.0 ,0.0 ,0.0);
		m_pBoneInfo[i].GetLength( &len );
		m_pBoneInfo[i].GetDisp( &disp );
		m_pBoneInfo[i].GetRot( &rot );
		m_pWriter->setPrecision( 1 );
		m_pWriter->output( IDTF_BONE_LENGTH, len );
		m_pWriter->output( IDTF_BONE_DISPLACEMENT, &disp );
		//		m_pWriter->output( IDTF_BONE_ORIENTATION, &rot );
		m_pWriter->output( IDTF_BONE_ORIENTATION, rot.w );
		m_pWriter->setPlain( TRUE );
		m_pWriter->output( L"", rot.x );
		m_pWriter->output( L"", rot.y );
		m_pWriter->output( L"", rot.z );
		m_pWriter->setPlain( FALSE );
		m_pWriter->restorePrecision();
		m_pWriter->end();
	}
	if( m_boneCount > 0 )
		m_pWriter->end();
	delete[] m_pBoneInfo;

	pBipedUtil->BipedOutOfReferenceMode();
	pBipedUtil->BipedReturnNonUniformScale();
	pBipedUtil->BipedExportComplete();
}

//------------- GLOBAL FUNCTIONS --------------------------------------------

//------------- LOCAL FUNCTIONS ---------------------------------------------

// Determine is the node has negative scaling.
// This is used for mirrored objects for example. They have a negative scale factor
// so when calculating the normal we should take the vertices counter clockwise.
// If we don't compensate for this the objects will be 'inverted'.
BOOL getTMNegParity( Matrix3& m )
{
	return (DotProd(CrossProd(m.GetRow(0),m.GetRow(1)),m.GetRow(2))<0.0)?1:0;
}

Point3 getVertexNormal( Mesh* pMesh, U32 faceNo, U32 vert )
{
	Point3 vertexNormal;
	if( !pMesh )
		return vertexNormal;
	Face*  pF = &pMesh->faces[faceNo];
	DWORD  smGroup = pF->smGroup;
	U32    numNormals;
	RVertex* pRv = pMesh->getRVertPtr( vert );

	// Is normal specified
	// SPECIFIED is not currently used, but may be used in future versions.
	if( pRv->rFlags & SPECIFIED_NORMAL )
	{
		vertexNormal = pRv->rn.getNormal();
	}
	// If normal is not specified it's only available if the face belongs
	// to a smoothing group
	else if( (numNormals = pRv->rFlags & NORCT_MASK) && smGroup )
	{
		// If there is only one vertex is found in the rn member.
		if( numNormals == 1 )
		{
			vertexNormal = pRv->rn.getNormal();
		}
		else
		{
			// If two or more vertices are there you need to step through them
			// and find the vertex with the same smoothing group as the current face.
			// You will find multiple normals in the ern member.
			for( U32 i = 0; i < numNormals; i++ )
			{
				if( pRv->ern[i].getSmGroup() & smGroup )
				{
					vertexNormal = pRv->ern[i].getNormal();
				}
			}
		}
	}
	else
	{
		// Get the normal from the Face if no smoothing groups are there
		vertexNormal = pMesh->getFaceNormal(faceNo);
	}

	return vertexNormal;
}

void IFXGeomObjectExporter::RecordTotalShdrMapUsage(Mtl* pMtl, U32 *pShaderMapTable, U32 *pShaderUseTable)
{
	if (pMtl && pShaderMapTable && pShaderUseTable )
	{
		if(pMtl->ClassID()==Class_ID(MULTI_CLASS_ID,0))
		{
			U32 numSubMtls = pMtl->NumSubMtls();

			// Determine the texture layer usage in all shaders used by this model
			for (U32 s = 0; s < numSubMtls; s++)
			{
				Mtl* subMtl = pMtl->GetSubMtl(s);
				if(subMtl)
				{
					CountShdrMapUsage(subMtl, pShaderMapTable, pShaderUseTable);
				}
				else
				{
					assert(0);
				}
			} // for all submaterials
		}
		else
		{
			CountShdrMapUsage(pMtl, pShaderMapTable, pShaderUseTable);
		}
	}
}

// pShaderMapTable - tells what texture coordinate layers are available on the model
// pShaderUseTable - tells which of those layers are actually used by the shaders
//                   currently attached to the model.
U32 IFXGeomObjectExporter::CountShdrMapUsage(Mtl* pMtl, U32 *pShaderMapTable, U32 *pShaderUseTable)
{
	if( !pMtl || !pShaderMapTable || !pShaderUseTable )
		return 0;
	U32   i;
	U32   numLayers = 0;
	U32   shaderSeen[MAX_MESHMAPS];
	Texmap *pTx = NULL;

	for (i = 0; i < MAX_MESHMAPS; i++)  shaderSeen[i] = 0;

	for( U32 mapIndex = 0; mapIndex < 12; mapIndex++ )//all map channels
		if ( pMtl->SubTexmapOn(mapIndex) && (pTx = pMtl->GetSubTexmap(mapIndex)) )
		{
			UVGen*  pUVGen = pTx->GetTheUVGen();
			if( pUVGen )
			{
				U32     mapChannel = pUVGen->GetMapChannel();

				if (pShaderMapTable[mapChannel] >= 0)
				{
					if (0 == shaderSeen[mapChannel])
					{
						numLayers++;
						shaderSeen[mapChannel] = 1;
						pShaderUseTable[mapChannel] = 1;
					}
				}  // A map channel is defined on the mesh
			}
		}


		return numLayers;
}
