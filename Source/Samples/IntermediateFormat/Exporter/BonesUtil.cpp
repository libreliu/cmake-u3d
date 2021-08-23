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

#include "BonesUtil.h"
#include "modstack.h"

//------------- PUBLIC METHODS ----------------------------------------------

BipedUtil::BipedUtil()
{
	m_pBipedRoot = NULL;
	m_pBipedInterface = NULL;
}

bool BipedUtil::FindBipedRoot(INode **pMAXNodeList, int in_Cnt)
{
	bool ifxResult = true;
	int     i;

	if (!pMAXNodeList)
		return false;

	if (0 ==in_Cnt)
		return ifxResult;

	for (i = 0; i < in_Cnt; i++)
	{
		if (CheckControlID(pMAXNodeList[i], BIPBODY_CONTROL_CLASS_ID))
		{
			m_pBipedRoot = pMAXNodeList[i];
			break;
		}
	}

	return ifxResult;
}


bool BipedUtil::BipedIntoReferenceMode()
{
	bool ret = true;

	if (!m_pBipedRoot)
		return ret;

	// GET BIPED INTERFACE (ONLY ONCE)
	if(!m_pBipedInterface)
	{
		ret = GetBipInterface();
	}

	if (m_pBipedInterface)
		m_pBipedInterface->BeginFigureMode(0);
	return ret;
}


bool BipedUtil::BipedOutOfReferenceMode()
{
	bool ret = true;

	if (!m_pBipedRoot)
		return ret;

	// GET BIPED INTERFACE (ONLY ONCE)
	if(!m_pBipedInterface)
	{
		ret = GetBipInterface();
	}

	if (m_pBipedInterface)
		m_pBipedInterface->EndFigureMode(0);

	return ret;
}

bool BipedUtil::BipedRemoveNonUniformScale()
{
	bool ret = true;

	if (!m_pBipedRoot)
		return ret;

	// GET BIPED INTERFACE (ONLY ONCE)
	if(!m_pBipedInterface)
	{
		ret = GetBipInterface();
	}

	if (m_pBipedInterface)
		m_pBipedInterface->RemoveNonUniformScale(1);

	return ret;
}

bool BipedUtil::BipedReturnNonUniformScale()
{
	bool ret = true;

	if (!m_pBipedRoot)
		return ret;

	// GET BIPED INTERFACE (ONLY ONCE)
	if(!m_pBipedInterface)
	{
		ret = GetBipInterface();
	}

	if (m_pBipedInterface)
		m_pBipedInterface->RemoveNonUniformScale(0);

	return ret;
}

bool BipedUtil::BipedExportComplete()
{
	if (!m_pBipedRoot)
		return true;

	Control *c = m_pBipedRoot->GetTMController();

	if (c && (NULL != m_pBipedInterface))
	{
		c->ReleaseInterface(I_BIPINTERFACE, m_pBipedInterface);
	}

	m_pBipedInterface = NULL;

	return true;
}

bool BipedUtil::GetBipInterface()
{
	bool ret = true;

	if (!m_pBipedRoot)
		return ret;

	Control *c = NULL;

	c = m_pBipedRoot->GetTMController();

	if (c && ((c->ClassID() == BIPSLAVE_CONTROL_CLASS_ID) ||
		(c->ClassID() == BIPBODY_CONTROL_CLASS_ID) ||
		(c->ClassID() == FOOTPRINT_CLASS_ID)) )
	{
		// Get the Biped Export Interface from the controller
		m_pBipedInterface = (IBipedExport *) c->GetInterface(I_BIPINTERFACE);
	}

	return true;
}

//------------- GLOBAL FUNCTIONS --------------------------------------------

/////////////////////////////////////////////////////////
// Function:  CheckID
// Parameters:  The pNode to check, the class ID to check against,
//    and a pointer to the MAX interface.
// Purpose:  Checks to see if a pNode if of the same class ID as
//    the one specified.  Due to the possibility that the pNode is
//    part of a modifier stack, if we do not immediately identify it
//    as the desired type we try to walk down the pNode's modifier stack looking
//    for a pNode of the desired type.
// Return Val:  TRUE if it is, FALSE if it is not.
/////////////////////////////////////////////////////////
bool CheckID(INode *pNode, Class_ID ClassID)
{
	bool result = false;
	if( pNode )
	{
		Object *node_object = pNode->GetObjectRef();
		Class_ID node_cid;

		while (node_object)
		{
			node_cid = node_object->ClassID();
			if(node_cid == ClassID)
			{
				result = true;
				break;
			}

			// Not a bone - check to see if the object is part of a modifier stack.
			SClass_ID node_sid = node_object->SuperClassID();
			if (node_sid == GEN_DERIVOB_CLASS_ID)
			{
				// We have a derived object - get the next item down the modifier
				// stack
				node_object = ((IDerivedObject *)(node_object))->GetObjRef();
			}
			else break;  // We don't have a derived object - bail.
		}  // while a m_BoneNode object
	}
	return result;
}

