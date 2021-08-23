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
#ifndef SampleOSLoader_H
#include "SampleOSLoader.h"
#endif

#include "UTHeaders.h"
#include "IFXOSLoader.h"

IFXRESULT SampleOSLoader::Run ()
{
  IFXRESULT result = IFXCOMInitialize();
    TEST( IFXSUCCESS(result), IFXException );

  if( IFXSUCCESS( result ))
  {
#if defined( WIN32 )
    IFXHANDLE handle = IFXLoadLibrary(L"IFXCore.dll");
#elif defined( LINUX )
    IFXHANDLE handle = IFXLoadLibrary(L"IFXCore.so");
#elif defined( MAC32 )
    IFXHANDLE handle = IFXLoadLibrary(L"IFXCore.dylib");
#else
    IFXTRACE_GENERIC(L"%s:%i Platform is not supported", __FILE__, __LINE__);
#endif

    TEST( handle != NULL, IFXException );

    if( NULL != handle )
    {
      IFXOSFUNC func = IFXGetAddress( handle, "IFXAllocate");

      TEST( NULL != func, IFXException );

      TEST( IFXSUCCESS( IFXReleaseLibrary(handle)), IFXException );
    }
  }

  result = IFXCOMUninitialize();

  return result;
}
