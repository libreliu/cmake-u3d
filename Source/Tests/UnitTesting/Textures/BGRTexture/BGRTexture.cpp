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
#ifndef BGRTEXTURE_H
#include "BGRTexture.h"
#endif

#include "UTHeaders.h"
#include "IFXLoadManager.h"
#include "IFXWriteManager.h"
#include "IFXStdio.h"
#include "IFXReadBuffer.h"
#include "IFXWriteBuffer.h"

IFXRESULT BGR_WriteRead::Run()
{
   	IFXRESULT rc = IFX_OK;

	rc = IFXCOMInitialize();

	if( IFXSUCCESS( rc ) )
		rc = Run_BGR();

	IFXASSERT(rc == IFX_OK);

	if( IFXSUCCESS( rc ) )
		rc = IFXCOMUninitialize();

	return rc;
}


IFXRESULT BGR_WriteRead::Run_BGR()
{
   	IFXRESULT rc = IFX_OK;

	IFXDECLARELOCAL(IFXCoreServices, pCoreServices);
	IFXDECLARELOCAL(IFXCoreServices, pCoreServicesMain);
	IFXDECLARELOCAL(IFXSceneGraph, pSceneGraph);
	TEST(IFXSUCCESS(InitializeSceneGraph_CoreService( &pCoreServices, &pCoreServicesMain, &pSceneGraph )), IFXException);

	{
		IFXString src_base,dst_base;
		IFXString src,dst;
		src_base = getenv("U3D_SOURCE");
		dst_base = getenv("U3D_SOURCE");
		TransfomType type = _NONE;
		int tmp = 0;

		//1
		src = src_base + "Tests/UnitTesting/Textures/BGRTexture/_test_red_tga.u3d";
		dst = dst_base + "Tests/UnitTesting/Textures/BGRTexture/_BGR.u3d";
		type = BGR;

		IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
		IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);
		IFXDECLARELOCAL(IFXWriteManager, pIFXWriteManager);
		IFXDECLARELOCAL(IFXWriteBuffer, pIFXWriteBuffer);
		IFXDECLARELOCAL(IFXStdio, pIFXStdio1);
		IFXDECLARELOCAL(IFXStdio, pIFXStdio2);
		IFXDECLARELOCAL(IFXScheduler, pScheduler);

		//1. load as atomic mode
		TEST(IFXSUCCESS(pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler)), IFXException);
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager)), IFXException);
		TEST(IFXSUCCESS(pIFXLoadManager->Initialize(pCoreServices)), IFXException);
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer)), IFXException);
		TEST(IFXSUCCESS(pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio1)), IFXException);

		TEST(IFXSUCCESS(pIFXStdio1->Open( (IFXCHAR*) src.Raw() )), IFXException);
		TEST(IFXSUCCESS(pIFXLoadManager->Load(pIFXReadBuffer, TRUE)), IFXException);
		TEST(IFXSUCCESS(pIFXStdio1->Close()), IFXException);

		//2. reset new parameters to U3D file

		U32 TexureObjectCount;
		IFXDECLARELOCAL(IFXPalette, pTexturePalette);
		TEST(IFXSUCCESS(pSceneGraph->GetPalette(IFXSceneGraph::TEXTURE, &pTexturePalette)), IFXException);
		pTexturePalette->GetPaletteSize(&TexureObjectCount);

		IFXUnknown* pObject[IFX_MAX_CONTINUATIONIMAGE_COUNT];
		IFXTextureObject* pTextureObject[IFX_MAX_CONTINUATIONIMAGE_COUNT];

		U32 imageBufferSize;
		STextureOutputInfo* pImageCopySrc;
		IFXRESULT res = IFX_OK;

		U32 ContImageCount = 0;
		U8 BlockCompressionType[IFX_MAX_CONTINUATIONIMAGE_COUNT];

		pImageCopySrc = (STextureOutputInfo*)malloc(sizeof(STextureOutputInfo)*TexureObjectCount);

		IFXTextureObject::ChannelType ImageChannelFlagsList[IFX_MAX_CONTINUATIONIMAGE_COUNT];
		ImageChannelFlagsList[0] = IFXTextureObject::IFXIMAGECHANNEL_RED;
		ImageChannelFlagsList[1] = IFXTextureObject::IFXIMAGECHANNEL_RED;
		ImageChannelFlagsList[2] = IFXTextureObject::IFXIMAGECHANNEL_RED;
		ImageChannelFlagsList[3] = IFXTextureObject::IFXIMAGECHANNEL_RED;

		U32 format = 0;
		U32 dirty = 1;
		for(U32 iTex=0; iTex<TexureObjectCount; iTex++)
		{
			pTexturePalette->GetResourcePtr(iTex,&(pObject[iTex]) );
			pObject[iTex]->QueryInterface(IID_IFXTextureObject, (void**)(&(pTextureObject[iTex])) );
			pObject[iTex]->Release();

			if ( pTextureObject[iTex] )
			{
				res = IFX_OK;
				res = pTextureObject[iTex]->MakeDirty(NULL);
				pTextureObject[iTex]->ForceCompressionState(IFXTEXTURECOMPRESSIONSTATE_FORCEDECOMPRESS);

				pTextureObject[iTex]->GetWidth(&(pImageCopySrc[iTex].m_width));
				pTextureObject[iTex]->GetHeight(&(pImageCopySrc[iTex].m_height));
				pTextureObject[iTex]->GetRawImageBufferSize( &imageBufferSize);
				pTextureObject[iTex]->GetRenderFormat(pImageCopySrc[iTex].eRenderFormat);

				pImageCopySrc[iTex].m_pData = (U8*)malloc( imageBufferSize );

				if (IFX_OK == res)
				{
					switch( type )
					{
						case BGR:
							BlockCompressionType[0] = IFXTextureObject::TextureType_Png;
							ImageChannelFlagsList[0] = (IFXTextureObject::ChannelType)(IFXTextureObject::IFXIMAGECHANNEL_RED | IFXTextureObject::IFXIMAGECHANNEL_GREEN |
														IFXTextureObject::IFXIMAGECHANNEL_BLUE);

							rc = pTextureObject[iTex]->SetImageCompressionProperties(1, BlockCompressionType, ImageChannelFlagsList, NULL,NULL);
							IFXASSERT(rc == IFX_OK );
							break;
						default:
							break;
					}

					if(IFXSUCCESS(rc))
					{
						rc = pTextureObject[iTex]->ForceCompressionState(IFXTEXTURECOMPRESSIONSTATE_FORCECOMPRESSED);
					}

				}
				else
				{ // something wrong w/ texture
					free(pImageCopySrc[iTex].m_pData);
					pImageCopySrc[iTex].m_pData = NULL;
					dirty = 0;
				}
			}
		}

		// 3. write
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXWriteManager, IID_IFXWriteManager, (void**)&pIFXWriteManager)), IFXException);
		TEST(IFXSUCCESS(pIFXWriteManager->Initialize(pCoreServices)), IFXException);
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioWriteBuffer, IID_IFXWriteBuffer, (void**)&pIFXWriteBuffer)), IFXException);
		TEST(IFXSUCCESS(pIFXWriteBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio2)), IFXException);
		TEST(IFXSUCCESS(pIFXStdio2->Open( (IFXCHAR*) dst.Raw() )), IFXException);

		TEST(IFXSUCCESS(pSceneGraph->Mark()), IFXException);
		TEST(IFXSUCCESS(pIFXWriteManager->Write(pIFXWriteBuffer, IFXEXPORT_EVERYTHING)), IFXException);
		TEST(IFXSUCCESS(pIFXStdio2->Close()), IFXException);

		if(dirty)
		{
			for(U32 k = 0; k<TexureObjectCount; k++)
			{
				free(pImageCopySrc[k].m_pData);
				pImageCopySrc[k].m_pData = NULL;
				pTextureObject[k]->Release();
			}
		}

		if(pImageCopySrc)
			free(pImageCopySrc);
	}

	return rc;
}

