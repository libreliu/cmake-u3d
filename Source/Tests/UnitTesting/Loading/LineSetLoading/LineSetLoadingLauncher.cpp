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
//Exec: 10
//Timeout: 1
//Category: Loading

#include "LineSetLoading.h"

int main()
{
	LineSetLoading test;

	test.NewLog();
	test.Write();
	test.Report();
	test.ReleaseLog();

	if( test.GetNumFailed() ) 
	{
		return 1;
	} 
	else
	{
		return 0;
	}
}
