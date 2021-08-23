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

#include "BonesExporter.h"
#include "BonesUtil.h"
#include "Common.h"

//------------- DEFINES -----------------------------------------------------

//------------- CONSTANTS ---------------------------------------------------

//------------- ENUMERATIONS ------------------------------------------------

//------------- CLASSES, STRUCTURES AND TYPES -------------------------------

//------------- LOCAL FUNCTIONS -----------------------------------

/////////////////////////////////////////////////////////
// Function:  ParentAbsolute
// Parameters:  A transformation matrix and a quaternion
// Purpose:  Converts the quaternion to a matrix and multiples
//   it by the specified matrix
// Return Val:  None
/////////////////////////////////////////////////////////
void ParentAbsolute(Matrix3 &mat, Quat & q)
{
  PreRotateMatrix(mat, q);
}


void MatrixFixup(Matrix3 &Matrix)
{
  for (U32 i=0; i < 4; i++)
  {
    Point3 row = Matrix.GetRow(i);
    U32    changedOne = 0;

    for (int j = 0; j < 3; j++)
    {
      if (fabs(row[j]) < EPSILON)
      {
        row[j] = 0.0f;
        changedOne = 1;
      }
    }

    if (changedOne==1)
      Matrix.SetRow(i, row);
  }
}

/////////////////////////////////////////////////////////
// Function:  AngleBetweenVectors
// Parameters:  Two vectors
// Purpose:  Finds the angle betweent two vectors
// Return Val:  The angle (radians)
/////////////////////////////////////////////////////////
F32 AngleBetweenVectors(Point3 *bone_vec, Point3 *x_vec)
{
  F32 dotP = DotProd(*bone_vec, *x_vec);

  if ((dotP < 1.0f) && (dotP > -1.0f))
  {
    return (F32) acos(dotP);
  }
  else if (dotP >= 1.0)
    return 0.0f;
  else
    return PI; //3.14159265358979323846f;
}

/////////////////////////////////////////////////////////
// Function:  RoundedTime
// Parameters:  The time to round
// Purpose:  Rounds the time to a reasonable number of
//    significant figures.
// Return Val:  The rounded time
/////////////////////////////////////////////////////////
// The following constant chosen since time in MAX has smallest
// units of 1/4800 of a second.
#define LARGESTNUMBER 10000.0

double RoundedTime(double inTime)
{
  double fraction = 0.0;
  double intPortion = 0.0;
  double work;

  // Move the decimal places we find interesting to the left of the
  // decimal point.
  work = inTime * LARGESTNUMBER;

  // Round up
  work += 0.5;

  // Separate the left and right of the decimal point
  fraction = modf(work, &intPortion);


  // Bring the remaining decimal points back over to the right and return this.
  return ((double)intPortion / (double)LARGESTNUMBER);
}

//------------- PUBLIC METHODS ----------------------------------------------

//---------------------------------------------------------------------------
//  IFXBonesExporter::IFXBonesExporter
//
//
//---------------------------------------------------------------------------
IFXBonesExporter::IFXBonesExporter()
{
  m_bInit = false;
  m_BoneNode = NULL;
  m_BoneIdx = IFX_BONE_INVALID_INDEX;
  m_ParentNode = NULL;
  m_ParentInfo = NULL;
  m_ParentIdx = IFX_BONE_INVALID_INDEX;
  m_GroupNode = NULL;
  m_Length = 0.0f;
  m_Disp.Set(0.0f,0.0f, 0.0f);
  m_Scale.Set(1.0f, 1.0f, 1.0f);
  m_Rot.Set(0.0f,0.0f, 0.0f, 1.0f);
  m_DeltaRot.Set(0.0f,0.0f, 0.0f, 1.0f);
  m_btmModified = false;
}

//---------------------------------------------------------------------------
//  IFXBonesExporter::Init
//
//
//---------------------------------------------------------------------------
IFXRESULT IFXBonesExporter::Init(U32 in_Idx, INode* in_Bone,
                 INode* in_GroupNode)
{
  IFXRESULT iResult = IFX_OK;

  if(SUCCEEDED(iResult))
  {
    if ((NULL == in_Bone) || !in_GroupNode)
    {
      iResult = E_POINTER;
    }
  }

  // We will assume that if we got this far we really intend to
  // treat these nodes as bones.
  if(SUCCEEDED(iResult))
  {
    m_BoneIdx = in_Idx;
    m_BoneNode = in_Bone;

    // Get transform of bone in model space
    //    m_boneXform = rBoneTM;

    m_ParentNode = in_Bone->GetParentNode();
    // parent Node is always valid -- for axis align.

    m_GroupNode = in_GroupNode;
  }


  // if we made it thru all that we can call the Bone conversion initialized.
  if(SUCCEEDED(iResult))
  {
    m_bInit = true;
  }


  return iResult;
}

