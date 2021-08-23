//***************************************************************************
//
//  Copyright (c) 2004 - 2006 Intel Corporation
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
	@file	ComponentConnector.h

			This module defines the ComponentConnector unit test class.

			It's used to test the CIFXConnector component.
*/


#ifndef COMPONENTCONNECTOR_H
#define COMPONENTCONNECTOR_H


//***************************************************************************
//	Includes
//***************************************************************************


#include "IFXTest.h"
#include "IFXException.h"


//***************************************************************************
//	Classes, structures and types
//***************************************************************************


//---------------------------------------------------------------------------
class ComponentConnector: public U3DTestSuite::IFXTest
{
	public:
//					ComponentConnector(){}
		IFXRESULT	Run();

	private:

		void		TestCreationAndUsage();
};


#endif


