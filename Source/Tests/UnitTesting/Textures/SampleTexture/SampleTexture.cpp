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
#include "SampleTexture.h"
#endif

#include "UTHeaders.h"
#include "IFXLoadManager.h"
#include "IFXWriteManager.h"
#include "IFXStdio.h"
#include "IFXReadBuffer.h"
#include "IFXWriteBuffer.h"

IFXRESULT ImageWriteRead::Run()
{
		IFXRESULT rc = IFX_FALSE;
		
		IFXCOMInitialize();

		{
		IFXString src_base,dst_base;
		IFXString src,dst;
		src_base = getenv("U3D_SOURCE");
		dst_base = getenv("U3D_SOURCE");
		TransfomType type = _NONE;
		int tmp = 0;

		//0
		src = src_base + "Tests/UnitTesting/Textures/SampleTexture/shading.u3d";
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTexture/__shading_dst_1_PNG.u3d";
		type = _JPEG24_TO_1_PNG;
if(1) {
			rc = ConvertTextureObjectCompressionProperties(src, dst, type );
			IFXASSERT(rc == IFX_OK);
}
		cls(src,dst);


		//1
		src = src_base + "Tests/UnitTesting/Textures/SampleTexture/shading.u3d";
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTexture/__shading_dst_1_jpeg24.u3d";
		type = _JPEG24_TO_1_JPEG24;
if(1) { 
			rc = ConvertTextureObjectCompressionProperties(src, dst, type );
			IFXASSERT(rc == IFX_OK);
}
		cls(src,dst);

		//2
		// jpeg24 convert to JPEG8 with 3 continuation image
		src = src_base + "Tests/UnitTesting/Textures/SampleTexture/shading.u3d";
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTexture/__shading_dst_3_jpeg8.u3d";
		type = _JPEG24_TO_3_JPEG8;
if(1) {
			rc = ConvertTextureObjectCompressionProperties(src, dst, type );
			IFXASSERT(rc == IFX_OK);
}
		cls(src,dst);

		//3
		// jpeg24 convert to PNG with 3 continuation image
		src = src_base + "Tests/UnitTesting/Textures/SampleTexture/shading.u3d";
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTexture/__shading_dst_3_png.u3d";
		type = _JPEG24_TO_3_PNG;

if(1) {
			rc = ConvertTextureObjectCompressionProperties(src, dst, type );
			IFXASSERT(rc == IFX_OK);
}
		cls(src,dst);

		//4
		//RGBA( 32 bype per pixel image) convert 1 PNG with 1 continuation image
		src = src_base + "Tests/UnitTesting/Textures/SampleTexture/shading_lines_alpha_tex1.u3d";
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTexture/__shading_dst_1_RGBA_PNG.u3d";
		type = _RGBA_TO_1_PNG;

if(1) {
			rc = ConvertTextureObjectCompressionProperties(src, dst, type );
			IFXASSERT(rc == IFX_OK);
}
		cls(src,dst);

		//5
		// RGBA( 32 bype per pixel image) convert to PNG+JPEG8 with 2 continuation images
		src = src_base + "Tests/UnitTesting/Textures/SampleTexture/shading_lines_alpha_tex1.u3d";
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTexture/__shading_dst_2_RGBA_PNG_JPEG8.u3d";
		type = _RGBA_TO_2_PNGandJPEG8;
if(1) {
			rc = ConvertTextureObjectCompressionProperties(src, dst, type );
			IFXASSERT(rc == IFX_OK);
}
		cls(src,dst);

		//6
		// RGBA( 32 bype per pixel image) convert to JPEG24+JPEG8 with 2 continuation images
		src = src_base + "Tests/UnitTesting/Textures/SampleTexture/shading_lines_alpha_tex1.u3d";
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTexture/__shading_dst_2_RGBA_JPEG24_JPEG8.u3d";
		type = _RGBA_TO_2_JPEG24andJPEG8;
if(1) {
		rc = ConvertTextureObjectCompressionProperties(src, dst, type );
		IFXASSERT(rc == IFX_OK);
}
		cls(src,dst);

		//7
		// RGBA( 32 bype per pixel image) convert to JPEG24+PNG with 2 continuation images
		src = src_base + "Tests/UnitTesting/Textures/SampleTexture/shading_lines_alpha_tex1.u3d"; ;
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTexture/__shading_dst_2_RGBA_JPEG24_PNG.u3d";
		type = _RGBA_TO_2_JPEG24andPNG;
if(1) {
		rc = ConvertTextureObjectCompressionProperties(src, dst, type );
		IFXASSERT(rc == IFX_OK);
}
		cls(src,dst);

		//8
		// RGBA( 32 bype per pixel image) convert to PNG+PNG with 2 continuation images
		src = src_base + "Tests/UnitTesting/Textures/SampleTexture/shading_lines_alpha_tex1.u3d";
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTexture/__shading_dst_2_RGBA_PNG_PNG.u3d";
		type = _RGBA_TO_2_PNGandPNG;
if(1) {
		rc = ConvertTextureObjectCompressionProperties(src, dst, type );
		IFXASSERT(rc == IFX_OK);
}
		cls(src,dst);

		// 9
		// _RGBA_TO_4_PNG ( 32 bytes per pixel ) convert to 4 continuation images by PNG
		src = src_base + "Tests/UnitTesting/Textures/SampleTexture/shading_lines_alpha_tex1.u3d";
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTexture/__shading_dst_4_RGBA_PNG.u3d";
		type = _RGBA_TO_4_PNG;
if(1) { //passed completely
		rc = ConvertTextureObjectCompressionProperties(src, dst, type );
		IFXASSERT(rc == IFX_OK);
}
		cls(src,dst);

		// 10
		// _RGBA_TO_4_JPEG8 ( 32 bytes per pixel ) convert to 4 continuation images by JPEG8
		src = src_base + "Tests/UnitTesting/Textures/SampleTexture/shading_lines_alpha_tex1.u3d";
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTexture/__shading_dst_4_RGBA_JPEG8.u3d";
		type = _RGBA_TO_4_JPEG8;
if(1) {
		rc = ConvertTextureObjectCompressionProperties(src, dst, type );
		IFXASSERT(rc == IFX_OK);
}
		cls(src,dst);

		// 11
		// _RGBA_TO_4_JPEG8_2_FILES ( 32 bytes per pixel ) convert to 4 continuation images by JPEG8 and
		//							makes refs for 2 external files for 2 blocks
		src = src_base + "Tests/UnitTesting/Textures/SampleTexture/shading_lines_alpha_tex1.u3d";
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTexture/__shading_dst_4_RGBA_JPEG8_2_FILES.u3d";
		type = _RGBA_TO_3_JPEG8_2_FILES;
if(false) {
		rc = ConvertTextureObjectCompressionProperties(src, dst, type );
		IFXASSERT(rc == IFX_OK);
}
		cls(src,dst);

		// 12
		// _RGBA_TO_2_JPEG8_2_PNG ( 32 bytes per pixel ) convert to 4 continuation images by JPEG8 and PNG
		src = src_base + "Tests/UnitTesting/Textures/SampleTexture/shading_lines_alpha_tex1.u3d";
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTexture/__shading_dst_4_RGBA_2JPEG8+2PNG.u3d";
		type = _RGBA_TO_4_JPEG8;
if(1) {
		rc = ConvertTextureObjectCompressionProperties(src, dst, type );
		IFXASSERT(rc == IFX_OK);
}
		cls(src,dst);

		// 13
		// _RGBA_TO_3_JPEG8_1_PNG ( 32 bytes per pixel ) convert to 4 continuation images by PNG and JPEG8
		src = src_base + "Tests/UnitTesting/Textures/SampleTexture/shading_lines_alpha_tex1.u3d";
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTexture/__shading_dst_4_RGBA_3JPEG8+1PNG.u3d";
		type = _RGBA_TO_3_JPEG8_1_PNG;
if(1) {
		rc = ConvertTextureObjectCompressionProperties(src, dst, type );
		IFXASSERT(rc == IFX_OK);
}
		cls(src,dst);

		// 14
		// _RGBA_TO_1_JPEG8_3_PNG ( 32 bytes per pixel ) convert to 4 continuation images by PNG and JPEG8
		src = src_base + "Tests/UnitTesting/Textures/SampleTexture/shading_lines_alpha_tex1.u3d";
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTexture/__shading_dst_4_RGBA_1JPEG8+3PNG.u3d";
		type = _RGBA_TO_1_JPEG8_3_PNG;
if(1) {
		rc = ConvertTextureObjectCompressionProperties(src, dst, type );
		IFXASSERT(rc == IFX_OK);
}
		cls(src,dst);

		// 15
		// _RGBA_TO_3_PNG_1_JPEG8 ( 32 bytes per pixel ) convert to 4 continuation images by PNG and JPEG8
		src = src_base + "Tests/UnitTesting/Textures/SampleTexture/shading_lines_alpha_tex1.u3d";
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTexture/__shading_dst_4_RGBA_3PNG+1JPEG8.u3d";
		type = _RGBA_TO_3_PNG_1_JPEG8;
if(1) {
		rc = ConvertTextureObjectCompressionProperties(src, dst, type );
		IFXASSERT(rc == IFX_OK);
}
		cls(src,dst);

		// 16
		// _RGBA_TO_2_PNG_2_JPEG8 ( 32 bytes per pixel ) convert to 4 continuation images by PNG and JPEG8
		src = src_base + "Tests/UnitTesting/Textures/SampleTexture/shading_lines_alpha_tex1.u3d"; 
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTexture/__shading_dst_4_RGBA_2PNG+2JPEG8.u3d";
		type = _RGBA_TO_2_PNG_2_JPEG8;
if(1) { 
		rc = ConvertTextureObjectCompressionProperties(src, dst, type );
		IFXASSERT(rc == IFX_OK);
}
		cls(src,dst);

		// 17
		// _RGBA_TO_1_PNG_3_JPEG8 ( 32 bytes per pixel ) convert to 4 continuation images by PNG and JPEG8
		src = src_base + "Tests/UnitTesting/Textures/SampleTexture/shading_lines_alpha_tex1.u3d"; 
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTexture/__shading_dst_4_RGBA_1PNG+3JPEG8.u3d";
		type = _RGBA_TO_1_PNG_3_JPEG8;
if(1) { 
		rc = ConvertTextureObjectCompressionProperties(src, dst, type );
		IFXASSERT(rc == IFX_OK);
}
		cls(src,dst);

		// 18
		// _RGB_TO_1_FILE ( 24 bytes per pixel ) convert to 1 continuation images by JPG24 and 
		//							makes refs for 1 external file for 1 block 
		src = src_base + "Tests/UnitTesting/Textures/SampleTexture/shading.u3d"; 
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTexture/__shading_dst_1_RGB_JPEG24_1_FILE.u3d";
		type = _RGB_TO_1_FILE;
if(1) {  
		rc = ConvertTextureObjectCompressionProperties(src, dst, type );
		IFXASSERT(rc == IFX_OK);
}
		cls(src,dst);

		//19
		// _PNG24_TO_1_FILE ( 24 bytes per pixel ) convert to 1 continuation images by png and 
		//							makes refs for 1 external file for 1 block 
		src = src_base + "Tests/UnitTesting/Textures/SampleTexture/shading.u3d"; 
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTexture/__shading_dst_1_RGB_PNG24_1_FILE.u3d";
		type = _PNG24_TO_1_FILE;
if(1) { 
		rc = ConvertTextureObjectCompressionProperties(src, dst, type );
		IFXASSERT(rc == IFX_OK);
}
		cls(src,dst);


		//20
		//mixed color channels checking
		//_RGBA_TO_4_PNG_mixed1,_RGBA_TO_4_PNG_mixed2,_RGBA_TO_4_PNG_mixed3,_RGBA_TO_4_PNG_mixed4
		src = src_base + "Tests/UnitTesting/Textures/SampleTexture/shading_lines_alpha_tex1.u3d"; 
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTexture/__dst_4_RGBA_PNG_mixed1.u3d";
		type = _RGBA_TO_4_PNG_mixed1;
if(1) { 
		rc = ConvertTextureObjectCompressionProperties(src, dst, type );
		IFXASSERT(rc == IFX_OK);
}
		cls(src,dst);

		//21
		src = src_base + "Tests/UnitTesting/Textures/SampleTexture/shading_lines_alpha_tex1.u3d"; 
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTexture/__dst_4_RGBA_PNG_mixed2.u3d";
		type = _RGBA_TO_4_PNG_mixed2;
if(1) { 
		rc = ConvertTextureObjectCompressionProperties(src, dst, type );
		IFXASSERT(rc == IFX_OK);
}
		cls(src,dst);

		//22
		src = src_base + "Tests/UnitTesting/Textures/SampleTexture/shading_lines_alpha_tex1.u3d"; 
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTexture/__dst_4_RGBA_PNG_mixed3.u3d";
		type = _RGBA_TO_4_PNG_mixed3;
if(1) { 
		rc = ConvertTextureObjectCompressionProperties(src, dst, type );
		IFXASSERT(rc == IFX_OK);
}
		cls(src,dst);


		//23
		src = src_base + "Tests/UnitTesting/Textures/SampleTexture/shading_lines_alpha_tex1.u3d"; 
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTexture/__dst_4_RGBA_PNG_mixed4.u3d";
		type = _RGBA_TO_4_PNG_mixed4;
if(1) { 
		rc = ConvertTextureObjectCompressionProperties(src, dst, type );
		IFXASSERT(rc == IFX_OK);
}
		cls(src,dst);

		//24
		// _PNG32_TO_1_FILE ( 32 bytes per pixel ) convert to 1 continuation images by png and 
		//							makes refs for 1 external file for 1 block 
		src = src_base + "Tests/UnitTesting/Textures/SampleTexture/shading_lines_alpha_tex1.u3d"; 
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTexture/_PNG32_TO_1_FILE.u3d";
		type = _PNG32_TO_1_FILE;
if(1) { 
		rc = ConvertTextureObjectCompressionProperties(src, dst, type );
		IFXASSERT(rc == IFX_OK);
}
		cls(src,dst);
	}
		
		IFXCOMUninitialize();
		return IFX_OK;
} // Run()
	
