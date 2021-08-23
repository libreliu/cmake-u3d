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

/**
	@file	Exporter.cpp

			This module contains the IFXExporter class.

	@note	As a starting point, portions of this module were based on the 
			Discreet* 3ds max* SDK sample exporters and may contain 
			materials owned by Autodesk*, Inc.  Copyright 1994 - 2004.

			* Other names and brands may be claimed as the property of 
			  others.
*/

//------------- INCLUDES ----------------------------------------------------
#include "Common.h"
#include "Exporter.h"
#include "ExporterPlugin.h" // for IDTFEXP_VER
#include "NodeExporter.h"
#include "AnimationExporter.h"
#include "BoneWeightModifierExporter.h"
#include "BonesUtil.h"

//------------- DEFINES -----------------------------------------------------

//------------- CONSTANTS ---------------------------------------------------

//------------- ENUMERATIONS ------------------------------------------------

//------------- CLASSES, STRUCTURES AND TYPES -------------------------------

//------------- LOCAL FUNCTION PROTOTYPES -----------------------------------

BOOL IsObjValid( ObjectState* pOs, INode* pNode );

//------------- PUBLIC METHODS ----------------------------------------------

IFXExporter::IFXExporter( const TCHAR* pFileName )
{
	m_pWriter = new IFXWriter( pFileName );
	m_pNodeCount = new U32[NODETYPE_NUM];
	m_pCurNodes = new U32[NODETYPE_NUM];
	for( U32 i = 0; i < NODETYPE_NUM; i++ )
	{
		m_pNodeCount[i] = 0;
		m_pCurNodes[i] = 0;
	}
	m_trackCount = 0;
	m_curTrack = 0;
	m_pShaderExporter = new IFXShaderExporter();
	m_pShaderExporter->Initialize( m_pWriter );
}

IFXExporter::~IFXExporter()
{
	delete m_pWriter;
	delete m_pNodeCount;
	delete m_pCurNodes;
	delete m_pShaderExporter;
}

BOOL IFXExporter::initialize( ExpInterface* pEi, Interface* pI )
{
	m_pEi = pEi;
	m_pI  = pI;
	m_pWriter->open();

	return true;
}

BOOL IFXExporter::finalize()
{
	m_pWriter->close();

	return true;
}

IFXRESULT IFXExporter::export()
{
	IFXRESULT ec = IFX_OK;

	// Get a total node count by traversing the scene
	U32 numNodes = 0;
	countNodes( m_pI->GetRootNode(), numNodes );

	// First we write out a file header
	exportHeaderInfo();

	// Export basic scene information.
	exportGlobalInfo();

	// get the number of child objects
	U32 numChildren = m_pI->GetRootNode()->NumberOfChildren();

	// Call our node enumerator.
	// The enumerateNode function will recurse into itself and
	// export each object found in the scene.
	for( U32 idx = 0; idx < numChildren; ++idx )
	{
		if( m_pI->GetCancel() == TRUE )
		{
			ec = IFX_E_ABORTED;
			break;
		}

		enumerateNode( m_pI->GetRootNode()->GetChildNode( idx ) );
	}

	for( U32 nt = 1; nt < NODETYPE_NUM-1; ++nt )//no resource for groups and for unknowns
	{
		if( m_pNodeCount[nt] )
		{
			startResourceList( nt, m_pNodeCount[nt] );
			for( U32 idx = 0; idx < numChildren; ++idx )
			{
				if( m_pI->GetCancel() == TRUE )
				{
					ec = IFX_E_ABORTED;
					break;
				}

				enumerateResource( m_pI->GetRootNode()->GetChildNode( idx ), nt );
			}
			endResourceList();
		}
	}

	// Export list of shader definitions
	m_pShaderExporter->export();

	if( m_trackCount > 0 )
	{
		m_pWriter->begin( IDTF_RESOURCE_LIST, "MOTION", true );
		m_pWriter->output( IDTF_RESOURCE_COUNT, m_trackCount );
	}
	for( U32 idx = 0; idx < numChildren; ++idx )
	{
		if( m_pI->GetCancel() == TRUE )
		{
			ec = IFX_E_ABORTED;
			break;
		}

		exportMotionResource( m_pI->GetRootNode()->GetChildNode( idx ) );
	}
	if( m_trackCount > 0 )
		m_pWriter->end();

	for( U32 idx = 0; idx < numChildren; ++idx )
	{
		if( m_pI->GetCancel() == TRUE )
		{
			ec = IFX_E_ABORTED;
			break;
		}

		exportShadingModifier( m_pI->GetRootNode()->GetChildNode( idx ) );
	}

	for( U32 idx = 0; idx < numChildren; ++idx )
	{
		if( m_pI->GetCancel() == TRUE )
		{
			ec = IFX_E_ABORTED;
			break;
		}

		exportAnimationModifier( m_pI->GetRootNode()->GetChildNode( idx ) );
	}

	for( U32 idx = 0; idx < numChildren; ++idx )
	{
		if( m_pI->GetCancel() == TRUE )
		{
			ec = IFX_E_ABORTED;
			break;
		}

		exportBoneWeightModifier( m_pI->GetRootNode()->GetChildNode( idx ) );
	}
	return ec;
}

