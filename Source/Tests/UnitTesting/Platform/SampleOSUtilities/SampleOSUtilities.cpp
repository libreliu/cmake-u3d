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
#ifndef SampleOSUtilities_H
#include "SampleOSUtilities.h"
#endif

#include "UTHeaders.h"
#include "IFXOSLoader.h"
#include "IFXUnknown.h"
#include "IFXMemory.h"
#include "IFXPlugin.h"
#include "IFXShapesCIDs.h"
#include "IFXOSUtilities.h"
#include "IFXIPP.h"


IFXRESULT SampleOSUtilities::Run ()
{
  U32 time1,time2;
  IFXCOMInitialize();
  IFXRESULT result = IFX_OK;
  
  IFXOSInitialize();
  TEST(IFXOSCheckCPUFeature(IFXCPUFeature_MMX), IFXException );
  TEST(IFXOSCheckCPUFeature(IFXCPUFeature_SSE), IFXException );
  TEST(IFXOSCheckCPUFeature(IFXCPUFeature_SSE2), IFXException );
//  TEST(IFXOSCheckCPUFeature(IFXCPUFeature_SSE3), IFXException );
  time1 = IFXOSGetTime();
  IFXOSSleep(500);
  time2 = IFXOSGetTime();
  TEST(time2-time1 <= 550, IFXException );
  TEST(time2-time1 >= 450, IFXException );
#ifdef DEBUG
  TEST(
	  IFXOSGetDebugLevel() == IFXDEBUG_NOTHING ||
	  IFXOSGetDebugLevel() == IFXDEBUG_ERROR ||
	  IFXOSGetDebugLevel() == IFXDEBUG_WARNING ||
	  IFXOSGetDebugLevel() == IFXDEBUG_MESSAGE
	  , IFXException );
  IFXOSOutputDebugString(L"TEST");
#endif
  IFXOSUninitialize();

  IFXCOMUninitialize();
  return result;
}