IFXRESULT ImageWriteRead::ConvertTextureObjectCompressionProperties( const IFXString& src, const IFXString& dst, const TransfomType type )
{
	//
	IFXRESULT rc = IFX_OK;

	// the main initialization part
	IFXDECLARELOCAL(IFXCoreServices, pCoreServices);
	IFXDECLARELOCAL(IFXCoreServices, pCoreServicesMain);
	IFXDECLARELOCAL(IFXSceneGraph, pSceneGraph);
	TEST(IFXSUCCESS(InitializeSceneGraph_CoreService( &pCoreServices, &pCoreServicesMain, &pSceneGraph )), IFXException);

	IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
	IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);
	IFXDECLARELOCAL(IFXWriteManager, pIFXWriteManager);
	IFXDECLARELOCAL(IFXWriteBuffer, pIFXWriteBuffer);
	IFXDECLARELOCAL(IFXStdio, pIFXStdio1);
	IFXDECLARELOCAL(IFXStdio, pIFXStdio2);

	//1. load as atomic mode
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager)), IFXException);
	TEST(IFXSUCCESS(pIFXLoadManager->Initialize(pCoreServices)), IFXException);
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer)), IFXException);
	TEST(IFXSUCCESS(pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio1)), IFXException);

	TEST(IFXSUCCESS(pIFXStdio1->Open( (IFXCHAR*) src.Raw() )), IFXException);
	TEST(IFXSUCCESS(pIFXLoadManager->Load(pIFXReadBuffer, TRUE)), IFXException);
	TEST(IFXSUCCESS(pIFXStdio1->Close()), IFXException);

	//2. reset new parameters to U3D file	

	IFXTextureObject* pTextureObject[IFX_MAX_CONTINUATIONIMAGE_COUNT];

	U32 imageBufferSize;		
	U8 ImageType = -1;
	STextureOutputInfo* pImageCopySrc = NULL;
	IFXRESULT res = IFX_OK;

	U32 ContImageCount = 0,  RetImageCount;
	U32 i;
	U8 BlockCompressionType[IFX_MAX_CONTINUATIONIMAGE_COUNT], RetBlockCompressionType[IFX_MAX_CONTINUATIONIMAGE_COUNT]; 

	U32 TexureObjectCount;
	IFXDECLARELOCAL(IFXPalette, pTexturePalette);
	TEST(IFXSUCCESS(pSceneGraph->GetPalette(IFXSceneGraph::TEXTURE, &pTexturePalette)), IFXException);
	pTexturePalette->GetPaletteSize(&TexureObjectCount);

	pImageCopySrc = (STextureOutputInfo*)malloc(sizeof(STextureOutputInfo)*TexureObjectCount);

	IFXTextureObject::ChannelType ImageChannelFlagsList[IFX_MAX_CONTINUATIONIMAGE_COUNT] = {IFXTextureObject::IFXIMAGECHANNEL_RED};
	IFXTextureObject::ChannelType RetImageChannelFlagsList[IFX_MAX_CONTINUATIONIMAGE_COUNT];

	BOOL FileRef[IFX_MAX_CONTINUATIONIMAGE_COUNT] = {FALSE}, RetFileRef[IFX_MAX_CONTINUATIONIMAGE_COUNT];

    IFXArray<IFXString*>	*FileName[IFX_MAX_CONTINUATIONIMAGE_COUNT], *RetFileName[IFX_MAX_CONTINUATIONIMAGE_COUNT];

	for(i=0; i<IFX_MAX_CONTINUATIONIMAGE_COUNT; i++)
	{
		FileName[i] = new IFXArray<IFXString*>;
		FileName[i]->ResizeToExactly(1);
		(*(FileName[i]))[0] = NULL;

		RetFileName[i] = new IFXArray<IFXString*>;
		RetFileName[i]->ResizeToExactly(1);
		(*(RetFileName[i]))[0] = NULL;
	}

	U32 dirty = 1, iTex;
	for(iTex=0; iTex<TexureObjectCount; iTex++)
	{
		IFXUnknown* pObject = NULL;
		pTexturePalette->GetResourcePtr(iTex,&pObject);
		pObject->QueryInterface(IID_IFXTextureObject, (void**)(&(pTextureObject[iTex])) );
		IFXRELEASE(pObject);

		if ( pTextureObject[iTex] )
		{
			res = IFX_OK;  // just reset it
			res = pTextureObject[iTex]->MakeDirty(NULL);
			pTextureObject[iTex]->ForceCompressionState(IFXTEXTURECOMPRESSIONSTATE_FORCEDECOMPRESS);

			pTextureObject[iTex]->GetWidth(&(pImageCopySrc[iTex].m_width));
			pTextureObject[iTex]->GetHeight(&(pImageCopySrc[iTex].m_height));
			pTextureObject[iTex]->GetRawImageBufferSize( &imageBufferSize);
			pTextureObject[iTex]->GetRenderFormat(pImageCopySrc[iTex].eRenderFormat);
		
			pImageCopySrc[iTex].m_pData = (U8*)malloc( imageBufferSize );

			// save for data comparison 
			m_texNumber = iTex;
			m_imageBufferSize = imageBufferSize;
			m_compare = false;

			m_pImageCopySrc = (U8*)malloc( imageBufferSize );
			memcpy(m_pImageCopySrc, pImageCopySrc[iTex].m_pData, m_imageBufferSize );

			if (IFX_OK == res)
			{
				switch( type )
				{
					case _JPEG24_TO_1_PNG:				
						BlockCompressionType[0] = IFXTextureObject::TextureType_Png;
						ImageChannelFlagsList[0] = (IFXTextureObject::ChannelType)(IFXTextureObject::IFXIMAGECHANNEL_RED | IFXTextureObject::IFXIMAGECHANNEL_GREEN | 
													IFXTextureObject::IFXIMAGECHANNEL_BLUE);
						rc = pTextureObject[iTex]->SetImageCompressionProperties(1, BlockCompressionType, ImageChannelFlagsList,NULL,NULL);
						IFXASSERT(rc == IFX_OK );
						m_compare = true;
						break;

					case _JPEG24_TO_1_JPEG24:

						BlockCompressionType[0] = IFXTextureObject::TextureType_Jpeg24;
						ImageChannelFlagsList[0] = (IFXTextureObject::ChannelType)(IFXTextureObject::IFXIMAGECHANNEL_RED | IFXTextureObject::IFXIMAGECHANNEL_GREEN | 
													IFXTextureObject::IFXIMAGECHANNEL_BLUE);
						rc = pTextureObject[iTex]->SetImageCompressionProperties(1, BlockCompressionType, ImageChannelFlagsList,NULL,NULL);
						IFXASSERT(rc == IFX_OK );						
						m_compare = true;
						break;

					case _JPEG24_TO_3_JPEG8:
						BlockCompressionType[0] = IFXTextureObject::TextureType_Jpeg8;
						BlockCompressionType[1] = IFXTextureObject::TextureType_Jpeg8;
						BlockCompressionType[2] = IFXTextureObject::TextureType_Jpeg8;
						ImageChannelFlagsList[0] = IFXTextureObject::IFXIMAGECHANNEL_RED;
						ImageChannelFlagsList[1] = IFXTextureObject::IFXIMAGECHANNEL_GREEN;
						ImageChannelFlagsList[2] = IFXTextureObject::IFXIMAGECHANNEL_BLUE;
						rc = pTextureObject[iTex]->SetImageCompressionProperties(3, BlockCompressionType, ImageChannelFlagsList,NULL,NULL);
						IFXASSERT(rc == IFX_OK );												
						m_compare = true;
						break;

					case _JPEG24_TO_3_PNG:
						BlockCompressionType[0] = IFXTextureObject::TextureType_Png;
						BlockCompressionType[1] = IFXTextureObject::TextureType_Png;
						BlockCompressionType[2] = IFXTextureObject::TextureType_Png;
						ImageChannelFlagsList[0] = IFXTextureObject::IFXIMAGECHANNEL_RED;
						ImageChannelFlagsList[1] = IFXTextureObject::IFXIMAGECHANNEL_GREEN;
						ImageChannelFlagsList[2] = IFXTextureObject::IFXIMAGECHANNEL_BLUE;
						rc = pTextureObject[iTex]->SetImageCompressionProperties(3, BlockCompressionType, ImageChannelFlagsList,NULL,NULL);
						IFXASSERT(rc == IFX_OK );												
						m_compare = true;
						break;

					case _RGBA_TO_1_PNG:
						if(iTex == 1){
							BlockCompressionType[0] = IFXTextureObject::TextureType_Png;
							ImageChannelFlagsList[0] = (IFXTextureObject::ChannelType) (IFXTextureObject::IFXIMAGECHANNEL_RED | 
														IFXTextureObject::IFXIMAGECHANNEL_BLUE | 
														IFXTextureObject::IFXIMAGECHANNEL_GREEN | IFXTextureObject::IFXIMAGECHANNEL_ALPHA );
							rc = pTextureObject[iTex]->SetImageCompressionProperties(1, BlockCompressionType, ImageChannelFlagsList,NULL,NULL);
							IFXASSERT(rc == IFX_OK );													
							m_compare = true;
						}
						break;

					case _RGBA_TO_2_PNGandJPEG8:						
						if(iTex == 1){
							BlockCompressionType[0] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[1] = IFXTextureObject::TextureType_Jpeg8;
							ImageChannelFlagsList[0] = (IFXTextureObject::ChannelType) (IFXTextureObject::IFXIMAGECHANNEL_RED |
														IFXTextureObject::IFXIMAGECHANNEL_BLUE | IFXTextureObject::IFXIMAGECHANNEL_GREEN );
							ImageChannelFlagsList[1] = IFXTextureObject::IFXIMAGECHANNEL_ALPHA;
							rc = pTextureObject[iTex]->SetImageCompressionProperties(2, BlockCompressionType, ImageChannelFlagsList,NULL,NULL);
							IFXASSERT(rc == IFX_OK );													
							m_compare = true;
						}
						break;

					case _RGBA_TO_2_JPEG24andJPEG8:
						if(iTex == 1){
							BlockCompressionType[0] = IFXTextureObject::TextureType_Jpeg24;
							BlockCompressionType[1] = IFXTextureObject::TextureType_Jpeg8;
							ImageChannelFlagsList[0] = (IFXTextureObject::ChannelType) (IFXTextureObject::IFXIMAGECHANNEL_RED | 
														IFXTextureObject::IFXIMAGECHANNEL_BLUE |IFXTextureObject::IFXIMAGECHANNEL_GREEN );
							ImageChannelFlagsList[1] = IFXTextureObject::IFXIMAGECHANNEL_ALPHA;
							rc = pTextureObject[iTex]->SetImageCompressionProperties(2, BlockCompressionType, ImageChannelFlagsList,NULL,NULL);
							IFXASSERT(rc == IFX_OK );													
							m_compare = true;
						}
						break;

					case _RGBA_TO_2_JPEG24andPNG:
						if(iTex == 1){
							BlockCompressionType[0] = IFXTextureObject::TextureType_Jpeg24;
							BlockCompressionType[1] = IFXTextureObject::TextureType_Png;
							ImageChannelFlagsList[0] = (IFXTextureObject::ChannelType) (IFXTextureObject::IFXIMAGECHANNEL_RED | 
														IFXTextureObject::IFXIMAGECHANNEL_BLUE |IFXTextureObject::IFXIMAGECHANNEL_GREEN );
							ImageChannelFlagsList[1] = IFXTextureObject::IFXIMAGECHANNEL_ALPHA;
							rc = pTextureObject[iTex]->SetImageCompressionProperties(2, BlockCompressionType, ImageChannelFlagsList,NULL,NULL);
							IFXASSERT(rc == IFX_OK );													
							m_compare = true;
						}
						break;

					case _RGBA_TO_2_PNGandPNG:
						if(iTex == 1){
							BlockCompressionType[0] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[1] = IFXTextureObject::TextureType_Png;
							ImageChannelFlagsList[1] = (IFXTextureObject::ChannelType) (IFXTextureObject::IFXIMAGECHANNEL_RED | 
														IFXTextureObject::IFXIMAGECHANNEL_BLUE |IFXTextureObject::IFXIMAGECHANNEL_GREEN );
							ImageChannelFlagsList[0] = IFXTextureObject::IFXIMAGECHANNEL_ALPHA;
							rc = pTextureObject[iTex]->SetImageCompressionProperties(2, BlockCompressionType, ImageChannelFlagsList,NULL,NULL);
							IFXASSERT(rc == IFX_OK );													
							m_compare = true;
						}
						break;

					case _RGBA_TO_4_PNG:
						if(iTex == 1){
							BlockCompressionType[0] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[1] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[2] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[3] = IFXTextureObject::TextureType_Png;
							ImageChannelFlagsList[0] = IFXTextureObject::IFXIMAGECHANNEL_RED;
							ImageChannelFlagsList[1] = IFXTextureObject::IFXIMAGECHANNEL_GREEN;
							ImageChannelFlagsList[2] = IFXTextureObject::IFXIMAGECHANNEL_BLUE;
							ImageChannelFlagsList[3] = IFXTextureObject::IFXIMAGECHANNEL_ALPHA;
							rc = pTextureObject[iTex]->SetImageCompressionProperties(4, BlockCompressionType, ImageChannelFlagsList,NULL,NULL);
							IFXASSERT(rc == IFX_OK );													
							m_compare = true;
						}
						break;

					case _RGBA_TO_4_JPEG8:
						if(iTex == 1){
							BlockCompressionType[0] = IFXTextureObject::TextureType_Jpeg8;
							BlockCompressionType[1] = IFXTextureObject::TextureType_Jpeg8;
							BlockCompressionType[2] = IFXTextureObject::TextureType_Jpeg8;
							BlockCompressionType[3] = IFXTextureObject::TextureType_Jpeg8;
							ImageChannelFlagsList[0] = IFXTextureObject::IFXIMAGECHANNEL_RED;
							ImageChannelFlagsList[1] = IFXTextureObject::IFXIMAGECHANNEL_GREEN;
							ImageChannelFlagsList[2] = IFXTextureObject::IFXIMAGECHANNEL_BLUE;
							ImageChannelFlagsList[3] = IFXTextureObject::IFXIMAGECHANNEL_ALPHA;
							rc = pTextureObject[iTex]->SetImageCompressionProperties(4, BlockCompressionType, ImageChannelFlagsList,NULL,NULL);
							IFXASSERT(rc == IFX_OK );													
							m_compare = true;
						}
						break;

					case _RGBA_TO_2_JPEG8_2_PNG: //12
						if(iTex == 1){
							BlockCompressionType[0] = IFXTextureObject::TextureType_Jpeg8;
							BlockCompressionType[1] = IFXTextureObject::TextureType_Jpeg8;
							BlockCompressionType[2] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[3] = IFXTextureObject::TextureType_Png;
							ImageChannelFlagsList[0] = IFXTextureObject::IFXIMAGECHANNEL_RED;
							ImageChannelFlagsList[1] = IFXTextureObject::IFXIMAGECHANNEL_GREEN;
							ImageChannelFlagsList[2] = IFXTextureObject::IFXIMAGECHANNEL_BLUE;
							ImageChannelFlagsList[3] = IFXTextureObject::IFXIMAGECHANNEL_ALPHA;
							rc = pTextureObject[iTex]->SetImageCompressionProperties(4, BlockCompressionType, ImageChannelFlagsList,NULL,NULL);
							IFXASSERT(rc == IFX_OK );													
							m_compare = true;
						}
						break;

					case _RGBA_TO_3_JPEG8_1_PNG: //13
						if(iTex == 1){
							BlockCompressionType[0] = IFXTextureObject::TextureType_Jpeg8;
							BlockCompressionType[1] = IFXTextureObject::TextureType_Jpeg8;
							BlockCompressionType[2] = IFXTextureObject::TextureType_Jpeg8;
							BlockCompressionType[3] = IFXTextureObject::TextureType_Png;
							ImageChannelFlagsList[0] = IFXTextureObject::IFXIMAGECHANNEL_RED;
							ImageChannelFlagsList[1] = IFXTextureObject::IFXIMAGECHANNEL_GREEN;
							ImageChannelFlagsList[2] = IFXTextureObject::IFXIMAGECHANNEL_BLUE;
							ImageChannelFlagsList[3] = IFXTextureObject::IFXIMAGECHANNEL_ALPHA;
							rc = pTextureObject[iTex]->SetImageCompressionProperties(4, BlockCompressionType, ImageChannelFlagsList,NULL,NULL);
							IFXASSERT(rc == IFX_OK );													
							m_compare = true;
						}
						break;

					case _RGBA_TO_1_JPEG8_3_PNG: //14
						if(iTex == 1){
							BlockCompressionType[0] = IFXTextureObject::TextureType_Jpeg8;
							BlockCompressionType[1] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[2] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[3] = IFXTextureObject::TextureType_Png;
							ImageChannelFlagsList[0] = IFXTextureObject::IFXIMAGECHANNEL_RED;
							ImageChannelFlagsList[1] = IFXTextureObject::IFXIMAGECHANNEL_GREEN;
							ImageChannelFlagsList[2] = IFXTextureObject::IFXIMAGECHANNEL_BLUE;
							ImageChannelFlagsList[3] = IFXTextureObject::IFXIMAGECHANNEL_ALPHA;
							rc = pTextureObject[iTex]->SetImageCompressionProperties(4, BlockCompressionType, ImageChannelFlagsList,NULL,NULL);
							IFXASSERT(rc == IFX_OK );							
							m_compare = true;
						}
						break;

					case _RGBA_TO_3_PNG_1_JPEG8: //15
						if(iTex == 1){
							BlockCompressionType[0] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[1] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[2] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[3] = IFXTextureObject::TextureType_Jpeg8;
							ImageChannelFlagsList[0] = IFXTextureObject::IFXIMAGECHANNEL_RED;
							ImageChannelFlagsList[1] = IFXTextureObject::IFXIMAGECHANNEL_GREEN;
							ImageChannelFlagsList[2] = IFXTextureObject::IFXIMAGECHANNEL_BLUE;
							ImageChannelFlagsList[3] = IFXTextureObject::IFXIMAGECHANNEL_ALPHA;
							rc = pTextureObject[iTex]->SetImageCompressionProperties(4, BlockCompressionType, ImageChannelFlagsList,NULL,NULL);
							IFXASSERT(rc == IFX_OK );													
							m_compare = true;
						}
						break;

					case _RGBA_TO_2_PNG_2_JPEG8: //16
						if(iTex == 1){
							BlockCompressionType[0] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[1] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[2] = IFXTextureObject::TextureType_Jpeg8;
							BlockCompressionType[3] = IFXTextureObject::TextureType_Jpeg8;
							ImageChannelFlagsList[0] = IFXTextureObject::IFXIMAGECHANNEL_RED;
							ImageChannelFlagsList[1] = IFXTextureObject::IFXIMAGECHANNEL_GREEN;
							ImageChannelFlagsList[2] = IFXTextureObject::IFXIMAGECHANNEL_BLUE;
							ImageChannelFlagsList[3] = IFXTextureObject::IFXIMAGECHANNEL_ALPHA;
							rc = pTextureObject[iTex]->SetImageCompressionProperties(4, BlockCompressionType, ImageChannelFlagsList,NULL,NULL);
							IFXASSERT(rc == IFX_OK );													
							m_compare = true;
						}
						break;
					
					case _RGBA_TO_1_PNG_3_JPEG8: //17
						if(iTex == 1){
							BlockCompressionType[0] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[1] = IFXTextureObject::TextureType_Jpeg8;
							BlockCompressionType[2] = IFXTextureObject::TextureType_Jpeg8;
							BlockCompressionType[3] = IFXTextureObject::TextureType_Jpeg8;
							ImageChannelFlagsList[0] = IFXTextureObject::IFXIMAGECHANNEL_RED;
							ImageChannelFlagsList[1] = IFXTextureObject::IFXIMAGECHANNEL_GREEN;
							ImageChannelFlagsList[2] = IFXTextureObject::IFXIMAGECHANNEL_BLUE;
							ImageChannelFlagsList[3] = IFXTextureObject::IFXIMAGECHANNEL_ALPHA;
							rc = pTextureObject[iTex]->SetImageCompressionProperties(4, BlockCompressionType, ImageChannelFlagsList,NULL,NULL);
							IFXASSERT(rc == IFX_OK );													
							m_compare = true;
						}
						break;					

					case _RGBA_TO_3_JPEG8_2_FILES:
						if(iTex == 1){
							
							BlockCompressionType[0] = IFXTextureObject::TextureType_Jpeg8;
							BlockCompressionType[1] = IFXTextureObject::TextureType_Jpeg8;
							BlockCompressionType[2] = IFXTextureObject::TextureType_Jpeg8;
							BlockCompressionType[3] = IFXTextureObject::TextureType_Jpeg8;
							ImageChannelFlagsList[0] = IFXTextureObject::IFXIMAGECHANNEL_RED;
							ImageChannelFlagsList[1] = IFXTextureObject::IFXIMAGECHANNEL_GREEN;
							ImageChannelFlagsList[2] = IFXTextureObject::IFXIMAGECHANNEL_BLUE;
							ImageChannelFlagsList[3] = IFXTextureObject::IFXIMAGECHANNEL_ALPHA;
							FileRef[0] = TRUE;
							FileRef[1] = TRUE;
							FileRef[2] = TRUE;
							FileRef[3] = TRUE;
							FileName[0]->ResizeToExactly(2);
							(*(FileName[0]))[0] = new IFXString ("file11");		
							(*(FileName[0]))[1] = new IFXString ("file12");		
							FileName[2]->ResizeToExactly(2);
							(*(FileName[2]))[0] = new IFXString ("file21");		
							(*(FileName[2]))[1] = new IFXString ("file22");		

							rc = pTextureObject[iTex]->SetImageCompressionProperties(4, BlockCompressionType, ImageChannelFlagsList,
																					FileRef, (IFXArray<IFXString*>**)FileName);
							IFXASSERT(rc == IFX_OK );
							pTextureObject[iTex]->GetContinuationImageCount(RetImageCount);
							rc = pTextureObject[iTex]->GetImageCompressionProperties((U8*)RetBlockCompressionType,
																					(IFXTextureObject::ChannelType*)(RetImageChannelFlagsList), 
																					(BOOL*)(RetFileRef), 
																					(IFXArray<IFXString*>**)RetFileName);

							IFXASSERT(rc == IFX_OK );
							if (RetImageCount == 4) {
								for (U32 iB=0; iB<RetImageCount; iB++) {
									if (BlockCompressionType[iB] != RetBlockCompressionType[iB]) {
										res = IFX_E_UNDEFINED;
										break; 
									}
									if (ImageChannelFlagsList[iB] != RetImageChannelFlagsList[iB] ) {
										res = IFX_E_UNDEFINED;
										break; 
									}
									if (FileRef[iB] != RetFileRef[iB] ) {
										res = IFX_E_UNDEFINED;
										break; 
									}
									U32 fileCount = FileName[iB]->GetNumberElements(); 
									if (fileCount != RetFileName[iB]->GetNumberElements()) {
										res = IFX_E_UNDEFINED;
										break; 
									}
									for (U32 iFName=0; iFName<fileCount; iFName++) {
										if (FileRef[iB]) {
											IFXString* fileName1 = RetFileName[iB]->GetElement(iFName);
											IFXString* fileName2 = FileName[iB]->GetElement(iFName);
											if ( (fileName1->Compare(fileName2)) ) {
												res = IFX_E_UNDEFINED;
												break; 
											}
										}
									}
								}
							} else {
								res = IFX_E_UNDEFINED;
							}
						}
						break; 

					case _RGB_TO_1_FILE:

						if(iTex == 1){

							IFXString src,src_base;
							src_base = getenv("U3D_SOURCE");
							src = src_base + "Tests/UnitTesting/Textures/SampleTexture/jpeg_24.JPG";
							
							BlockCompressionType[0] = IFXTextureObject::TextureType_Jpeg24;
							ImageChannelFlagsList[0]= (IFXTextureObject::ChannelType)(IFXTextureObject::IFXIMAGECHANNEL_RED | IFXTextureObject::IFXIMAGECHANNEL_GREEN | 
													    IFXTextureObject::IFXIMAGECHANNEL_BLUE );
							FileRef[0] = TRUE; 
							FileRef[1] = FALSE; 
							FileRef[2] = FALSE; 
							FileRef[3] = FALSE;
							FileName[0]->ResizeToExactly(1);
							(*(FileName[0]))[0] = new IFXString (src);		
	
							rc = pTextureObject[iTex]->SetImageCompressionProperties(1, BlockCompressionType, ImageChannelFlagsList,
																					FileRef, (IFXArray<IFXString*>**)FileName);
							IFXASSERT(rc == IFX_OK );

						
						}
						break;

					case _PNG24_TO_1_FILE:

						if(iTex == 1){
							IFXString src,src_base;
							src_base = getenv("U3D_SOURCE");
							src = src_base + "Tests/UnitTesting/Textures/SampleTexture/png24.PNG";
							
							BlockCompressionType[0] = IFXTextureObject::TextureType_Png;
							ImageChannelFlagsList[0]= (IFXTextureObject::ChannelType)(IFXTextureObject::IFXIMAGECHANNEL_RED | IFXTextureObject::IFXIMAGECHANNEL_GREEN | 
													    IFXTextureObject::IFXIMAGECHANNEL_BLUE );
							FileRef[0] = TRUE; 
							FileRef[1] = FALSE; 
							FileRef[2] = FALSE; 
							FileRef[3] = FALSE;
							FileName[0]->ResizeToExactly(1);
							(*(FileName[0]))[0] = new IFXString (src);		
	
							rc = pTextureObject[iTex]->SetImageCompressionProperties(1, BlockCompressionType, ImageChannelFlagsList,
																					FileRef, (IFXArray<IFXString*>**)FileName);
							IFXASSERT(rc == IFX_OK );
						}
						break;			

					case _RGBA_TO_4_PNG_mixed1:
						if(iTex == 1){ // b+g+r
							BlockCompressionType[0] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[1] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[2] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[3] = IFXTextureObject::TextureType_Png;
							ImageChannelFlagsList[0] = IFXTextureObject::IFXIMAGECHANNEL_BLUE;
							ImageChannelFlagsList[1] = IFXTextureObject::IFXIMAGECHANNEL_GREEN;							
							ImageChannelFlagsList[2] = IFXTextureObject::IFXIMAGECHANNEL_RED;
							ImageChannelFlagsList[3] = IFXTextureObject::IFXIMAGECHANNEL_ALPHA;
							rc = pTextureObject[iTex]->SetImageCompressionProperties(4, BlockCompressionType, ImageChannelFlagsList,NULL,NULL);
							IFXASSERT(rc == IFX_OK );													
							m_compare = true;
						}
						break;

					case _RGBA_TO_4_PNG_mixed2:

						if(iTex == 1){ // g+a+b+r
							BlockCompressionType[0] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[1] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[2] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[3] = IFXTextureObject::TextureType_Png;
							ImageChannelFlagsList[0] = IFXTextureObject::IFXIMAGECHANNEL_GREEN;
							ImageChannelFlagsList[1] = IFXTextureObject::IFXIMAGECHANNEL_ALPHA;
							ImageChannelFlagsList[2] = IFXTextureObject::IFXIMAGECHANNEL_BLUE;
							ImageChannelFlagsList[3] = IFXTextureObject::IFXIMAGECHANNEL_RED;
							rc = pTextureObject[iTex]->SetImageCompressionProperties(4, BlockCompressionType, ImageChannelFlagsList,NULL,NULL);
							IFXASSERT(rc == IFX_OK );													
							m_compare = true;
						}
						break;
						

					case _RGBA_TO_4_PNG_mixed3:
						
						if(iTex == 1){ // r+a+b+g
							BlockCompressionType[0] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[1] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[2] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[3] = IFXTextureObject::TextureType_Png;
							ImageChannelFlagsList[0] = IFXTextureObject::IFXIMAGECHANNEL_RED;
							ImageChannelFlagsList[1] = IFXTextureObject::IFXIMAGECHANNEL_ALPHA;
							ImageChannelFlagsList[2] = IFXTextureObject::IFXIMAGECHANNEL_BLUE;
							ImageChannelFlagsList[3] = IFXTextureObject::IFXIMAGECHANNEL_GREEN;
							rc = pTextureObject[iTex]->SetImageCompressionProperties(4, BlockCompressionType, ImageChannelFlagsList,NULL,NULL);
							IFXASSERT(rc == IFX_OK );													
							m_compare = true;
						}
						break;


					case _RGBA_TO_4_PNG_mixed4:
						
						if(iTex == 1){ // g+r+a+b
							BlockCompressionType[0] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[1] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[2] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[3] = IFXTextureObject::TextureType_Png;
							ImageChannelFlagsList[0] = IFXTextureObject::IFXIMAGECHANNEL_GREEN;
							ImageChannelFlagsList[1] = IFXTextureObject::IFXIMAGECHANNEL_RED;
							ImageChannelFlagsList[2] = IFXTextureObject::IFXIMAGECHANNEL_ALPHA;
							ImageChannelFlagsList[3] = IFXTextureObject::IFXIMAGECHANNEL_BLUE;

							rc = pTextureObject[iTex]->SetImageCompressionProperties(4, BlockCompressionType, ImageChannelFlagsList,NULL,NULL);
							IFXASSERT(rc == IFX_OK );													
							m_compare = true;
						}
						break;

					case _PNG32_TO_1_FILE:
						if(iTex == 1){
							IFXString src,src_base;
							src_base = getenv("U3D_SOURCE");
							src = src_base + "Tests/UnitTesting/Textures/SampleTexture/png32.PNG";
							
							BlockCompressionType[0] = IFXTextureObject::TextureType_Png;
							ImageChannelFlagsList[0]= (IFXTextureObject::ChannelType)(IFXTextureObject::IFXIMAGECHANNEL_RED | IFXTextureObject::IFXIMAGECHANNEL_GREEN | 
													    IFXTextureObject::IFXIMAGECHANNEL_BLUE | IFXTextureObject::IFXIMAGECHANNEL_ALPHA );
							FileRef[0] = TRUE; 
							FileRef[1] = FALSE; 
							FileRef[2] = FALSE; 
							FileRef[3] = FALSE;
							FileName[0]->ResizeToExactly(1);
							(*(FileName[0]))[0] = new IFXString (src);		

							rc = pTextureObject[iTex]->SetImageCompressionProperties(1, BlockCompressionType, ImageChannelFlagsList,
																					FileRef, (IFXArray<IFXString*>**)FileName);
							IFXASSERT(rc == IFX_OK );
							m_compare = true;
						}
						break;

					default:
						break;
				}
			
				if(IFXSUCCESS(rc))  {
					rc = pTextureObject[iTex]->ForceCompressionState(IFXTEXTURECOMPRESSIONSTATE_FORCECOMPRESSED);
				}

			}
			else
			{ // something wrong w/ texture
				if(pImageCopySrc[iTex].m_pData)
				{
					free(pImageCopySrc[iTex].m_pData);
					pImageCopySrc[iTex].m_pData = NULL;
				}
				dirty = 0;
			}
		}
	}

	switch( type ) {
		case _RGBA_TO_3_JPEG8_2_FILES:				
			delete (*(FileName[0]))[0];
			delete (*(FileName[0]))[1];
			delete (*(FileName[2]))[0];
			delete (*(FileName[2]))[1];
			break;
		case _RGB_TO_1_FILE:
			delete (*(FileName[0]))[0];
			break;

		case _PNG24_TO_1_FILE:
			delete (*(FileName[0]))[0];
			break;

		case _PNG32_TO_1_FILE:
			delete (*(FileName[0]))[0];
				break;
		default:
			break;
	}

	for(i=0; i<IFX_MAX_CONTINUATIONIMAGE_COUNT; i++)
	{
		delete FileName[i];
		delete RetFileName[i];
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
	}

	if(pImageCopySrc)
		free(pImageCopySrc);

	// will be compare only for texture part with m_compare == true
	if(m_compare)
	{
		//rc = BinaryComparison( dst );
		IFXASSERT( rc == IFX_OK );
	}

	if(m_pImageCopySrc)
	{
		free(m_pImageCopySrc);
		m_pImageCopySrc = NULL;
	}

	return rc;  
}//ConvertTextureObjectCompressionProperties()



