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
  @file IFXShapesCIDs.h

      This header defines all of the CIDs that identify the various
      components contained in the Shapes DL.
*/

#ifndef IFXShapesCIDs_H
#define IFXShapesCIDs_H

//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXGUID.h"

//***************************************************************************
//  Component identifiers
//***************************************************************************

//---------------------------------------------------------------------------
//  CID_IFXSphereGenerator
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXSphereGenerator.  It is not a singleton.
//---------------------------------------------------------------------------
// {FC01DA04-0C4A-11d4-8479-00A0C939B104}
IFXDEFINE_GUID(CID_IFXSphereGenerator,
0xfc01da04, 0xc4a, 0x11d4, 0x84, 0x79, 0x0, 0xa0, 0xc9, 0x39, 0xb1, 0x4);

//---------------------------------------------------------------------------
//  CID_IFXBoxGenerator
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXBoxGenerator.  It is not a singleton.
//---------------------------------------------------------------------------
// {C7C0E616-614A-449f-A5D8-0C76A150691A}
IFXDEFINE_GUID(CID_IFXBoxGenerator,
0xc7c0e616, 0x614a, 0x449f, 0xa5, 0xd8, 0xc, 0x76, 0xa1, 0x50, 0x69, 0x1a);

//---------------------------------------------------------------------------
//  CID_IFXPlaneGenerator
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXPlaneGenerator.  It is not a singleton.
//---------------------------------------------------------------------------
// {AFD853E2-CD45-4e00-94DD-9F1BFE432DAF}
IFXDEFINE_GUID(CID_IFXPlaneGenerator,
0xafd853e2, 0xcd45, 0x4e00, 0x94, 0xdd, 0x9f, 0x1b, 0xfe, 0x43, 0x2d, 0xaf);

//---------------------------------------------------------------------------
//  CID_IFXCylinderGenerator
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXCylinderGenerator.  It is not a singleton.
//---------------------------------------------------------------------------
// {029EDACE-1FF0-4f66-BD21-785306EB42C3}
IFXDEFINE_GUID(CID_IFXCylinderGenerator,
0x29edace, 0x1ff0, 0x4f66, 0xbd, 0x21, 0x78, 0x53, 0x6, 0xeb, 0x42, 0xc3);

//---------------------------------------------------------------------------
//  CID_IFXPrimitiveGenerator
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXPrimitiveGenerator.  It is not a singleton.
//---------------------------------------------------------------------------
// {D9039C00-A75D-11d3-A0E9-00A0C9A0F93B}
IFXDEFINE_GUID(CID_IFXPrimitiveGenerator,
0xd9039c00, 0xa75d, 0x11d3, 0xa0, 0xe9, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

// IFXCLODModifier2 stuff

// {606D001E-BE63-4fc1-8681-F5DCF8C757CD}
IFXDEFINE_GUID(CID_IFXCLODModifier2Decoder, 
0x606d001e, 0xbe63, 0x4fc1, 0x86, 0x81, 0xf5, 0xdc, 0xf8, 0xc7, 0x57, 0xcd);

// {956BEC2A-6B7B-4456-AF23-399582097C4E}
IFXDEFINE_GUID(CID_IFXCLODModifier2Encoder, 
0x956bec2a, 0x6b7b, 0x4456, 0xaf, 0x23, 0x39, 0x95, 0x82, 0x9, 0x7c, 0x4e);

// {F5FDD21F-AD22-4fb8-B8A3-98DEFE09D818}
IFXDEFINE_GUID(CID_IFXCLODModifier2, 
0xf5fdd21f, 0xad22, 0x4fb8, 0xb8, 0xa3, 0x98, 0xde, 0xfe, 0x9, 0xd8, 0x18);



// {8FDE894D-61F7-4da2-9C95-99EDA537ECCC}
IFXDEFINE_GUID(DID_IFXShapesTest, 
0x8fde894d, 0x61f7, 0x4da2, 0x9c, 0x95, 0x99, 0xed, 0xa5, 0x37, 0xec, 0xcc);


#endif
