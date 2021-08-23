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

#include "AnimationExporter.h"
#include "Tokens.h"
#include "Writer.h"
#include "decomp.h" // for affines
#include "interpik.h" //for jointparams
#include "bipexp.h"

//------------- DEFINES -----------------------------------------------------
#define ALMOST_ZERO 1.0e-3f

//------------- CONSTANTS ---------------------------------------------------

//------------- ENUMERATIONS ------------------------------------------------

//------------- CLASSES, STRUCTURES AND TYPES -------------------------------

//------------- LOCAL FUNCTION PROTOTYPES -----------------------------------
BOOL isEqualPoint3( Point3, Point3 );

//------------- PUBLIC METHODS ----------------------------------------------
IFXAnimationExporter::IFXAnimationExporter()
: m_pData(0), m_boneCount(0)
{
	m_ppBoneList = NULL;
	m_pBoneInfo = NULL;
}

IFXAnimationExporter::IFXAnimationExporter( IFXNodeData* node )
: m_pData(node)
{
	m_boneCount = 0;
	m_ppBoneList = new INode*[MAX_BONE_NUM];
	InitBoneAnimExport();
}

IFXAnimationExporter::~IFXAnimationExporter()
{
	TermBoneAnimExport();
	m_pData = NULL;
	if( m_ppBoneList)
		delete[] m_ppBoneList;
}


