//***************************************************************************
//
//  Copyright (c) 2004 - 2006 Intel Corporation
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

#ifndef SAMPLEGLYPH_H
#include "SampleGlyph.h"
#endif

#include "IFXSphereGenerator.h"
#include "IFXModifierChain.h"
#include "IFXGlyph2DModifier.h"
#include "IFXGlyphCommandList.h"

void GlyphStamper( IFXGlyphCommandList* pComList )
{
  pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_STARTGLYPH );
  pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_STARTPATH );
  pComList->AddMoveToBlock( 1, 1 );
  pComList->AddLineToBlock( 1, 50 );
  pComList->AddLineToBlock( 100, 50 );
  pComList->AddLineToBlock( 100, 1 );
  pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_ENDPATH );
  pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_STARTPATH );
  pComList->AddMoveToBlock( 4, 4 );
  pComList->AddLineToBlock( 97, 4 );
  pComList->AddLineToBlock( 97, 47 );
  pComList->AddLineToBlock( 4, 47 );
  pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_ENDPATH );
  pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_ENDGLYPH, 70, 20 );
}

IFXRESULT AddGlyph2DModifier(
               IFXModel * in_pModel,
               IFXGlyph2DModifier ** out_ppGlyph2DModifier,
               F64 defaultWidth, F64 defaultSpacing, F64 defaultHeight,
               IFXGlyphCommandList* pCommandList)
{
  IFXRESULT     result = IFX_OK;
  IFXGlyph2DModifier* pGlyph2DModifier = NULL;
  IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );

  if ( !in_pModel || !out_ppGlyph2DModifier)
  {
    result = IFX_E_NOT_INITIALIZED;
    IFXASSERT(0);
  }

  // Get the pointer to the scenegraph from the IFXModel
  if ( in_pModel && IFXSUCCESS(result) )
  {
    result = in_pModel->GetSceneGraph(&pSceneGraph);

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
  if ( pGlyph2DModifier && IFXSUCCESS(result) )
  {
    result = pGlyph2DModifier->SetSceneGraph( pSceneGraph ) ;
  }

  if ( IFXSUCCESS(result) )
  {
    pGlyph2DModifier->Initialize( defaultWidth, defaultSpacing, defaultHeight );
    IFXSimpleList* pGlyphData = NULL;
    pCommandList->GetList( &pGlyphData );
    pGlyph2DModifier->SetGlyphCommandList( pGlyphData );
    IFXRELEASE( pGlyphData );
  }
  // Add it to the end of the modifier chain associated with the IFXModel
  if ( in_pModel && IFXSUCCESS(result) )
  {
    IFXDECLARELOCAL( IFXModifierChain, pModChain );

    result = in_pModel->GetModifierChain( &pModChain );

    if ( pModChain && pGlyph2DModifier && IFXSUCCESS( result ) )
      result = pModChain->AddModifier( *pGlyph2DModifier );
  }

  if ( out_ppGlyph2DModifier && IFXSUCCESS(result) )
  {
    // Releasing pShading is left to the caller
    *out_ppGlyph2DModifier = pGlyph2DModifier;
  }

  return result;
}

