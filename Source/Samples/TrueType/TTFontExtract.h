//***************************************************************************
//
//  Copyright (c) 2005 - 2006 Intel Corporation
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


#ifndef TTFONTEXTRACT_H
#define TTFONTEXTRACT_H


//***************************************************************************
//  Includes
//***************************************************************************


#include "IFXDataTypes.h"


//***************************************************************************
//  Classes, structures and types
//***************************************************************************


class IFXGlyphCommandList;


//***************************************************************************
//	Global function prototypes
//***************************************************************************


//---------------------------------------------------------------------------
/**
	Creates a list of glyph generator commands for drawing a
	string using a font specified

	@param pComList		the resulting command list
	@param pString		string to draw
	@param pFont		the name of the font to use
	@param size			size of the font

	@return An IFXRESULT value.
*/
IFXRESULT CreateGlyphString(IFXGlyphCommandList* pComList, IFXCHAR* pString, IFXCHAR* pFont, U32 size);


#endif