//---------------------------------------------------------------------------
//  IFXBonesExporter::CalcLengthDisp
//
//
//---------------------------------------------------------------------------
IFXRESULT IFXBonesExporter::CalcLengthDisp(TimeValue in_Time, Point3* out_Disp, F32* out_Length)
{
  IFXRESULT iResult = IFX_OK;

  F32 tpf = (F32)GetTicksPerFrame();
  F32 length = 0.0f;

  TimeValue curr_time(in_Time);


  Point3 displacement(0.0f, 0.0f, 0.0f);
  Control *bon_cont = m_BoneNode->GetTMController();
  INode * parent_node = NULL;
  Matrix3 bone_tm = m_BoneNode->GetNodeTM(curr_time);
  Matrix3 local_tm(1);
  //  Matrix3 modelOrigin = m_modelXform;
  bool isRoot = (m_ParentIdx == IFX_BONE_INVALID_INDEX); // isRootBone(m_BoneNode);


  // if root m_BoneNode
  if(isRoot)
  {
    //    bone_tm = m_boneXform;  // Transform us into model space

    displacement = bone_tm.GetTrans();
    // model-space displacement
    ///@todo - should this be this value or zero?  Need to make sure that this displacement
    // is at least 0 at time = 0, since at that time Bip01 is at the origin of the
    // model space defined by itself.
    //    displacement = bone_tm.GetTrans() - m_modelXform.GetTrans();

    if ( CheckID( m_BoneNode, Class_ID( DUMMY_CLASS_ID, 0 ) ) )
    {
      ///@todo - this should never get called - kill?
      length = 0;
      m_Length = length;

      Matrix3 localtm = bone_tm * Inverse(m_GroupNode->GetObjectTM(curr_time));
      displacement = localtm.GetTrans();

      // DisPLACEMENT
      *out_Disp = displacement;
      return iResult;
    }
  }

  // MAX bones are defined by the lengths of their parents, not their children
  // as in Biped.
  if (!IsBiped(m_BoneNode))
  {
    if (isRoot)
    {
      if (curr_time == 0)
      {
        *out_Length = length;
      }

      Matrix3 localtm = bone_tm * Inverse(m_GroupNode->GetObjectTM(curr_time));
      displacement = localtm.GetTrans();

      *out_Disp = displacement;
      return iResult;
    }


    parent_node = m_BoneNode->GetParentNode();

    if (parent_node)  // This better be true!!!
    {
      Matrix3 parent_tm(1);
      Interval ivalid;
      Control *parent_pos_cont = NULL;
      Control *bone_pos_cont = NULL;
      Point3 local_disp;

      parent_tm = parent_node->GetNodeTM(curr_time);

      if(m_ParentInfo->m_btmModified)
      {
        Quat parent_delta(m_ParentInfo->m_DeltaRot[0],m_ParentInfo->m_DeltaRot[1],
          m_ParentInfo->m_DeltaRot[2],  m_ParentInfo->m_DeltaRot[3]);
        ParentAbsolute(parent_tm, parent_delta);
      }

      if(m_btmModified)
      {
        Quat curr_delta(m_DeltaRot[0], m_DeltaRot[1],
          m_DeltaRot[2], m_DeltaRot[3]);
        ParentAbsolute(bone_tm, curr_delta);
      }

      // Get the local transformation of the bone in the parent's coordinate space.
      local_tm = bone_tm * Inverse(parent_tm);

      if ( CheckID( parent_node, Class_ID( DUMMY_CLASS_ID, 0 ) ) )
      {
        // If our parent is a dummy (e.g. not a real bone) we have no bone
        // (which connect parent and child), just a displacement).

        // LENGTH
        length = 0.0f;

        // DisPLACEMENT
        displacement = bone_tm.GetTrans() - parent_tm.GetTrans();
      }
      else if (IsBiped(parent_node))
      {
        // BULL.MAX uses this to model horns on top of a head.
        // In this particular model a very odd local transformation
        // matrix exists for the bones attached to the biped - all
        // cells are zero except for the translation.  And the translation
        // is nearly zero.  Hence the code here, which treats the biped
        // almost like a second root m_BoneNode (except that we use a length
        // in world space rather than a displacement in world space).
        ///@todo - this hack doesn't completely work because we are here when the Biped model
        // figure mode and no-uniform scaling mode.  The bones don't see this,
        // so the two transformations are unrelated - they are dealing with
        // different world spaces, and we have lost the necessary information
        // from the biped objects.  Going to have to export the
        // biped model without going into non-uniform scaling mode or something...
        length = Length(bone_tm.GetTrans() - parent_tm.GetTrans());

        if (curr_time == 0)
        {
          *out_Length = length;
        }
      }
      else
      {  // Parent is a real bone.
        if (curr_time == 0)
        {
          // For time 0, we record simply a length, and zero displacement.
          length = Length(bone_tm.GetTrans() - parent_tm.GetTrans());

          // If this vector is nearly zero (bone tiny), this bone should
          // be considered to have zero length.
          if (length < SMALLEST_BONE)
          {
            length = 0.0f;
          }
          *out_Length = length;
        }
        else
        {
          Matrix3 orig_parent_tm = parent_node->GetNodeTM(0);
          Matrix3 orig_bone_tm = m_BoneNode->GetNodeTM(0);
          Matrix3 orig_local_tm(1);

          if(m_ParentInfo->m_btmModified)
          {
            Quat parent_delta(m_ParentInfo->m_DeltaRot[0],m_ParentInfo->m_DeltaRot[1],
              m_ParentInfo->m_DeltaRot[2],  m_ParentInfo->m_DeltaRot[3]);
            ParentAbsolute(orig_parent_tm, parent_delta);
          }

          if(m_btmModified)
          {
            Quat curr_delta(m_DeltaRot[0], m_DeltaRot[1],
              m_DeltaRot[2], m_DeltaRot[3]);
            ParentAbsolute(orig_bone_tm, curr_delta);
          }

          orig_local_tm = orig_bone_tm * Inverse(orig_parent_tm);

          Point3 new_vec, orig_vec;

          new_vec = Point3(local_tm.GetTrans());

          orig_vec = Point3(orig_local_tm.GetTrans());

          local_disp = new_vec - orig_vec;
          displacement.x = local_disp.x;
          displacement.y = local_disp.y;
          displacement.z = local_disp.z;
        }  // It's not time 0.
      }  // parent not a dummy

    }  // found a parent


    *out_Length = length;
    *out_Disp = displacement;

    return iResult;
  }  // MAX bone Node

  INodeTab childnodeTab;

  // MAKE SURE DisPLACEMENT INFO NOT NEEDED FOR THis NODE
  if(m_BoneNode->GetNodeLong() & NODE_NEEDS_DISPLACEMENT)
  {
    INode * parent = NULL;
    parent = m_BoneNode->GetParentNode();

    Matrix3 parent_tm = parent->GetNodeTM(curr_time);

    ///@todo:
    // ??? What about axes that are not X-aligned???  Note that biped nodes
    // with more than one child will always be so as per code in isBoneXAxisAligned,
    // which can't handle more than one child

    // Construct a translation matrix that will move the parent CS
    // along the X-axis by the length of its bone
    Point3 x_axis(parent_tm.GetRow(0));
    x_axis = Normalize(x_axis);
    x_axis *= m_ParentInfo->m_Length;
    Matrix3 translate(parent_tm);
    translate.Translate(x_axis);

    // Subtract the current world position of the bone from the end of its
    // parent m_BoneNode (same as bone_tm.GetTrans() - translate.GetTrans()).
    displacement = bone_tm.GetTrans() * Inverse(translate);
  }


  // COUNT CHILDREN THAT ARE BONES
  // When using dummy objects, this catches any dummy children and the
  // true root bone m_BoneNode.
  U32 child_count = m_BoneNode->NumberOfChildren();

  for(U32 child_idx = 0; child_idx < child_count; child_idx++)
  {
    INode * child_node = m_BoneNode->GetChildNode(child_idx);

    // BIPED BONE
    if ( IsBiped( child_node ) )
    {
      childnodeTab.Append(1, &child_node);
      continue;
    }

    // MAX BONE
    if( CheckID( child_node, BONE_OBJ_CLASSID ) )
    {
      childnodeTab.Append(1, &child_node);
      continue;
    }

    // DUMMY OBJECT
    if ( !CheckControlID( m_BoneNode, BIPBODY_CONTROL_CLASS_ID ) )
    {
      // Only look at dummy objects that are not children of the
      // biped root.  The only children of the root that are
      // related to Biped are the footsteps and the pelvis.
      if( CheckID( child_node, Class_ID( DUMMY_CLASS_ID, 0 ) ) )
      {
        childnodeTab.Append(1, &child_node);
        continue;
      }
    }
  }

  // This code assumes that bone lengths are defined by the lengths of their
  // children, not their parents as MAX bones do it...
  // IFX bones and Biped work in the former way, MAX bones in the latter.
  if(childnodeTab.Count() == 0)
  {
    // IF THis BONE is A LEAF BONE with no length.

    // LENGTH
    *out_Length = 0.0f;

    if (curr_time == 0)
    {
      *out_Length = length;
    }

    // DisPLACEMENT
    *out_Disp = displacement;
  }
  else if(childnodeTab.Count() == 1)
  {
    // IF CURR_BONE HAS ONLY ONE CHILD -> GET BONE LENGTH FROM ONLY CHILD BONE

    if( CheckControlID( m_BoneNode, BIPBODY_CONTROL_CLASS_ID ) )
    {
      // The center of gravity doesn't have any length, although it is the
      // top of the bone hierarchy.  It has at most a displacement.
      // WARNING - Bip01 really has *two* children - the footsteps and the
      //    pelvis.  The only reason this code works is because
      //    IsMAXBipedNode() above doesn't see the footsteps.  If it did,
      //    we'd need to move this code into the MULTICHILDREN case.

      length = 0;
      if (curr_time == 0)
      {
        m_Length = length;
      }

      Matrix3 groupTM = m_GroupNode->GetNodeTM(curr_time);
      Matrix3 local_tm = m_BoneNode->GetObjectTM(curr_time) * Inverse(groupTM);

      displacement = local_tm.GetTrans();

      // DisPLACEMENT
      *out_Disp = displacement;

      U32 child_count = m_BoneNode->NumberOfChildren();

      // Mark children as needing displacement calculations
      for (U32 chld_idx = 0; chld_idx < child_count; chld_idx++)
      {
        INode * child_node = m_BoneNode->GetChildNode(chld_idx);

        // The pelvis is purely displaced from the COG.  Its length should be defined
        // by its children.
        if(child_node && IsBiped(child_node))
        {
          long props = child_node->GetNodeLong();
          child_node->SetNodeLong(props | NODE_NEEDS_DISPLACEMENT);
        }
      }
    }
    else
    {
      // This isn't the biped root node.
      Matrix3 child_tm = childnodeTab[0]->GetNodeTM(curr_time);

      // length
      length = Length(Point3(bone_tm.GetTrans() - child_tm.GetTrans()));

      *out_Length = length;

      if (curr_time == 0)
      {
        m_Length = length;
      }
      *out_Disp = displacement;
    }
  }
  else
  {
    // MULTI-CHILDREN

    Point3 pivot(bone_tm.GetTrans());
    bool SHARE_SPACE = true;

    length = 10000.0f;

    // FIND OUT IF CHILDREN SHARE SAME WORLD SPACE
    // IF SO CAN JUST USE CHILD N TO DETERMINE BONE LENGTH
    for(U32 i = 0; i < childnodeTab.Count(); i++)
    {
      INode * child = childnodeTab[i];
      if(pivot != child->GetNodeTM(curr_time).GetTrans())
      {
        SHARE_SPACE = false;
      }
    }

    // IF EACH CHILD HAS SAME WORLD SPACE PIVOT -> GET LENGTH FROM CHILD BONE
    if(childnodeTab.Count() && SHARE_SPACE)
    {

      Matrix3 child_tm = childnodeTab[0]->GetNodeTM(curr_time);

      // LENGTH
      length = Length(Point3(bone_tm.GetTrans() - child_tm.GetTrans()));

      *out_Length = length;
      //      if (curr_time == 0)
      //      {
      //        *out_Length = length;
      //      }
      if (curr_time == 0)
      {
        m_Length = length;
      }

      // DisPLACEMENT
      *out_Disp = displacement;
    }

    // DIFFERENT CHILDREN PIVOTS
    else
    {
      // MARK CHILD NODES AS NEEDING DisPLACEMENTS ASSOCIATED WITH THEM

      for(U32 child_idx = 0; child_idx < childnodeTab.Count(); child_idx++)
      {
        // ONLY DOING DisPLACEMENT WITH BIPED NODES
        INode * child = childnodeTab[child_idx];

        if( child &&IsBiped(child) )
        {
          long props = child->GetNodeLong();
          //          child->SetNodeLong(props | NODE_NEEDS_DISPLACEMENT);///@todo fix it
        }

        // IF CHILD SAME TYPE -> GET MINIMUM LENGTH BETWEEN ALL CHILDREN AND PARENT
        if((bon_cont->ClassID() == child->GetTMController()->ClassID()) ||
          (m_BoneNode->GetObjectRef()->ClassID() == child->GetObjectRef()->ClassID()) ||
          (IsBiped(m_BoneNode) && CheckID(child, BONE_OBJ_CLASSID)))
        {
          length = min(length, Length(bone_tm.GetTrans() - child->GetNodeTM(curr_time).GetTrans()));
        }
        ///@todo
        // ADD LENGTH 0 HERE LATER

        else
        {

        }
      }

      if (curr_time == 0)
      {
        m_Length = length;
      }

      *out_Length = length;

      // DisPLACEMENT 0
      *out_Disp = displacement;
    }  // different child pivots
  }  // multiple children

  return iResult;

}
/*
/////////////////////////////////////////////////////////
// Function:  ComputeJoints
// Parameters:  The MAX node and the output file handle
// Purpose:  Writes the IK information for the MAX bones
// Return Val:  None
/////////////////////////////////////////////////////////
IFXRESULT IFXBonesExporter::ComputeJoints()
{
IFXRESULT iResult = IFX_OK;
InitJointData pos_joints;
InitJointData rot_joints;


// JOINT INFO
INode* bone_node = m_BoneNode;
if (!m_pMAXNodeUtils->IsMAXBipedNode(bone_node))
{
// Remember, MAX bones have their joint at the tip.  Our bones have
// the joint at the base.  So this code is necessary to export the
// correct constraints for the correct bone.
if(!m_pMAXNodeUtils->IsRootBone(bone_node))
{
bone_node = m_ParentNode;
}
}

// IF BIPED BONES -> NO IK CAN BE SET (we can't specify constraints)
// SO JUST RETURN DEFAULTS
if(m_pMAXNodeUtils->IsMAXBipedNode(bone_node))
{
//    m_uBoneAttributes = IFXSKELETON_ROTATIONCONSTRAINTACTIVE_X | IFXSKELETON_ROTATIONCONSTRAINTACTIVE_Y | IFXSKELETON_ROTATIONCONSTRAINTACTIVE_Z;
return iResult;
}

if (!m_pMAXNodeUtils->IsMAXBipedNode(bone_node))
{
Control * cont;
//    DebugPrint("Node is %s\n", bone_node->GetName());
U32 *pIKSlaveCont;
TSTR myclassname; //jr

m_pMAXNodeUtils->SelectNode(bone_node);
cont = bone_node->GetTMController();

// Looks for IK constraints, ignoring any specific Rotation or
// position controllers which (so far as we know) do not contain
// constraint data.  This includes subcontrollers (for rotation
// or position) of the IK controller for a bone -- such as the
// subcontrollers which are added when a bone is made an end effector.
if (cont)
{
// Export Rotation Constraints
GetCOREInterface()->SelectNode(bone_node);
bone_node->GetTMController()->GetClassName(myclassname);

// Can we copy IK rotation parameters with this controller
if (cont->CanCopyIKParams(COPYPASTE_IKROT))
{
// yes - is it an IK Controller?
if (strncmp(myclassname,"IK Controller",13 ) == 0 )
{
// it is - try to use the IKSlaveInterface
pIKSlaveCont = (U32 *)bone_node->GetTMController()->GetInterface(I_IKSLAVE);

if( pIKSlaveCont )
{
pIKSlaveCont = (U32 *)bone_node->GetTMController()->GetInterface(I_IKSLAVE);

IKSlaveClipObject *jrclip = (IKSlaveClipObject *)((Control *)pIKSlaveCont)->CopyIKParams(COPYPASTE_IKROT);

m_uBoneAttributes |= jrclip->flags & JOINT_ROT_ACTIVE_X ? IFXSKELETON_ROTATIONCONSTRAINTACTIVE_X : 0;
m_uBoneAttributes |= jrclip->flags & JOINT_ROT_ACTIVE_Y ? IFXSKELETON_ROTATIONCONSTRAINTACTIVE_Y : 0;
m_uBoneAttributes |= jrclip->flags & JOINT_ROT_ACTIVE_Z ? IFXSKELETON_ROTATIONCONSTRAINTACTIVE_Z : 0;

m_uBoneAttributes |= jrclip->flags & JOINT_ROT_LIMIT_X ? IFXSKELETON_ROTATIONCONSTRAINTLIMITED_X : 0;
m_uBoneAttributes |= jrclip->flags & JOINT_ROT_LIMIT_Y ? IFXSKELETON_ROTATIONCONSTRAINTLIMITED_Y : 0;
m_uBoneAttributes |= jrclip->flags & JOINT_ROT_LIMIT_Z ? IFXSKELETON_ROTATIONCONSTRAINTLIMITED_Z : 0;

m_fRotationConstraintXMax = jrclip->max[3];
m_fRotationConstraintXMin = jrclip->min[3];
m_fRotationConstraintYMax = jrclip->max[4];
m_fRotationConstraintYMin = jrclip->min[4];
m_fRotationConstraintZMax = jrclip->max[5];
m_fRotationConstraintZMin = jrclip->min[5];


} // got an IKSlaveInterface
else
{
m_uBoneAttributes = IFXSKELETON_ROTATIONCONSTRAINTACTIVE_X | IFXSKELETON_ROTATIONCONSTRAINTACTIVE_Y | IFXSKELETON_ROTATIONCONSTRAINTACTIVE_Z;
} // end IKSlave Interface
} // It is an IK Controller
else
{
// it's ! an IKSlave Controller - try to use the IKSlaceClipObject interface
IKSlaveClipObject *jrclip = (IKSlaveClipObject *)cont->CopyIKParams(COPYPASTE_IKROT);

m_uBoneAttributes |= jrclip->flags & JOINT_ROT_ACTIVE_X ? IFXSKELETON_ROTATIONCONSTRAINTACTIVE_X : 0;
m_uBoneAttributes |= jrclip->flags & JOINT_ROT_ACTIVE_Y ? IFXSKELETON_ROTATIONCONSTRAINTACTIVE_Y : 0;
m_uBoneAttributes |= jrclip->flags & JOINT_ROT_ACTIVE_Z ? IFXSKELETON_ROTATIONCONSTRAINTACTIVE_Z : 0;

m_uBoneAttributes |= jrclip->flags & JOINT_ROT_LIMIT_X ? IFXSKELETON_ROTATIONCONSTRAINTLIMITED_X : 0;
m_uBoneAttributes |= jrclip->flags & JOINT_ROT_LIMIT_Y ? IFXSKELETON_ROTATIONCONSTRAINTLIMITED_Y : 0;
m_uBoneAttributes |= jrclip->flags & JOINT_ROT_LIMIT_Z ? IFXSKELETON_ROTATIONCONSTRAINTLIMITED_Z : 0;

m_fRotationConstraintXMax = jrclip->max[3];
m_fRotationConstraintXMin = jrclip->min[3];
m_fRotationConstraintYMax = jrclip->max[4];
m_fRotationConstraintYMin = jrclip->min[4];
m_fRotationConstraintZMax = jrclip->max[5];
m_fRotationConstraintZMin = jrclip->min[5];

} // end it's not an IK Controller
} // end can copy
else
{
// There are no IK parameters to copy - give this joint
// no rotational constraints
m_uBoneAttributes = IFXSKELETON_ROTATIONCONSTRAINTACTIVE_X | IFXSKELETON_ROTATIONCONSTRAINTACTIVE_Y | IFXSKELETON_ROTATIONCONSTRAINTACTIVE_Z;
}
}  // we have a TM controller
}  // maxbone
else  // NEITHER MAX BONES OR BIPED BONES
{   m_uBoneAttributes = IFXSKELETON_ROTATIONCONSTRAINTACTIVE_X | IFXSKELETON_ROTATIONCONSTRAINTACTIVE_Y | IFXSKELETON_ROTATIONCONSTRAINTACTIVE_Z;

}
return iResult;
}
*/


