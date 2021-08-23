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
//  IFXSphereGenerator.h
//
//  DESCRIPTION
//      IFXSphereGenerator interface.
//
//  NOTES
//
//
//***************************************************************************

/**
  @file IFXSphereGenerator.h

      This header defines the IFXSphereGenerator class and its functionality.
*/
#ifndef __IFXSPHEREGENERATOR_H__
#define __IFXSPHEREGENERATOR_H__

#include "IFXUnknown.h"
#include "IFXPrimitive.h"
#include "IFXMesh.h"

// {FC01DA05-0C4A-11d4-8479-00A0C939B104}
IFXDEFINE_GUID(IID_IFXSphereGenerator,
0xfc01da05, 0xc4a, 0x11d4, 0x84, 0x79, 0x0, 0xa0, 0xc9, 0x39, 0xb1, 0x4);

/**
  This is the main interface for IFXSphereGenerator.

  @note The associated IID is named IID_IFXSphereGenerator.
*/
class IFXSphereGenerator : virtual public IFXPrimitive
{
public:

  /**
    This method returns the radius of the sphere.

    @return Returns a F32 representing the sphere's top.\n
  */
  virtual F32       GetRadius( void )=0;

  /**
    This method sets the radius of the sphere's top.

    @param  fInRadius  A F32 representing the radius of the sphere.\n

    @return IFX_OK
  */
  virtual IFXRESULT SetRadius( F32 fInRadius )=0;

  /**
    This method returns the start angle of the sphere in the range
    [0-360].

    @return Returns a F32 representing the sphere's start angle.\n
  */
  virtual F32       GetStartAngle( void )=0;

  /**
    This method sets the start angle of the sphere in the range
    [0-360].

    @param  fInStartAngle  A F32 representing the start angle of
                 the sphere.\n

    @return IFX_OK
  */
  virtual IFXRESULT SetStartAngle( F32 fInStartAngle )=0;

  /**
    This method returns the end angle of the sphere in the range [0-360].

    @return Returns a F32 representing the sphere's end angle.\n
  */
  virtual F32       GetEndAngle( void )=0;

  /**
    This method sets the end angle of the sphere in the range
    [0-360].

    @param  fInendAngle  A F32 representing the end angle of
               the sphere.\n

    @return IFX_OK
  */
  virtual IFXRESULT SetEndAngle( F32 fInEndAngle )=0;

  /**
    This method returns the resolution of the sphere along its
    circumference.

    @return Returns a U32 representing the number of segments the
        sphere has around it's circumference.\.\n
  */
  virtual U32       GetResolution ( void )=0;

  /**
    This method sets the resolution of the sphere.

    @param  uResolution   A U32 representing number of segments the
                sphere along its circumference.\n
    @return IFX_OK.\n
  */
  virtual IFXRESULT SetResolution( U32 uResolution )=0;

    /**
    This method initializes geometry of the sphere for using it as
        a bound.
  */
    virtual void InitAsBound( void )=0;

    /**
    This method returns pointer to sphere's mesh.
  */
    virtual IFXRESULT GetMesh( IFXMesh** pMesh )=0;
};


#endif
