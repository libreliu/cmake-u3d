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

#ifndef SCENEUTILITIES_H
#define SCENEUTILITIES_H

//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXCoreServices.h"
#include "IFXSceneGraph.h"
#include "IFXAutoRelease.h"

//***************************************************************************
//  Classes, structures and types
//***************************************************************************

/**
This is the implementation of a class that is used to create U3D scene objects.
*/
class SceneUtilities
{
public:
	SceneUtilities();
	~SceneUtilities();

	void WriteU3DFileX( IFXCHAR* pFileName );
	void LoadU3DFileX( IFXCHAR* pFileName );

	void ConvertToCompressedBaseMeshX();

private:
	IFXDECLAREMEMBER(IFXCoreServices,m_pCoreServicesMain);
	IFXDECLAREMEMBER(IFXCoreServices,m_pCoreServices);
	IFXDECLAREMEMBER(IFXSceneGraph,m_pSceneGraph);
};

#endif