IFXRESULT SampleGlyph::Run( U3DSceneUtilities* pSGUtils )
{
  U32 uID = 0;
  SGNodeHeader nh;
  IFXLight *pLight = NULL;
  IFXModifier     *pGenerator = NULL;
  U32           uResourceID = 0;
  U32           uCloneID  = 0;
  struct SGNodeHeader   nodeHeader;
  IFXPalette        *pShaderPalette = NULL;
  U32           uShaderID = 0;
  IFXVector4 colorWhite(1.0f, 1.0f, 1.0f);

  TEST(IFXSUCCESS(pSGUtils->InitializeScene()), IFXException);

  // Create camera
  {
    IFXView* pViewNode = NULL;

    wcscpy(nh.Name, L"Camera1");
    wcscpy(nh.ParentName, PALETTE_DEFAULT_GROUP);
    nh.NodeTm.Reset();
    TEST(IFXSUCCESS(pSGUtils->CreateCameraNode(&nh, &pViewNode)), IFXException);
    // Set up the camera based on its type
    pViewNode->SetAttributes( 0 );
    TEST(IFXSUCCESS(pViewNode->SetProjection(5.0)), IFXException);

    const F32 initnode[] = {sqrt(F32(2))/2,sqrt(F32(2))/2,0,0,
      -sqrt(F32(2))/2,sqrt(F32(2))/2,0,0,
      0,0,1,0,
      0,0,2000,1};
    IFXMatrix4x4 tmnode = IFXMatrix4x4( initnode );
    TEST( IFXSUCCESS( pViewNode->SetMatrix( 0, &tmnode ) ), IFXException );

    IFXRELEASE(pViewNode);
  }

  wcscpy(nh.Name, L"Light1");
  wcscpy(nh.ParentName, PALETTE_DEFAULT_GROUP);
  // Create a light node with this name
  //  Log("Light %s \tParent %s", nh.Name, nh.ParentName);
  Log("Light");
  //@TODO: UNICODE  Log(nh.Name);
  Log("Parent");
  //@TODO: UNICODE  Log(nh.ParentName);

  TEST(IFXSUCCESS(pSGUtils->CreateLightNode(&nh, &pLight)), IFXException);

  // Get the light resource palette
  IFXDECLARELOCAL( IFXPalette, pLightResourcePalette );
  IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );
  TEST(IFXSUCCESS(pSGUtils->m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph )), IFXException);
  TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::LIGHT, &pLightResourcePalette )), IFXException);
  U32 uLightResourceId = 0;

  // Look for the light resource in the light resource palette
  IFXLightResource* pLightResource = NULL;
  // create a light resource and add the light resource to the palette
  IFXRESULT rc = pLightResourcePalette->Add( nh.Name, &uLightResourceId );
  if (IFXSUCCESS(rc))
  {
    TEST(IFXSUCCESS( IFXCreateComponent( CID_IFXLightResource, IID_IFXLightResource, (void**)&pLightResource ) ), IFXException);
    IFXDECLARELOCAL( IFXUnknown, pUnk );
    TEST(IFXSUCCESS( pLightResource->QueryInterface( IID_IFXUnknown, (void**)&pUnk ) ), IFXException);

    IFXDECLARELOCAL( IFXMarker, pMarker );
    TEST(IFXSUCCESS(pUnk->QueryInterface( IID_IFXMarker, (void**)&pMarker )), IFXException);
    TEST(IFXSUCCESS(pMarker->SetSceneGraph( pSceneGraph )), IFXException);
    TEST(IFXSUCCESS( pLightResourcePalette->SetResourcePtr( uLightResourceId, pUnk ) ), IFXException);
    IFXRELEASE(pUnk);
  }

  // Link the light node to the light resource pallete entry
  TEST(IFXSUCCESS( pLight->SetLightResourceID(uLightResourceId) ), IFXException);
  // Ambient light
  pLightResource->SetType(IFXLightResource::DIRECTIONAL);
  pLightResource->SetColor(colorWhite);
  IFXRELEASE(pLightResource);
  IFXRELEASE(pLight);

  // --- Create a sphere primitive to indicate the picking location
  TEST(IFXSUCCESS(CreatePrimitive("PickSphere", CID_IFXSphereGenerator, 0,
    &uResourceID, &pGenerator, pSGUtils->m_pGeneratorPalette, pSGUtils->m_pSceneGraph) ), IFXException);

  // Create a model node in the node palette for the sphere
  wcscpy(nodeHeader.Name, L"PickSphere");
  wcscpy(nodeHeader.ParentName, PALETTE_DEFAULT_GROUP);
  nodeHeader.NodeTm.Reset();
  nodeHeader.NodeTm.Translate3x4(IFXVector3(0, 0, -1000));
    const F32 initnode[] = {sqrt(F32(2))/2,sqrt(F32(2))/2,0,0,
      -sqrt(F32(2))/2,sqrt(F32(2))/2,0,0,
      0,0,1,0,
      0,0,-1000,1};
  nodeHeader.NodeTm = IFXMatrix4x4( initnode );

  TEST(IFXSUCCESS(pSGUtils->CreateModelNode(&nodeHeader, uResourceID)), IFXException);
  // Set some sphere properties
  IFXSphereGenerator  *pSphereGenerator = NULL;
  TEST(IFXSUCCESS(pGenerator->QueryInterface( IID_IFXSphereGenerator, (void**)&pSphereGenerator )), IFXException);
  TEST(IFXSUCCESS(pSphereGenerator->SetRadius(20.0f)), IFXException);
  TEST(IFXSUCCESS(pSphereGenerator->SetResolution(6)), IFXException);

  // Access the shader and turn on line rendering for the default
  // shader so we can see the bounding sphere.

  // Get the shader palette
  pShaderPalette = pSGUtils->m_pShaderPalette;
  TEST(pShaderPalette != NULL, IFXException);

  IFXModel        *pModel = NULL;
  IFXGlyph2DModifier   *pGlyph = NULL;
  TEST( IFXSUCCESS( pSGUtils->GetModelFromName(L"PickSphere", &pModel) ), IFXException);

  IFXDECLARELOCAL( IFXGlyphCommandList, pComList );
  IFXCreateComponent( CID_IFXGlyphCommandList, IID_IFXGlyphCommandList, (void**)&pComList );
  pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_STARTGLYPHSTRING );
  GlyphStamper( pComList );
  GlyphStamper( pComList );
  GlyphStamper( pComList );

  pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_ENDGLYPHSTRING );
  AddGlyph2DModifier( pModel, &pGlyph, 10, 10, 10, pComList );
  IFXMatrix4x4 offset;
  offset.MakeIdentity();
  offset[12] = -17;
  offset[13] = -23;
  offset[14] = -30;
  pGlyph->SetTransform( offset );
  pGlyph->SetAttributes( IFXGlyph2DModifier::BILLBOARD | IFXGlyph2DModifier::SINGLE_SHADER );
