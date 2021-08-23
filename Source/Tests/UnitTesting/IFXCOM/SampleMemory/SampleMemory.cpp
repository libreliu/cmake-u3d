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
#ifndef SampleMemory_H
#include "SampleMemory.h"
#endif

#include "UTHeaders.h"
#include "IFXOSLoader.h"
#include "IFXUnknown.h"
#include "IFXMemory.h"
#include "IFXPlugin.h"
#include "IFXShapesCIDs.h"
#include "IFXOSUtilities.h"
#include "IFXIPP.h"

void* TestFunction(size_t count)
{
  printf("test\n");
  return NULL;
}
IFXRESULT SampleMemory::Run ()
{
	IFXAllocateFunction*  pAllocateFunction;
	IFXDeallocateFunction*  pDeallocateFunction;
	IFXReallocateFunction*  pReallocateFunction;
	IFXRESULT result = IFX_OK;
	result = IFXCOMInitialize();
	TEST( IFXSUCCESS(result), IFXException );
	if(IFXSUCCESS(result))
	{
		U32* p1 = (U32*) IFXAllocate( 10 * sizeof(U32));
		for(int i=0;i<10;i++) p1[i]= i;
		U32* p2 = (U32*) IFXReallocate(p1, 5 * sizeof(U32));
		TEST(IFXSUCCESS(IFXGetMemoryFunctions(&pAllocateFunction,
		&pDeallocateFunction,&pReallocateFunction)),IFXException );
		TEST(IFXSUCCESS(IFXSetMemoryFunctions(TestFunction,pDeallocateFunction,
		pReallocateFunction)),IFXException );
		U32* p3 = (U32*) IFXAllocate( 10 * sizeof(U32));
		TEST(IFXSUCCESS(IFXSetMemoryFunctions(pAllocateFunction,pDeallocateFunction,
		pReallocateFunction)),IFXException );
		IFXDeallocate(p2);
	
		result = IFXCOMUninitialize();
		TEST( IFXSUCCESS(result), IFXException );

	}
	
	return result;
}
