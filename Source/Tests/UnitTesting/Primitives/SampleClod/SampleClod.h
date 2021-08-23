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
#ifndef SAMPLE_CLOD_H
#define SAMPLE_CLOD_H
#include "IFXTest.h"
#include "IFXException.h"
#include "UTHeaders.h"


float distance(IFXVector3 &pnt1, IFXVector3& pnt2 );

class SampleClod: public U3DTestSuite::IFXTest
{
public:
  SampleClod(){}
  IFXRESULT Run() { return IFX_OK; }
  IFXRESULT Run( U3DSceneUtilities* pSGUtils );
};

#endif