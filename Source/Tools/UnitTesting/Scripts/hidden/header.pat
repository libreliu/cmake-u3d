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
  @file <!--Perl: test_name-->.h

      This module contains unit test case declaration
*/


#ifndef <!--Perl: uc test_name-->_H
#define <!--Perl: uc test_name-->_H
<!--Perl: utheaders_windows-->
#include "IFXTest.h"

/*
  TODO: Provide additional exception handlers
*/
#include "IFXException.h"

class <!--Perl: test_name-->: public U3DTestSuite::IFXTest
{
public:
  <!--Perl: test_name-->(){}

/*
  TODO: provide some additional functions if needed
*/
  int Test1();
  int Test2();
  int Test3();

/**
  This function defines boolean expressions that form test for CONSOLE subsystem
*/
  IFXRESULT Run()
  {
/*
  TODO: uncomment these lines and set appropriate conditions and exceptions
*/
//    TEST( condition1, IFXException );
//    TEST( condition2, exception2 );
//    TEST( condition3, exception3 );
    return IFX_OK;
  }

/**
  This function defines boolean expressions that form test for WINDOWS subsystem
*/
  IFXRESULT Run( U3DSceneUtilities* pSGUtils )
  {
/*
  TODO: uncomment these lines and set appropriate conditions and exceptions
*/
    TEST(IFXSUCCESS(pSGUtils->InitializeScene()), IFXException);
//    TEST( condition1, IFXException );
//    TEST( condition2, exception2 );
//    TEST( condition3, exception3 );
    return IFX_OK;
  }


};

#endif