//------------- PROTECTED METHODS -------------------------------------------

//------------- PRIVATE METHODS ---------------------------------------------
BOOL IFXExporter::exportHeaderInfo()
{
	// write IDTF File Header
	m_pWriter->output( IDTF_FORMAT, "IDTF" );
	m_pWriter->output( IDTF_VERSION, IDTF_FORMAT_VER );
	return TRUE;
}

void IFXExporter::exportGlobalInfo()
{
	Interval range = m_pI->GetAnimRange();

	m_pWriter->begin( IDTF_SCENE );
	/// @todo: metadata

	m_pWriter->end();
}


/**
* This method is the main object exporter.
* It is called once of every node in the scene. The objects are
* exported as they are encoutered.
*
* Before recursing into the children of a node, we will export it.
* The benefit of this is that a nodes parent is always before the
* children in the resulting file. This is desired since a child's
* transformation matrix is optionally relative to the parent.
*
* This method also counts the number of each type of resources and
* motion tracks
*/
BOOL IFXExporter::enumerateNode( INode* pNode )
{
	if( pNode == 0 )
		return FALSE;

	// if we're only exporting selected nodes, and
	// the given node isn't selected, skip it
	//if (m_exportSelected && node->Selected() == FALSE)
	//    return FALSE;

	// Stop recursing if the user pressed Cancel
	if( m_pI->GetCancel() )
		return FALSE;

	// The ObjectState is a 'thing' that flows down the pipeline containing
	// all information about the object. By calling EvalWorldState() we tell
	// max to eveluate the object at end of the pipeline.
	ObjectState os = pNode->EvalWorldState( m_pI->GetTime() );

	// The obj member of ObjectState is the actual object we will export.
	if( IsObjValid( &os, pNode ) )
		//to prevent bones export as meshes
	{
		IFXNodeData data;
		data.pNode = pNode;
		data.pWriter = m_pWriter;
		data.pShaderExporter = m_pShaderExporter;
		data.pAnimInterval = &m_pI->GetAnimRange();

		IFXNodeExporter nodeExporter( &data );
		nodeExporter.exportNode(m_pNodeCount[nodeExporter.getEnumNodeType()]);
		m_pNodeCount[nodeExporter.getEnumNodeType()]++;
		IFXAnimationExporter animCounter( &data );
		if( animCounter.checkForAnimation() )
			m_trackCount++;
	}

	// For each child of this node, we recurse into ourselves
	// until no more children are found.
	for( U32 c = 0; c < pNode->NumberOfChildren(); c++ )
	{
		if ( !enumerateNode( pNode->GetChildNode(c) ) )
			return FALSE;
	}

	return TRUE;
}

BOOL IFXExporter::enumerateResource( INode* pNode, U32 nodeType ) const
{
	if( pNode == 0 )
		return FALSE;

	if( m_pI->GetCancel() )
		return FALSE;

	ObjectState os = pNode->EvalWorldState( m_pI->GetTime() );
	if( IsObjValid( &os, pNode ) )
	{
		IFXNodeData data;
		data.pNode = pNode;
		data.pWriter = m_pWriter;
		data.pShaderExporter = m_pShaderExporter;
		data.pAnimInterval = &m_pI->GetAnimRange();

		IFXNodeExporter nodeExporter( &data );
		if( nodeExporter.getEnumNodeType() == nodeType )
		{
			m_pWriter->begin( IDTF_RESOURCE, m_pCurNodes[nodeType] );
			IFXCHAR nts[MAX_TOKEN_LEN];
			swprintf( nts, L"%d", m_pCurNodes[nodeType] );
			IFXCHAR resourceName[MAX_TOKEN_LEN];
			wcscpy( resourceName, IDTF_RESOURCE );
			wcscat( resourceName, L"_" );
			wcscat( resourceName, nts );
			m_pWriter->output( IDTF_RESOURCE_NAME, resourceName );
			nodeExporter.exportResource( nodeType );
			m_pWriter->end();
			m_pCurNodes[nodeType]++;
		}
	}

	for( U32 c = 0; c < pNode->NumberOfChildren(); c++ )
	{
		if ( !enumerateResource( pNode->GetChildNode(c), nodeType ) )
			return FALSE;
	}

	return TRUE;
}

BOOL IFXExporter::exportShadingModifier( INode* pNode ) const
{
	if( pNode == 0 )
		return FALSE;

	if( m_pI->GetCancel() )
		return FALSE;

	ObjectState os = pNode->EvalWorldState( m_pI->GetTime() );
	if( IsObjValid( &os, pNode ) )
	{
		IFXNodeData data;
		data.pNode = pNode;
		data.pWriter = m_pWriter;
		data.pShaderExporter = m_pShaderExporter;
		data.pAnimInterval = &m_pI->GetAnimRange();

		IFXNodeExporter nodeExporter( &data );
		nodeExporter.exportShadingModifier();
	}

	for( U32 c = 0; c < pNode->NumberOfChildren(); c++ )
	{
		if ( !exportShadingModifier( pNode->GetChildNode(c) ) )
			return FALSE;
	}

	return TRUE;
}

