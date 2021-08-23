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
//  IFXBoxGenerator.h
//
//  DESCRIPTION
//      IFXBoxGenerator interface.
//
//  NOTES
//
//
//***************************************************************************

/**
  @file IFXBoxGenerator.h

      This header defines the IFXBoxGenerator class and its functionality.
*/

#ifndef __IFXBOXGENERATOR_H__
#define __IFXBOXGENERATOR_H__

#include "IFXUnknown.h"
#include "IFXPrimitive.h"

IFXDEFINE_GUID(IID_IFXBoxGenerator,
0x6e954697, 0x8c1e, 0x4821, 0x85, 0x1e, 0x94, 0x22, 0x8, 0x5a, 0x97, 0x25);

/**
  This is the main interface for IFXBoxGenerator.

  @note The associated IID is named IID_IFXBoxGenerator.
*/
class IFXBoxGenerator : virtual public IFXPrimitive
{
public:

  /**
    This method returns TRUE if the box contains a top and FALSE otherwise.

    @return Returns a BOOL.\n

    - @b TRUE - The box has a top.\n

    - @b FALSE  - The box does not have a top.
  */
  virtual BOOL    GetTop (void) = 0;

  /**
    This method sets the BOOL for the top of the box.

    @param  bInTop  A BOOL representing if the box has a top.\n

    - @b TRUE - The box has a top.\n

    - @b FALSE  - The box does not have a top.

    @return IFX_OK.\n

  */
  virtual IFXRESULT SetTop (BOOL bInTop) = 0;

  /**
    This method returns TRUE if the box contains a bottom and FALSE otherwise.

    @return Returns a BOOL.\n

    - @b TRUE - The box has a bottom.\n

    - @b FALSE  - The box does not have a bottom.
  */
  virtual BOOL    GetBottom (void) = 0;

  /**
    This method sets the BOOL for the bottom of the box.

    @param  bInTop  A BOOL representing if the box has a bottom.\n

    - @b TRUE - The box has a bottom.\n

    - @b FALSE  - The box does not have a bottom.

    @return IFX_OK.\n

  */
  virtual IFXRESULT SetBottom (BOOL bInBottom) = 0;

  /**
    This method returns TRUE if the box contains a front and FALSE otherwise.

    @return Returns a BOOL.\n

    - @b TRUE - The box has a front.\n

    - @b FALSE  - The box does not have a front.
  */
  virtual BOOL    GetFront (void) = 0;

  /**
    This method sets the BOOL for the front of the box.

    @param  bInTop  A BOOL representing if the box has a front.\n

    - @b TRUE - The box has a front.\n

    - @b FALSE  - The box does not have a front.

    @return IFX_OK.\n

  */
  virtual IFXRESULT SetFront (BOOL bInFront) = 0;

  /**
    This method returns TRUE if the box contains a back and FALSE otherwise.

    @return Returns a BOOL.\n

    - @b TRUE - The box has a back.\n

    - @b FALSE  - The box does not have a back.
  */
  virtual BOOL    GetBack (void) = 0;

  /**
    This method sets the BOOL for the back of the box.

    @param  bInTop  A BOOL representing if the box has a back.\n

    - @b TRUE - The box has a back.\n

    - @b FALSE  - The box does not have a back.

    @return IFX_OK.\n

  */
  virtual IFXRESULT SetBack (BOOL bInBack) = 0;

  /**
    This method returns TRUE if the box contains a left side and FALSE otherwise.

    @return Returns a BOOL.\n

    - @b TRUE - The box has a left side.\n

    - @b FALSE  - The box does not have a left side.
  */
  virtual BOOL    GetLeft (void) = 0;

  /**
    This method sets the BOOL for the left side of the box.

    @param  bInTop  A BOOL representing if the box has a left side.\n

    - @b TRUE - The box has a left side.\n

    - @b FALSE  - The box does not have a left side.

    @return IFX_OK.\n

  */
  virtual IFXRESULT SetLeft (BOOL bInLeft) = 0;

  /**
    This method returns TRUE if the box contains a right side and FALSE otherwise.

    @return Returns a BOOL.\n

    - @b TRUE - The box has a right side.\n

    - @b FALSE  - The box does not have a right side.
  */
  virtual BOOL    GetRight (void) = 0;

  /**
    This method sets the BOOL for the right side of the box.

    @param  bInTop  A BOOL representing if the box has a right side.\n

    - @b TRUE - The box has a right side.\n

    - @b FALSE  - The box does not have a right side.

    @return IFX_OK.\n

  */
  virtual IFXRESULT SetRight (BOOL bInRight) = 0;


  /**
    This method returns the height of the box.

    @return Returns a F32 representing the height of the box.\n
  */
  virtual F32     GetHeight (void) = 0;

  /**
    This method sets the height of the box.

    @param  fInHeight  A F32 representing the height of the box.\n

    @return IFX_OK
  */
  virtual IFXRESULT SetHeight (F32 fInHeight) = 0;

  /**
    This method returns the width of the box.

    @return Returns a F32 representing the width of the box.\n
  */
  virtual F32     GetWidth (void) = 0;

  /**
    This method sets the width of the box.

    @param  fInWidth   A F32 representing the width of the box.\n

    @return IFX_OK
  */
  virtual IFXRESULT SetWidth (F32 fInWidth) = 0;

  /**
    This method returns the length of the box.

    @return Returns a F32 representing the length of the box.\n
  */
  virtual F32     GetLength (void) = 0;

  /**
    This method sets the length of the box.

    @param  fInLength  A F32 representing the length of the box.\n

    @return IFX_OK
  */
  virtual IFXRESULT SetLength (F32 fInLength) = 0;

  /**
    This method returns the number of vertices in the height of the box.

    @return Returns a U32 representing the number of height vertices.\n
  */
  virtual U32     GetNumHeightVertices (void) = 0;

  /**
    This method sets the number of height vertices of the box.

    @param  iInNumHeightVertices  A U32 representing the number of height
                    vertices of the box.\n

    @return IFX_OK
  */
  virtual IFXRESULT SetNumHeightVertices (U32 iInNumHeightVertices) = 0;

  /**
    This method returns the number of vertices in the width of the box.

    @return Returns a U32 representing the number of width vertices.\n
  */
  virtual U32     GetNumWidthVertices (void) = 0;

  /**
    This method sets the number of width vertices of the box.

    @param  iInNumWidthVertices   A U32 representing the number of width
                    vertices of the box.\n

    @return IFX_OK
  */
  virtual IFXRESULT SetNumWidthVertices (U32 iInNumWidthVertices) = 0;

  /**
    This method returns the number of vertices in the length of the box.

    @return Returns a U32 representing the number of length vertices.\n
  */
  virtual U32     GetNumLengthVertices (void) = 0;

  /**
    This method sets the number of length vertices of the box.

    @param  iInNumLengthVertices  A U32 representing the number of length
                    vertices of the box.\n

    @return IFX_OK
  */
  virtual IFXRESULT SetNumLengthVertices (U32 iInNumLengthVertices) = 0;
};

#endif
