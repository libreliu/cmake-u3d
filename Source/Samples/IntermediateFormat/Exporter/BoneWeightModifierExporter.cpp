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
#include "BoneWeightModifierExporter.h"
#include "Tokens.h"
#include "Writer.h"
#include "IFXDataTypes.h"
#include "BonesUtil.h"

//------------- DEFINES -----------------------------------------------------

#define BONE_WEIGHT_PREC 0.1

//------------- CONSTANTS ---------------------------------------------------

//------------- ENUMERATIONS ------------------------------------------------

//------------- CLASSES, STRUCTURES AND TYPES -------------------------------

//------------- LOCAL FUNCTION PROTOTYPES -----------------------------------

//------------- PUBLIC METHODS ----------------------------------------------
IFXBoneWeightModifierExporter::IFXBoneWeightModifierExporter()
: m_pData(0)
{
}

IFXBoneWeightModifierExporter::IFXBoneWeightModifierExporter( IFXNodeData* node )
: m_pData(node)
{
}

IFXBoneWeightModifierExporter::~IFXBoneWeightModifierExporter()
{
	m_pData = NULL;
}



IFXRESULT IFXBoneWeightModifierExporter::export()
{
	IFXRESULT result = IFX_OK;
	ISkin* pSkin = NULL;
	ISkinContextData* pSkinContext = NULL;
///@todo: implement physique export
//	IPhysiqueExport* pPhysique = NULL;
//	IPhyContextExport* pPhysiqueContext = NULL;

	//trying to get skin modifier
	Modifier* pModifier = GetModifier( getNode(), OSM_CLASS_ID, SKIN_CLASSID );
	if( pModifier )
		pSkin = (ISkin*)pModifier->GetInterface(I_SKIN);
	if(pSkin)
		pSkinContext = pSkin->GetContextInterface(getNode());

	if( pSkinContext )
	{
		U32 vertNum = pSkinContext->GetNumPoints();
		U32 vertBoneNum = 0;
		getWriter()->begin( IDTF_MODIFIER, IDTF_BONE_WEIGHT_MODIFIER, true );
		getWriter()->output( IDTF_MODIFIER_NAME, getNode()->GetName() );
		getWriter()->output( IDTF_MODIFIER_CHAIN_INDEX, -1 );
		getWriter()->begin( IDTF_PARAMETERS );
		getWriter()->output( IDTF_ATTRIBUTES, IDTF_MESH );
		getWriter()->setPrecision( 1 );
		getWriter()->output( IDTF_INVERSE_QUANT, (F32)BONE_WEIGHT_PREC );
		getWriter()->restorePrecision();
		getWriter()->output( IDTF_POSITION_COUNT, vertNum );
		getWriter()->begin( IDTF_POSITION_BONE_WEIGHT_LIST );
		for( U32 vert = 0; vert < vertNum; vert++ )
		{
			getWriter()->begin( IDTF_BONE_WEIGHT_LIST, vert );
			vertBoneNum = pSkinContext->GetNumAssignedBones( vert );
			getWriter()->output( IDTF_BONE_WEIGHT_COUNT, vertBoneNum );
			if( vertBoneNum > 0 )
			{
				getWriter()->begin( IDTF_BONE_INDEX_LIST );
				for( U32 bone = 0; bone < vertBoneNum; bone++ )
					getWriter()->output( L"", pSkinContext->GetAssignedBone( vert, bone ) );
				getWriter()->end();
			}
			if( vertBoneNum > 1 )
			{
				getWriter()->begin( IDTF_BONE_WEIGHT_LIST );
				for( U32 bone = 0; bone < vertBoneNum-1; bone++ )//-1
					getWriter()->output( L"", (U32)(pSkinContext->GetBoneWeight( vert, bone )/BONE_WEIGHT_PREC) );
				getWriter()->end();
			}
			getWriter()->end();
		}
		getWriter()->end();
		getWriter()->end();
		getWriter()->end();
	}
	else//attempting to get Physique
	{
	}
	return result;
}


//------------- PROTECTED METHODS -------------------------------------------

//------------- PRIVATE METHODS ---------------------------------------------

//------------- GLOBAL FUNCTIONS --------------------------------------------

//------------- LOCAL FUNCTIONS ---------------------------------------------
