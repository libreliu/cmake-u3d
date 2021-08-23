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
#ifndef IMAGEWRITEREAD_H
#include "SampleTextureCombine.h"
#endif

#include "UTHeaders.h"
#include "IFXLoadManager.h"
#include "IFXWriteManager.h"
#include "IFXStdio.h"
#include "IFXReadBuffer.h"
#include "IFXWriteBuffer.h"

IFXRESULT ImageWriteReadCombine::Run()
{
	IFXRESULT rc = IFX_OK;

	rc = IFXCOMInitialize();

	if(IFXSUCCESS(rc))
		rc = RunTest();

	if(IFXSUCCESS(rc))
		rc = IFXCOMUninitialize();

	return rc;
}

IFXRESULT ImageWriteReadCombine::RunTest()
{
	IFXRESULT rc = IFX_OK;

	IFXDECLARELOCAL(IFXCoreServices, pCoreServices);
	IFXDECLARELOCAL(IFXCoreServices, pCoreServicesMain);
	IFXDECLARELOCAL(IFXSceneGraph, pSceneGraph);
	
	rc = InitializeSceneGraph_CoreService( &pCoreServices, &pCoreServicesMain, &pSceneGraph );

	if(IFXSUCCESS(rc))
	{
		IFXString src_base,dst_base;
		IFXString src,dst;
		src_base = getenv("U3D_SOURCE");
		dst_base = getenv("U3D_SOURCE");
		int tmp = 0;

		//0
		//RGBA( 32 bype per pixel image) convert 1 PNG with 1 continuation image
		src = src_base + "Tests/UnitTesting/Textures/SampleTextureCombine/InputImages/rgba.u3d"; 
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTextureCombine/__AlphaExt+RGBInternal.u3d";

		//1. load as atomic mode
		IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
		IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);
		IFXDECLARELOCAL(IFXStdio, pIFXStdio1);

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

		IFXTextureObject* pTextureObject[IFX_MAX_CONTINUATIONIMAGE_COUNT];

		U32 imageBufferSize;		
		U8 ImageType = -1;
		STextureOutputInfo* pImageCopySrc = NULL;
		IFXRESULT res = IFX_OK;

		U32 ContImageCount = 0;
		U32 i;
		U8 BlockCompressionType[IFX_MAX_CONTINUATIONIMAGE_COUNT];

		pImageCopySrc = (STextureOutputInfo*)malloc(sizeof(STextureOutputInfo)*TexureObjectCount);
		IFXASSERT(pImageCopySrc != IFX_OK );

		IFXTextureObject::ChannelType ImageChannelFlagsList[IFX_MAX_CONTINUATIONIMAGE_COUNT] = {IFXTextureObject::IFXIMAGECHANNEL_RED};

		BOOL	FileRef[IFX_MAX_CONTINUATIONIMAGE_COUNT] = {FALSE};

		IFXArray<IFXString*>* FileName[IFX_MAX_CONTINUATIONIMAGE_COUNT];

		for(i=0; i<IFX_MAX_CONTINUATIONIMAGE_COUNT; i++)
		{
			FileName[i] = new IFXArray<IFXString*>;
			FileName[i]->ResizeToExactly(1);
			(*(FileName[i]))[0] = NULL;	
		}

		IFXString src0;
		U32 dirty = 1, iTex;
		for(iTex=0; iTex<TexureObjectCount; iTex++)
		{
			IFXUnknown* pObject = NULL;
			pTexturePalette->GetResourcePtr(iTex,&pObject);
			pObject->QueryInterface(IID_IFXTextureObject, (void**)(&(pTextureObject[iTex])) );
			IFXRELEASE(pObject);

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
					if(iTex == 1)
					{
						BlockCompressionType[0] = IFXTextureObject::TextureType_Jpeg24;
						BlockCompressionType[1] = IFXTextureObject::TextureType_Png;
						ImageChannelFlagsList[0] = (IFXTextureObject::ChannelType) (IFXTextureObject::IFXIMAGECHANNEL_RED |
																					IFXTextureObject::IFXIMAGECHANNEL_BLUE | 
																					IFXTextureObject::IFXIMAGECHANNEL_GREEN);
						ImageChannelFlagsList[1] = IFXTextureObject::IFXIMAGECHANNEL_ALPHA;

						FileRef[0] = FALSE; 
						FileRef[1] = TRUE;

						src0 = src_base + "Tests/UnitTesting/Textures/SampleTextureCombine/InputImages/_alpha.png";
						FileName[0]->ResizeToExactly(0);
						FileName[1]->ResizeToExactly(1);
						(*(FileName[1]))[0] = new IFXString (src0);		

						rc = pTextureObject[iTex]->SetImageCompressionProperties(2, BlockCompressionType, ImageChannelFlagsList,
																					FileRef, (IFXArray<IFXString*>**)FileName);

						IFXASSERT(rc == IFX_OK );													
					}			
					if(IFXSUCCESS(rc))
					{
						rc = pTextureObject[iTex]->ForceCompressionState(IFXTEXTURECOMPRESSIONSTATE_FORCECOMPRESSED);
					}

				}
				else
				{ // if something wrong with compression
					if(pImageCopySrc[iTex].m_pData)
    				{
						free(pImageCopySrc[iTex].m_pData);
						pImageCopySrc[iTex].m_pData = NULL;
					}

					if(pImageCopySrc)
						free( pImageCopySrc );
					dirty = 0;
				}
			}
		}

		delete (*(FileName[1]))[0];
		for(i=0; i<IFX_MAX_CONTINUATIONIMAGE_COUNT; i++)
		{
			delete FileName[i];
		}

		// 3. write	
		IFXDECLARELOCAL(IFXWriteManager, pIFXWriteManager);
		IFXDECLARELOCAL(IFXWriteBuffer, pIFXWriteBuffer);
		IFXDECLARELOCAL(IFXStdio, pIFXStdio2);

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
			U32 k;
			for(k = 0; k<TexureObjectCount; k++)
			{
				if(pImageCopySrc[k].m_pData)
				{
					free(pImageCopySrc[k].m_pData);
					pImageCopySrc[k].m_pData = NULL;
				}
				IFXRELEASE(pTextureObject[k]);
			}

			if(pImageCopySrc)
				free( pImageCopySrc );
		}
	}

	return rc;
}

