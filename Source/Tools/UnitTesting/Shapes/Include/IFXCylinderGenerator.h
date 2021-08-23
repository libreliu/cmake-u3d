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
//  IFXCylinderGenerator.h
//
//  DESCRIPTION
//      IFXCylinderGenerator interface.
//
//  NOTES
//
//
//***************************************************************************

/**
  @file IFXCylinderGenerator.h

      This header defines the IFXCylinderGenerator class and its functionality.
*/

#ifndef __IFXCYLINDERGENERATOR_H__
#define __IFXCYLINDERGENERATOR_H__

#include "IFXUnknown.h"
#include "IFXPrimitive.h"

// {02256F9F-F42C-4e48-8742-9286FEF651EF}
IFXDEFINE_GUID(IID_IFXCylinderGenerator,
0x2256f9f, 0xf42c, 0x4e48, 0x87, 0x42, 0x92, 0x86, 0xfe, 0xf6, 0x51, 0xef);

/**
  This is the main interface for IFXCylinderGenerator.

  @note The associated IID is named IID_IFXCylinderGenerator.
*/
class IFXCylinderGenerator : virtual public IFXPrimitive
{
public:

  /**
    This method returns the radius of the cylinder's top.

    @return Returns a F32 representing the cylinder's top.\n
  */
  virtual F32       GetTopRadius( void )=0;

  /**
    This method sets the radius of the cylinder's top.

    @param  fInRadius  A F32 representing the top radius of the cylinder.\n

    @return IFX_OK
  */
  virtual IFXRESULT SetTopRadius( F32 fInRadius )=0;

  /**
    This method returns the radius of the cylinder's bottom.

    @return Returns a F32 representing the cylinder's bottom.\n
  */

  virtual F32       GetBottomRadius( void )=0;

  /**
    This method sets the radius of the cylinder's bottom.

    @param  fInRadius  A F32 representing the bottom radius of the cylinder.\n

    @return IFX_OK
  */
  virtual IFXRESULT SetBottomRadius( F32 fInRadius )=0;

  /**
    This method returns the height of the cylinder.

    @return Returns a F32 representing the cylinder's height.\n
  */
  virtual F32       GetHeight( void )=0;

  /**
    This method sets the height of the cylinder's.

    @param  fInHeight  A F32 representing the height radius of the cylinder.\n

    @return IFX_OK
  */
  virtual IFXRESULT SetHeight( F32 fInHeight )=0;

  /**
    This method returns the start angle of the cylinder in the range
    [0-360].

    @return Returns a F32 representing the cylinder's start angle.\n
  */
  virtual F32       GetStartAngle( void )=0;

  /**
    This method sets the start angle of the cylinder in the range
    [0-360].

    @param  fInStartAngle  A F32 representing the start angle of
                 the cylinder.\n

    @return IFX_OK
  */
  virtual IFXRESULT SetStartAngle( F32 fInStartAngle )=0;

  /**
    This method returns the end angle of the cylinder in the range [0-360].

    @return Returns a F32 representing the cylinder's end angle.\n
  */
  virtual F32       GetEndAngle( void )=0;

  /**
    This method sets the end angle of the cylinder in the range
    [0-360].

    @param  fInendAngle  A F32 representing the end angle of
               the cylinder.\n

    @return IFX_OK
  */
  virtual IFXRESULT SetEndAngle( F32 fInEndAngle )=0;

  /**
    This method returns a flag stating if the cylinder as a top.

    @return Returns a BOOL.\n

    - @b TRUE - The cylinder has a top.\n

    - @b FALSE  - The cylinder does not have a top.
  */
  virtual BOOL    GetTopCap (void) = 0;

  /**
    This method sets the BOOL for the top of the cylinder.

    @param  bInTopCap A BOOL representing if the cylinder has a top.\n

    - @b TRUE - The cylinder has a top.\n

    - @b FALSE  - The cylinder does not have a top.

    @return IFX_OK.\n

  */
  virtual IFXRESULT SetTopCap (BOOL bInTopCap) = 0;

  /**
    This method returns a flag stating if the cylinder as a bottom.

    @return Returns a BOOL.\n

    - @b TRUE - The cylinder has a bottom.\n

    - @b FALSE  - The cylinder does not have a bottom.
  */
  virtual BOOL    GetBottomCap (void) = 0;

  /**
    This method sets the BOOL for the bottom of the cylinder.

    @param  bInBottomCap  A BOOL representing if the cylinder has a bottom.\n

    - @b TRUE - The cylinder has a bottom.\n

    - @b FALSE  - The cylinder does not have a bottom.

    @return IFX_OK.\n

  */
  virtual IFXRESULT SetBottomCap (BOOL bInBottomCap) = 0;

  /**
    This method returns the number of segments the cylinder has
    along it's main axis.

    @return Returns a U32 representing the number of segments the
        cylinder has.\n
  */
  virtual U32     GetNumSegments (void) = 0;

  /**
    This method sets the segments of the cylinder.

    @param  iNumSegments  A U32 representing number of segments the
                cylinder along its main axis.\n
    @return IFX_OK.\n
  */
  virtual IFXRESULT SetNumSegments (U32 iNumSegments) = 0;

  /**
    This method returns the resolution of the cylinder along its
    circumference.

    @return Returns a U32 representing the number of segments the
        cylinder has around it's circumference.\.\n
  */
  virtual U32     GetResolution (void) = 0;

  /**
    This method sets the resolution of the cylinder.

    @param  iResolution   A U32 representing number of segments the
                cylinder along its circumference.\n
    @return IFX_OK.\n
  */
  virtual IFXRESULT SetResolution (U32 iResolution) = 0;
};

#endif
