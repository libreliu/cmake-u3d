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
#ifndef __BONESEXPORTER_H__
#define __BONESEXPORTER_H__

//------------- INCLUDES ----------------------------------------------------

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "stdmat.h"
#include "decomp.h"
#include "shape.h"
#include "interpik.h"
#include "bmmlib.h"
#include "bipexp.h"

#include "IFXDataTypes.h"
#include "Writer.h"

//------------- DEFINES -----------------------------------------------------

/** \def IFX_BONE_INVALID_INDEX
	\brief 
*/
#define IFX_BONE_INVALID_INDEX 0xFFFFFFFF
/** \def SMALLEST_BONE
	\brief 
*/
#define SMALLEST_BONE       EPSILON
/** \def NODE_NEEDS_DISPLACEMENT
	\brief 
*/
#define NODE_NEEDS_DISPLACEMENT	(1)
/** \def MIN_WEIGHT_VALUE
	\brief 
*/
#define MIN_WEIGHT_VALUE	EPSILON


//------------- CONSTANTS ---------------------------------------------------

//------------- ENUMERATIONS ------------------------------------------------

//------------- CLASSES, STRUCTURES AND TYPES -------------------------------

/**	\class IFXBonesExporter
	\brief The main converter library to convert a hierachy of Max bones to IDTF skeleton

	\b Description:
		 Holds per bone conversion information. 
		 Also provides the bone conversion functions		

	\remarks
*/
class IFXBonesExporter
{
public:
	/** \fn IFXBonesExporter()
		\brief 

		\b Description:
		\param YYY
		\exception None
		\return IFXRESULT
		\remarks 
	*/
	IFXBonesExporter();

	/** \fn IFXRESULT Init(unsigned int in_Idx, INode* in_Bone, INode *in_GroupNode)
		\brief Initialize the object for use. 

		\b Description:
		\param unsigned int in_Idx - The Id of the bone.
		\param INode* in_Bone - The Bone to be converted.
		\param INode *in_GroupNode - The group head of the bone. 
		\exception None
		\return IFXRESULT
		\remarks 
	*/
	IFXRESULT Init(U32 in_Idx, INode* in_Bone, INode *in_GroupNode);

	/** \fn void SetParent(unsigned int in_PIdx, IFXBonesExporter* in_PInfo )
		\brief Set the information about the parent bone. 

		\b Description:
		\param unsigned int in_PIdx - Array index of the parent bone. 
		\param IFXBonesExporter* in_PInfo - The IFXBonesExporter object 
			associated with the bones parent. 
		\exception None
		\return IFXRESULT
		\remarks 
	*/
	void SetParent(U32 in_PIdx, IFXBonesExporter* in_PInfo ) 
				{ m_ParentIdx = in_PIdx; m_ParentInfo =in_PInfo; };

	/** \fn IFXRESULT ComputeBoneInfo()
		\brief Calculates the axis alignment, Length, and Displacement, , 

		\b Description:
		\param None
		\exception None
		\return IFXRESULT
		\remarks 
	*/
	IFXRESULT ComputeBoneInfo();	

	/** \fn void ComputeInitialTransform()
		\brief Computes the initial transformation of the bone

		\b Description:
		\param None
		\exception None
		\remarks 
	*/
	void ComputeInitialTransform();

	/** \fn IFXRESULT AxisAlign()
		\brief Computes the tranform needed to bring the bone into aligment 
			with its parent. 

		\b Description:
		\param None
		\exception None
		\return IFXRESULT
		\remarks 
	*/
	IFXRESULT AxisAlign();

	/** \fn IFXRESULT CollectBoneAnimData(unsigned int uSampleInterval, unsigned int uAnimationStart, unsigned int uAnimationEnd)
		\brief Collects the animation data for this bone

		\b Description:
		\param unsigned int uSampleInterval - The step between samples. 
		\param unsigned int uAnimationStart - The first frame to sample.
		\param unsigned int uAnimationEnd - The last frame to sample.
		\exception None
		\return IFXRESULT
		\remarks 
	*/
	IFXRESULT CollectBoneAnimData( Quat& rot, Point3& bonePos, Point3& boneScale, F32 time, TimeValue initTime, IFXWriter* pW );

