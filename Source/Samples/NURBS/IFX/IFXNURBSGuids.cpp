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
  @file IFXNURBSGuids.cpp

      This file forces the declaration of all the GUIDs (CIDs, IIDs,
      etc.) associated with the NURBS DL.
*/


#define IFX_INIT_GUID

#include "IFXNURBSModifierInterfaces.h"
#include "IFXNURBSCIDs.h"

#include "IFXDecoderX.h"
#include "IFXEncoderX.h"
#include "IFXBitStreamX.h"
#include "IFXMeshGroup.h"
#include "IFXMesh.h"
#include "IFXNeighborMesh.h"
#include "IFXBoundSphereDataElement.h"
#include "IFXMarkerX.h"
#include "IFXDidRegistry.h"
#include "IFXCoreCIDs.h"
#include "IFXDids.h"
#include "IFXExtensionEncoderX.h"
