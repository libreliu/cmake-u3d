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

/**
  @file IFXNURBSCIDs.h

      This header defines all of the CIDs that identify the various
      components contained in the NURBS DL.
*/

#ifndef IFXNURBSCIDs_H
#define IFXNURBSCIDs_H

//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXGUID.h"

//***************************************************************************
//  Component identifiers
//***************************************************************************

// {146B5698-3F0C-4686-9561-DC376D8877F8}
IFXDEFINE_GUID(CID_IFXNURBSModifier,
  0x146b5698, 0x3f0c, 0x4686, 0x95, 0x61, 0xdc, 0x37, 0x6d, 0x88, 0x77, 0xf8);

// {C45AF17E-6F76-4fb8-88A1-836BA765AE6D}
IFXDEFINE_GUID(CID_IFXNURBSModifierEncoder,
  0xc45af17e, 0x6f76, 0x4fb8, 0x88, 0xa1, 0x83, 0x6b, 0xa7, 0x65, 0xae, 0x6d);

// {6FFF7035-435C-4682-A27C-52E4F493F959}
IFXDEFINE_GUID(CID_IFXNURBSModifierDecoder,
  0x6fff7035, 0x435c, 0x4682, 0xa2, 0x7c, 0x52, 0xe4, 0xf4, 0x93, 0xf9, 0x59);

#endif
