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
#include "SampleJPGTextureResize.h"

#include "UTHeaders.h"
#include "IFXLoadManager.h"
#include "IFXWriteManager.h"
#include "IFXStdio.h"
#include "IFXReadBuffer.h"
#include "IFXWriteBuffer.h"

IFXRESULT ResizeJPGTextureImage::Run()
{
	IFXRESULT rc = IFX_FALSE;
	
	rc = IFXCOMInitialize();

	if( IFXSUCCESS(rc) )
	{
		IFXString src_base,dst_base;
		IFXString src,dst;
		src_base = getenv("U3D_SOURCE");
		dst_base = getenv("U3D_SOURCE");
		TransfomType type = _NONE;
		int tmp = 0;

		//1 Flag_128.PNG
		src = src_base + "Tests/UnitTesting/Textures/SampleJPGTextureResize/InputImages/shading.u3d";
		dst = dst_base + "Tests/UnitTesting/Textures/SampleJPGTextureResize/RussiaFlag_128.u3d";
		type = JPG_128;

		rc = ConvertTextureObjectCompressionProperties(src, dst, type );
		IFXASSERT(rc == IFX_OK);

		//2 Flag_256.PNG
		src = src_base + "Tests/UnitTesting/Textures/SampleJPGTextureResize/InputImages/shading.u3d";
		dst = dst_base + "Tests/UnitTesting/Textures/SampleJPGTextureResize/RussiaFlag_256.u3d";
		type = JPG_256;

		rc = ConvertTextureObjectCompressionProperties(src, dst, type );
		IFXASSERT(rc == IFX_OK);

		//3 Flag_512.PNG
		src = src_base + "Tests/UnitTesting/Textures/SampleJPGTextureResize/InputImages/shading.u3d";
		dst = dst_base + "Tests/UnitTesting/Textures/SampleJPGTextureResize/RussiaFlag_512.u3d";
		type = JPG_512;

		rc = ConvertTextureObjectCompressionProperties(src, dst, type );
		IFXASSERT(rc == IFX_OK);
	}

	if( IFXSUCCESS(rc) )
		rc = IFXCOMUninitialize();

	return rc;
} // Run()
	