/*	IFXDECLARELOCAL( IFXModifierDataPacket, pPacket );
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
*/
  IFXRELEASE(pSphereGenerator);
  IFXRELEASE(pGenerator);
  IFXRELEASE(pGlyph);

  U32 idx;
  {
    //create new material
    IFXDECLARELOCAL( IFXMaterialResource, pMResource );
    IFXDECLARELOCAL( IFXShaderLitTexture, pShaderLitTexture );
    TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXMaterialResource, IID_IFXMaterialResource, (void**)&pMResource)), IFXException);
    IFXVector4 color(1,(F32)0.1,(F32)0.3,1);
    pMResource->SetSpecular(color);
    pMResource->SetDiffuse(color);
    pMResource->SetSceneGraph( pSGUtils->m_pSceneGraph );
    pSGUtils->m_pMaterialPalette->Add( L"NewMaterial", &idx );
    pSGUtils->m_pMaterialPalette->SetResourcePtr( idx, pMResource );
  }
  {
    //create new material
    IFXDECLARELOCAL( IFXMaterialResource, pMResource );
    TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXMaterialResource, IID_IFXMaterialResource, (void**)&pMResource)), IFXException);
    IFXVector4 color(1,(F32)1,(F32)0.3,1);
    pMResource->SetSpecular(color);
    pMResource->SetDiffuse(color);
    pMResource->SetSceneGraph( pSGUtils->m_pSceneGraph );
    pSGUtils->m_pMaterialPalette->Add( L"AnotherMaterial", &idx );
    pSGUtils->m_pMaterialPalette->SetResourcePtr( idx, pMResource );
  }

  U32 idxsh;

  {
    //create new shader lit texture
    IFXDECLARELOCAL( IFXShaderLitTexture, pShaderLitTexture );
    TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderLitTexture, IID_IFXShaderLitTexture, (void**)&pShaderLitTexture)), IFXException);
    TEST( IFXSUCCESS( pShaderLitTexture->SetMaterialID( 1 )), IFXException);
    pShaderLitTexture->SetSceneGraph( pSGUtils->m_pSceneGraph );

    TEST(IFXSUCCESS(pShaderPalette->Add(L"NewShader", &idxsh)), IFXException);
    TEST(IFXSUCCESS(pShaderPalette->SetResourcePtr( idxsh, pShaderLitTexture )), IFXException);
  }
  {
    //create new shader lit texture
    IFXDECLARELOCAL( IFXShaderLitTexture, pShaderLitTexture );
    TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderLitTexture, IID_IFXShaderLitTexture, (void**)&pShaderLitTexture)), IFXException);
    TEST( IFXSUCCESS( pShaderLitTexture->SetMaterialID( 2 )), IFXException);
    pShaderLitTexture->SetSceneGraph( pSGUtils->m_pSceneGraph );

    TEST(IFXSUCCESS(pShaderPalette->Add(L"AnotherShader", &idxsh)), IFXException);
    TEST(IFXSUCCESS(pShaderPalette->SetResourcePtr( idxsh, pShaderLitTexture )), IFXException);
  }

  //create shader set
  IFXDECLARELOCAL( IFXShaderList, pShaderList );
  TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderList, IID_IFXShaderList, (void**)&pShaderList)), IFXException);
  pShaderList->Allocate(1);
  pShaderList->SetShader(0,1);
  IFXDECLARELOCAL( IFXShaderList, pShaderList2 );
  TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXShaderList, IID_IFXShaderList, (void**)&pShaderList2)), IFXException);
  pShaderList2->Allocate(1);
  pShaderList2->SetShader(0,2);
  // Create a shader modifier
  IFXDECLARELOCAL( IFXShadingModifier, pShadingModifier );

  TEST( IFXSUCCESS( IFXCreateComponent( CID_IFXShadingModifier, IID_IFXShadingModifier,
    (void**)&pShadingModifier ) ), IFXException);
  pShadingModifier->SetSceneGraph( pSGUtils->m_pSceneGraph );
  TEST(IFXSUCCESS(pShadingModifier->SetAttributes( IFXShadingModifier::GLYPH | IFXShadingModifier::MESH)), IFXException);
  TEST(IFXSUCCESS(pShadingModifier->SetElementShaderList( 0, pShaderList )), IFXException);
  TEST(IFXSUCCESS(pShadingModifier->SetElementShaderList( 1, pShaderList2 )), IFXException);
  IFXDECLARELOCAL( IFXModifierChain, pModChain );
  TEST( IFXSUCCESS( pModel->GetModifierChain(&pModChain) ), IFXException );
  TEST( IFXSUCCESS( pModChain->AddModifier( *pShadingModifier ) ), IFXException );
  return IFX_OK;
}
