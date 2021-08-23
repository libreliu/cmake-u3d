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

#include "ShaderExporter.h"

//------------- DEFINES -----------------------------------------------------

//------------- CONSTANTS ---------------------------------------------------

//------------- ENUMERATIONS ------------------------------------------------

//------------- CLASSES, STRUCTURES AND TYPES -------------------------------

//------------- LOCAL FUNCTION PROTOTYPES -----------------------------------

//------------- PUBLIC METHODS ----------------------------------------------

// destructor
IFXShaderExporter::~IFXShaderExporter()
{
	m_pMaterial = 0;
}

// constructor
IFXShaderExporter::IFXShaderExporter()
{
}

// initialization
void IFXShaderExporter::Initialize( IFXWriter* pWriter )
{
	m_pWriter = pWriter;
}

IFXRESULT IFXShaderExporter::export()
{
	IFXRESULT ec = IFX_OK;
	IFXRESULT ecShader = IFX_OK;
	TCHAR nts[MAX_TOKEN_LEN];
	TCHAR resourceName[MAX_TOKEN_LEN];
	if( getShaderCount() > 0 )//materials export
	{
		m_pWriter->begin( IDTF_RESOURCE_LIST, IDTF_MATERIAL, true );
		m_pWriter->output( IDTF_RESOURCE_COUNT, getTotalMatCount() );
		U32 matNum = 0;
		for( U32 shaderID = 0; shaderID < getShaderCount(); ++shaderID )
		{
			m_pMaterial = m_mtlTab[shaderID];

			if( m_mtlTab[shaderID] && m_mtlTab[shaderID]->NumSubMtls() > 0 )
			{
				Mtl* pRoot = m_pMaterial;
				for( U32 i = 0; i < m_mtlTab[shaderID]->NumSubMtls() && ecShader == IFX_OK; i++ )
				{
					m_pWriter->begin( IDTF_RESOURCE, matNum++ );
					_itoa( i, nts, 10 );
					strcpy( resourceName, getNodeName( shaderID ) );
					strcat( resourceName, nts );
					m_pWriter->output( IDTF_RESOURCE_NAME, resourceName );

					m_pMaterial = pRoot->GetSubMtl( i );
					ecShader = exportMaterial( shaderID );
					m_pWriter->end();
				}
			}
			else
			{
				m_pWriter->begin( IDTF_RESOURCE, matNum++ );
				strcpy( resourceName, getNodeName( shaderID ) );
				strcat( resourceName, "0" );
				m_pWriter->output( IDTF_RESOURCE_NAME, resourceName );
				ecShader = exportMaterial( shaderID );
				m_pWriter->end();
			}
			if( ecShader != IFX_OK )
				ec = ecShader;
		}
		m_pWriter->end();
	}

	U32 totalTexNum = getTotalTextureResourceCount();
	if(  totalTexNum > 0 )//texture export
	{
		m_pWriter->begin( IDTF_RESOURCE_LIST, IDTF_TEXTURE, true );
		m_pWriter->output( IDTF_RESOURCE_COUNT, totalTexNum );	
		for( U32 textureID = 0; textureID < m_texTab.Count(); ++textureID )
			ecShader = exportTextureResource( textureID );
		m_pWriter->end();
	}

	if( getShaderCount() > 0 )//shader export
	{
		m_pWriter->begin( IDTF_RESOURCE_LIST, IDTF_SHADER, true );
		m_pWriter->output( IDTF_RESOURCE_COUNT, getTotalMatCount() );
		U32 matNum = 0;
		for( U32 shaderID = 0; shaderID < getShaderCount(); ++shaderID )
		{
			m_pMaterial = m_mtlTab[shaderID];

			if( m_mtlTab[shaderID] && m_mtlTab[shaderID]->NumSubMtls() > 0 )
			{
				Mtl* pRoot = m_pMaterial;
				ecShader = IFX_OK;
				for( U32 i = 0; i < m_mtlTab[shaderID]->NumSubMtls() && ecShader == IFX_OK; i++ )
				{
					m_pMaterial = pRoot->GetSubMtl( i );
					ecShader = exportTextureShader( matNum, shaderID, i );
				}
			}
			else
				ecShader = exportTextureShader( matNum, shaderID, 0 );
			if( ecShader != IFX_OK )
				ec = ecShader;
		}
		m_pWriter->end();
	}
	return ec;
}

