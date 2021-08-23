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
#ifndef __LINE_SET_LOADING_H__
#define __LINE_SET_LOADING_H__


#include "IFXTest.h"
#include "IFXException.h"

class U3DSceneUtilities;

class LineSetLoading: public U3DTestSuite::IFXTest
{

public:
  
	LineSetLoading(){}

	IFXRESULT Run() { return IFX_OK; }

	IFXRESULT Write();

	IFXRESULT Load();

	IFXRESULT CreateLineSetObject( U3DSceneUtilities* pSGUtils );

	IFXRESULT Run( U3DSceneUtilities* pSGUtils );

	IFXRESULT Load(U3DSceneUtilities* pSGUtils );

};

#endif
