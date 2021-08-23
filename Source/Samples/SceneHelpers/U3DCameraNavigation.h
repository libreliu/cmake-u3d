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

#ifndef __U3DCAMERANAVIGATIONH__
#define __U3DCAMERANAVIGATIONH__

#define	ToRadians(x)	((x)*3.1415926535f/180.0f)

#include "IFXDids.h" // DID_IFXRenderableGroupBounds

#include "IFXBoundSphereDataElement.h"
#include "IFXPerformanceTimer.h"
#include "IFXDevice.h"

IFXRESULT PanView( F32 fInX, F32 fInY, IFXMatrix4x4 *pViewMatrix );
IFXRESULT RotateView( F32 fInX, F32 fInY, BOOL mod, BOOL yKeyDown, 
					 IFXVector3 pivotPoint, IFXMatrix4x4 *pViewMatrix );
IFXRESULT ZoomView( F32 fInX, F32 fInY, BOOL mod, IFXMatrix4x4 *pViewMatrix);
IFXRESULT ComputePivot(IFXPalette * pNodePalette, IFXVector3 *pPivotPoint);

#endif