IFXRESULT ResizeJPGTextureImage::ConvertTextureObjectCompressionProperties( const IFXString& src, const IFXString& dst, const TransfomType type )
{
	//
	IFXRESULT rc = IFX_OK;

	// the main initialization part
	IFXDECLARELOCAL(IFXCoreServices,pCoreServices);
	IFXDECLARELOCAL(IFXCoreServices, pCoreServicesMain);
	IFXDECLARELOCAL(IFXSceneGraph, pSceneGraph);

	rc = InitializeSceneGraph_CoreService( &pCoreServices, &pCoreServicesMain, &pSceneGraph );

	{
		IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
		IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);
		IFXDECLARELOCAL(IFXStdio, pIFXStdio1);

		//1. load as atomic mode
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager)), IFXException);
		TEST(IFXSUCCESS(pIFXLoadManager->Initialize(pCoreServices)), IFXException);
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer)), IFXException);
		TEST(IFXSUCCESS(pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio1)), IFXException);

		TEST(IFXSUCCESS(pIFXStdio1->Open( (IFXCHAR*) src.Raw())), IFXException);
		TEST(IFXSUCCESS(pIFXLoadManager->Load(pIFXReadBuffer, TRUE)), IFXException);
		TEST(IFXSUCCESS(pIFXStdio1->Close()), IFXException);
	}

	//2. reset new parameters to U3D file
	{
		IFXDECLARELOCAL(IFXPalette, pTexturePalette);
		U32 TexureObjectCount;

		TEST(IFXSUCCESS(pSceneGraph->GetPalette(IFXSceneGraph::TEXTURE, &pTexturePalette)), IFXException);
		pTexturePalette->GetPaletteSize(&TexureObjectCount);

		IFXTextureObject* pTextureObject[IFX_MAX_CONTINUATIONIMAGE_COUNT] = {NULL};

		U32 imageBufferSize;
		U8 ImageType = -1;
		IFXRESULT res = IFX_OK;

		U32 i, ContImageCount = 0;
		U8 BlockCompressionType[IFX_MAX_CONTINUATIONIMAGE_COUNT];

		STextureOutputInfo* pImageCopySrc =
			(STextureOutputInfo*)malloc(sizeof(STextureOutputInfo)*TexureObjectCount);

		IFXTextureObject::ChannelType
			ImageChannelFlagsList[IFX_MAX_CONTINUATIONIMAGE_COUNT] = {IFXTextureObject::IFXIMAGECHANNEL_RED};

		BOOL FileRef[IFX_MAX_CONTINUATIONIMAGE_COUNT] = {FALSE};

		IFXArray<IFXString*>* FileName[IFX_MAX_CONTINUATIONIMAGE_COUNT];
		IFXArray<IFXString*>* RetFileName[IFX_MAX_CONTINUATIONIMAGE_COUNT];

		for(i=0; i<IFX_MAX_CONTINUATIONIMAGE_COUNT; i++)
		{

			FileName[i] = new IFXArray<IFXString*>;
			FileName[i]->ResizeToExactly(1);
			(*(FileName[i]))[0] = NULL;

			RetFileName[i] = new IFXArray<IFXString*>;
			RetFileName[i]->ResizeToExactly(1);
			(*(RetFileName[i]))[0] = NULL;
		}

		IFXString src_base;
		IFXString src0,src1,src2,src3;
		src_base = getenv("U3D_SOURCE");

		U32 dirty = 1;
		for(U32 iTex=0; iTex<TexureObjectCount; iTex++)
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
					switch( type )
					{
						case JPG_128:
							if(iTex == 1)
							{

								src0 = src_base + "Tests/UnitTesting/Textures/SampleJPGTextureResize/InputImages/RussiaFlag_128.jpg";   //128128 pcs
								BlockCompressionType[0] = IFXTextureObject::TextureType_Jpeg24;

								ImageChannelFlagsList[0] = (IFXTextureObject::ChannelType)(IFXTextureObject::IFXIMAGECHANNEL_RED | IFXTextureObject::IFXIMAGECHANNEL_GREEN |
															IFXTextureObject::IFXIMAGECHANNEL_BLUE );

								FileRef[0] = TRUE;
								FileName[0]->ResizeToExactly(1);
								(*(FileName[0]))[0] = new IFXString (src0);		

								rc = pTextureObject[iTex]->SetImageCompressionProperties(1, BlockCompressionType, ImageChannelFlagsList,
																						FileRef, (IFXArray<IFXString*>**)FileName );
								IFXASSERT( rc == IFX_OK );
							}
							src0.Clear();src1.Clear();src2.Clear();
							break;

						case JPG_256:
							if(iTex == 1)
							{
								src0 = src_base + "Tests/UnitTesting/Textures/SampleJPGTextureResize/InputImages/RussiaFlag_256.jpg";   //256256 pcs
								BlockCompressionType[0] = IFXTextureObject::TextureType_Jpeg24;

								ImageChannelFlagsList[0] = (IFXTextureObject::ChannelType)(IFXTextureObject::IFXIMAGECHANNEL_RED | IFXTextureObject::IFXIMAGECHANNEL_GREEN |
															IFXTextureObject::IFXIMAGECHANNEL_BLUE);

								FileRef[0] = TRUE;
								FileName[0]->ResizeToExactly(1);
								(*(FileName[0]))[0] = new IFXString (src0);

								rc = pTextureObject[iTex]->SetImageCompressionProperties(1, BlockCompressionType, ImageChannelFlagsList,
																						FileRef, (IFXArray<IFXString*>**)FileName );
								IFXASSERT( rc == IFX_OK );
							}
							src0.Clear();src1.Clear();src2.Clear();
							break;

						case JPG_512:
							if(iTex == 1)
							{
								src0 = src_base + "Tests/UnitTesting/Textures/SampleJPGTextureResize/InputImages/RussiaFlag_512.jpg";   //512512 pcs
								BlockCompressionType[0] = IFXTextureObject::TextureType_Jpeg24;

								ImageChannelFlagsList[0] = (IFXTextureObject::ChannelType)(IFXTextureObject::IFXIMAGECHANNEL_RED | IFXTextureObject::IFXIMAGECHANNEL_GREEN | 
															IFXTextureObject::IFXIMAGECHANNEL_BLUE);

								FileRef[0] = TRUE;
								FileName[0]->ResizeToExactly(1);
								(*(FileName[0]))[0] = new IFXString (src0);

								rc = pTextureObject[iTex]->SetImageCompressionProperties(1, BlockCompressionType, ImageChannelFlagsList,
																						FileRef, (IFXArray<IFXString*>**)FileName );
								IFXASSERT( rc == IFX_OK );
							}
							src0.Clear();src1.Clear();src2.Clear();
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
				{
					free(pImageCopySrc[iTex].m_pData);
					pImageCopySrc[iTex].m_pData = NULL;
					dirty = 0;
				}
			}
		}

		for(i=0; i<IFX_MAX_CONTINUATIONIMAGE_COUNT; i++)
		{
			IFXDELETE(FileName[i]);
			IFXDELETE(RetFileName[i]);
		}

		// 3. write
		{
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
		}

		if(dirty)
		{
			for(U32 k = 0; k<TexureObjectCount; k++)
			{
				free(pImageCopySrc[k].m_pData);
				pImageCopySrc[k].m_pData = NULL;
				IFXRELEASE(pTextureObject[k]);
			}
		}

		if(pImageCopySrc)
			free(pImageCopySrc);
	}

	return rc;
}

