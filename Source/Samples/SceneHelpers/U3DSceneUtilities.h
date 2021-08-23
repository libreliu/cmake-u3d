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

#ifndef __U3DSceneUtilities_H__
#define __U3DSceneUtilities_H__

#include "IFXScheduler.h"
#include "IFXLoadManager.h"
#include "IFXCLODModifier.h"
#include "IFXAuthorCLODResource.h"
#include "IFXShaderLitTexture.h"
#include "IFXMaterialResource.h"
#include "IFXLight.h"
#include "IFXView.h"
#include "IFXStdio.h"
#include "IFXShadingModifier.h"

#include "IFXAutoRelease.h"

#include "IFXDids.h" // DID_IFXRenderableGroupBounds
#include "IFXBoundSphereDataElement.h"
#include "IFXPerformanceTimer.h"
#include "IFXDevice.h"

const U32 SGSTRLEN = 512;
#define NUM_OF_LM 10
#define NUM_OF_PARENTS  10

struct SGNodeHeader
{
	IFXCHAR Name[SGSTRLEN];       ///< The name of the node to create *case sensitive*
	IFXCHAR ParentName[SGSTRLEN]; ///< The name of the node's parent  *case sensitive*
	IFXMatrix4x4 NodeTm;       ///< The node's local transform (with respect to its parent)

	U32 NumberOfParents;
	IFXCHAR ParentNames[NUM_OF_PARENTS][SGSTRLEN]; ///< The name of the node's parent  *case sensitive*
	IFXMatrix4x4 NodeTms[NUM_OF_PARENTS];       ///< The node's local transform (with respect to its parent)
};

class U3DSceneUtilities
{
public:
	// Constructors/destructors
	U3DSceneUtilities();
	virtual ~U3DSceneUtilities();  // Scene destroyed here.

	// Scenegraph initialization
	IFXRESULT InitializeScene( U32 profile = 0, F64 units = 1.0f );

	void LoadU3DFileInit(IFXCHAR* FileName, BOOL bLoadingAtomic, BOOL bRemoteConnection = FALSE);
	BOOL IsU3DFileLoaded();
	BOOL IsLoadingCanceled();
	U64 LoadRemainder();
	F32 LoadingProgress();
	void CancelLoading();

	void ProcessScheduler();
	void ResetSimulationX();
	void SetSimulationStateX(const BOOL state, const BOOL autoReset);
	BOOL GetSimulationStateX();
	U32 GetSystemTimeX();

	// Node utilities:  creation and finding
	IFXRESULT CreateCameraNode( SGNodeHeader * pNodeHeader,
		IFXView** ppView, BOOL mps = FALSE );
	IFXRESULT CreateGroupNode(  SGNodeHeader * pNodeHeader, BOOL mps = FALSE);
	IFXRESULT CreateLightNode(  SGNodeHeader * pNodeHeader,
		IFXLight** ppLight, BOOL mps = FALSE);
	IFXRESULT CreateModelNode(  SGNodeHeader * pNodeHeader,
		U32 ResourceId, BOOL mps = FALSE);
	IFXRESULT FindNode(     const IFXCHAR * pName,
		IFXNode ** ppNode);

	IFXRESULT FindNextModel(U32* , IFXModel** );
	IFXRESULT GetMeshGroup(IFXModel* , IFXMeshGroup** );

	// Resources:  finding
	IFXRESULT GetACLODResourceFromName( const IFXCHAR *pModelName,
		IFXAuthorCLODResource **ppACLODResource);
	IFXRESULT GetModelFromName(     const IFXCHAR *pModelName,
		IFXModel **ppModel);

	// Misc utilities
	IFXRESULT KeepTextureDecompressed();

	IFXRESULT SetMaterialResource(const IFXCHAR *pModelName, 
		IFXVector4 *Ambient,
		IFXVector4 *Diffuse,
		IFXVector4 *Emission,
		IFXVector4 *Specular,
		F32 Opacity,
		F32 Reflectivity,
		BOOL Transparent,
		U32 ShaderFlags = IFXShaderLitTexture::MATERIAL);

	IFXDECLAREMEMBER(IFXSceneGraph,m_pSceneGraph);        ///< IFX scenegraph
	IFXDECLAREMEMBER(IFXScheduler,m_pScheduler);				///< IFX scheduler
	IFXDECLAREMEMBER(IFXCoreServices,m_pCoreServicesMain);
	IFXDECLAREMEMBER(IFXCoreServices,m_pCoreServices);      ///< IFX core services object
	IFXDECLAREMEMBER(IFXPalette,m_pMaterialPalette);     ///< IFX material palette
	IFXDECLAREMEMBER(IFXPalette,m_pGeneratorPalette);    ///< IFX generator palette
	IFXDECLAREMEMBER(IFXPalette,m_pNodePalette);       ///< IFX node palette
	IFXDECLAREMEMBER(IFXPalette,m_pShaderPalette);     ///< IFX shader palette
	IFXDECLAREMEMBER(IFXPalette,m_pTexturePalette);      ///< IFX texture palette

private:
	IFXLoadManager*   m_pIFXLoadManagers[NUM_OF_LM];
	U32         m_LoadManagerCounter;
	IFXDECLAREMEMBER(IFXReadBuffer,m_pIFXReadBuffer);
	IFXDECLAREMEMBER(IFXStdio,m_pIFXStdio);
	BOOL        m_bInit;                    ///< Whether or not this component is initialized
	BOOL        m_bEndOfU3DFile;
	BOOL        m_bCancelLoading;
	BOOL		m_nodeFound;
};

#endif
