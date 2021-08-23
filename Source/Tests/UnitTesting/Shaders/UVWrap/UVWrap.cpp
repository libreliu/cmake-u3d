//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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

#ifndef UVWrap_H
#include "UVWrap.h"
#endif

#include "U3DSceneUtilities.h"

IFXRESULT UVWrap::Run( U3DSceneUtilities* pSGUtils )
{
  TEST(IFXSUCCESS(pSGUtils->InitializeScene()), IFXException);

  // load file with texture
  {
    IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
    IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);
    IFXDECLARELOCAL(IFXStdio, pIFXStdio);

    TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager)), IFXException);
    TEST(IFXSUCCESS(pIFXLoadManager->Initialize(pSGUtils->m_pCoreServices)), IFXException);
    TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer)), IFXException);
    TEST(IFXSUCCESS(pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio)), IFXException);

	IFXString src;
	src = getenv("U3D_SOURCE");
	src += L"Tests/UnitTesting/Shaders/UVWrap/sphere_shading.u3d";

	IFXRESULT res = pIFXStdio->Open( (IFXCHAR*) src.Raw() );
    TEST(IFXSUCCESS( res ), IFXException );

    TEST(IFXSUCCESS(pIFXLoadManager->Load(pIFXReadBuffer)), IFXException);
    TEST(IFXSUCCESS(pIFXStdio->Close()), IFXException);
  }

  //
  {
    IFXPalette* pPalette = NULL;
    IFXShaderLitTexture* pShader = NULL;

    // Get shader palette
    {
      IFXSceneGraph* pSceneGraph = NULL;

      TEST(
        IFXSUCCESS(pSGUtils->m_pCoreServices->GetSceneGraph(
                            IID_IFXSceneGraph,
                            (void**)&pSceneGraph )),
        IFXException);

      TEST(
        IFXSUCCESS(pSceneGraph->GetPalette(
                      IFXSceneGraph::SHADER,
                      &pPalette )),
        IFXException);

      IFXRELEASE( pSceneGraph );
    }

    // Get Shader and play with wrap transform
    {
      U32 shaderId;
      IFXMatrix4x4* pWrapTransform = NULL;

      TEST(
        IFXSUCCESS(pPalette->Find(L"Sphere010", &shaderId)),
        IFXException);

      TEST(
        IFXSUCCESS(pPalette->GetResourcePtr(
                      shaderId, IID_IFXShaderLitTexture,
                      (void **)&pShader)),
        IFXException);

      IFXRELEASE( pPalette );

      TEST(IFXSUCCESS(pShader->GetWrapTransform(
                          0,
                          &pWrapTransform)),
        IFXException);

      //pWrapTransform->Rotate3x4(0.01f, IFX_Z_AXIS);
	  pWrapTransform->Rotate3x4( 0.5f, IFX_Y_AXIS);

      IFXRELEASE( pPalette );
      IFXRELEASE( pShader );
    }
  }

  return IFX_OK;
}