//------------- PROTECTED METHODS -------------------------------------------

// export material data
IFXRESULT IFXShaderExporter::countMaterialTextureResource( Mtl* pMtl )
{
	IFXRESULT ec = IFX_OK;
	U32 i, j;

	if( pMtl != NULL )
	{
		U32 num = pMtl->NumSubTexmaps();
		// do not export sub-texture maps for multi-material
		if( num > 0 )
		{
			// sub-texture maps
			for( i = 0; i < num; ++i )
			{
				Texmap* pSubTex = pMtl->GetSubTexmap(i);
				F32 amount = 1.0f;
				if( pSubTex != NULL && pSubTex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00) )
				{
					// If it is a standard material we can see if the map is enabled.
					if( pMtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0) )
					{
						if( !((StdMat*)pMtl)->MapEnabled(i) )
							continue;
						amount = ((StdMat*)pMtl)->GetTexmapAmt(i, 0);
					}

					TSTR mapName = ((BitmapTex *)pSubTex)->GetMapName();
					for( j = 0; j < m_texTab.Count() && strcmp( m_texTab[j]->GetMapName(), mapName ); j++ );
					if( j < m_texTab.Count() ) continue;
					BitmapTex* pBmTex = (BitmapTex*)pSubTex;
					m_texTab.Append( 1, &pBmTex, 25 );
					m_mtlTexInd.Append( 1, &pMtl, 25 );
				}
			}
		}
	}
	else
		ec = IFX_E_NOT_INITIALIZED;
	return ec;
}

U32 IFXShaderExporter::getTotalTextureResourceCount()
{
	U32 result = 0;
	for( U32 shaderID = 0; shaderID < getShaderCount(); ++shaderID )
	{
		for( U32 i = 0; m_mtlTab[shaderID] && i < m_mtlTab[shaderID]->NumSubMtls(); i++ )
			countMaterialTextureResource( m_mtlTab[shaderID]->GetSubMtl( i ) );
		countMaterialTextureResource( m_mtlTab[shaderID] );
	}
	return m_texTab.Count();
}

// export common material data
IFXRESULT IFXShaderExporter::exportMaterial( U32 shaderID )
{
	IFXRESULT ec = IFX_OK;

	if( m_pMaterial != NULL )
	{
		// Note about material colors:
		// This is only the color used by the interactive renderer in MAX.
		// To get the color used by the scanline renderer, we need to evaluate
		// the material using the mtl->Shade() method.
		// Since the materials are procedural there is no real "diffuse" color for a MAX material
		// but we can at least take the interactive color.
		m_pWriter->output( IDTF_MATERIAL_AMBIENT, &m_pMaterial->GetAmbient() );
		m_pWriter->output( IDTF_MATERIAL_DIFFUSE, &m_pMaterial->GetDiffuse() );
		m_pWriter->output( IDTF_MATERIAL_SPECULAR, &m_pMaterial->GetSpecular() );
		m_pWriter->output( IDTF_MATERIAL_EMISSIVE, &m_pMaterial->GetSelfIllumColor() );
		F32 reflectivity = 0;
		if( m_pMaterial->GetShinStr() > 0 )
			reflectivity = m_pMaterial->GetShininess();//m_pMaterial->GetShinStr();
		m_pWriter->output( IDTF_MATERIAL_REFLECTIVITY, reflectivity );/// @todo: check it
		m_pWriter->output( IDTF_MATERIAL_OPACITY, 1-m_pMaterial->GetXParency() );
	}
	else
		//export default material
	{
		U32 dif = getNode( shaderID )->GetWireColor();
		m_pWriter->output( IDTF_MATERIAL_AMBIENT, &Color( 0.18, 0.06, 0.06 ) );
		m_pWriter->output( IDTF_MATERIAL_DIFFUSE, &Color(
			GetRValue( dif )/255.0f, GetGValue( dif )/255.0f, GetBValue( dif )/255.0f ) );
		m_pWriter->output( IDTF_MATERIAL_SPECULAR, &Color( 0.32, 0.32, 0.32 ) );
		m_pWriter->output( IDTF_MATERIAL_EMISSIVE, &Color( 0, 0, 0 ) );
		m_pWriter->output( IDTF_MATERIAL_REFLECTIVITY, (F32)0.225 );
		m_pWriter->output( IDTF_MATERIAL_OPACITY, (F32)1.0 );
	}

	return ec;
}

