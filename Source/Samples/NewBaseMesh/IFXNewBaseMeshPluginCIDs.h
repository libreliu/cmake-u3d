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
	@file	IFXNewBaseMeshPluginCIDs.h

			This header defines all of the CIDs that identify the various 
			components contained in the NewBaseMeshPlugin DL.
*/


#ifndef IFXNEWBASEMESHPLUGINCIDS_H
#define IFXNEWBASEMESHPLUGINCIDS_H


//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXGUID.h"


//***************************************************************************
//	Component identifiers
//***************************************************************************
// {999AC438-D447-45b8-B081-B70D4CEB1404}
IFXDEFINE_GUID(CID_IFXNewBaseMesh,
0x7df87228, 0xe0d3, 0x417d, 0x82, 0x11, 0x6a, 0x60, 0x9c, 0x07, 0xca, 0xb7);

// {70A837A9-92E6-4234-8B76-2D2F5C1E4F22}
IFXDEFINE_GUID(CID_IFXNewBaseMeshDecoder, 
0x86b396a1, 0x420e, 0x40db, 0xbf, 0x51, 0x96, 0x6a, 0x9b, 0xf1, 0xfd, 0xd9);

// {FEA76CC9-08C8-46fd-B072-829F95F9312F}
IFXDEFINE_GUID(CID_IFXNewBaseMeshEncoder, 
0x96e0593b, 0x5853, 0x46fa, 0x8b, 0x34, 0x01, 0x5f, 0x93, 0xb7, 0xd2, 0xe5);

#endif
