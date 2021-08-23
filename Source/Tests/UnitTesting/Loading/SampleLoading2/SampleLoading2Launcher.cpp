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
//Exec: 60
//Timeout: 1
//Category: Loading

#include "IFXTest.h"
#include "IFXSuite.h"
#include "SampleLoading2.h"
#include "IFXCOM.h"

using namespace U3DTestSuite;

int main()
{
	IFXRESULT result = IFXCOMInitialize();
	U32 failed = 0;

	if( IFXSUCCESS( result ) )
	{
		SampleLoading2 test;
		test.NewLog();
		IFXRESULT result = test.Run();
		test.Report();
		test.ReleaseLog();
		failed = test.GetNumFailed();
	}

	if( IFXSUCCESS( result ) )
		result = IFXCOMUninitialize();

	if( failed ) return 1;
	else return 0;
}

