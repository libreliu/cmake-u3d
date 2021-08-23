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
#include "SampleTextureExt.h"
#include "UTHeaders.h"
#include "IFXLoadManager.h"
#include "IFXWriteManager.h"
#include "IFXStdio.h"
#include "IFXReadBuffer.h"
#include "IFXWriteBuffer.h"

IFXRESULT ExtImageWriteRead::Run()
{
	IFXRESULT rc = IFX_FALSE;
	
	rc = IFXCOMInitialize();

	if(IFXSUCCESS(rc))
	{
		IFXString src_base( getenv("U3D_SOURCE") );
		IFXString dst_base( src_base );
		IFXString src,dst;
		TransfomType type = _NONE;
	
		//0
		// _JPG8_3_IMAGES_TO_1_FILE ( 24 bytes per pixel ) convert to 3 continuation images by jpeg's comression types and 
		//							  makes refs for 1 external file 
		src = src_base + "Tests/UnitTesting/Textures/SampleTextureExt/InputImages/shading.u3d"; 
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTextureExt/_JPG8_3_IMAGES_TO_1_FILE.u3d";
		type = _JPG8_3_IMAGES_TO_1_FILE;

		if(1) { 	    
			if(IFXSUCCESS(rc))
				rc = ConvertTextureObjectCompressionProperties(src, dst, type );
			IFXASSERT(rc == IFX_OK);
		}
	
		//1
		// _PNG32_4IMAGES_TO_1_FILE ( 32 bytes per pixel ) convert to 4 continuation images by png's comression types and 
		//							makes refs for 1 external file for 4 blocks 
		src = src_base + "Tests/UnitTesting/Textures/SampleTextureExt/InputImages/shading_lines_alpha_tex1.u3d"; 
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTextureExt/_PNG32_4_IMAGES_TO_1_FILE.u3d";
		type = _PNG32_4_IMAGES_TO_1_FILE;

		if(1) { 	    
			if(IFXSUCCESS(rc))
				rc = ConvertTextureObjectCompressionProperties(src, dst, type );
			IFXASSERT(rc == IFX_OK);
		}

		//2 _PNG_3_IMAGES_TO_1_FILE,
		src = src_base + "Tests/UnitTesting/Textures/SampleTextureExt/InputImages/shading.u3d"; 
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTextureExt/_PNG_3_IMAGES_TO_1_FILE.u3d";
		type = _PNG_3_IMAGES_TO_1_FILE;

		if(1) { 	    
			if(IFXSUCCESS(rc))
				rc = ConvertTextureObjectCompressionProperties(src, dst, type );
			IFXASSERT(rc == IFX_OK);
		}

		//3 _JPG24_1_IMAGES_TO_1_FILE
		src = src_base + "Tests/UnitTesting/Textures/SampleTextureExt/InputImages/shading.u3d"; 
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTextureExt/_JPG24_1_IMAGES_TO_1_FILE.u3d";
		type = _JPG24_1_IMAGES_TO_1_FILE;

		if(1) { 	    
			if(IFXSUCCESS(rc))
				rc = ConvertTextureObjectCompressionProperties(src, dst, type );
			IFXASSERT(rc == IFX_OK);
		}
		
		//4 _PNG_1_IMAGES_TO_1_FILE,
		src = src_base + "Tests/UnitTesting/Textures/SampleTextureExt/InputImages/shading_lines_alpha_tex1.u3d"; 
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTextureExt/_PNG_1_IMAGES_TO_1_FILE.u3d";
		type = _PNG_1_IMAGES_TO_1_FILE;

		if(1) { 	    
			if(IFXSUCCESS(rc))
				rc = ConvertTextureObjectCompressionProperties(src, dst, type );
			IFXASSERT(rc == IFX_OK);
		}

		//5
		// _PNG32_4_IMAGES_TO_1_FILE_INTERNET ( 32 bytes per pixel ) convert to 4 continuation images by png's comression types and 
		//							makes refs for 1 external file for 4 blocks 
		src = src_base + "Tests/UnitTesting/Textures/SampleTextureExt/InputImages/shading_lines_alpha_tex1.u3d"; 
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTextureExt/_PNG32_4_IMAGES_TO_1_FILE_INTERNET.u3d";
		type = _PNG32_4_IMAGES_TO_1_FILE_INTERNET;

		if(1) { 	    
			if(IFXSUCCESS(rc))
				rc = ConvertTextureObjectCompressionProperties(src, dst, type );
			IFXASSERT(rc == IFX_OK);
		}

		//7 _JPG24_1_IMAGES_TO_1_FILE_INTERNET
		src = src_base + "Tests/UnitTesting/Textures/SampleTextureExt/InputImages/shading.u3d"; 
		dst = dst_base + "Tests/UnitTesting/Textures/SampleTextureExt/_JPG24_1_IMAGES_TO_1_FILE_INTERNET.u3d";
		type = _JPG24_1_IMAGES_TO_1_FILE_INTERNET;

		if(1) { 	    
			if(IFXSUCCESS(rc))
				rc = ConvertTextureObjectCompressionProperties(src, dst, type );
			IFXASSERT(rc == IFX_OK);
		}
	}

	if(IFXSUCCESS(rc))
		rc = IFXCOMUninitialize();

	return rc;
}
	
