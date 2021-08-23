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
#ifndef SUITE_H
#define SUITE_H
#ifndef IFXTEST_H
#include "IFXTest.h"
#endif

#include "IFXException.h"
#include "IFXMemory.h"
#include "IFXArray.h"
#include "IFXDataTypes.h"
#include "IFXResult.h"
#include "IFXDebug.h"

#pragma warning( disable : 4290 )

namespace U3DTestSuite {

class IFXTestSuiteError : public IFXException {
public:
  IFXTestSuiteError(const IFXString& s = "") { m_text = s.Raw(); }
};

class IFXSuite {
private:
  IFXString m_name;
  IFXArray<IFXTest*> m_pTests;
  // Disallowed ops:
  IFXSuite(const IFXSuite&);
  IFXSuite& operator=(const IFXSuite&);
public:
  IFXSuite(const IFXString& name): m_name(name) {}

  /**
  This function returns the name of the suite
  */
  IFXString GetName() const { return m_name; }

  /**
  This function adds test to suite. If test is empty then IFXTestSuiteError
  exception is thrown
  */
  IFXRESULT AddTestX(IFXTest* t) throw(IFXTestSuiteError);

  /**
  This function adds existing suite to the current suite
  */
  IFXRESULT AddSuite(const IFXSuite&);

  /**
  This function calls IFXTest::Run() repeatedly
  */
  IFXRESULT Run();

  /**
  This function prints out suite result
  */
  IFXRESULT Report() const;

  /**
  This function returns total number of passed tests
  */
  U32 GetNumPassed() const;

  /**
  This function returns total number of failed tests
  */
  U32 GetNumFailed() const;

  /**
  This function releases all resources allocated by suite
  */
  IFXRESULT Free();  // Deletes tests

  /**
  This function resets counters for all tests in the suite
  */
  IFXRESULT Reset();
};

} // namespace U3DTestSuite
#endif