/*
	binary comparison between m_pImageCopySrc and texture uncompressed from "src" U3D file
*/
IFXRESULT ImageWriteRead::BinaryComparison( const IFXString& src )
{
	IFXRESULT rc = IFX_FALSE;

	IFXDECLARELOCAL(IFXCoreServices, pCoreServices);
	IFXDECLARELOCAL(IFXCoreServices, pCoreServicesMain);
	IFXDECLARELOCAL(IFXSceneGraph, pSceneGraph);
	TEST(IFXSUCCESS(InitializeSceneGraph_CoreService( &pCoreServices, &pCoreServicesMain, &pSceneGraph )), IFXException);

	IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
	IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);
	IFXDECLARELOCAL(IFXWriteManager, pIFXWriteManager);
	IFXDECLARELOCAL(IFXWriteBuffer, pIFXWriteBuffer);
	IFXDECLARELOCAL(IFXStdio, pIFXStdio1);
	IFXDECLARELOCAL(IFXStdio, pIFXStdio2);

	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager)), IFXException);
	TEST(IFXSUCCESS(pIFXLoadManager->Initialize(pCoreServices)), IFXException);
	TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer)), IFXException);
	TEST(IFXSUCCESS(pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio1)), IFXException);

	TEST(IFXSUCCESS(pIFXStdio1->Open( (IFXCHAR*) src.Raw() )), IFXException);
	TEST(IFXSUCCESS(pIFXLoadManager->Load(pIFXReadBuffer, TRUE)), IFXException);
	TEST(IFXSUCCESS(pIFXStdio1->Close()), IFXException);

	//2. reset new parameters to U3D file	
	U32 TexureObjectCount, imageBufferSize;
	IFXDECLARELOCAL(IFXPalette, pTexturePalette);
	TEST(IFXSUCCESS(pSceneGraph->GetPalette(IFXSceneGraph::TEXTURE, &pTexturePalette)), IFXException);
	pTexturePalette->GetPaletteSize(&TexureObjectCount);

	IFXUnknown* pObject[IFX_MAX_CONTINUATIONIMAGE_COUNT];
	IFXTextureObject* pTextureObject[IFX_MAX_CONTINUATIONIMAGE_COUNT];

	STextureOutputInfo* pImageCopySrc  = (STextureOutputInfo*)malloc(sizeof(STextureOutputInfo)*TexureObjectCount);

	for(U32 iTex=0; iTex<TexureObjectCount; iTex++) {

		pTexturePalette->GetResourcePtr(iTex,&(pObject[iTex]) );
		pObject[iTex]->QueryInterface(IID_IFXTextureObject, (void**)(&(pTextureObject[iTex])) );
		pObject[iTex]->Release();

		if ( pTextureObject[iTex] ) {
	
			rc = pTextureObject[iTex]->MakeDirty(NULL);
			pTextureObject[iTex]->ForceCompressionState(IFXTEXTURECOMPRESSIONSTATE_FORCEDECOMPRESS);

			pTextureObject[iTex]->GetWidth(&(pImageCopySrc[iTex].m_width));
			pTextureObject[iTex]->GetHeight(&(pImageCopySrc[iTex].m_height));
			pTextureObject[iTex]->GetRawImageBufferSize( &imageBufferSize);
			pTextureObject[iTex]->GetRenderFormat(pImageCopySrc[iTex].eRenderFormat);
		
			pImageCopySrc[iTex].m_pData = (U8*)malloc( imageBufferSize );

		}
	}

	// the direct comparison  with pTextureObject[m_texNumber]
	U8 s = 0;
	U8 d = 0;
	for ( U32 i=0; i < m_imageBufferSize; i++ )
	{
		s = m_pImageCopySrc[i];
		d = pImageCopySrc[m_texNumber].m_pData[i];
		IFXASSERT( s == d );
	}

	//release
	for(U32 iTex=0; iTex<TexureObjectCount; iTex++)
	{
		free(pImageCopySrc[iTex].m_pData);
	}

	if(m_pImageCopySrc)
		free(m_pImageCopySrc);

	if(pImageCopySrc)
		free(pImageCopySrc);

	// the end 
	return rc;
}


