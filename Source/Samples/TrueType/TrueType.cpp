//***************************************************************************
//
//  Copyright (c) 2005 - 2006 Intel Corporation
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

#define IFX_INIT_GUID

#include "TrueType.h"
#include "IFXModifierChain.h"
#include "IFXGlyph2DModifier.h"
#include "IFXGlyphCommandList.h"
#include "IFXShadingModifier.h"
#include "TTFontExtract.h"
#include "IFXLoadManager.h"
#include "IFXReadBuffer.h"
#include "IFXCoreCIDs.h"
#include "IFXImportingCIDs.h"
#include "IFXPickObject.h"
#include "IFXImageCodec.h"
#include "IFXTaskCallback.h"
#include "IFXModifierChain.h"

IFXRESULT AddGlyph2DModifier(
							 IFXModel * pModel,
							 IFXGlyph2DModifier ** ppGlyph2DModifier,
							 F64 defaultWidth, F64 defaultSpacing, F64 defaultHeight,
							 IFXGlyphCommandList* pCommandList)
{
	IFXRESULT     result = IFX_OK;
	IFXGlyph2DModifier* pGlyph2DModifier = NULL;
	IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );

	if ( !pModel || !ppGlyph2DModifier)
		result = IFX_E_NOT_INITIALIZED;

	// Get the pointer to the scenegraph from the IFXModel
	if ( IFXSUCCESS(result) )
	{
		result = pModel->GetSceneGraph(&pSceneGraph);

		if ( IFXSUCCESS(result) && !pSceneGraph)
			result = IFX_E_INVALID_POINTER;
	}

	// Create a mesh Shading modifier
	if ( IFXSUCCESS(result) )
	{
		result = IFXCreateComponent( CID_IFXGlyph2DModifier, IID_IFXGlyph2DModifier,
			(void**)&pGlyph2DModifier );
	}

	// Initialize it
	if ( IFXSUCCESS(result) )
		result = pGlyph2DModifier->SetSceneGraph( pSceneGraph );
	if ( IFXSUCCESS(result) )
		pGlyph2DModifier->Initialize( defaultWidth, defaultSpacing, defaultHeight );

	if ( IFXSUCCESS(result) )
	{
		IFXSimpleList* pGlyphData = NULL;
		pCommandList->GetList( &pGlyphData );
		pGlyph2DModifier->SetGlyphCommandList( pGlyphData );
		IFXRELEASE( pGlyphData );
	}
	// Add it to the end of the modifier chain associated with the IFXModel
	if ( IFXSUCCESS(result) )
	{
		IFXDECLARELOCAL( IFXModifierChain, pModChain );

		result = pModel->GetModifierChain( &pModChain );
		if ( IFXSUCCESS( result ) )
			result = pModChain->AddModifier( *pGlyph2DModifier );
	}

	if ( IFXSUCCESS(result) )
	{
		// Releasing pShading is left to the caller
		*ppGlyph2DModifier = pGlyph2DModifier;
	}

	return result;
}

