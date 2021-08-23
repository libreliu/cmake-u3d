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
#ifndef CoreStaticTest_H
#include "CoreStaticTest.h"
#endif

#include "UTHeaders.h"
#include "IFXOSLoader.h"
#include "IFXUnknown.h"
#include "IFXMemory.h"
#include "IFXPlugin.h"
#include "CIFXSimpleList.h"


IFXRESULT CoreStaticTest::Run ()
{
  IFXAllocateFunction*  pAllocateFunction;
  IFXDeallocateFunction*  pDeallocateFunction;
  IFXReallocateFunction*  pReallocateFunction;

  IFXRESULT result = IFX_OK;
  IFXDECLARELOCAL( IFXSimpleList, p );

  result = IFXCOMInitialize();
  TEST( IFXSUCCESS(result), IFXException );

  U32* p1 = (U32*) IFXAllocate( 10 * sizeof(U32));
#ifdef WIN32
  ///@todo fix crash on Linux
  for(int i=0;i<10;i++) p1[i]= i;
#endif
  U32* p2 = (U32*) IFXReallocate(p1, 5 * sizeof(U32));
  TEST(IFXSUCCESS(IFXGetMemoryFunctions(&pAllocateFunction,
    &pDeallocateFunction,&pReallocateFunction)),IFXException );
  TEST(IFXSUCCESS(IFXSetMemoryFunctions(pAllocateFunction,pDeallocateFunction,
    pReallocateFunction)),IFXException );
  IFXDeallocate(p2);

  TEST( IFXSUCCESS( IFXCreateComponent( CID_IFXSimpleList,
      IID_IFXSimpleList, (void**)&p
      )), IFXException );
  IFXRELEASE(p);

  result = IFXCOMUninitialize();

  return result;
}