// export texture resource
IFXRESULT IFXShaderExporter::exportTextureResource( U32 texNum )
{
	IFXRESULT ec = IFX_OK;
	m_pWriter->begin( IDTF_RESOURCE, texNum );
	TSTR mapName = m_texTab[texNum]->GetMapName();
	char drive[MAX_TOKEN_LEN];
	char dir[MAX_TOKEN_LEN];
	char name[MAX_TOKEN_LEN];
	char ext[MAX_TOKEN_LEN];
	_splitpath( mapName, drive, dir, name, ext );
	char texName[MAX_TOKEN_LEN];
	strcpy( texName, name );
	strcat( texName, ext );
	m_pWriter->output( IDTF_RESOURCE_NAME, texName );
	FILE* pTexFile = fopen( mapName, "r" );
	if(  pTexFile )
	{
		m_pWriter->output( IDTF_TEXTURE_PATH, mapName );
		fclose( pTexFile );
	}
	else
		m_pWriter->output( IDTF_TEXTURE_PATH, texName );
	m_pWriter->end();
	return ec;
}

IFXRESULT IFXShaderExporter::exportTextureShader( U32& texNum, U32 shaderID, U32 subMtl )
{
	IFXRESULT ec = IFX_OK;
	U32 i;
	TCHAR resourceName[MAX_TOKEN_LEN];
	TCHAR nts[MAX_TOKEN_LEN];
	U32 texLayer = 0;

	m_pWriter->begin( IDTF_RESOURCE, texNum++ );
	strcpy( resourceName, getNodeName( shaderID ) );
	_itoa( subMtl, nts, 10 );
	strcpy( resourceName, getNodeName( shaderID ) );
	strcat( resourceName, nts );
	m_pWriter->output( IDTF_RESOURCE_NAME, resourceName );
	m_pWriter->output( IDTF_SHADER_MATERIAL_NAME, resourceName );
	m_pWriter->output( IDTF_SHADER_ACTIVE_TEXTURE_COUNT, getActiveTextureCount( m_pMaterial ) );
	if( getActiveTextureCount( m_pMaterial ) > 0 )
	{
		m_pWriter->begin( IDTF_SHADER_TEXTURE_LAYER_LIST );
		U32 num = m_pMaterial->NumSubTexmaps();
		// do not export sub-texture maps for multi-material
		for( i = 0; i < num; ++i )
		{
			Texmap* pSubTex = m_pMaterial->GetSubTexmap(i);
			F32 amount = 1.0f;
			if( pSubTex != NULL && pSubTex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00) )
			{
				// If it is a standard material we can see if the map is enabled.
				if( m_pMaterial->ClassID() == Class_ID(DMTL_CLASS_ID, 0) )
				{
					if( !((StdMat*)m_pMaterial)->MapEnabled(i) )
						continue;
					amount = ((StdMat*)m_pMaterial)->GetTexmapAmt(i, 0);
				}
				m_pWriter->begin( IDTF_TEXTURE_LAYER, texLayer++ );
				TSTR mapName = ((BitmapTex*)pSubTex)->GetMapName();
				char drive[MAX_TOKEN_LEN];
				char dir[MAX_TOKEN_LEN];
				char name[MAX_TOKEN_LEN];
				char ext[MAX_TOKEN_LEN];
				_splitpath( mapName, drive, dir, name, ext );
				char texName[MAX_TOKEN_LEN];
				strcpy( texName, name );
				strcat( texName, ext );
				m_pWriter->output( IDTF_TEXTURE_LAYER_INTENSITY, amount );
				m_pWriter->output( IDTF_TEXTURE_NAME, texName );
				m_pWriter->end();
			}
		}
		m_pWriter->end();
	}
	m_pWriter->end();
	return ec;
}

