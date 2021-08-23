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

#include "LightExporter.h"

//------------- DEFINES -----------------------------------------------------

//------------- CONSTANTS ---------------------------------------------------

//------------- ENUMERATIONS ------------------------------------------------

//------------- CLASSES, STRUCTURES AND TYPES -------------------------------

//------------- LOCAL FUNCTION PROTOTYPES -----------------------------------

//------------- PUBLIC METHODS ----------------------------------------------
IFXLightExporter::~IFXLightExporter()
{
	m_pLight = 0;
}

IFXLightExporter::IFXLightExporter( const IFXNodeExporter* pNodeExp )
: IFXObjectExporter( pNodeExp )
{
	m_pLight = (GenLight*)IFXObjectExporter::getState().obj;
}

IFXRESULT IFXLightExporter::exportNode()
{
	return IFX_OK;
}

// export light object data
IFXRESULT IFXLightExporter::exportResource( const IFXNodeData* pData )
{
	IFXRESULT  ec = IFX_OK;
	LightState state;
	Point3 att;

	RefResult refRes = m_pLight->EvalLightState( 0, FOREVER, &state );

	if( refRes == REF_SUCCEED )
	{
		switch( state.type )
		{
		case OMNI_LGT: m_pWriter->output( IDTF_LIGHT_TYPE, IDTF_POINT_LIGHT ); break;
		case SPOT_LGT: m_pWriter->output( IDTF_LIGHT_TYPE, IDTF_SPOT_LIGHT ); break;
		case DIRECT_LGT: m_pWriter->output( IDTF_LIGHT_TYPE, IDTF_DIRECTIONAL_LIGHT ); break;
		case AMBIENT_LGT: m_pWriter->output( IDTF_LIGHT_TYPE, IDTF_AMBIENT_LIGHT ); break;
		}
		m_pWriter->output( IDTF_LIGHT_COLOR, &state.color );
		att[0] = 1;
		att[1] = 0;
		att[2] = 0;
		m_pWriter->output( IDTF_LIGHT_ATTENUATION, &att );/// @todo: figure out what attenuation is


		if( state.type == SPOT_LGT )
			m_pWriter->output( IDTF_LIGHT_SPOT_ANGLE, m_pLight->GetHotspot( 0 ) );

		m_pWriter->output( IDTF_LIGHT_INTENSITY, state.intens );
	}
	else
	{
		ec = IFX_E_BAD_PARAM;
	}

	return ec;
}

//------------- PROTECTED METHODS -------------------------------------------

//------------- PRIVATE METHODS ---------------------------------------------

//------------- GLOBAL FUNCTIONS --------------------------------------------

//------------- LOCAL FUNCTIONS ---------------------------------------------