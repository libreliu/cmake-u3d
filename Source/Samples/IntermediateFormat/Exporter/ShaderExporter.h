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
#ifndef __IFXSHADEREXPORTER_H__
#define __IFXSHADEREXPORTER_H__

//------------- INCLUDES ----------------------------------------------------

#include "Max.h" // INode
#include "IFXCOM.h"
#include "Writer.h"
#include "Common.h"
#include "Tokens.h"
#include "StdMat.h"


//------------- DEFINES -----------------------------------------------------

//------------- CONSTANTS ---------------------------------------------------

//------------- ENUMERATIONS ------------------------------------------------

//------------- CLASS, STRUCTURE AND TYPES ----------------------------------

/**
*  IFXShaderExporter class is used to export Materials
*/

class IFXShaderExporter
{
public:
	virtual ~IFXShaderExporter();
	IFXShaderExporter();

	void Initialize( IFXWriter* writer );
	IFXRESULT export();
	BOOL addShader( INode* pNode );
	Mtl* getShader( U32 id ) const;
	TCHAR* getNodeName( U32 id ) const;
	U32  getShaderID( const Mtl* pMtl ) const;
	U32 getShaderID( const U32 nodeInd ) const;
	U32  getShaderCount() const;
	INode* getNode( U32 id ) const;
	U32 getTotalMatCount() const;

protected:
	IFXShaderExporter( const IFXShaderExporter& ) {};
	IFXShaderExporter& operator= ( const IFXShaderExporter& ) {};

	IFXRESULT exportMaterial( U32 shaderID );
	IFXRESULT exportTextureResource( U32 texNum );
	IFXRESULT exportTextureShader( U32& texNum, U32 shaderID, U32 subMtl );
	IFXRESULT countMaterialTextureResource( Mtl* pMtl );
	U32 getTotalTextureResourceCount();

private:
	IFXWriter* m_pWriter;
	Mtl* m_pMaterial;

	Tab<Mtl*> m_mtlTab;// shaders tab
	Tab<INode*> m_nodeTab;// shader indices for node tab
	Tab<U32> m_mtlInd;//nodes tab
	Tab<BitmapTex*> m_texTab;//texture resources tab
	Tab<Mtl*> m_mtlTexInd;//textures indices for shaders tab
};

//------------- GLOBAL FUNCTION PROTOTYPES ----------------------------------
U32 getActiveTextureCount( Mtl* pMtl );

//------------- GLOBAL DATA -------------------------------------------------

#endif