void IFXAnimationExporter::export()
{
	//	BOOL bPosAnim;
	//	BOOL bRotAnim;
	//	BOOL bScaleAnim;
	Quat prevQ;
	prevQ.Identity();

	TimeValue start = getAnimRange()->Start();
	TimeValue end = getAnimRange()->End();
	int delta = GetTicksPerFrame() * getKeyFrameStep();

	if( checkForGeneralAnimation() )
	{
		// how to get roll angle from TM for targeted nodes (light and cameras)???
		getWriter()->output( IDTF_MOTION_TRACK_COUNT, 1 );
		getWriter()->begin( IDTF_MOTION_TRACK_LIST );
		getWriter()->begin( IDTF_MOTION_TRACK, 0 );
		TCHAR trackName[MAX_TOKEN_LEN];
		TCHAR trackNum[MAX_TOKEN_LEN];
		strcpy( trackName, "Track" );
		_itoa( 0, trackNum, 10 );
		strcat( trackName, trackNum );
		getWriter()->output( IDTF_MOTION_TRACK_NAME, trackName );

///@ todo: use this technique in future
/*		Control* pControl = getNode()->GetTMController();
		Control* pPosCont;
		Control* pRotCont;
		Control* pSclCont;
		IKeyControl* pPosKeys;
		IKeyControl* pRotKeys;
		IKeyControl* pSclKeys;
		if( pControl )
		{
			pPosCont = pControl->GetPositionController();
			pRotCont = pControl->GetRotationController();
			pSclCont = pControl->GetScaleController();
		}

		if( pPosCont )
			pPosKeys = GetKeyControlInterface(pPosCont);
		if( pRotCont )
			pRotKeys = GetKeyControlInterface(pRotCont);
		if( pSclCont )
			pSclKeys = GetKeyControlInterface(pSclCont);
		if( false )
//		if( pPosKeys || pRotKeys || pSclKeys )
		{
			U32 posInd = 0;
			U32 rotInd = 0;
			U32 sclInd = 0;
			U32 keyInd = 0;


			Quat prevQ;
			prevQ.Identity();
			U32 time = 0;
			ITCBPoint3Key posKey;
			posKey.time = 0;
			posKey.val.Set( 0.0, 0.0, 0.0 );
			ITCBRotKey rotKey;
			rotKey.time = 0;
			rotKey.val.Set( 0.0, 0.0, 0.0, 1.0 );
			ITCBScaleKey sclKey;
			sclKey.time = 0;
			sclKey.val.s.Set( 1.0, 1.0, 1.0 );

			getWriter()->setPrecision( 1 );
			getWriter()->output( IDTF_MOTION_TRACK_SAMPLE_COUNT, pRotKeys->GetNumKeys() );
			getWriter()->begin( IDTF_KEY_FRAME_LIST );

//			getWriter()->output( "POS", pPosKeys->GetNumKeys() );
//			getWriter()->output( "ROT", pRotKeys->GetNumKeys() );
//			getWriter()->output( "SCL", pSclKeys->GetNumKeys() );
			time = 0;
//			while( rotInd < pRotKeys->GetNumKeys() )
			while( posInd < pPosKeys->GetNumKeys() || rotInd < pRotKeys->GetNumKeys() || sclInd < pSclKeys->GetNumKeys() )
//			for(U32 i = 0; i < 3; i++)
			{
				getWriter()->begin( IDTF_KEY_FRAME, keyInd );
				if( pPosKeys && posInd < pPosKeys->GetNumKeys() )
				{
					pPosKeys->GetKey( posInd, &posKey );
					time = posKey.time;
				}
				if( pRotKeys && rotInd < pRotKeys->GetNumKeys() )
				{
					pRotKeys->GetKey( rotInd, &rotKey );
					if( rotKey.time < time )
						time = rotKey.time;
				}
				if( pSclKeys && sclInd < pSclKeys->GetNumKeys() )
				{
					pSclKeys->GetKey( sclInd, &sclKey );
					if( sclKey.time < time )
						time = sclKey.time;
				}
				if( time == posKey.time )
					posInd++;
				else
					pPosKeys->GetKey( posInd-1, &posKey );
				if( time == rotKey.time )
					rotInd++;
				else
					pRotKeys->GetKey( rotInd-1, &rotKey );
				if( time == sclKey.time )
					sclInd++;
				else
					pSclKeys->GetKey( sclInd-1, &sclKey );

//				getWriter()->output( "cur pos", posInd );
//				getWriter()->output( "cur rot", rotInd );
				getWriter()->output( IDTF_KEY_FRAME_TIME, time/4800.0f );
				Matrix3 tm = Inverse(getNode()->GetParentTM(0)) * getNode()->GetNodeTM(0);
				AffineParts ap;
				decomp_affine(tm, &ap);
				Point3 pos;
				pos[0] = posKey.val[0] - ap.t[0];
				pos[1] = posKey.val[1] - ap.t[1];
				pos[2] = posKey.val[2] - ap.t[2];
				Matrix3 tminv = Inverse( tm );
				Point3 vec;
				Point3 postm;
				vec = pos*tminv.GetColumn3(0);
				postm[0] = vec[0]+vec[1]+vec[2];
				vec = pos*tminv.GetColumn3(1);
				postm[1] = vec[0]+vec[1]+vec[2];
				vec = pos*tminv.GetColumn3(2);
				postm[2] = vec[0]+vec[1]+vec[2];

				getWriter()->output( IDTF_KEY_FRAME_DISPLACEMENT, &(postm) );

				Point3 axistm;
				vec = rotKey.val.axis*tminv.GetColumn3(0);
				axistm[0] = vec[0]+vec[1]+vec[2];
				vec = rotKey.val.axis*tminv.GetColumn3(1);
				axistm[1] = vec[0]+vec[1]+vec[2];
				vec = rotKey.val.axis*tminv.GetColumn3(2);
				axistm[2] = vec[0]+vec[1]+vec[2];
				Quat q = QFromAngAxis(rotKey.val.angle, axistm);
//				Quat q = QFromAngAxis(rotKey.val.angle, rotKey.val.axis);
				getWriter()->output( "AXIS!", &rotKey.val.axis );
//				q = q*prevQ;
//				prevQ = q;
				getWriter()->output( IDTF_KEY_FRAME_ROTATION, q.w );
				getWriter()->setPlain( TRUE );
				getWriter()->output( "", q.x );
				getWriter()->output( "", q.y );
				getWriter()->output( "", q.z );
				getWriter()->setPlain( FALSE );

				getWriter()->output( IDTF_KEY_FRAME_SCALE, &(sclKey.val.s) );
				getWriter()->end();
				keyInd++;
			}
		}
		else*/
		{
			getWriter()->output( IDTF_MOTION_TRACK_SAMPLE_COUNT, (U32)((end-start)/delta)+1 );
			getWriter()->begin( IDTF_KEY_FRAME_LIST );
			getWriter()->setPrecision( 1 );

			Matrix3 tm, tminv;
			AffineParts ap;
			Point3	prevPos;
			U32 num = 0;

			for( TimeValue t = start; t <= end; t += delta)
			{
				getWriter()->begin( IDTF_KEY_FRAME, num );
				getWriter()->output( IDTF_KEY_FRAME_TIME, (F32)delta*num/4800 );
#ifdef SHOW_BONES
				if( getNode()->EvalWorldState(0).obj->ClassID() == BONE_OBJ_CLASSID )
					//debug bones animation
					tm = getNode()->GetObjectTM(t) * Inverse(getNode()->GetObjectTM(0));
				else
				{
					//key-frame animation
					tm = Inverse(getNode()->GetParentTM(t)) * getNode()->GetObjectTM(t);
					tminv = Inverse(getNode()->GetObjectTM(0)) * getNode()->GetParentTM(0);
					tm = tm*tminv;
				}
#else
				tm = getNode()->GetObjectTM(t)*Inverse(getNode()->GetParentTM(t));
				tminv = getNode()->GetParentTM(0)*Inverse(getNode()->GetObjectTM(0));
				tm = tm*tminv;
#endif
				decomp_affine(tm, &ap);

				//position
				Point3 pos = ap.t;
				getWriter()->output( IDTF_KEY_FRAME_DISPLACEMENT, &pos );

				//rotation
				Quat q = ap.q;
				getWriter()->output( IDTF_KEY_FRAME_ROTATION, q.w );
				getWriter()->setPlain( TRUE );
				getWriter()->output( L"", -q.x );
				getWriter()->output( L"", -q.y );
				getWriter()->output( L"", -q.z );
				getWriter()->setPlain( FALSE );

				//scaling
				getWriter()->output( IDTF_KEY_FRAME_SCALE, &ap.k );
				getWriter()->end();
				num++;
			}
		}
		getWriter()->restorePrecision();
		getWriter()->end();
		getWriter()->end();
		getWriter()->end();
	}
	if( checkForBonesAnimation() )
	{
		getWriter()->setPrecision( 1 );
		getWriter()->output( IDTF_MOTION_TRACK_COUNT, m_boneCount );
		getWriter()->begin( IDTF_MOTION_TRACK_LIST );

		//bones animation
		bipedUtil.BipedRemoveNonUniformScale();

		// iterate the bones in the list and initialize the bones conversion info. 
		unsigned int i, j; 
		m_pBoneInfo = new IFXBonesExporter[m_boneCount];
		for(i = 0; i < m_boneCount; ++i )
		{
			m_pBoneInfo[i].Init(i, m_ppBoneList[i], m_ppBoneList[0]);
		}

		for(i = 0; i < m_boneCount; ++i )
			m_pBoneInfo[i].ComputeBoneInfo();

		for(i = 0; i < m_boneCount; ++i )
			m_pBoneInfo[i].ComputeInitialTransform();

		// a n^2 pass to set up the Bone parent 
		for(i = 0; i < m_boneCount; ++i )
		{
			// don't search for the parent on bones with the 
			// scene root for the parent
			if(m_pBoneInfo[i].GetParentNode())
			{
				for(j = 0; j < m_boneCount; ++j )
				{
					if(m_pBoneInfo[j].GetNode() == m_pBoneInfo[i].GetParentNode())
					{
						m_pBoneInfo[i].SetParent(j, &m_pBoneInfo[j]);
						break;
					}
				}
			}
		}
		Quat rot;
		Point3 bonePos, boneScale;
		U32 num;
		for(i = 0; i < m_boneCount; ++i )
		{
			getWriter()->begin( IDTF_MOTION_TRACK, i );
			getWriter()->output( IDTF_MOTION_TRACK_NAME, m_ppBoneList[i]->GetName() );
			getWriter()->output( IDTF_MOTION_TRACK_SAMPLE_COUNT, (U32)((end-start)/delta)+1 );
			getWriter()->begin( IDTF_KEY_FRAME_LIST );
			num = 0;
			for( TimeValue t = start; t <= end; t += delta)
			{
				m_pBoneInfo[i].CollectBoneAnimData( rot, bonePos, boneScale, t, start, getWriter() );
				getWriter()->begin( IDTF_KEY_FRAME, num );
				getWriter()->output( IDTF_KEY_FRAME_TIME, (F32)delta*num/4800 );
				getWriter()->output( IDTF_KEY_FRAME_DISPLACEMENT, &bonePos );
				getWriter()->output( IDTF_KEY_FRAME_ROTATION, rot.w );
				getWriter()->setPlain( TRUE );
				getWriter()->output( L"", -rot.x );
				getWriter()->output( L"", -rot.y );
				getWriter()->output( L"", -rot.z );
				getWriter()->setPlain( FALSE );

				getWriter()->output( IDTF_KEY_FRAME_SCALE, &boneScale );
				getWriter()->end();
				num++;
			}
			getWriter()->end();
			getWriter()->end();
		}

		bipedUtil.BipedReturnNonUniformScale();
		delete[] m_pBoneInfo;
		getWriter()->restorePrecision();
		getWriter()->end();
	}
}

