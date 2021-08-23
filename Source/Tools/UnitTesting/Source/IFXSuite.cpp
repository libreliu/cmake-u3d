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
#ifndef IFXSUITE_H
#include "IFXSuite.h"
#endif

#include <wchar.h>

using namespace U3DTestSuite;

IFXRESULT IFXSuite::AddTestX(IFXTest* t) throw(IFXTestSuiteError)
{
  if(t == 0)
    throw IFXTestSuiteError("Null test in IFXSuite::AddTest");
  m_pTests.CreateNewElement() = t;
  t->Reset();
  return IFX_OK;
}

IFXRESULT IFXSuite::AddSuite(const IFXSuite& s)
{
  U32 i;
  for(i = 0; i < s.m_pTests.GetNumberElements(); ++i)
  {
    IFXASSERT(m_pTests[i]);
    AddTestX(s.m_pTests[i]);
  }
  return IFX_OK;
}

IFXRESULT IFXSuite::Run()
{
  Reset();
  U32 i;
  for(i = 0; i < m_pTests.GetNumberElements(); ++i)
  {
    IFXASSERT(m_pTests[i]);
    m_pTests[i]->Run();
  }
  return IFX_OK;
}

IFXRESULT IFXSuite::Report() const
{
  U32 totFail = 0;
  wprintf(L"Suite \"%ls\"\n=======", m_name.Raw() );
  U32 i;
  for(i = 0; i < m_name.LengthU8(); ++i)
    printf( "=" );
  printf( "=\n" );
  for(i = 0; i < m_pTests.GetNumberElements(); ++i)
  {
    IFXASSERT(m_pTests[i]);
    totFail += m_pTests[i]->Report();
  }
  printf( "\n=======" );
  for(i = 0; i < m_name.LengthU8(); ++i)
    printf( "=" );
  printf( "=\n" );
  return IFX_OK;
}

U32 IFXSuite::GetNumPassed() const
{
  U32 totPass = 0;
  U32 i;
  for(i = 0; i < m_pTests.GetNumberElements(); ++i)
  {
    IFXASSERT(m_pTests[i]);
    totPass += m_pTests[i]->GetNumPassed();
  }
  return totPass;
}

U32 IFXSuite::GetNumFailed() const
{
  U32 totFail = 0, i;
  for(i = 0; i < m_pTests.GetNumberElements(); ++i)
  {
    IFXASSERT(m_pTests[i]);
    totFail += m_pTests[i]->GetNumFailed();
  }
  return totFail;
}

IFXRESULT IFXSuite::Free()
{
  U32 i;
  for(i = 0; i < m_pTests.GetNumberElements(); ++i)
  {
    delete m_pTests[i];
    m_pTests[i] = 0;
  }
  return IFX_OK;
}

IFXRESULT IFXSuite::Reset()
{
  U32 i;
  for(i = 0; i < m_pTests.GetNumberElements(); ++i)
  {
    IFXASSERT(m_pTests[i]);
    m_pTests[i]->Reset();
  }
  return IFX_OK;
}

