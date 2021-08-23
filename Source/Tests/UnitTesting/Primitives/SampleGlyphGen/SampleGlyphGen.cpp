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

#ifndef SAMPLEGLYPHGEN_H
#include "SampleGlyphGen.h"
#endif

#include "IFXGlyph2DModifier.h"
#include "IFXModel.h"

IFXRESULT SampleGlyphGen::Run( U3DSceneUtilities* pSGUtils )
{
  U32 uID = 0;
  SGNodeHeader nh;
  IFXLight *pLight = NULL;
  IFXModifier     *pGenerator = NULL;
  U32           uResourceID = 0;
  struct SGNodeHeader   nodeHeader;
  IFXPalette        *pShaderPalette = NULL;
  U32           uShaderID = 0;
  IFXShaderLitTexture   *pShader = NULL;
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

		IFXRELEASE(pViewNode);
	}

  wcscpy(nh.Name, L"Light1");
  wcscpy(nh.ParentName, PALETTE_DEFAULT_GROUP);
  // Create a light node with this name
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

  IFXRESULT     result = IFX_OK;
//  IFXModifier*    pGlyphGenerator  = NULL;
  IFXUnknown*     pUnknown = NULL;

  // Create a new generator:
  // Freeing pGenerator is left to the caller
      result = IFXCreateComponent(  CID_IFXGlyph2DModifier,
        IID_IFXModifier,
        (void**)&pGenerator) ;

  // Initialize the generator through the marker interface:
    result = pGenerator->SetSceneGraph( pSGUtils->m_pSceneGraph ) ;

    pGenerator->SetPriority( 0 ) ;

  // Get the IFXUnknown interface from the primitive we created.
    result = pGenerator->QueryInterface(IID_IFXUnknown, (void**)&pUnknown);

  // Make an entry for this resource in the generator palette
    IFXString NodeName( "Glyph" );
    result = pSGUtils->m_pGeneratorPalette->Add( &NodeName, &uResourceID );

  // And point the the palette entry to the actual resource
    result = pSGUtils->m_pGeneratorPalette->SetResourcePtr( uResourceID, pUnknown );

  IFXRELEASE(pUnknown);




  wcscpy(nodeHeader.Name, L"Glyph");
  wcscpy(nodeHeader.ParentName, PALETTE_DEFAULT_GROUP);
  nodeHeader.NodeTm.Reset();
  nodeHeader.NodeTm.Translate3x4(IFXVector3(0, 0, -1000));

  IFXGlyph2DModifier* pGModifier = NULL;

  pGenerator->QueryInterface(IID_IFXGlyph2DModifier, (void**)&pGModifier);

  pGModifier->StartGlyphString();
  pGModifier->StartGlyph();
  pGModifier->StartPath();
  pGModifier->MoveTo(-30,-30);
  pGModifier->LineTo(-40,-30);
  pGModifier->LineTo(-40,-40);
  pGModifier->LineTo(-30,-40);
  pGModifier->EndPath();
  pGModifier->EndGlyph(10,10);
  pGModifier->StartGlyph();
  pGModifier->StartPath();
  pGModifier->MoveTo(-10,-10);
  pGModifier->LineTo(10,10);
  pGModifier->CurveTo(10,10,10,-15,-10,-10);
  pGModifier->EndPath();
  pGModifier->EndGlyph(10,10);
  pGModifier->StartGlyph();
  pGModifier->StartPath();
  pGModifier->MoveTo(20,10);
  pGModifier->LineTo(30,10);
  pGModifier->LineTo(30,20);
  pGModifier->LineTo(20,20);
  pGModifier->EndPath();
  pGModifier->EndGlyph(10,10);
  pGModifier->EndGlyphString();
  pGModifier->SetAttributes( IFXGlyph2DModifier::BILLBOARD );

  TEST(IFXSUCCESS(pSGUtils->CreateModelNode(&nodeHeader, uResourceID)), IFXException);

  IFXVector4 Diffuse = IFXVector4(1,(F32)0.1,(F32)0.8);
  //Set material resource
  TEST(IFXSUCCESS(pSGUtils->SetMaterialResource(nodeHeader.Name,
	  NULL, //&Ambient, 
	  &Diffuse, 
	  NULL, //&Emission, 
	  NULL, //&Specular,
	  0.5f,
	  0.f,
	  FALSE,
	  /*IFXShaderLitTexture::WIRE |*/ IFXShaderLitTexture::MATERIAL
	  )), IFXException);

  // Access the shader and turn on line rendering for the default
  // shader so we can see the bounding sphere.

  // Get the shader palette
  pShaderPalette = pSGUtils->m_pShaderPalette;
  TEST(pShaderPalette != NULL, IFXException);

  // Get the Shader on this model
  TEST(IFXSUCCESS(pShaderPalette->Find(L"", &uShaderID)), IFXException);
  TEST(IFXSUCCESS(pShaderPalette->GetResourcePtr(uShaderID, IID_IFXShaderLitTexture, (void **)&pShader)), IFXException);
  pShader->SetDrawLines(TRUE);
  pShader->SetLightingEnabled(TRUE);
  IFXRELEASE(pShader);

  IFXSimpleList* pList = NULL;
//  pGModifier->GetGlyphCommandList( &pList );

  IFXRELEASE(pGModifier);
  IFXRELEASE(pGenerator);
  IFXWriteManager *pIFXWriteManager = NULL;
  IFXWriteBuffer *pIFXWriteBuffer = NULL;
  IFXDECLARELOCAL(IFXStdio, pIFXStdio);

  //saving to file
/*  TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXWriteManager, IID_IFXWriteManager, (void**)&pIFXWriteManager)), IFXException);
  TEST(IFXSUCCESS(pIFXWriteManager->Initialize(pSGUtils->m_pCoreServices)), IFXException);
  TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioWriteBuffer, IID_IFXWriteBuffer, (void**)&pIFXWriteBuffer)), IFXException);
  TEST(IFXSUCCESS(pIFXWriteBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio)), IFXException);
  IFXString src1;
  src1 = getenv("U3D_SOURCE");
  src1 += "Tests/UnitTesting/Primitives/SampleGlyphGen/output.u3d";
  TEST(IFXSUCCESS(pIFXStdio->Open( (IFXCHAR*)src1.Raw() ) ), IFXException);
  TEST(IFXSUCCESS(pSGUtils->m_pSceneGraph->Mark()), IFXException);
  TEST(IFXSUCCESS(pIFXWriteManager->Write(pIFXWriteBuffer, IFXEXPORT_EVERYTHING)), IFXException);
  TEST(IFXSUCCESS(pIFXStdio->Close()), IFXException);

  IFXRELEASE( pIFXWriteBuffer );
  IFXRELEASE( pIFXWriteManager );*/
  return IFX_OK;
}
