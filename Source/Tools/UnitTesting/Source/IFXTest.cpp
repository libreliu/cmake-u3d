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

#ifndef IFXTEST_H
#include "IFXTest.h"
#endif
#include <stdlib.h>
#include <string.h>
#include <typeinfo>

using namespace U3DTestSuite;

#define MAX_FILE_SIZE 1048575
#define MAX_CLASS_NAME 128

void trim_name(char *strDestination, const char *strSource)
{
	int i, len;

	len = strlen(strSource);
	
	i=0;
	while( (i < len) && (strSource[i] >= '0') && (strSource[i] <= '9') ) i++;
	if (i == len) i--;

	strcpy(strDestination, &(strSource[i]));
}

/**
This function verifies value of condition and makes decision if the test
failed or not
*/
IFXRESULT IFXTest::DoTest(
						BOOL cond, const char* lbl,
						const char* fname, U32 lineno)
{
  if(!cond)
    return DoFail(lbl, fname, lineno);
  else
    return Succeed();
}

/**
This function prints failure message
*/
IFXRESULT IFXTest::DoFail(
						const char* lbl,
						const char* fname, U32 lineno)
{
  ++m_nFail;
  long position = ftell(m_pLog);

  if( position > MAX_FILE_SIZE)
  {
    fclose( m_pLog );
    m_pLog = fopen( "trace.txt", "w" );
    fprintf( m_pLog, "Trace file buffer overflow!\n" );
    m_nPass = 0;
    m_nFail = 1;
    Report();
    fclose( m_pLog );
    exit( IFX_E_INVALID_RANGE );
  }

  char name[MAX_CLASS_NAME];

  trim_name(name, typeid(*this).name());
  fprintf( m_pLog, name );
  fprintf( m_pLog, " failure " );

  fprintf( m_pLog, "(%s):\n\t", lbl );
  fprintf( m_pLog, fname );
  fprintf( m_pLog, " (line %d)\n", lineno );

  return IFX_OK;
}

IFXRESULT IFXTest::NewLog()
{
  m_pLog = fopen( "trace.txt", "w" );
  if( m_pLog != NULL )
    return IFX_OK;
  else
    return IFX_E_NOT_INITIALIZED;
}

IFXRESULT IFXTest::ReleaseLog()
{
  if( fclose( m_pLog ) == 0 )
    return IFX_OK;
  else
    return IFX_E_INVALID_POINTER;
}

IFXRESULT IFXTest::Log( const char* format, ... )
{
  va_list argptr;
  va_start( argptr, format );
  fprintf(m_pLog, format, argptr);
  fprintf(m_pLog, "\n");
  //  fprintf( m_pLog, mes );
  return IFX_OK;
}

IFXRESULT IFXTest::Report() const
{
  char name[MAX_CLASS_NAME];

  trim_name(name, typeid(*this).name());
  fprintf( m_pLog, "Test \"");
  fprintf( m_pLog, name );
  fprintf( m_pLog, " \":\n\tPassed: %d", m_nPass );
  fprintf( m_pLog, "\tFailed: %d", m_nFail );
  return IFX_OK;
}