IFXRESULT IFXAnimationExporter::exportAnimationModifier()
{
	IFXRESULT ec = IFX_OK;

	if( checkForAnimation() )
	{
		U32 shaderListCount = 1;
		getWriter()->begin( IDTF_MODIFIER, "ANIMATION", true );
		getWriter()->output( IDTF_MODIFIER_NAME, getNode()->GetName() );
		getWriter()->output( IDTF_MODIFIER_CHAIN_INDEX, -1 );
		getWriter()->begin( IDTF_PARAMETERS );
		getWriter()->output( IDTF_ATTRIBUTE_ANIMATION_PLAYING, "TRUE" );
		getWriter()->output( IDTF_ATTRIBUTE_ROOT_BONE_LOCKED, "FALSE" );
		if( checkForBonesAnimation() )
			getWriter()->output( IDTF_ATTRIBUTE_SINGLE_TRACK, "FALSE" );
		else
			getWriter()->output( IDTF_ATTRIBUTE_SINGLE_TRACK, "TRUE" );
		getWriter()->output( IDTF_ATTRIBUTE_AUTO_BLEND, "TRUE" );
		getWriter()->setPrecision( 1 );
		getWriter()->output( IDTF_TIME_SCALE, (F32)1.0 );
		getWriter()->output( IDTF_BLEND_TIME, (F32)0.5 );

		U32 motionCount = 1;
		TCHAR motionName[MAX_TOKEN_LEN];
		getWriter()->output( IDTF_MOTION_COUNT, motionCount );
		getWriter()->begin( IDTF_MOTION_INFO_LIST );
		for( U32 i = 0; i < motionCount; i++ )
		{
			getWriter()->begin( IDTF_MOTION_INFO, i );
			strcpy( motionName, getNode()->GetName() );
			strcat( motionName, "-Key" );
			getWriter()->output( IDTF_MOTION_NAME, motionName );
			getWriter()->output( IDTF_ATTRIBUTE_LOOP, "TRUE" );
			if( checkForBonesAnimation() )
				getWriter()->output( IDTF_ATTRIBUTE_SYNC, "FALSE" );
			else
				getWriter()->output( IDTF_ATTRIBUTE_SYNC, "TRUE" );
			getWriter()->output( IDTF_TIME_OFFSET, (F32)0.0 );
			getWriter()->output( IDTF_TIME_SCALE, (F32)1.0 );
			getWriter()->end();
		}

		getWriter()->restorePrecision();
		getWriter()->end();
		getWriter()->end();
		getWriter()->end();
	}
	return ec;
}