/////////////////////////////////////////////////////////
// Function:  ComputeInitialTransform
// Purpose:  Get the transformation for the bone in its starting
//   position.
// Return Val:  None
/////////////////////////////////////////////////////////
void IFXBonesExporter::ComputeInitialTransform()
{

  Quat rot(1);

  Matrix3 localTM(1);
  Quat parent_delta(1);
  Quat curr_delta(1);
  IFXBonesExporter* parent_bone = m_ParentInfo;
  INode * parent_node = m_ParentNode;
  INode * bone_node  = m_BoneNode;

  // Get parent bone
  //  if (parent_node &&
  //    (IsMAXBipedNode(bone_node) || isDummyNode(bone_node) || isMaxBoneNode(bone_node)))
  //  {
  //    (m_ParentIdx != -1) ? (parent_bone = &m_BonesData.m_pBones[m_ParentIdx]) :
  //                  parent_bone = NULL;
  //  }


  // MAX BONE
  if (!IsBiped(bone_node))
  {
    // This code is correct only for getting rotations
    if(!IsRootBone(bone_node))
    {
      bone_node = parent_node;
      parent_node = bone_node->GetParentNode();
    }
  }


  Matrix3 node_tm = bone_node->GetNodeTM(0);

  // ROOT BONE ->ABSOLUTE WORLD VALUES
  if(!parent_bone || (m_ParentIdx == IFX_BONE_INVALID_INDEX))
  {
    if (IsBiped(m_BoneNode))
    {
      if (m_GroupNode == m_BoneNode)
      {
        rot = Quat(node_tm);
      }
      else
      {
        // If the root bone is not the head of the group
        // of nodes we are exporting, make sure we express
        // its rotation in terms of the head node.
        Matrix3 group_tm(1);
        Matrix3 local_tm(1);

        if (m_GroupNode)
          group_tm = m_GroupNode->GetNodeTM(0);

        local_tm = node_tm * Inverse(group_tm);

        rot = Quat(local_tm);
      }

      rot.Normalize();

      m_Rot[0] = rot.x;
      m_Rot[1] = rot.y;
      m_Rot[2] = rot.z;
      m_Rot[3] = rot.w;
    }

    ///@todo - what about the scale???
    return;
  }
  else  // NON-ROOT BONE -> RELATIVE TO PARENT
  {
    Matrix3 parent_tm = parent_node->GetNodeTM(0);

    // PARENT NEEDS TO ROTATE TO BE X-ALIGNED FIRST
    // Apply the rotation to the parent transform
    // to make the parent bone axis aligned.
    if(parent_bone->m_btmModified)
    {
      parent_delta = Quat(parent_bone->m_DeltaRot[0], parent_bone->m_DeltaRot[1],
        parent_bone->m_DeltaRot[2], parent_bone->m_DeltaRot[3]);
      ParentAbsolute(parent_tm, parent_delta);
    }

    // CURRENT BONE NEEDS TO BE ROTATED TO BE X-AXis ALIGNED
    // apply the rotation to the current bone to make it axis alignes.
    if(m_btmModified)
    {
      curr_delta = Quat(m_DeltaRot[0],m_DeltaRot[1], m_DeltaRot[2],  m_DeltaRot[3]);
      ParentAbsolute(node_tm, curr_delta);
    }

    // IF USER WANTS TRANSFORMS RELATIVE TO PARENT
    // GET CURRENT TRANSFORM RELATIVE TO THESE _POSSIBLY_ ALTERED FRAMES
    localTM = node_tm * Inverse(parent_tm);

    AffineParts parts;

    MatrixFixup(localTM);  // Avoid degeneracy in algorithm
    decomp_affine(localTM, &parts);

    rot = parts.q;  // The rotation quaternion.

    // QUAT
    m_Rot[0] = rot.x;
    m_Rot[1] = rot.y;
    m_Rot[2] = rot.z;
    m_Rot[3] = rot.w;
    m_Rot.Normalize();

    ///@todo - what about the scale???

  }  // Non-root bone
}

