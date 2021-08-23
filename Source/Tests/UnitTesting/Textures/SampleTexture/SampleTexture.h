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

#ifndef SAMPLETEXTURE_H
#define SAMPLETEXTURE_H
#include "IFXTest.h"
#include "IFXException.h"

class ImageWriteRead : public U3DTestSuite::IFXTest
{
public:
	enum TransfomType
	{
		_NONE,
		_JPEG24_TO_1_PNG,
		_JPEG24_TO_1_JPEG24,
		_JPEG24_TO_3_JPEG8,
		_JPEG24_TO_3_PNG,
		_RGBA_TO_1_PNG,
		_RGBA_TO_2_PNGandJPEG8,
		_RGBA_TO_2_JPEG24andJPEG8,
		_RGBA_TO_2_JPEG24andPNG,
		_RGBA_TO_2_PNGandPNG,
		_RGBA_TO_4_PNG,
		_RGBA_TO_4_JPEG8,
		_RGBA_TO_2_JPEG8_2_PNG,
		_RGBA_TO_3_JPEG8_1_PNG,
		_RGBA_TO_1_JPEG8_3_PNG,
		_RGBA_TO_3_PNG_1_JPEG8,
		_RGBA_TO_2_PNG_2_JPEG8,
		_RGBA_TO_1_PNG_3_JPEG8,
		_RGBA_TO_3_JPEG8_2_FILES,
		_RGB_TO_1_FILE,
		_PNG24_TO_1_FILE,
		_PNG32_TO_1_FILE,
		_RGBA_TO_4_PNG_mixed1,
		_RGBA_TO_4_PNG_mixed2,
		_RGBA_TO_4_PNG_mixed3,
		_RGBA_TO_4_PNG_mixed4
	};

	bool m_compare;
	U32 m_texNumber;
	U32 m_imageBufferSize;
	U8* m_pImageCopySrc; // will be store uncompressed texture data

	ImageWriteRead(){m_compare = false;  m_texNumber = 0; m_imageBufferSize = 0; m_pImageCopySrc=NULL; }

	IFXRESULT Run();

protected:
	IFXRESULT ConvertTextureObjectCompressionProperties( const IFXString& src, const IFXString& dst, const TransfomType type );
	IFXINLINE void cls(IFXString &src,IFXString &dst){ src.Clear();dst.Clear();}
	IFXRESULT BinaryComparison( const IFXString& src );
	IFXRESULT VerifyingInputParameters();
};

#endif











