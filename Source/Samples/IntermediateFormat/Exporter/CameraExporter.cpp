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

#include "CameraExporter.h"

//------------- DEFINES -----------------------------------------------------

//------------- CONSTANTS ---------------------------------------------------

//------------- ENUMERATIONS ------------------------------------------------

//------------- CLASSES, STRUCTURES AND TYPES -------------------------------

//------------- LOCAL FUNCTION PROTOTYPES -----------------------------------

//------------- PUBLIC METHODS ----------------------------------------------
IFXCameraExporter::~IFXCameraExporter()
{
	m_pCamera = 0;
}

IFXCameraExporter::IFXCameraExporter( const IFXNodeExporter* pNodeExp )
: IFXObjectExporter( pNodeExp )
{
	m_pCamera = (CameraObject*)IFXObjectExporter::getState().obj;
}

// export camera object data
IFXRESULT IFXCameraExporter::exportNode()
{
	IFXRESULT ec = IFX_OK;
	CameraState state;

	RefResult refRes = m_pCamera->EvalCameraState( 0, FOREVER, &state );

	if( refRes == REF_SUCCEED )
	{
		m_pWriter->begin( IDTF_VIEW_DATA );

		if( state.isOrtho )
			m_pWriter->output( IDTF_VIEW_TYPE, IDTF_ORTHO_VIEW );
		else
			m_pWriter->output( IDTF_VIEW_TYPE, IDTF_PERSPECTIVE_VIEW );

		if( state.hither > 0 )
			m_pWriter->output( IDTF_VIEW_NEAR_CLIP, state.hither );
		else
			m_pWriter->output( IDTF_VIEW_NEAR_CLIP, EPSILON );
		m_pWriter->output( IDTF_VIEW_FAR_CLIP, state.yon );

		if( state.isOrtho )
			m_pWriter->output( IDTF_VIEW_PORT_HEIGHT, state.fov/ONE_DEGREE_RAD );///@todo: correct
		else
			m_pWriter->output( IDTF_VIEW_PROJECTION, state.fov/ONE_DEGREE_RAD );
		m_pWriter->end();
	}
	else
		ec = IFX_E_BAD_PARAM;

	return ec;
}

IFXRESULT IFXCameraExporter::exportResource( const IFXNodeData* pData )
{
	U32 rootNode = 0;
	m_pWriter->output( IDTF_VIEW_PASS_COUNT, 1 );
	m_pWriter->begin( IDTF_VIEW_ROOT_NODE_LIST );
	m_pWriter->begin( IDTF_ROOT_NODE, rootNode );
	m_pWriter->output( IDTF_ROOT_NODE_NAME, "<NULL>" );/// @todo: custom root node
	m_pWriter->end();
	m_pWriter->end();
	return IFX_OK;
}

//------------- PROTECTED METHODS -------------------------------------------

//------------- PRIVATE METHODS ---------------------------------------------

//------------- GLOBAL FUNCTIONS --------------------------------------------

//------------- LOCAL FUNCTIONS ---------------------------------------------