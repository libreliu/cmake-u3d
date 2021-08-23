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

#include "NodeExporter.h"

#include "ObjectExporter.h" // needed for makeObjExporter
#include "GeomObjectExporter.h"
#include "CameraExporter.h"
#include "LightExporter.h"
#include "ShaderExporter.h"

//------------- DEFINES -----------------------------------------------------

//------------- CONSTANTS ---------------------------------------------------

//------------- ENUMERATIONS ------------------------------------------------

//------------- CLASSES, STRUCTURES AND TYPES -------------------------------

//------------- LOCAL FUNCTION PROTOTYPES -----------------------------------

IFXObjectExporter* makeObjExporter( IFXNodeExporter* );
void fixupMatrix( Matrix3 &Matrix );

//------------- PUBLIC METHODS ----------------------------------------------

// destructor
IFXNodeExporter::~IFXNodeExporter()
{
	m_pData = 0;
	m_pName = 0;
	m_pParentName = 0;
	m_pType = 0;
}

// constructor
IFXNodeExporter::IFXNodeExporter( IFXNodeData* pData ): m_pData( pData )
{
}

// export node
IFXRESULT IFXNodeExporter::exportNode( U32 node )
{
	IFXRESULT ec = IFX_OK;
	IFXObjectExporter* objExp = makeObjExporter( this );
	EIFXNodeType idx = getEnumNodeType();
	TCHAR* nodetype;
	switch( idx )
	{
	case IFXNodeTypeModel: nodetype = "MODEL";
		break;
	case IFXNodeTypeLight: nodetype = "LIGHT";
		break;
	case IFXNodeTypeView: nodetype = "VIEW";
		break;
	case IFXNodeTypeGroup: nodetype = "GROUP";
		break;
	default: nodetype = "UNKNOWN";
		return 0;
	}
	getWriter()->begin( IDTF_NODE, nodetype, true );
	ec = exportNodeHeader();
	if( IFXSUCCESS( ec ) )
	{
		if( idx != IFXNodeTypeGroup )
		{
			IFXCHAR nts[MAX_TOKEN_LEN] = {0};
			swprintf( nts, L"%ld", node );
			IFXCHAR resourceName[MAX_TOKEN_LEN];
			wcscpy( resourceName, IDTF_RESOURCE );
			wcscat( resourceName, L"_" );
			wcscat( resourceName, nts );
			getWriter()->output( IDTF_RESOURCE_NAME, resourceName );
		}
		if( objExp != 0 )
			ec = objExp->exportNode();
		getWriter()->end();
	}
	else
		ec = IFX_E_NOT_DONE;
	delete objExp;
	objExp = 0;
	return ec;
}

// export resource
IFXRESULT IFXNodeExporter::exportResource(U32 node)
{
	IFXRESULT ec = IFX_OK;
	IFXObjectExporter* pObjExp = makeObjExporter( this );
	if(pObjExp != 0 )
	{
		EIFXNodeType idx = getEnumNodeType();
		if( node && node != idx )
			return ec;
		ec = pObjExp->exportResource( m_pData );
		delete pObjExp;
		pObjExp = NULL;
	}
	else
		ec = IFX_E_NOT_INITIALIZED;
	return ec;
}

//------------- PROTECTED METHODS -------------------------------------------

// export node header
IFXRESULT IFXNodeExporter::exportNodeHeader()
{
	IFXRESULT ec = IFX_OK;
	TCHAR* parentName;
	// write out node name
	getWriter()->output( IDTF_NODE_NAME, getNode()->GetName() );
	INode* pParent = getNode()->GetParentNode();
	getWriter()->begin( IDTF_PARENT_LIST );
	getWriter()->output( IDTF_PARENT_COUNT, 1 );
	if( pParent && !pParent->IsRootNode())
		parentName = pParent->GetName();
	else
		parentName = "<NULL>";
	getWriter()->begin( IDTF_PARENT );
	// write out parent info
	getWriter()->output( IDTF_PARENT_NAME, parentName );
	// write out node tm
	exportTM();
	getWriter()->end();
	getWriter()->end();
	return ec;
}

// export node TM
IFXRESULT IFXNodeExporter::exportTM()
{
	IFXRESULT ec = IFX_OK;
	Matrix3 ltm;
	Matrix3 ptm;
	ltm = getNode()->GetObjectTM(0);
	fixupMatrix(ltm);
	ptm = Inverse( getNode()->GetParentTM(0) );
	fixupMatrix(ptm);
	ltm = ltm * ptm;
	getWriter()->output( IDTF_PARENT_TM, &ltm );
	return ec;
}