BOOL IFXExporter::exportAnimationModifier( INode* pNode ) const
{
	if( pNode == 0 )
		return FALSE;

	if( m_pI->GetCancel() )
		return FALSE;

	ObjectState os = pNode->EvalWorldState( m_pI->GetTime() );
	if( IsObjValid( &os, pNode ) )
	{
		IFXNodeData data;
		data.pNode = pNode;
		data.pWriter = m_pWriter;
		data.pShaderExporter = m_pShaderExporter;
		data.pAnimInterval = &m_pI->GetAnimRange();

		IFXAnimationExporter animExporter( &data );
		animExporter.exportAnimationModifier();
	}

	for( U32 c = 0; c < pNode->NumberOfChildren(); c++ )
	{
		if ( !exportAnimationModifier( pNode->GetChildNode(c) ) )
			return FALSE;
	}

	return TRUE;
}

BOOL IFXExporter::exportBoneWeightModifier( INode* pNode ) const
{
	if( pNode == 0 )
		return FALSE;

	if( m_pI->GetCancel() )
		return FALSE;

	ObjectState os = pNode->EvalWorldState( m_pI->GetTime() );
	if( IsObjValid( &os, pNode ) )
	{
		IFXNodeData data;
		data.pNode = pNode;
		data.pWriter = m_pWriter;
		data.pShaderExporter = m_pShaderExporter;
		data.pAnimInterval = &m_pI->GetAnimRange();

		IFXBoneWeightModifierExporter bwmExporter( &data );
		bwmExporter.export();
	}

	for( U32 c = 0; c < pNode->NumberOfChildren(); c++ )
	{
		if ( !exportBoneWeightModifier( pNode->GetChildNode(c) ) )
			return FALSE;
	}

	return TRUE;
}

BOOL IFXExporter::exportMotionResource( INode* pNode )
{
	if( pNode == 0 )
		return FALSE;

	if( m_pI->GetCancel() )
		return FALSE;

	ObjectState os = pNode->EvalWorldState( m_pI->GetTime() );
	if( IsObjValid( &os, pNode ) )
	{
		IFXNodeData data;
		data.pNode = pNode;
		data.pWriter = m_pWriter;
		data.pShaderExporter = m_pShaderExporter;
		data.pAnimInterval = &m_pI->GetAnimRange();

		IFXAnimationExporter animExporter( &data );
		if( animExporter.checkForAnimation() )
		{
			m_pWriter->begin( IDTF_RESOURCE, m_curTrack++ );
			TCHAR resourceName[MAX_TOKEN_LEN];
			strcpy( resourceName, pNode->GetName() );
			strcat( resourceName, "-Key" );
			m_pWriter->output( IDTF_RESOURCE_NAME, resourceName );
			animExporter.export();
			m_pWriter->end();
		}
	}

	for( U32 c = 0; c < pNode->NumberOfChildren(); c++ )
	{
		if ( !exportMotionResource( pNode->GetChildNode(c) ) )
			return FALSE;
	}

	return TRUE;
}

void IFXExporter::countNodes( INode* pNode, U32& rNodeCount )
{
	rNodeCount++;

	// Add the node shader to out shader exporter
	// Null entries are ignored when added...
	m_pShaderExporter->addShader( pNode );

	// For each child of this node, we recurse into ourselves
	// and increment the counter until no more children are found.
	for( U32 c = 0; c < pNode->NumberOfChildren(); c++ )
	{
		countNodes( pNode->GetChildNode(c), rNodeCount );
	}
}

void IFXExporter::startResourceList( U32 nt, U32 nc )
{
	TCHAR* nts;
	switch( nt )
	{
	case IFXNodeTypeModel: nts = "MODEL";
		break;
	case IFXNodeTypeLight: nts = "LIGHT";
		break;
	case IFXNodeTypeView: nts = "VIEW";
		break;
	case IFXNodeTypeGroup: nts = "GROUP";
		break;
	default: nts = "UNKNOWN";
		break;
	}
	m_pWriter->begin( IDTF_RESOURCE_LIST, nts, true );
	m_pWriter->output( IDTF_RESOURCE_COUNT, nc );
}

void IFXExporter::endResourceList()
{
	m_pWriter->end();
}

//------------- GLOBAL FUNCTIONS --------------------------------------------

//------------- LOCAL FUNCTIONS ---------------------------------------------
BOOL IsObjValid( ObjectState* pOs, INode* pNode )
{
#ifdef SHOW_BONES
	return pOs->obj != NULL && !pNode->IsTarget();
#else
	return pOs->obj != NULL &&
		pOs->obj->ClassID() != BONE_OBJ_CLASSID &&
		!pNode->IsTarget()/* && !IsBiped( pNode )*/;
#endif
}
