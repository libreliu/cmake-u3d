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
//Exec: 3
//Timeout: 1
//Category: Platform

#include "IFXTest.h"
#include "IFXSuite.h"
#ifndef DLLTest_H
#include "DLLTest.h"
#endif

using namespace U3DTestSuite;

int main()
{
	TestDLLTest test;
	test.NewLog();
	test.Run();
	test.Report();
	test.ReleaseLog();
	if( test.GetNumFailed() ) return 1;
		else return 0;
}
