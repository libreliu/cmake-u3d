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

#ifndef RESIZEJPGTEXTUREIMAGE_H
#define SampleTextureResize
#include "IFXTest.h"
#include "IFXException.h"

class ResizeJPGTextureImage : public U3DTestSuite::IFXTest
{
public:
	enum TransfomType
	{
		_NONE,
		JPG_128,
		JPG_256,
		JPG_512,
	};

	IFXRESULT Run();

protected:
	IFXRESULT ConvertTextureObjectCompressionProperties(const IFXString& src, const IFXString& dst, const TransfomType type );
};

#endif