IFXRESULT ExtImageWriteRead::ConvertTextureObjectCompressionProperties( const IFXString& src, const IFXString& dst, const TransfomType type )
{
	IFXRESULT rc = IFX_OK;

	// the main initialization part

	IFXDECLARELOCAL(IFXCoreServices, pCoreServices);
	IFXDECLARELOCAL(IFXCoreServices, pCoreServicesMain);
	IFXDECLARELOCAL(IFXSceneGraph, pSceneGraph);
	TEST(IFXSUCCESS(InitializeSceneGraph_CoreService( &pCoreServices, &pCoreServicesMain, &pSceneGraph )), IFXException);

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

	U32 TexureObjectCount;
	IFXDECLARELOCAL(IFXPalette, pTexturePalette);
	TEST(IFXSUCCESS(pSceneGraph->GetPalette(IFXSceneGraph::TEXTURE, &pTexturePalette)), IFXException);
	pTexturePalette->GetPaletteSize(&TexureObjectCount);

	IFXTextureObject* pTextureObject[IFX_MAX_CONTINUATIONIMAGE_COUNT];

	U32 i, imageBufferSize;		
	U8 ImageType = -1;
	STextureOutputInfo* pImageCopySrc;
	IFXRESULT res = IFX_OK;

	U32 ContImageCount = 0;
	U8 BlockCompressionType[IFX_MAX_CONTINUATIONIMAGE_COUNT];

	pImageCopySrc = (STextureOutputInfo*)malloc(sizeof(STextureOutputInfo)*TexureObjectCount);

	IFXTextureObject::ChannelType ImageChannelFlagsList[IFX_MAX_CONTINUATIONIMAGE_COUNT] = {IFXTextureObject::IFXIMAGECHANNEL_RED};

	BOOL FileRef[IFX_MAX_CONTINUATIONIMAGE_COUNT] = {FALSE};

    IFXArray<IFXString*>** ppFileName = new IFXArray<IFXString*>*[IFX_MAX_CONTINUATIONIMAGE_COUNT];

	for( i = 0; i < IFX_MAX_CONTINUATIONIMAGE_COUNT; ++i )
	{
		ppFileName[i] = new IFXArray<IFXString*>;
		ppFileName[i]->ResizeToExactly(1);
		(*(ppFileName[i]))[0] = NULL;		
	}

	IFXString src_base( getenv("U3D_SOURCE") );
	IFXString src0,src1,src2,src3;

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
		
			pImageCopySrc[iTex].m_pData = new U8[imageBufferSize];

			if (IFX_OK == res)
			{
				switch( type )
				{
					case _PNG32_4_IMAGES_TO_1_FILE:
						if(iTex == 1)
						{
							src0 = src_base + "Tests/UnitTesting/Textures/SampleTextureExt/InputImages/__test0.png";
							src1 = src_base + "Tests/UnitTesting/Textures/SampleTextureExt/InputImages/__test1.png";
							src2 = src_base + "Tests/UnitTesting/Textures/SampleTextureExt/InputImages/__test2.png";
							src3 = src_base + "Tests/UnitTesting/Textures/SampleTextureExt/InputImages/__test3.png";
							
							BlockCompressionType[0] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[1] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[2] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[3] = IFXTextureObject::TextureType_Png;
							
							ImageChannelFlagsList[0] = IFXTextureObject::IFXIMAGECHANNEL_RED;
							ImageChannelFlagsList[1] = IFXTextureObject::IFXIMAGECHANNEL_GREEN;
							ImageChannelFlagsList[2] = IFXTextureObject::IFXIMAGECHANNEL_BLUE;
							ImageChannelFlagsList[3] = IFXTextureObject::IFXIMAGECHANNEL_ALPHA;

							FileRef[0] = TRUE; 
							FileRef[1] = TRUE; 
							FileRef[2] = TRUE; 
							FileRef[3] = TRUE;

							(*ppFileName[0])[0] = new IFXString (src0);		
							(*ppFileName[1])[0] = new IFXString (src1);		
							(*ppFileName[2])[0] = new IFXString (src2);		
							(*ppFileName[3])[0] = new IFXString (src3);

							rc = pTextureObject[iTex]->SetImageCompressionProperties(4, BlockCompressionType, ImageChannelFlagsList,
																						FileRef, ppFileName);
							IFXASSERT( rc == IFX_OK );	
						}
						break;

					case _PNG32_4_IMAGES_TO_1_FILE_INTERNET:
						if(iTex == 1) 
						{
							src0 = "http://nnsu3d1/u3d/Images/__test0.png";
							src1 = "http://nnsu3d1/u3d/Images/__test1.png";
							src2 = "http://nnsu3d1/u3d/Images/__test2.png";
							src3 = "http://nnsu3d1/u3d/Images/__test3.png";

							BlockCompressionType[0] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[1] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[2] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[3] = IFXTextureObject::TextureType_Png;
							
							ImageChannelFlagsList[0] = IFXTextureObject::IFXIMAGECHANNEL_RED;
							ImageChannelFlagsList[1] = IFXTextureObject::IFXIMAGECHANNEL_GREEN;
							ImageChannelFlagsList[2] = IFXTextureObject::IFXIMAGECHANNEL_BLUE;
							ImageChannelFlagsList[3] = IFXTextureObject::IFXIMAGECHANNEL_ALPHA;

							FileRef[0] = TRUE; 
							FileRef[1] = TRUE; 
							FileRef[2] = TRUE; 
							FileRef[3] = TRUE;

							(*ppFileName[0])[0] = new IFXString (src0);		
							(*ppFileName[1])[0] = new IFXString (src1);		
							(*ppFileName[2])[0] = new IFXString (src2);		
							(*ppFileName[3])[0] = new IFXString (src3);		

							rc = pTextureObject[iTex]->SetImageCompressionProperties(4, BlockCompressionType, ImageChannelFlagsList,
																						FileRef, ppFileName);
							IFXASSERT( rc == IFX_OK );	
						}
						break;

					case _JPG8_3_IMAGES_TO_1_FILE:

						if(iTex == 1)
						{
							src0 = src_base + "Tests/UnitTesting/Textures/SampleTextureExt/InputImages/__test1.jpg";
							src1 = src_base + "Tests/UnitTesting/Textures/SampleTextureExt/InputImages/__test2.jpg";
							src2 = src_base + "Tests/UnitTesting/Textures/SampleTextureExt/InputImages/__test3.jpg";
											   
							BlockCompressionType[0] = IFXTextureObject::TextureType_Jpeg8;
							BlockCompressionType[1] = IFXTextureObject::TextureType_Jpeg8;
							BlockCompressionType[2] = IFXTextureObject::TextureType_Jpeg8;
							
							ImageChannelFlagsList[0] = IFXTextureObject::IFXIMAGECHANNEL_RED;
							ImageChannelFlagsList[1] = IFXTextureObject::IFXIMAGECHANNEL_GREEN;
							ImageChannelFlagsList[2] = IFXTextureObject::IFXIMAGECHANNEL_BLUE;

							FileRef[0] = TRUE; 
							FileRef[1] = TRUE; 
							FileRef[2] = TRUE; 
									
							(*ppFileName[0])[0] = new IFXString (src0);		
							(*ppFileName[1])[0] = new IFXString (src1);		
							(*ppFileName[2])[0] = new IFXString (src2);		

							rc = pTextureObject[iTex]->SetImageCompressionProperties(3, BlockCompressionType, ImageChannelFlagsList,
																						FileRef, ppFileName);
							IFXASSERT( rc == IFX_OK );
						}
						break;

					case _PNG_3_IMAGES_TO_1_FILE:

						if(iTex == 1)
						{
							src0 = src_base + "Tests/UnitTesting/Textures/SampleTextureExt/InputImages/__test0.png";
							src1 = src_base + "Tests/UnitTesting/Textures/SampleTextureExt/InputImages/__test1.png";
							src2 = src_base + "Tests/UnitTesting/Textures/SampleTextureExt/InputImages/__test2.png";
							
							BlockCompressionType[0] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[1] = IFXTextureObject::TextureType_Png;
							BlockCompressionType[2] = IFXTextureObject::TextureType_Png;
							
							ImageChannelFlagsList[0] = IFXTextureObject::IFXIMAGECHANNEL_RED;
							ImageChannelFlagsList[1] = IFXTextureObject::IFXIMAGECHANNEL_GREEN;
							ImageChannelFlagsList[2] = IFXTextureObject::IFXIMAGECHANNEL_BLUE;

							FileRef[0] = TRUE; 
							FileRef[1] = TRUE; 
							FileRef[2] = TRUE; 
														
							(*ppFileName[0])[0] = new IFXString (src0);		
							(*ppFileName[1])[0] = new IFXString (src1);		
							(*ppFileName[2])[0] = new IFXString (src2);		

							rc = pTextureObject[iTex]->SetImageCompressionProperties(3, BlockCompressionType, ImageChannelFlagsList,
																						FileRef, ppFileName);
							IFXASSERT( rc == IFX_OK );
						}
						break;

					case _JPG24_1_IMAGES_TO_1_FILE:
						if(iTex == 1)
						{
							src0 = src_base + "Tests/UnitTesting/Textures/SampleTextureExt/InputImages/RussiaFlag.jpg";
							BlockCompressionType[0] = IFXTextureObject::TextureType_Jpeg24;
							ImageChannelFlagsList[0] = (IFXTextureObject::ChannelType)(IFXTextureObject::IFXIMAGECHANNEL_RED | IFXTextureObject::IFXIMAGECHANNEL_GREEN | 
													IFXTextureObject::IFXIMAGECHANNEL_BLUE);

							FileRef[0] = TRUE;

							(*ppFileName[0])[0] = new IFXString (src0);		

							rc = pTextureObject[iTex]->SetImageCompressionProperties(1, BlockCompressionType, ImageChannelFlagsList,
																					 FileRef, ppFileName );
							IFXASSERT( rc == IFX_OK );
						}
						break;

					case _JPG24_1_IMAGES_TO_1_FILE_INTERNET:
						if(iTex == 1)
						{
							src0 = "http://nnsu3d1/u3d/Images/RussiaFlag.jpg";				   
							BlockCompressionType[0] = IFXTextureObject::TextureType_Jpeg24;

							ImageChannelFlagsList[0] = (IFXTextureObject::ChannelType)(IFXTextureObject::IFXIMAGECHANNEL_RED | IFXTextureObject::IFXIMAGECHANNEL_GREEN | 
													IFXTextureObject::IFXIMAGECHANNEL_BLUE);

							FileRef[0] = TRUE;

							(*ppFileName[0])[0] = new IFXString (src0);		

							rc = pTextureObject[iTex]->SetImageCompressionProperties(1, BlockCompressionType, ImageChannelFlagsList,
																					 FileRef, ppFileName );
							IFXASSERT( rc == IFX_OK );
						}
						break;

					case _PNG_1_IMAGES_TO_1_FILE:
						if(iTex == 1) 
						{
							src0 = src_base + "Tests/UnitTesting/Textures/SampleTextureExt/InputImages/Flag.png";
							BlockCompressionType[0] = IFXTextureObject::TextureType_Png;

							ImageChannelFlagsList[0] = (IFXTextureObject::ChannelType)(IFXTextureObject::IFXIMAGECHANNEL_RED | IFXTextureObject::IFXIMAGECHANNEL_GREEN | 
														IFXTextureObject::IFXIMAGECHANNEL_BLUE | IFXTextureObject::IFXIMAGECHANNEL_ALPHA);

							FileRef[0] = TRUE;

							(*ppFileName[0])[0] = new IFXString (src0);		

							rc = pTextureObject[iTex]->SetImageCompressionProperties(1, BlockCompressionType, ImageChannelFlagsList,
																					 FileRef, ppFileName );
							IFXASSERT( rc == IFX_OK );
						}
						break;

					default:
						break;
				}
			
				if(IFXSUCCESS(rc))
					rc = pTextureObject[iTex]->ForceCompressionState(IFXTEXTURECOMPRESSIONSTATE_FORCECOMPRESSED);

			}
			else
			{
				if(pImageCopySrc[iTex].m_pData)
				{
					free(pImageCopySrc[iTex].m_pData);
					pImageCopySrc[iTex].m_pData = NULL;
				}
				dirty = 0;
			}
		}
	}

	switch(type)
	{
		case _PNG32_4_IMAGES_TO_1_FILE:
		case _PNG32_4_IMAGES_TO_1_FILE_INTERNET:
			delete (*ppFileName[0])[0];
			delete (*ppFileName[1])[0];
			delete (*ppFileName[2])[0];
			delete (*ppFileName[3])[0];
			break;
		case _JPG8_3_IMAGES_TO_1_FILE:
		case _PNG_3_IMAGES_TO_1_FILE:
			delete (*ppFileName[0])[0];
			delete (*ppFileName[1])[0];
			delete (*ppFileName[2])[0];
			break;
		case _JPG24_1_IMAGES_TO_1_FILE_INTERNET:
		case _JPG24_1_IMAGES_TO_1_FILE:
		case _PNG_1_IMAGES_TO_1_FILE:
			delete (*ppFileName[0])[0];
			break;
		default:
			break;
	}

	for(i = 0; i < IFX_MAX_CONTINUATIONIMAGE_COUNT; ++i )
	{
		IFXDELETE( ppFileName[i] );
	}
	IFXDELETE_ARRAY( ppFileName );

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

	return rc;  
}
