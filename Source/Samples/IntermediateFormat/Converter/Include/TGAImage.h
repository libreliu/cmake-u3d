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

/**
@file TGAImage.h

This module defines ...
*/

#ifndef TGAImage_H
#define TGAImage_H

//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXResult.h"

namespace U3D_IDTF
{
	//***************************************************************************
	//  Defines
	//***************************************************************************


	//***************************************************************************
	//  Constants
	//***************************************************************************


	//***************************************************************************
	//  Enumerations
	//***************************************************************************


	//***************************************************************************
	//  Classes, structures and types
	//***************************************************************************

	/**
	This class is used for manipulation with TGA images.
	*/
	class TGAImage
	{
	public:
		TGAImage();
		virtual ~TGAImage();

		U8* GetData() { return m_RGBPixels;};
		U32 GetWidth() { return m_Width; };
		U32 GetHeight() { return m_Height; };
		U32 GetChannels() { return m_Channels; };
		IFXRESULT Initialize( U32 width, U32 height, U32 channels );
		IFXRESULT Deallocate();

		IFXRESULT Write( const IFXCHAR* pFileName );
		IFXRESULT Read( const IFXCHAR* pFileName );

	private:
		U32 m_Width, m_Height;
		U32 m_Channels;
		U8* m_RGBPixels; // RGBA
		U8* m_BGRPixels; // BGRA
	};

	//***************************************************************************
	//  Inline functions
	//***************************************************************************


	//***************************************************************************
	//  Global function prototypes
	//***************************************************************************


	//***************************************************************************
	//  Global data
	//***************************************************************************

	//***************************************************************************
	//  Failure return codes
	//***************************************************************************

	/**
	@todo:  Insert module/interface specific return code description.
	*/
	//#define IFX_E_????  MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_????, 0x0000 )

}

#endif