	/** \fn INode* GetParentNode()
		\brief return the MAX node of this bones parent. 

		\b Description:
		\param None
		\exception None
		\return IFXRESULT
		\remarks 
	*/
	INode* GetParentNode() { return m_ParentNode;};

	/** \fn INode* GetNode()
		\brief Gets the MAX Node associated with this bone. 

		\b Description:
		\param None
		\exception None
		\return IFXRESULT
		\remarks 
	*/
	INode* GetNode() { return m_BoneNode;};
	
	void GetLength( F32* pLen ){ *pLen = (F32)m_Length; }
	void GetDisp( Point3* pDisp ){ *pDisp = m_Disp; }
	void GetRot( Quat* pRot ){ *pRot = m_Rot; }

private:
	/** \fn IFXRESULT CalcLengthDisp(TimeValue in_Time, Point3* out_Disp, float* out_Length);
		\brief Calculate the length and displacement of the bone at a given time. 

		\b Description:
		\param TimeValue in_Time
		\param Point3* out_Disp
		\param float* out_Length
		\exception None
		\return IFXRESULT
		\remarks 
	*/
	IFXRESULT CalcLengthDisp(TimeValue in_Time, Point3* out_Disp, F32* out_Length);

	/** \fn IFXRESULT ComputeJoints();
		\brief 

		\b Description:
		\param none
		\exception None
		\return IFXRESULT
		\remarks This data is not used by Tron/Rushmore(put your favorite code name here). 
	*/
	IFXRESULT ComputeJoints();


	/** \var BOOL m_bInit
		\brief Has the object been initialized. 

		\b Description:
		\remarks
	*/
	BOOL m_bInit;

	/** \var INode* m_BoneNode
		\brief The bone node being converted. 

		\b Description:
		\remarks
	*/
	INode* m_BoneNode;

	/** \var U32 m_BoneIdx
		\brief The index being assigned to this bone. 

		\b Description:
		\remarks
	*/	
	U32 m_BoneIdx;

	/** \var INode* m_ParentNode
		\brief The Parent node of this bone. 

		\b Description:
		\remarks May be Null if this is the Root bone. 
	*/
	INode* m_ParentNode;

	/** \var IFXBonesExporter* m_ParentInfo
		\brief Pointer to the ConverionsInfo structure for the parent. 

		\b Description: 
		\remarks May be Null if this is the Root bone.  
	*/
	IFXBonesExporter* m_ParentInfo;

	/** \var U32 m_ParentIdx
		\brief The ID being assigned to the parent. 

		\b Description:
		\remarks 
	*/
	U32 m_ParentIdx;

	/** \var INode *m_GroupNode
		\brief The Group head of the group being converted. 

		\b Description:
		\remarks
	*/
	INode *m_GroupNode;

	/** \var F32			m_Length
		\brief The initial length of this bone. 

		\b Description:
		\remarks
	*/
	F32			m_Length;

	/** \var BOOL	m_btmModified
		\brief 

		\b Description:
		\remarks
	*/
	BOOL	m_btmModified; 

	/** \var Point3		m_Disp
		\brief The initial displacement of this bone. 

		\b Description:
		\remarks
	*/
	Point3		m_Disp;

	/** \var Point3		m_Scale
		\brief The initial scale of this bone. 

		\b Description:
		\remarks
	*/
	Point3		m_Scale;
	/** \var IFXQuaternion   m_Rot
		\brief The initial rotation of this bone. 

		\b Description:
		\remarks
	*/
	Quat   m_Rot;
	/** \var IFXQuaternion   m_DeltaRot
		\brief The rotation needed to make the bone X axis aligned. 

		\b Description:
		\remarks
	*/
	Quat   m_DeltaRot;
};

//------------- GLOBAL FUNCTION PROTOTYPES ----------------------------------

//------------- GLOBAL DATA -------------------------------------------------

#endif