// To really see if a node is animated we can step through the animation range
// and decompose the TM matrix for every frame and examine the components.
// This way we can identify position, rotation and scale animation separately.
// 
// Some controllers makes it problematic to examine the TMContollers instead of
// the actual TMMatrix. For example, a path controller is a position controller, 
// but if you turn on follow and banking, it will also affect the rotation component.
// If we want to, we can examine the position, rotation and scale controllers and
// if they all are Linear, Hybrid (bezier) or TCB, then we could export the actual keys.
// 
// This method will check the actual controller to determine if the controller is known.
// If we know how to work this controller, its actual keys will be exported,
// otherwise the controller will be sampled using the user specified sampling frequency.

BOOL IFXAnimationExporter::checkForAnimation()
{
	return checkForGeneralAnimation() || checkForBonesAnimation();
}

BOOL IFXAnimationExporter::checkForGeneralAnimation()
{
	bool result = false;

	//firstly we check presence of general key-frame animation
	BOOL bPos = FALSE;
	BOOL bRot = FALSE;
	BOOL bScale = FALSE;
	TimeValue start = getAnimRange()->Start();
	TimeValue end   = getAnimRange()->End();
	Matrix3 tm;
	AffineParts ap;
	Point3 firstPos;
	float  rotAngle, firstRotAngle;
	Point3 rotAxis, firstRotAxis;
	Point3 firstScaleFactor;

	for( TimeValue t = start; t <= end; t += GetTicksPerFrame() ) 
	{
		tm = getNode()->GetNodeTM(t) * Inverse(getNode()->GetParentTM(t));

		decomp_affine(tm, &ap);

		AngAxisFromQ(ap.q, &rotAngle, rotAxis);

		if( t != start )
		{
			if( !isEqualPoint3(ap.t, firstPos) )
			{
				bPos = TRUE;
				break;
			}
			// MAX 2.x:
			// We examine the rotation angle to see if the rotation component
			// has changed.
			// Although not entierly true, it should work.
			// It is rare that the rotation axis is animated without
			// the rotation angle being somewhat affected.
			// MAX 3.x:
			// The above did not work, I have a repro scene that doesn't export a rotation track
			// because of this. I fixed it to also compare the axis.
			AngAxisFromQ(ap.q, &rotAngle, rotAxis);

			if( fabs(rotAngle - firstRotAngle) > ALMOST_ZERO )
			{
				bRot = TRUE;
				break;
			}
			else if( !isEqualPoint3(rotAxis, firstRotAxis) )
			{
				bRot = TRUE;
				break;
			}

			if( !isEqualPoint3(ap.k, firstScaleFactor) )
			{
				bScale = TRUE;
				break;
			}
		}
		else 
		{
			firstPos = ap.t;
			firstRotAngle = rotAngle;
			firstRotAxis = rotAxis;
			firstScaleFactor = ap.k;
		}
	}

	if( bPos || bRot || bScale )
		result = true;
	return result;
}