IFXRESULT IFXNodeExporter::exportShadingModifier()
{
	IFXRESULT ec = IFX_OK;

	IFXShaderExporter* pShExp = m_pData->pShaderExporter;
	TCHAR* pName;
	if( getNode()->GetMtl() )
		pName = pShExp->getNodeName( pShExp->getShaderID( getNode()->GetMtl() ) );
	else
		pName = getNode()->GetName();
	if( getEnumNodeType() == IFXNodeTypeModel )
	{
		U32 shaderListCount = 1;
		TCHAR ids[MAX_TOKEN_LEN];
		Mtl* pMtl = getNode()->GetMtl();
		if( pMtl && pMtl->NumSubMtls() > 0 )
			shaderListCount = pMtl->NumSubMtls();
		getWriter()->begin( IDTF_MODIFIER, "SHADING", true );
		getWriter()->output( IDTF_MODIFIER_NAME, getNode()->GetName() );
		getWriter()->output( IDTF_MODIFIER_CHAIN_INDEX, -1 );
		getWriter()->begin( IDTF_PARAMETERS );
		getWriter()->output( IDTF_SHADER_LIST_COUNT, shaderListCount );
		getWriter()->begin( IDTF_SHADER_LIST_LIST );
		for( U32 shaderListID = 0; shaderListID < shaderListCount; shaderListID++ )
		{
			getWriter()->begin( IDTF_SHADER_LIST, shaderListID );
			getWriter()->output( IDTF_SHADER_COUNT, 1 );
			getWriter()->begin( IDTF_SHADER_NAME_LIST );
			getWriter()->output( IDTF_SHADER, 0 );
			getWriter()->setPlain( true );
			TCHAR nodeName[MAX_TOKEN_LEN];
			strcpy( nodeName, pName );
			_itoa( shaderListID, ids, 10 );
			strcat( nodeName, ids );
			getWriter()->output( IDTF_SHADER_NAME, nodeName );
			getWriter()->setPlain( false );
			getWriter()->end();
			getWriter()->end();
		}
		getWriter()->end();
		getWriter()->end();
		getWriter()->end();
	}
	return ec;
}

//------------- GLOBAL FUNCTIONS --------------------------------------------

//------------- LOCAL FUNCTIONS ---------------------------------------------

/**
* @brief Zeros any values below epsilon.
* @param Matrix3& Matrix   The matrix to fix.
* @return  void
*/
void fixupMatrix( Matrix3 &Matrix )
{
	for( U32 i=0; i < 4; i++ )
	{
		Point3 row = Matrix.GetRow(i);
		U32    changedOne = 0;

		for( int j = 0; j < 3; j++ )
		{
			if( fabs(row[j]) < EPSILON )
			{
				row[j] = 0.0f;
				changedOne = 1;
			}
		}

		if( changedOne == 1 )
			Matrix.SetRow( i, row );
	}
}

/**
@brief  Factory functions. Makes ObjectExporter.
@param  IFXNodeExporter* pNodeExp    The Node an ObjectExporter is made for.
@return IFXObjectExporter*
*/
IFXObjectExporter* makeObjExporter( IFXNodeExporter* pNodeExp )
{
	IFXObjectExporter* pOe = NULL;
	Object* pObj = pNodeExp->getNode()->EvalWorldState(0).obj;

#ifndef SHOW_BONES
	if( pObj->ClassID() != BONE_OBJ_CLASSID )
#endif

	if( pObj != NULL )
		//to prevent bones export as meshes
	{
		// We look at the super class ID to determine the type of the object.
		switch( pObj->SuperClassID() )
		{
		case GEOMOBJECT_CLASS_ID:
			{
				// Targets are actually geomobjects.
				// We will not export them from the camera and light objects.
				pOe = new IFXGeomObjectExporter( pNodeExp );
				break;
			}
		case LIGHT_CLASS_ID:
			pOe = new IFXLightExporter( pNodeExp );
			break;

		case CAMERA_CLASS_ID:
			pOe = new IFXCameraExporter( pNodeExp );
			break;
		default:
			break;
		}
	}

	return pOe;
}

/**
@brief  Node type
@return EIFXNodeType
*/
EIFXNodeType IFXNodeExporter::getEnumNodeType()
{
	Object* pObj = getNode()->EvalWorldState(0).obj;

	if( pObj != NULL )
	{
		// We look at the super class ID to determine the type of the object.
		switch( pObj->SuperClassID() )
		{
		case GEOMOBJECT_CLASS_ID:
			return IFXNodeTypeModel;
		case LIGHT_CLASS_ID:
			return IFXNodeTypeLight;
		case CAMERA_CLASS_ID:
			return IFXNodeTypeView;
		case HELPER_CLASS_ID:
			// group
			if( pObj->ClassID() == Class_ID(DUMMY_CLASS_ID, 0) )
				return IFXNodeTypeGroup;
			return IFXNodeTypeUnknown;
		default:
			return IFXNodeTypeUnknown;
		}
	}

	return IFXNodeTypeUnknown;
}