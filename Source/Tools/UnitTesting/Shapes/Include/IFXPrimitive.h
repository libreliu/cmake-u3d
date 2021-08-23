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
//
//  IFXPrimitive.h
//
//  DESCRIPTION
//      IFXPrimitive interface.
//
//  NOTES
//
//
//***************************************************************************

/**
  @file IFXPrimitive.h

      This header file defines the IFXPrimitive class and its functionality.
*/

#ifndef __IFXPRIMITIVE_H__
#define __IFXPRIMITIVE_H__

#include "IFXUnknown.h"
#include "IFXModifier.h"
#include "IFXGenerator.h"
#include "IFXVector4.h"
#include "IFXMatrix4x4.h"

// {0B80FB31-7491-11d4-A14C-00A0C9A0F93B}
IFXDEFINE_GUID(IID_IFXPrimitive,
0xb80fb31, 0x7491, 0x11d4, 0xa1, 0x4c, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

/**
  This is the main interface for IFXPrimitive.

  @note The associated IID is named IID_IFXPrimitive.
*/

class IFXPrimitive : virtual public IFXGenerator

{
public:

  /**
  An enumeration describing which side of the primitive will be rendered:\n

  @b EIPrimitiveFacing

  - @b eFrontFacing - Front faces rendered.
  - @b eBackFacing  - Back faces rendered.
  - @b eBothFacing  - Both front and back faces rendered.
  */
  enum EIPrimitiveFacing
  {
    eFrontFacing,
    eBackFacing,
    eBothFacing
  };

  /**
    This method returns the facing of the primitive.

    @param  pFacing   An EIPrimitiveFacing pointer to the facing.

    - @b eFrontFacing - Front faces rendered.
    - @b eBackFacing  - Back faces rendered.
    - @b eBothFacing  - Both front and back faces rendered.

    @return IFXRESULT\n

    - @b IFX_OK         - No error.
    - @b IFX_E_INVALID_POINTER  - Invalid pointer passed in.
  */
  virtual IFXRESULT IFXAPI	GetFacing(EIPrimitiveFacing* pFacing)=0;

  /**
    This method sets the facing of the primitive.

    @param  eFacing   EIPrimitiveFacing.

    - @b eFrontFacing - Front faces rendered.
    - @b eBackFacing  - Back faces rendered.
    - @b eBothFacing  - Both front and back faces rendered.

    @return IFXRESULT

    - @b IFX_OK - No problem.
  */
  virtual IFXRESULT IFXAPI	SetFacing(EIPrimitiveFacing eFacing)=0;

  /**
  The following accessors are needed to control the creation
  orientation which changed midway through the project to resolve
  consistency issues.  Support of both orientation was critical to
  ensure that existing content need not be changed.\n

  EIPrimitiveOrientation:\n

  - @b eUninitialized
  - @b enegZFrontposYup
  - @b eposZFrontposYup
  */
  enum EIPrimitiveOrientation
  {
    eUninitialized,
    enegZFrontposYup,
    eposZFrontposYup
  };

  /**
    This method returns the orientation of the primitive.

    @return EIPrimitiveOrientation\n

    - @b eUninitialized
    - @b enegZFrontposYup
    - @b eposZFrontposYup
  */
  virtual EIPrimitiveOrientation IFXAPI	GetOrientation(void)=0;

  /**
    This method sets the orientation of the primitive.

    @param  eInOrientation  Primitive orientation\n

    - @b eUninitialized
    - @b enegZFrontposYup
    - @b eposZFrontposYup
  */
  virtual void IFXAPI	SetOrientation(EIPrimitiveOrientation eInOrientation)=0;

  /**
    This method returns the bounding sphere of the primitive.

    @return IFXVector4  A reference to the primitive's boundingsphere.
  */
  virtual const IFXVector4& GetBoundingSphere()=0;

  /**
    This method sets the bounding sphere of the primitive.

    @param  vInBoundingSphere An IFXVector4 reference to the boundingsphere.

    @return IFXRESULT\n

    - @b IFX_OK         - No problem.
    - @b IFX_E_INVALIDE_RANGE - Internal modifierDataPacket range is invalid.
  */
  virtual IFXRESULT IFXAPI	SetBoundingSphere(const IFXVector4& vInBoundingSphere)=0;

  /**
    This method returns the transform of the primitive.

    @return IFIFXMatrix4x4XVector4  A reference to the primitive's transform.
  */
  virtual const IFXMatrix4x4& IFXAPI GetTransform()=0;

  /**
    This method sets the transform of the primitive.

    @param  tInTransform  An IFXMatrix4x4 reference to the transform.

    @return IFXRESULT\n

    - @b IFX_OK         - No problem.
    - @b IFX_E_INVALIDE_RANGE - Internal modifierDataPacket range is invalid.
  */
  virtual IFXRESULT IFXAPI SetTransform(const IFXMatrix4x4& tInTransform)=0;
};

#endif
