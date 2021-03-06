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
@file IConverter.h

This header defines the ... functionality.

@note
*/


#ifndef IConverter_H
#define IConverter_H



//***************************************************************************
//  Includes
//***************************************************************************

#include "ConverterResult.h"

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
	This is the implementation of a class that is used to @todo: usage.

	It supports the following interfaces:  @todo: interfaces.
	*/
	class IConverter
	{
	public:
		/**
		@todo:  INSERT CUSTOM METHOD PROTOTYPES AND DESCRIPTIONS.
		DOCUMENT THEM IN ONLY ONE SPOT.  IT'S BEST TO DO THIS IN THE
		INTERFACE HEADER HERE TO REDUCE DUPLICATION.

		@return Upon success, IFX_OK is returned.  Otherwise, one of the
		following failures are returned:  @todo: return codes.
		*/
		virtual IFXRESULT Convert() = 0;

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

}

#endif