IFXRESULT RunSample( U3DSceneUtilities* pSGUtils )
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
	IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);
	IFXDECLARELOCAL(IFXStdio, pIFXStdio);
	IFXDECLARELOCAL(IFXModel, pModel);
	IFXDECLARELOCAL(IFXGlyph2DModifier, pGlyph);
	IFXDECLARELOCAL(IFXScheduler, pScheduler);
	IFXDECLARELOCAL(IFXGlyphCommandList, pComList);

	result = pSGUtils->InitializeScene();

	//load CLOD from U3D file
	if( IFXSUCCESS( result ) )
		result = IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager);
	if( IFXSUCCESS( result ) )
		result = pIFXLoadManager->Initialize(pSGUtils->m_pCoreServices);
	if( IFXSUCCESS( result ) )
		result = IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer);
	if( IFXSUCCESS( result ) )
		result = pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio);

	IFXString src = "shading.u3d";
	if( IFXSUCCESS( result ) )
		result = pIFXStdio->Open( (IFXCHAR*)src.Raw() );
	if( IFXSUCCESS( result ) )
		result = pSGUtils->m_pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler);
	else
		IFXASSERTBOX(0,"Please copy file \"shading.u3d\" to the current directory and restart application");
	if( IFXSUCCESS( result ) )
		result = pIFXLoadManager->Load(pIFXReadBuffer, FALSE);
	EIFXLoadState eState = IFX_LOAD_STATE_UNLOADED;
	if( IFXSUCCESS( result ) )
	{
		while ( eState < IFX_LOAD_STATE_MEDIA_LOADED )
		{
			pScheduler->Service();
			pIFXLoadManager->GetAggregateLoadState(eState);
		}
	}
	if( IFXSUCCESS( result ) )
		result = pIFXStdio->Close();
	else
		if( pIFXStdio )
			pIFXStdio->Close();

	//Apply glyph string to the model
	if( IFXSUCCESS( result ) )
		result = pSGUtils->GetModelFromName(L"Box01", &pModel);
	if( IFXSUCCESS( result ) )
		result = IFXCreateComponent( CID_IFXGlyphCommandList, IID_IFXGlyphCommandList, (void**)&pComList );
	if( IFXSUCCESS( result ) )
		result = pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_STARTGLYPHSTRING );
	if( IFXSUCCESS( result ) )
		result = CreateGlyphString( pComList, L"3", L"WebDings", 40 );
	if( IFXSUCCESS( result ) )
		result = CreateGlyphString( pComList, L"Universal 3D", L"Times New Roman", 40 );
	if( IFXSUCCESS( result ) )
		result = CreateGlyphString( pComList, L"4", L"WebDings", 40 );
	if( IFXSUCCESS( result ) )
		result = pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_ENDGLYPHSTRING );
	if( IFXSUCCESS( result ) )
		result = AddGlyph2DModifier( pModel, &pGlyph, 10, 10, 10, pComList );
	IFXDECLARELOCAL( IFXModifierDataPacket, pPacket );
	pGlyph->GetDataPacket( pPacket );
	U32 meshIdx;
	pPacket->GetDataElementIndex( DID_IFXRenderableGroup, meshIdx );
	IFXDECLARELOCAL( IFXUnknown, pUnk );
	IFXDECLARELOCAL( IFXMeshGroup, pMeshGroup );
	BOOL nr = FALSE;
	U32 numMeshes = 0;
	if( meshIdx != (U32)-1 )
	{
		pGlyph->GenerateOutput( meshIdx, (void*&)pUnk, nr );
		pUnk->QueryInterface( IID_IFXMeshGroup, (void**) &pMeshGroup );
		numMeshes = pMeshGroup->GetNumMeshes();
		IFXRELEASE( pMeshGroup );
	}

	IFXMatrix4x4 offset;
	offset.MakeIdentity();
	offset[13] = 0;
	offset[0] = (F32)0.5;
	offset[5] = (F32)0.9;

	if( IFXSUCCESS( result ) )
		result = pGlyph->SetTransform( offset );
	if( IFXSUCCESS( result ) )
	{
		result = pGlyph->SetAttributes( IFXGlyph2DModifier::BILLBOARD );
		// Uncommenting the next line will cause glyph to be compiled
		// into a single mesh. So the second shader won't work since
		// it addresses 11th and 12th mesh.
//		result = pGlyph->SetAttributes( IFXGlyph2DModifier::BILLBOARD | IFXGlyph2DModifier::SINGLE_SHADER );
	}

	IFXDECLARELOCAL(IFXShaderLitTexture, pShaderLitTexture);
	IFXDECLARELOCAL(IFXMaterialResource, pMaterialResource);
	IFXDECLARELOCAL(IFXShaderList, pShaderList);
	U32 idx = 0;

	if( IFXSUCCESS( result ) )
		IFXCreateComponent(CID_IFXMaterialResource, IID_IFXMaterialResource, (void**)&pMaterialResource);
	if( IFXSUCCESS( result ) )
	{
		IFXVector4 color((F32)0.1,(F32)1,(F32)0.3,1);
		pMaterialResource->SetSpecular(color);
		pMaterialResource->SetDiffuse(color);
		pMaterialResource->SetSceneGraph( pSGUtils->m_pSceneGraph );
		result = pSGUtils->m_pMaterialPalette->Add( L"Material1", &idx );
		if( IFXSUCCESS( result ) )
			result = pSGUtils->m_pMaterialPalette->SetResourcePtr( idx, pMaterialResource );
	}
	if( IFXSUCCESS( result ) )
		result = IFXCreateComponent(CID_IFXShaderLitTexture, IID_IFXShaderLitTexture, (void**)&pShaderLitTexture);
	if( IFXSUCCESS( result ) )
	{
		pShaderLitTexture->SetMaterialID( idx );
		pShaderLitTexture->SetSceneGraph( pSGUtils->m_pSceneGraph );
		result = pSGUtils->m_pShaderPalette->Add(L"Shader1", &idx);
		if( IFXSUCCESS( result ) )
			result = pSGUtils->m_pShaderPalette->SetResourcePtr( idx, pShaderLitTexture );
	}
	if( IFXSUCCESS( result ) )
		result = IFXCreateComponent(CID_IFXShaderList, IID_IFXShaderList, (void**)&pShaderList);
	if( IFXSUCCESS( result ) )
		result = pShaderList->Allocate(1);
	if( IFXSUCCESS( result ) )
		result = pShaderList->SetShader(0,idx);

	{
		IFXDECLARELOCAL( IFXShadingModifier, pShadingModifier );

		if( IFXSUCCESS( result ) )
			result = IFXCreateComponent( CID_IFXShadingModifier, IID_IFXShadingModifier,	(void**)&pShadingModifier );
		if( IFXSUCCESS( result ) )
			result = pShadingModifier->SetSceneGraph( pSGUtils->m_pSceneGraph );
		if( IFXSUCCESS( result ) )
		{
			result = pShadingModifier->SetAttributes( IFXShadingModifier::GLYPH );
			// Uncommenting the next line will cause 1st shader to be applied
			// to both glyph ("Universal 3D") and mesh (box).
//			result = pShadingModifier->SetAttributes( IFXShadingModifier::MESH | IFXShadingModifier::GLYPH );
		}
		if( IFXSUCCESS( result ) )
		{
			// Removing FALSE or changing it to TRUE will cause 1st shader
			// to be applied to (numMeshes-1)th mesh only (the last mesh in
			// the meshgroup) instead of 0, 1, 2, ..., (numMeshes-1).
			result = pShadingModifier->SetElementShaderList( numMeshes-1, pShaderList, FALSE );
			// Uncommenting the next line will cause 1st shader to be applied
			// to 0th and 1st meshes. The 0th is box and the 1st is either
			// "Universal 3D" in case of Single Shader enabled (in this case
			//  the 1st mesh is the last mesh in the meshgroup) or the 1st
			// symbol of the text otherwise. With this line commented out
			// 0, 1, 2, ..., (numMeshes-1) meshes will be shaded.
//			result = pShadingModifier->SetElementShaderList( 1, pShaderList, FALSE );
		}
		IFXDECLARELOCAL( IFXModifierChain, pModChain );
		if( IFXSUCCESS( result ) )
			result = pModel->GetModifierChain(&pModChain);
		if( IFXSUCCESS( result ) )
			result = pModChain->AddModifier( *pShadingModifier );
	}
	{
		IFXDECLARELOCAL(IFXShaderLitTexture, pShaderLitTexture);
		IFXDECLARELOCAL(IFXMaterialResource, pMaterialResource);
		IFXDECLARELOCAL(IFXShaderList, pShaderList);
		U32 idx = 0;

		if( IFXSUCCESS( result ) )
			IFXCreateComponent(CID_IFXMaterialResource, IID_IFXMaterialResource, (void**)&pMaterialResource);
		if( IFXSUCCESS( result ) )
		{
			IFXVector4 color(1,(F32)0.1,(F32)0.3,1);
			pMaterialResource->SetSpecular(color);
			pMaterialResource->SetDiffuse(color);
			pMaterialResource->SetSceneGraph( pSGUtils->m_pSceneGraph );
			result = pSGUtils->m_pMaterialPalette->Add( L"Material2", &idx );
			if( IFXSUCCESS( result ) )
				result = pSGUtils->m_pMaterialPalette->SetResourcePtr( idx, pMaterialResource );
		}
		if( IFXSUCCESS( result ) )
			result = IFXCreateComponent(CID_IFXShaderLitTexture, IID_IFXShaderLitTexture, (void**)&pShaderLitTexture);
		if( IFXSUCCESS( result ) )
		{
			pShaderLitTexture->SetMaterialID( idx );
			pShaderLitTexture->SetSceneGraph( pSGUtils->m_pSceneGraph );
			result = pSGUtils->m_pShaderPalette->Add(L"Shader2", &idx);
			if( IFXSUCCESS( result ) )
				result = pSGUtils->m_pShaderPalette->SetResourcePtr( idx, pShaderLitTexture );
		}
		if( IFXSUCCESS( result ) )
			result = IFXCreateComponent(CID_IFXShaderList, IID_IFXShaderList, (void**)&pShaderList);
		if( IFXSUCCESS( result ) )
			result = pShaderList->Allocate(1);
		if( IFXSUCCESS( result ) )
			result = pShaderList->SetShader(0,idx);
		IFXDECLARELOCAL( IFXShadingModifier, pShadingModifier );

		if( IFXSUCCESS( result ) )
			result = IFXCreateComponent( CID_IFXShadingModifier, IID_IFXShadingModifier,	(void**)&pShadingModifier );
		if( IFXSUCCESS( result ) )
			result = pShadingModifier->SetSceneGraph( pSGUtils->m_pSceneGraph );
		if( IFXSUCCESS( result ) )
			result = pShadingModifier->SetAttributes( IFXShadingModifier::GLYPH );
			// Uncommenting the next line will produce no effect since
			// both 11th and 12th meshes are glyph meshes.
//			result = pShadingModifier->SetAttributes( IFXShadingModifier::MESH | IFXShadingModifier::GLYPH );
		if( IFXSUCCESS( result ) )
			result = pShadingModifier->SetElementShaderList( 11, pShaderList );
		if( IFXSUCCESS( result ) )
			result = pShadingModifier->SetElementShaderList( 12, pShaderList );
		IFXDECLARELOCAL( IFXModifierChain, pModChain );
		if( IFXSUCCESS( result ) )
			result = pModel->GetModifierChain(&pModChain);
		if( IFXSUCCESS( result ) )
			result = pModChain->AddModifier( *pShadingModifier );
	}
	return result;
}