//---------------------------------------------------------------------------
//  IFXBonesExporter::ComputeBoneInfo
//
//
//---------------------------------------------------------------------------
IFXRESULT IFXBonesExporter::ComputeBoneInfo()
{
  IFXRESULT iResult = IFX_OK;

  if(!m_bInit)
  {
    iResult = E_UNEXPECTED;
  }

  // Axis Align
  if(SUCCEEDED(iResult))
  {
    iResult = AxisAlign();
  }


  if(SUCCEEDED(iResult))
  {
    // BONE LENGTH AND DisPLACEMENT
    iResult = CalcLengthDisp(0, &m_Disp, &m_Length);
  }
  /*
  if(SUCCEEDED(iResult))
  {
  iResult = ComputeJoints();
  }
  */
  return iResult;
}

//---------------------------------------------------------------------------
//  MAXBoneConversionInfo::AxisAlign
//
//
//---------------------------------------------------------------------------
IFXRESULT IFXBonesExporter::AxisAlign()
{
  IFXRESULT iResult = IFX_OK;

  if(!m_bInit)
  {
    iResult = E_UNEXPECTED;
  }


  U32 num_children = m_BoneNode->NumberOfChildren();
  U32 child_idx;
  INode * child_node = NULL;
  Matrix3 bone_mat(1);

  //  DBGOUTFMT((__FILE__, __LINE__, "Parent is %s\nBone is %s\n", m_ParentNode->GetName(),
  //    m_BoneNode->GetName()));

  //  IFXTRACE_GENERIC("Bone is %s,  Parent is %s\n", m_BoneNode->GetName(), m_ParentNode->GetName());


  // BIPED BONE
  if(num_children)
  {
    // REFERENCE CHILD NODE
    child_node = m_BoneNode->GetChildNode(0);

    if(IsBiped(child_node))
    {

      // IF BONE HAS ONLY ONE CHILD -> COMPARE MATRIX X-AXis WITH VECTOR FORMED BY
      // LINK TO CHILD
      if(num_children == 1)
      {

        bone_mat = m_BoneNode->GetNodeTM(0);
        Matrix3 child_mat = child_node->GetNodeTM(0);
        Point3 bone_vec = child_mat.GetTrans() - bone_mat.GetTrans();

        // If this vector is nearly zero (bone tiny), this bone will have the
        // rotation of its parent (we ignore its properties).
        if (Length(bone_vec) < SMALLEST_BONE)
        {
          m_btmModified = false;
          return iResult;
        }

        bone_vec = Normalize(bone_vec);
        Point3 x_vec = Normalize(bone_mat.GetRow(0));
        F32 delta = AngleBetweenVectors(&bone_vec, &x_vec);

        // IF ANGLES OFF BY MORE THAN ONE DEGREE
        if(delta > (ONE_DEGREE_RAD*0.01))
        {
          Point3 z_axis, y_axis;

          if (delta < (PI-(ONE_DEGREE_RAD*0.01)))
          {
            // CALCULATE PERPENDICULAR VECTORS FOR NEW X-AXis ALIGNED MATRIX
            z_axis = bone_vec - x_vec;
            z_axis = Normalize(z_axis);
          }
          else
          {
            // Let's not have a zero Z-axis if we are almost aligned to the x-axis
            z_axis = Point3(0.0f, 0.0f, 1.0f);
          }

          y_axis = z_axis ^ bone_vec;
          y_axis = Normalize(y_axis);

          // Now re-build the Z axis so that the three vectors are truly orthogonal
          z_axis = bone_vec ^ y_axis;
          z_axis = Normalize(z_axis);

          Matrix3 rot_mat(1);
          rot_mat.SetRow(0, bone_vec);
          rot_mat.SetRow(1, y_axis);
          rot_mat.SetRow(2, z_axis);
          rot_mat.SetTrans(bone_mat.GetTrans());
          rot_mat.NoScale();

          // CALCULATE OFFSET MATRIX TO ROTATE INTO X-ALIGNED SPACE
          Matrix3 loc_mat = rot_mat * Inverse(bone_mat);

          // CREATE QUAT TO ROTATE INTO PLACE
          AffineParts parts;

          MatrixFixup(loc_mat);  // Avoid degeneracy in algorithm
          decomp_affine(loc_mat, &parts);

          m_DeltaRot = parts.q;  // The rotation quaternion.

          m_btmModified = true;
          return iResult;
        }
      }

      // MORE THAN ONE CHILD SO LOOK FOR SIMILAR CHILD TYPES (IE MAX BONES -> MAX BONES)
      if(num_children > 1)
      {

        Class_ID id = m_BoneNode->GetObjectRef()->ClassID();

        U32 similar_type_count = 0;
        U32 similar_type_idx = 0;

        // COUNT TOTAL CHILDREN OF SIMILAR TYPE
        for(child_idx = 0; child_idx < num_children; child_idx++)
        {
          ///@todo : MORE RELIABLE CHECK
          if(id == m_BoneNode->GetChildNode(child_idx)->GetObjectRef()->ClassID())
          {
            ++similar_type_count;
            similar_type_idx = child_idx;
          }
        }

        // IF CURRENT NODE HAS ONLY ONE CHILD OF THE SAME TYPE -> USE THis CHILD AS
        // BONE ENDPOINT TO CALCULATE X-ALIGNMENT
        if(similar_type_count == 1)
        {

          bone_mat = m_BoneNode->GetNodeTM(0);

          Matrix3 child_mat = m_BoneNode->GetChildNode(similar_type_idx)->GetNodeTM(0);

          Point3 bone_vec = child_mat.GetTrans() - bone_mat.GetTrans();

          // If this vector is nearly zero (bone tiny), this bone will have the
          // rotation of its parent (we ignore its properties).
          if (Length(bone_vec) < SMALLEST_BONE)
          {
            m_btmModified = false;
            return iResult;
          }

          bone_vec = Normalize(bone_vec);

          Point3 x_vec = Normalize(bone_mat.GetRow(0));

          F32 delta = AngleBetweenVectors(&bone_vec, &x_vec);

          // IF ANGLE GREATER THAN ONE DEGREE THEN NEED TO CALCULATE ROTATION OFFSET
          if(delta > (ONE_DEGREE_RAD*0.01))
          {
            Point3 z_axis, y_axis;

            if (delta < (PI-(ONE_DEGREE_RAD*0.01)))
            {
              // CALCULATE PERPENDICULAR VECTORS FOR NEW X-AXis ALIGNED MATRIX
              z_axis = bone_vec - x_vec;
              z_axis = Normalize(z_axis);
            }
            else
            {
              // Let's not have a zero Z-axis if we are almost aligned to the x-axis
              z_axis = Point3(0.0f, 0.0f, 1.0f);
            }

            y_axis = z_axis ^ bone_vec;
            y_axis = Normalize(y_axis);

            // Now re-build the Z axis so that the three vectors are truly orthogonal
            z_axis = bone_vec ^ y_axis;
            z_axis = Normalize(z_axis);

            Matrix3 rot_mat(1);
            rot_mat.SetRow(0, bone_vec);
            rot_mat.SetRow(1, y_axis);
            rot_mat.SetRow(2, z_axis);
            rot_mat.SetTrans(bone_mat.GetTrans());
            rot_mat.NoScale();

            Matrix3 loc_mat = rot_mat * Inverse(bone_mat);

            AffineParts parts;

            MatrixFixup(loc_mat);  // Avoid degeneracy in algorithm
            decomp_affine(loc_mat, &parts);

            m_DeltaRot = parts.q;  // The rotation quaternion.
            m_btmModified =  true;
            return iResult;
          }  // non-zero angle
        }  // similar count == 1
        ///@todo  - handle the case where similar_type_count > 1

      }  // more than one child
    }  // Biped m_BoneNode
  }  // num_children

  // MAX BONE
  if (!IsBiped(m_BoneNode))
  {
    // For a top-level m_BoneNode, there is no bone (in MAX, defined by vector
    // between m_BoneNode and its parent).  Also, by definition root-level
    // bones have no rotation (being defined in world space).  Thus there will
    // be no axis alignment for these suckers.
    if (IFX_BONE_INVALID_INDEX == m_ParentIdx)
    {
      return iResult;
    }

    bone_mat = m_BoneNode->GetNodeTM(0);

    Matrix3 parent_mat = m_ParentNode->GetNodeTM(0);

    MatrixFixup(bone_mat);
    MatrixFixup(parent_mat);

    // bone vector in world space.
    Point3 bone_vec = bone_mat.GetTrans() - parent_mat.GetTrans();

    // If this bone has a vanishingly small length, we still need to check
    // to see if it has any odd rotations happening to it.  Can't just
    // assume it is X-axis aligned.
    F32 b_length = Length(bone_vec);
    if (b_length < SMALLEST_BONE)
    {
      AffineParts parts;
      Quat no_rot_quat(0.0f, 0.0f, 0.0f, 1.0f);
      Matrix3 loc_mat = bone_mat * Inverse(parent_mat);

      MatrixFixup(loc_mat);  // Avoid degeneracy in algorithm
      decomp_affine(loc_mat, &parts);

      if ((parts.q.x != no_rot_quat.x) || (parts.q.y != no_rot_quat.y) ||
        (parts.q.z != no_rot_quat.z) || (parts.q.w != no_rot_quat.w))
      {
        // We have some sort of rotation happening here.
        // Record it.
        m_DeltaRot = parts.q;
        m_btmModified = true;  // WARNING - this claims we are not X-aligned,
        // but we are.  In this case we are signaling
        // the presence of an odd rotation...
        return iResult;
      }
      m_btmModified = false;
      return iResult;

    }

    bone_vec = Normalize(bone_vec);

    // The x-axis of the bone.
    Point3 x_vec = Normalize(parent_mat.GetRow(0));

    F32 delta = AngleBetweenVectors(&bone_vec, &x_vec);

    // IF ANGLES OFF BY MORE THAN ONE DEGREE
    if(delta > (ONE_DEGREE_RAD*0.01))
    {
      Point3 z_axis, y_axis;

      if (delta < (PI-(ONE_DEGREE_RAD*0.01)))
      {
        // CALCULATE PERPENDICULAR VECTORS FOR NEW X-AXis ALIGNED MATRIX
        z_axis = bone_vec - x_vec;
        z_axis = Normalize(z_axis);
      }
      else
      {
        // Let's not have a zero Z-axis if we are almost aligned to the x-axis
        z_axis = Point3(0.0f, 0.0f, 1.0f);
      }

      y_axis = z_axis ^ bone_vec;
      y_axis = Normalize(y_axis);

      // Now re-build the Z axis so that the three vectors are truly orthogonal
      z_axis = bone_vec ^ y_axis;
      z_axis = Normalize(z_axis);

      Matrix3 rot_mat(1);
      rot_mat.SetRow(0, bone_vec);
      rot_mat.SetRow(1, y_axis);
      rot_mat.SetRow(2, z_axis);
      rot_mat.SetTrans(parent_mat.GetTrans());  // world position of parent
      rot_mat.NoScale();

      // CALCULATE OFFSET MATRIX TO ROTATE INTO X-ALIGNED SPACE
      MatrixFixup(rot_mat);
      Matrix3 loc_mat = rot_mat * Inverse(parent_mat);

      // CREATE QUAT TO ROTATE INTO PLACE
      AffineParts parts;

      MatrixFixup(loc_mat);  // Avoid degeneracy in algorithm
      decomp_affine(loc_mat, &parts);

      m_DeltaRot = parts.q;  // The rotation quaternion.
      m_btmModified = true;
      return iResult;
    }  // X-axis aligned by more than a degree
    else
    {
      // Just because we are X-axis aligned doesn't mean that we aren't
      // rotated about the X-axis.  Need to handle that case for the
      // root m_BoneNode...
      if (IFX_BONE_INVALID_INDEX == m_ParentIdx)
      {
        ///@todo - is this valid - seems to me like it isn't in model space.  This
        // may work differently for Biped and MAX Bone skeletons...

        ///@todo - make sure this is a model-space, not world-space test.
        AffineParts parts;
        Quat no_rot_quat(0.0f, 0.0f, 0.0f, 1.0f);

        MatrixFixup(bone_mat);  // Avoid degeneracy in algorithm
        decomp_affine(bone_mat, &parts);

        if ((parts.q.x != no_rot_quat.x) || (parts.q.y != no_rot_quat.y) ||
          (parts.q.z != no_rot_quat.z) || (parts.q.w != no_rot_quat.w))
        {
          // Some sort of rotation is happening in world space.  Check
          // the local transformation to see if we need to worry.
          Matrix3 loc_mat = bone_mat * Inverse(parent_mat);

          MatrixFixup(loc_mat);  // Avoid degeneracy in algorithm
          decomp_affine(loc_mat, &parts);

          if ((parts.q.x != no_rot_quat.x) || (parts.q.y != no_rot_quat.y) ||
            (parts.q.z != no_rot_quat.z) || (parts.q.w != no_rot_quat.w))
          {
            // We have some sort of rotation happening here.
            // Record it.
            m_DeltaRot = parts.q;
            m_btmModified = true;  // WARNING - this claims we are not X-aligned,
            // but we are.  In this case we are signaling
            // the presence of an odd rotation...
          }
        } // World rotation happening
      }  // Root m_BoneNode needs help
    }  // Angle less than a degree
  } // MAX bone

  return iResult;
}