/*
input  - inappropriate parameters
output - IFX_BAD_PARAM if all is OK
*/

IFXRESULT ImageWriteRead::VerifyingInputParameters()
{
	IFXRESULT rc = IFX_OK;

	U32 countContImages = 0;
	U8 BlockCompressionType[IFX_MAX_CONTINUATIONIMAGE_COUNT];
	IFXTextureObject::ChannelType ImageChannelFlagsList[IFX_MAX_CONTINUATIONIMAGE_COUNT];
	BOOL	FileRef[IFX_MAX_CONTINUATIONIMAGE_COUNT];
    IFXArray<IFXString*> *FileName[IFX_MAX_CONTINUATIONIMAGE_COUNT];
	
	IFXDECLARELOCAL(IFXCoreServices, pCoreServices);
	IFXDECLARELOCAL(IFXCoreServices, pCoreServicesMain);
	IFXDECLARELOCAL(IFXSceneGraph, pSceneGraph);
	TEST(IFXSUCCESS(InitializeSceneGraph_CoreService( &pCoreServices, &pCoreServicesMain, &pSceneGraph )), IFXException);

	IFXUnknown* pObject[IFX_MAX_CONTINUATIONIMAGE_COUNT];
	IFXTextureObject* pTextureObject[IFX_MAX_CONTINUATIONIMAGE_COUNT];

	IFXDECLARELOCAL(IFXPalette, pTexturePalette);
	TEST(IFXSUCCESS(pSceneGraph->GetPalette(IFXSceneGraph::TEXTURE, &pTexturePalette)), IFXException);

	pTexturePalette->GetResourcePtr(0,&(pObject[0]) );
	pObject[0]->QueryInterface(IID_IFXTextureObject, (void**)(&(pTextureObject[0])) );
	pObject[0]->Release();

	/***********************************************************************************/

	for(U32 i=0; i<IFX_MAX_CONTINUATIONIMAGE_COUNT; i++){
		FileName[i] = new IFXArray<IFXString*>;
		FileName[i]->ResizeToExactly(1);
		(*(FileName[i]))[0] = NULL;	
	}

	IFXString src,src_base;
	src_base = getenv("U3D_SOURCE");
	src = src_base + "Tests/UnitTesting/Textures/SampleTexture/png32.PNG";
	
	BlockCompressionType[0] = IFXTextureObject::TextureType_Png;
	BlockCompressionType[1] = IFXTextureObject::TextureType_Png;
	BlockCompressionType[2] = IFXTextureObject::TextureType_Png;
	BlockCompressionType[3] = IFXTextureObject::TextureType_Png;
	ImageChannelFlagsList[0] = IFXTextureObject::IFXIMAGECHANNEL_RED;
	ImageChannelFlagsList[1] = IFXTextureObject::IFXIMAGECHANNEL_GREEN;
	ImageChannelFlagsList[2] = IFXTextureObject::IFXIMAGECHANNEL_BLUE;
	ImageChannelFlagsList[3] = IFXTextureObject::IFXIMAGECHANNEL_ALPHA;

	FileRef[0] = true; 
	FileRef[1] = false; 
	FileRef[2] = false; 
	FileRef[3] = false;
	FileName[0]->ResizeToExactly(1);
	(*(FileName[0]))[0] = new IFXString (src);		

	countContImages = 4;
	rc = pTextureObject[0]->SetImageCompressionProperties(countContImages, BlockCompressionType, ImageChannelFlagsList,
															FileRef, (IFXArray<IFXString*>**)FileName);
	IFXASSERT(rc == IFX_E_BAD_PARAM);

	rc = IFX_OK;
	FileRef[0] = true; 
	FileRef[1] = true; 
	FileRef[2] = false; 
	FileRef[3] = false;
	FileName[0]->ResizeToExactly(2);
	(*(FileName[0]))[0] = new IFXString (src);		

	rc = pTextureObject[0]->SetImageCompressionProperties(countContImages, BlockCompressionType, ImageChannelFlagsList,
																FileRef, (IFXArray<IFXString*>**)FileName);
	IFXASSERT(rc == IFX_E_BAD_PARAM);

	rc = IFX_OK;
	FileRef[0] = true; 
	FileRef[1] = true; 
	FileRef[2] = true; 
	FileRef[3] = false;
	FileName[0]->ResizeToExactly(3);
	(*(FileName[0]))[0] = new IFXString (src);		
	rc = pTextureObject[0]->SetImageCompressionProperties(countContImages, BlockCompressionType, ImageChannelFlagsList,
																FileRef, (IFXArray<IFXString*>**)FileName);
	IFXASSERT(rc == IFX_E_BAD_PARAM);

	return rc;
} //VerifyingInputParameters
