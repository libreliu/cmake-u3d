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
//Exec: 20
//Timeout: 1
//Category: Datatypes

#include "IFXTest.h"
#include "IFXSuite.h"
#include "TestIFXString.h"
#include "IFXCOM.h"

using namespace U3DTestSuite;

int main()
{
  int isFailed = 1;
  IFXRESULT result = IFX_OK;

  result = IFXCOMInitialize();

  if (IFXSUCCESS(result))
  {
    TestIFXString test;

    result = test.NewLog();

    if (IFXSUCCESS(result))
      result = test.Run();

    if (IFXSUCCESS(result))
      result = test.Report();

    if (IFXSUCCESS(result))
      result = test.ReleaseLog();

    if (0 == test.GetNumFailed() && IFXSUCCESS(result))
      isFailed = 0;

    if (IFXSUCCESS(result))
      result = IFXCOMUninitialize();
  }

  return isFailed;
}