/////////////////////////////////////////////////////////
// Function:  GetBoneAnimData
// Parameters:  The MAX bone for which to collect information,
//     the index of the bone in our scene information table,
//     and the MAX interval associated with that bone.
// Purpose:  Allocates memory for bone keyframe data
// Return Val:  TRUE if all OK, FALSE if we had a memory allocation failure.
/////////////////////////////////////////////////////////
IFXRESULT IFXBonesExporter::CollectBoneAnimData( Quat& rot, Point3& bonePos, Point3& boneScale, F32 time, TimeValue initTime, IFXWriter* pW )
{
  IFXRESULT iResult = IFX_OK;

  F32 Length;
  INode* pBone = m_BoneNode;

  Matrix3 bone_tm(1);
  Matrix3 group_tm(1);
  Matrix3 node_tm(1);

  Point3 initBonePos;
  Quat initRot(1);
  CalcLengthDisp(0, &initBonePos, &Length);
  initRot = m_Rot;
  // DISPLACEMENT
  if( !IsRootBone( pBone ) )
    CalcLengthDisp(time, &bonePos, &Length);
  else
  {
    // Root node - displacement is in reference to its position at time zero

    if ( pBone != m_GroupNode )
    {
      // The root bone is not the top-level node in this collection of
      // nodes we are converting into a single model resource.  Thus,
      // we must animate it with respect to the group node at this
      // time, rather than its location at time zero.
      bone_tm = pBone->GetNodeTM(time);
      group_tm = m_GroupNode->GetNodeTM(time);///@todo: should it be root?
    }
    else
    {
      // If the root bone is also the top-level node in this collection
      // of nodes, we animate it from its position at the animation start.
      // NOTE - this will conflict with keyframe animation on this node.
      // If you want to keyframe a collection of bones, make a dummy
      // node the root bone's parent.
      bone_tm = pBone->GetNodeTM(time);
      group_tm = pBone->GetNodeTM(initTime);
    }

    bone_tm = bone_tm * Inverse(group_tm);
    bonePos = bone_tm.GetTrans();
  }

  bonePos -= initBonePos;

  // CURRENT TM
  node_tm = pBone->GetNodeTM(time) * Inverse(pBone->GetObjectTM(0));

  MatrixFixup(node_tm);
  AffineParts parts;
  decomp_affine(node_tm, &parts);

  // Write out the rotations for bones that are not the root bone, or are
  // Biped root bones.  MAX root bones are treated differently since they
  // have zero length and are used as the root for their children.
  if ( !IsRootBone( pBone ) || ( IsRootBone( pBone ) && ( IsBiped( pBone ) ) ) )
    rot = parts.q;

  boneScale.x = 1.0f;
  boneScale.y = 1.0f;
  boneScale.z = 1.0f;

  return iResult;
}