bool CheckControlID( INode* pNode, Class_ID ClassID )
{
	bool result = false;
	if( pNode )
	{
		Control *c = pNode->GetTMController();
		if (c != NULL && c->ClassID() == ClassID )
			result = true;
	}
	return result;
}

bool IsBiped( INode* pNode )
{
	return  CheckControlID( pNode, BIPBODY_CONTROL_CLASS_ID ) ||
		CheckControlID( pNode, BIPSLAVE_CONTROL_CLASS_ID ) ||
		CheckControlID( pNode, FOOTPRINT_CLASS_ID );
}

bool IsRootBone( INode* pNode )
{
	// EVEN A BONE?
	if (pNode && (IsBiped(pNode) || CheckID(pNode, BONE_OBJ_CLASSID)))
	{
		INode * pParentNode = pNode->GetParentNode();
		// IF NO PARENT OR PARENT NOT A BONE -> THIS NODE IS ROOT BONE OF HEIRARCHY BRANCH
		if(!pParentNode || (!IsBiped(pParentNode) && !CheckID(pParentNode, BONE_OBJ_CLASSID)))
			return true;
	}
	return false;
}

bool HasSkeleton( INode* pNode )
{
	if( pNode )
	{
		ObjectState os = pNode->EvalWorldState(0); 

		BOOL bipedBody = CheckControlID( pNode, BIPBODY_CONTROL_CLASS_ID );
		BOOL bipedSlave = CheckControlID( pNode, BIPSLAVE_CONTROL_CLASS_ID );

		if( CheckID( pNode, BONE_OBJ_CLASSID ) || bipedBody || bipedSlave )
			return true;

		// For each child of this node, we recurse into ourselves 
		// until no more children are found.
		for( U32 c = 0; c < pNode->NumberOfChildren(); c++ )
			if( HasSkeleton( pNode->GetChildNode( c ) ) )
				return true;
	}
	return false;
}

Modifier* GetModifier(INode* node, int scid, Class_ID cid)
{
	IDerivedObject* dobj = NULL;
	int i;

	if (scid == WSM_CLASS_ID)//world space
	{
		if (!node->GetProperty(PROPID_HAS_WSM))
			return NULL;

		for (i = 0 ; i < node->NumRefs() ; ++i)
		{
			ReferenceTarget *refTarg = node->GetReference(i);

			if((refTarg != NULL) && (refTarg->ClassID() == Class_ID(WSM_DERIVOB_CLASS_ID,0)))
				dobj = (IDerivedObject *) refTarg;
		}
	}
	else//object space
	{
		assert(scid == OSM_CLASS_ID);
		dobj = (IDerivedObject*)node->GetObjectRef();
	}

	while (dobj && (dobj->SuperClassID() == GEN_DERIVOB_CLASS_ID))//true derived object
	{
		int numMods = dobj->NumModifiers();
		Modifier* mod;

		for (i = 0 ; i < numMods ; ++i)
		{
			mod = dobj->GetModifier(i);
			if (mod && (mod->ClassID() == cid))
				return mod;
		}
		dobj = (IDerivedObject*)dobj->GetObjRef();//move down through the stack
	}
	return NULL;
}

IFXRESULT CollectBones( INode* pNode, INode** ppBoneList, U32* pBoneCount )
{
	IFXRESULT result = IFX_OK;
	if( pNode && ppBoneList && pBoneCount )
	{
		ISkin* pSkin = NULL;
        //trying to get skin modifier
		Modifier* pModifier = GetModifier( pNode, OSM_CLASS_ID, SKIN_CLASSID );
		if( pModifier )
			pSkin = (ISkin*)pModifier->GetInterface(I_SKIN);
		if( pSkin )
		{
			*pBoneCount = pSkin->GetNumBones();
			for ( U32 i = 0; i < *pBoneCount; i++ )
				ppBoneList[i] = pSkin->GetBone( i );
		}

//		ObjectState os = pNode->EvalWorldState(0); 

//		BOOL bipedBody = CheckControlID( pNode, BIPBODY_CONTROL_CLASS_ID );
//		BOOL bipedSlave = CheckControlID( pNode, BIPSLAVE_CONTROL_CLASS_ID );

//		if( CheckID( pNode, BONE_OBJ_CLASSID )/* || bipedBody || bipedSlave */)
//			ppBoneList[(*pBoneCount)++] = pNode;

		// For each child of this node, we recurse into ourselves 
		// until no more children are found.
//		for( U32 c = 0; c < pNode->NumberOfChildren(); c++ )
//			CollectBones( pNode->GetChildNode( c ), ppBoneList, pBoneCount );
	}
	else
		result = IFX_E_NOT_INITIALIZED;
	return result;
}