void scan_and_replace( TCHAR* pBuf, TCHAR src, TCHAR dst )
{
	while( *pBuf != _T('\0') )
	{
		if( *pBuf == src ) *pBuf = dst;
		pBuf = _tcsinc( pBuf );
	}
}

//shader palette methods
/**
Add shader to shader palette
*/
BOOL IFXShaderExporter::addShader( INode* pNode )
{
	if( pNode && pNode->EvalWorldState(0).obj && pNode->EvalWorldState(0).obj->SuperClassID() == GEOMOBJECT_CLASS_ID )
	{
		m_nodeTab.Append( 1, &pNode, 25 );
		U32 mtlInd = getShaderID( pNode->GetMtl() );
		if( mtlInd == (U32)-1 )// new material
		{
			U32 nextInd = (U32)m_mtlTab.Count();
			Mtl* pMtl = pNode->GetMtl();
			m_mtlInd.Append( 1, &nextInd, 25 );
			m_mtlTab.Append( 1, &pMtl, 25 );
		}
		else
		{
			m_mtlInd.Append( 1, &mtlInd, 25 );
		}
	}

	return TRUE;
}

/**
get shader ID
*/
U32 IFXShaderExporter::getShaderID( const Mtl* pMtl ) const
{
	if( pMtl )
	{
		for( U32 i = 0; i < m_mtlTab.Count(); ++i )
		{
			if( m_mtlTab[i] == pMtl )
				return i;
		}
	}
	return -1;
}

U32 IFXShaderExporter::getShaderID( const U32 nodeInd ) const
{
	U32 result = -1;
	if( nodeInd < m_mtlTab.Count() )
		result = m_mtlInd[nodeInd];
	return result;
}

/**
get shader number
*/
U32 IFXShaderExporter::getShaderCount() const
{
	return m_mtlTab.Count();
}

/**
get total material number
*/
U32 IFXShaderExporter::getTotalMatCount() const
{
	U32 numMtls = m_mtlTab.Count();
	U32 cnt = 0;
	for( U32 i = 0; i < numMtls; ++i )
	{
		if( m_mtlTab[i] && m_mtlTab[i]->NumSubMtls() > 0 )
			cnt += m_mtlTab[i]->NumSubMtls();
		else
			cnt++;
	}
	return cnt;
}

/**
get shader
*/
Mtl* IFXShaderExporter::getShader( U32 id ) const
{
	return m_mtlTab[m_mtlInd[id]];
}

/**
get node name
*/
TCHAR* IFXShaderExporter::getNodeName( U32 id ) const
{
	TCHAR* pName = NULL;
	for( U32 i = 0; i < m_mtlInd.Count(); i++ )
	{
		if( m_mtlInd[i] == id )
		{
			pName = m_nodeTab[i]->GetName();
			break;
		}
	}
	return pName;
}

/**
get node
*/
INode* IFXShaderExporter::getNode( U32 id ) const
{
	return m_nodeTab[id];
}




//------------- PRIVATE METHODS ---------------------------------------------

//------------- GLOBAL FUNCTIONS --------------------------------------------
U32 getActiveTextureCount( Mtl* pMtl )
{
	U32 result = 0;
	if( pMtl )
	{
		U32 num = pMtl->NumSubTexmaps();
		// sub-texture maps
		for( U32 i = 0; i < num; ++i )
		{
			Texmap* pSubTex = pMtl->GetSubTexmap(i);
			if( pSubTex != NULL && pSubTex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00) &&
				( pMtl->ClassID() != Class_ID(DMTL_CLASS_ID, 0) || ((StdMat*)pMtl)->MapEnabled(i) ) )
					result++;
		}
	}
	return result;
}

//------------- LOCAL FUNCTIONS ---------------------------------------------