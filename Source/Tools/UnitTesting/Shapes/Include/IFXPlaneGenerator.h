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
//  IFXplaneGenerator.h
//
//  DESCRIPTION
//      IFXSphereGenerator interface.
//
//  NOTES
//
//
//***************************************************************************

/**
  @file IFXPlaneGenerator.h

      This header defines the IFXPlaneGenerator class and its functionality.
*/

#ifndef __IFXPLANEGENERATOR_H__
#define __IFXPLANEGENERATOR_H__

#include "IFXUnknown.h"
#include "IFXPrimitive.h"

// {DDBBB50A-4420-4d0f-9EAF-D5A58C859E97}
IFXDEFINE_GUID(IID_IFXPlaneGenerator,
0xddbbb50a, 0x4420, 0x4d0f, 0x9e, 0xaf, 0xd5, 0xa5, 0x8c, 0x85, 0x9e, 0x97);


/**
  This is the main interface for IFXPlaneGenerator.

  @note The associated IID is named IID_IFXPlaneGenerator.
*/
class IFXPlaneGenerator : virtual public IFXPrimitive,
              virtual public IFXMarker
{
public:

  /**
    This method returns length of the plane.

    @return Returns a F32 representing the length of the plane.\n
  */
  virtual F32     GetLength (void) = 0;

  /**
    This method sets the length of the plane.

    @param  fInLength  A F32 representing the length of the plane.\n

    @return IFX_OK
  */
  virtual IFXRESULT SetLength (F32 fInLength) = 0;

  /**
    This method returns width of the plane.

    @return Returns a F32 representing the width of the plane.\n
  */
  virtual F32     GetWidth (void) = 0;

  /**
    This method sets the width of the plane.

    @param  fInWidth   A F32 representing the width of the plane.\n

    @return IFX_OK
  */
  virtual IFXRESULT SetWidth  (F32 fInWidth) = 0;

  /**
    This method returns number of vertices in the length of the plane.

    @return Returns a U32 representing the number of length vertices.\n
  */
  virtual U32     GetLengthVertices (void) = 0;

  /**
    This method sets the number of length vertices of the plane.

    @param  iInNumLengthVertices  A U32 representing the number of length
                    vertices of the plane.\n

    @return IFX_OK
  */
  virtual IFXRESULT SetLengthVertices (U32 iLengthVertices) = 0;

  /**
    This method returns number of vertices in the width of the plane.

    @return Returns a U32 representing the number of width vertices.\n
  */
  virtual U32     GetWidthVertices (void) = 0;

  /**
    This method sets the number of width vertices of the plane.

    @param  iInNumWidthVertices   A U32 representing the number of width
                    vertices of the plane.\n

    @return IFX_OK
  */
  virtual IFXRESULT SetWidthVertices (U32 iWidthVertices) = 0;
};

#endif
