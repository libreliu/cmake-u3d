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
#ifndef TEST_H
#define TEST_H

#include "IFXDataTypes.h"
#include "IFXResult.h"
#include "IFXString.h"
#include "IFXException.h"

#define TEST(cond, except) \
  try{ \
  try{DoTest(cond, #cond, __FILE__, __LINE__);} \
  catch(except&){DoFail(#except, __FILE__, __LINE__);} \
  }catch(...){DoFail("Unrecognized exception", __FILE__, __LINE__);}
#define FAIL(str) \
  DoFail(str, __FILE__, __LINE__)

namespace U3DTestSuite {

class IFXTest 
{
private:
  U32 m_nPass;
  U32 m_nFail;
  FILE* m_pLog;
  // Disallowed:
  IFXTest(const IFXTest&);
  IFXTest& operator=(const IFXTest&);
protected:
  IFXRESULT DoTest(
				BOOL cond, const char* lbl,
				const char* fname, U32 lineno);
  IFXRESULT DoFail(
				const char* lbl,
				const char* fname, U32 lineno);
public:
  IFXTest() { m_nPass = m_nFail = 0; }
  virtual ~IFXTest() {}

  /**
  This function opens log file
  */
  IFXRESULT NewLog();

  /**
  This function closes log file
  */
  IFXRESULT ReleaseLog();

  /**
  This function prints message to the log file
  */
//  IFXRESULT Log( char* mes );
  IFXRESULT Log( const char* format, ... );

    /**
  This function should be overridden in subclass
  and should contain verified test conditions
  */
  virtual IFXRESULT Run() = 0;

    /**
  This function returns number of passed tests
  */
  U32 GetNumPassed() const { return m_nPass; }

    /**
  This function returns number of failed tests
  */
  U32 GetNumFailed() const { return m_nFail; }

    /**
  This function increases number of passed tests
  */
  IFXRESULT Succeed() { ++m_nPass; return IFX_OK; }

    /**
  This function prints message with number of passed
  and failed conditions
  */
  IFXRESULT Report() const;

    /**
  This function resets counters of passed and failed tests
  */
  virtual IFXRESULT Reset() { m_nPass = m_nFail = 0; return IFX_OK; }
};
} // namespace U3DTestSuite
#endif

