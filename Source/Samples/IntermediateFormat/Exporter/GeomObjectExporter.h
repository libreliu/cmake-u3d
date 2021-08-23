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
#ifndef __GEOMOBJEXPORTER_H__
#define __GEOMOBJEXPORTER_H__

//------------- INCLUDES ----------------------------------------------------

#include "ObjectExporter.h"
#include "BonesExporter.h"
#include "BonesUtil.h"

//------------- DEFINES -----------------------------------------------------

//------------- CONSTANTS ---------------------------------------------------

//------------- ENUMERATIONS ------------------------------------------------

//------------- CLASS, STRUCTURE AND TYPES ----------------------------------

/**
*  IFXGeomObjectExporter class is used to export GeometryObjects
*/
class IFXGeomObjectExporter : public IFXObjectExporter
{
public:
	virtual ~IFXGeomObjectExporter();
	IFXGeomObjectExporter( const IFXNodeExporter* );

	IFXRESULT IFXAPI exportNode();
	IFXRESULT IFXAPI exportResource( const IFXNodeData* pData );
	virtual const IFXCHAR* getNodeType() const { return IDTF_MODEL; }

protected:
	IFXGeomObjectExporter() {};

private:
	IFXGeomObjectExporter( const IFXGeomObjectExporter& ) {};
	IFXGeomObjectExporter& operator= ( const IFXGeomObjectExporter& ) {};

	void      exportParams( Mesh*, Mtl* );
	void      exportShadingDescription( Mesh* );
	void      exportFacePositionList( Mesh*, U32* );
	void      exportFaceNormalList( Mesh* );
	void      exportMeshFaceShadingList( Mesh* );
	void      exportMeshFaceTextureLayerList( Mesh*, U32* vo );
	void      exportMeshFaceDiffuseColorList( Mesh*, U32* vo );
	void      exportMeshFaceSpecularColorList( Mesh* );
	void      exportMeshPositionList( Mesh*, Matrix3& );
	void      exportMeshNormalList( Mesh*, U32* );
	void      exportMeshDiffuseColorList( Mesh* );
	void      exportMeshSpecularColorList( Mesh* );
	void      exportModelTextureCoordList( Mtl*, Mesh* );
	void      exportModelSkeleton( BipedUtil* pBipedUtil );

	void      makeFaceUV( Face*, Point3* );

	U32	CountShdrMapUsage(Mtl*, U32*, U32*);
	void RecordTotalShdrMapUsage(Mtl*, U32*, U32*);
//	void CollectBones( INode* pNode );

	Object* m_pObj;
	U32* m_pShaderMapTable;
	U32* m_pShaderUseTable;
	BOOL m_isModelTextured;

	U32 m_boneCount;
	INode** m_ppBoneList;
	IFXBonesExporter* m_pBoneInfo;
}; 


//------------- GLOBAL FUNCTION PROTOTYPES ----------------------------------

//------------- GLOBAL DATA -------------------------------------------------

#endif