BOOL IFXAnimationExporter::checkForBonesAnimation()
{
	BOOL result = false;
	if( m_boneCount )
	{
		result = true;
	}
	return result;
}
//------------- PROTECTED METHODS -------------------------------------------

//------------- PRIVATE METHODS ---------------------------------------------

void IFXAnimationExporter::InitBoneAnimExport()
{
	CollectBones( getNode(), m_ppBoneList, &m_boneCount );
	bipedUtil.FindBipedRoot( m_ppBoneList, m_boneCount );
	bipedUtil.BipedRemoveNonUniformScale();
}

void IFXAnimationExporter::TermBoneAnimExport()
{
	bipedUtil.BipedReturnNonUniformScale();
	bipedUtil.BipedExportComplete();
}
//------------- GLOBAL FUNCTIONS --------------------------------------------
// Not truly the correct way to compare floats of arbitary magnitude...
BOOL isEqualPoint3(Point3 p1, Point3 p2)
{
	if (fabs(p1.x - p2.x) > ALMOST_ZERO)
		return FALSE;
	if (fabs(p1.y - p2.y) > ALMOST_ZERO)
		return FALSE;
	if (fabs(p1.z - p2.z) > ALMOST_ZERO)
		return FALSE;

	return TRUE;
}

//------------- LOCAL FUNCTIONS ---------------------------------------------
