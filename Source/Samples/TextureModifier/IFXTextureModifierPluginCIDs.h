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
	@file	IFXTextureModifierPluginCIDs.h

			This header defines all of the CIDs that identify the various 
			components contained in the TextureModifierPlugin DL.
*/


#ifndef IFXTextureModifierPluginCIDs_H
#define IFXTextureModifierPluginCIDs_H


//***************************************************************************
//	Includes
//***************************************************************************


#include "IFXGUID.h"


//***************************************************************************
//	Component identifiers
//***************************************************************************

// {999AC438-D447-45b8-B081-B70D4CEB1404}
IFXDEFINE_GUID(CID_IFXTextureModifier,
0x999ac438, 0xd447, 0x45b8, 0xb0, 0x81, 0xb7, 0xd, 0x4c, 0xeb, 0x14, 0x4);

// {70A837A9-92E6-4234-8B76-2D2F5C1E4F22}
IFXDEFINE_GUID(CID_IFXTextureModifierDecoder, 
0x70a837a9, 0x92e6, 0x4234, 0x8b, 0x76, 0x2d, 0x2f, 0x5c, 0x1e, 0x4f, 0x22);

// {FEA76CC9-08C8-46fd-B072-829F95F9312F}
IFXDEFINE_GUID(CID_IFXTextureModifierEncoder, 
0xfea76cc9, 0x8c8, 0x46fd, 0xb0, 0x72, 0x82, 0x9f, 0x95, 0xf9, 0x31, 0x2f);


#endif
