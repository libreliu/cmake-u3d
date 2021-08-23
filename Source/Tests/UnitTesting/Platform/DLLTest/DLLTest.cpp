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
#ifndef DLLTest_H
#include "DLLTest.h"
#endif

#include "UTHeaders.h"
#include "IFXOSLoader.h"
#include "IFXUnknown.h"
#include "IFXMemory.h"
#include "IFXPlugin.h"

/*
Constants define number of components in corresponding plug-in DLs.
Update them if you add or remove componets.
*/
const U32 EXPORTING_COMPONENT_COUNT = 26;
const U32 IMPORTING_COMPONENT_COUNT = 28;
const U32 SCHEDULING_COMPONENT_COUNT = 15;
const U32 RENDERING_WIN32_COMPONENT_COUNT = 28;
const U32 RENDERING_LINUX_COMPONENT_COUNT = 16;
const U32 RENDERING_MAC32_COMPONENT_COUNT = 16;

typedef IFXRESULT ( IFXAPI IFXCOMInitializeFunction )();
typedef IFXRESULT ( IFXAPI IFXCOMUninitializeFunction )();
typedef IFXRESULT ( IFXAPI IFXCreateComponentFunction )(
                          IFXREFCID componentId,
                            IFXREFIID interfaceId,
                            void**    ppInterface );
typedef IFXRESULT ( IFXAPI IFXRegisterComponentFunction )(
                            IFXComponentDescriptor* pComponentDescriptor );
typedef IFXRESULT ( IFXAPI IFXGetMemoryFunctionsFunction )(
              IFXAllocateFunction**   ppAllocateFunction,
                            IFXDeallocateFunction** ppDeallocateFunction,
                            IFXReallocateFunction** ppReallocateFunction );
typedef IFXRESULT ( IFXAPI IFXSetMemoryFunctionsFunction )(
                            IFXAllocateFunction*    pAllocateFunction,
                            IFXDeallocateFunction*  pDeallocateFunction,
                            IFXReallocateFunction*  pReallocateFunction );

IFXRESULT TestDLLTest::Run ()
{

  IFXHANDLE m_handle = NULL;
  IFXRESULT result = IFX_OK;
  IFXCOMInitialize();

  //IFXCore.dll
  IFXCOMInitializeFunction      *pIFXCOMInitializeFunction      = NULL;
  IFXCOMUninitializeFunction    *pIFXCOMUninitializeFunction    = NULL;
  IFXCreateComponentFunction    *pIFXCreateComponentFunction    = NULL;
  IFXAllocateFunction           *pIFXAllocateFunction           = NULL;
  IFXDeallocateFunction         *pIFXDeallocateFunction         = NULL;
  IFXReallocateFunction         *pIFXReallocateFunction         = NULL;
  IFXRegisterComponentFunction  *pIFXRegisterComponentFunction  = NULL;
  IFXGetMemoryFunctionsFunction *pIFXGetMemoryFunctionsFunction = NULL;
  IFXSetMemoryFunctionsFunction *pIFXSetMemoryFunctionsFunction = NULL;

#if defined( WIN32 )
  m_handle = IFXLoadLibrary(L"IFXCore.dll");
#elif defined( LINUX )
  m_handle = IFXLoadLibrary(L"IFXCore.so");
#elif defined( MAC32 )
  m_handle = IFXLoadLibrary(L"IFXCore.dylib");
#else
  IFXTRACE_GENERIC(L"%s:%i Platform is not supported", __FILE__, __LINE__);
#endif

  TEST( m_handle != NULL, IFXException );
  pIFXCOMInitializeFunction=(IFXCOMInitializeFunction*)IFXGetAddress(m_handle,"IFXCOMInitialize");
  TEST(pIFXCOMInitializeFunction != NULL, IFXException );

  pIFXCOMUninitializeFunction=(IFXCOMUninitializeFunction*)IFXGetAddress(m_handle,"IFXCOMUninitialize");
  TEST(pIFXCOMUninitializeFunction != NULL, IFXException );

  pIFXCreateComponentFunction=(IFXCreateComponentFunction*)IFXGetAddress(m_handle,"IFXCreateComponent");
  TEST(pIFXCreateComponentFunction != NULL, IFXException );

  pIFXAllocateFunction=(IFXAllocateFunction*)IFXGetAddress( m_handle, "IFXAllocate");
  TEST(pIFXAllocateFunction != NULL, IFXException );

  pIFXDeallocateFunction=(IFXDeallocateFunction*)IFXGetAddress( m_handle, "IFXDeallocate");
  TEST(pIFXDeallocateFunction != NULL, IFXException );

  pIFXReallocateFunction=(IFXReallocateFunction*)IFXGetAddress( m_handle, "IFXReallocate");
  TEST(pIFXReallocateFunction != NULL, IFXException );

  pIFXRegisterComponentFunction=(IFXRegisterComponentFunction*)IFXGetAddress( m_handle, "IFXRegisterComponent");
  TEST(pIFXRegisterComponentFunction != NULL, IFXException );

  pIFXGetMemoryFunctionsFunction=(IFXGetMemoryFunctionsFunction*)IFXGetAddress( m_handle, "IFXGetMemoryFunctions");
  TEST(pIFXGetMemoryFunctionsFunction != NULL, IFXException );

  pIFXSetMemoryFunctionsFunction=(IFXSetMemoryFunctionsFunction*)IFXGetAddress( m_handle, "IFXSetMemoryFunctions");
  TEST(pIFXSetMemoryFunctionsFunction != NULL, IFXException );

  U32* p=(U32*)pIFXAllocateFunction(2*sizeof(U32));
  TEST( IFXSUCCESS( IFXReleaseLibrary(m_handle)), IFXException );


  IFXPluginRegisterFunction pRegister = NULL;
  IFXPluginCanUnloadNowFunction pCanUnloadNow = NULL;
  U32 componentNumber = 0;
  IFXComponentDescriptor* pComponentDescriptorList = NULL;

  //IFXExporting.dll
#if defined( WIN32 )
  m_handle = IFXLoadLibrary(L"Plugins/IFXExporting.dll");
#elif defined( LINUX )
  m_handle = IFXLoadLibrary(L"Plugins/IFXExporting.so");
#elif defined( MAC32 )
  m_handle = IFXLoadLibrary(L"Plugins/IFXExporting.dylib");
#else
  IFXTRACE_GENERIC(L"%s:%i Platform is not supported", __FILE__, __LINE__);
#endif

  TEST( m_handle != NULL, IFXException );

  if( m_handle != NULL )
  {
    pRegister=(IFXPluginRegisterFunction)IFXGetAddress( m_handle, "IFXPluginRegister");
    TEST( pRegister != NULL, IFXException );
    pCanUnloadNow = (IFXPluginCanUnloadNowFunction)IFXGetAddress(m_handle,"IFXPluginCanUnloadNow" );
    TEST( pCanUnloadNow != NULL, IFXException );

    componentNumber = 0;
    TEST( IFXSUCCESS(pRegister( &componentNumber, &pComponentDescriptorList)),IFXException );
    TEST(componentNumber == EXPORTING_COMPONENT_COUNT, IFXException );
    printf(" \t %d \n",componentNumber);
    TEST( IFXSUCCESS(pCanUnloadNow()),IFXException );
    TEST( IFXSUCCESS( IFXReleaseLibrary(m_handle)), IFXException );
  }

  //IFXImporting.dll
#if defined( WIN32 )
  m_handle = IFXLoadLibrary(L"Plugins/IFXImporting.dll");
#elif defined( LINUX )
  m_handle = IFXLoadLibrary(L"Plugins/IFXImporting.so");
#elif defined( MAC32 )
  m_handle = IFXLoadLibrary(L"Plugins/IFXImporting.dylib");
#else
  IFXTRACE_GENERIC(L"%s:%i Platform is not supported", __FILE__, __LINE__);
#endif

  TEST( m_handle != NULL, IFXException );

  if( m_handle != NULL )
  {
    pRegister=(IFXPluginRegisterFunction)IFXGetAddress( m_handle, "IFXPluginRegister");
    TEST( pRegister != NULL, IFXException );
    pCanUnloadNow = (IFXPluginCanUnloadNowFunction)IFXGetAddress(m_handle,"IFXPluginCanUnloadNow" );
    TEST( pCanUnloadNow != NULL, IFXException );

    componentNumber = 0;
    TEST( IFXSUCCESS(pRegister( &componentNumber, &pComponentDescriptorList)),IFXException );
    TEST( IFXSUCCESS(pCanUnloadNow()),IFXException );

    TEST(componentNumber == IMPORTING_COMPONENT_COUNT, IFXException );
    printf(" \t %d \n",componentNumber);
    TEST( IFXSUCCESS( IFXReleaseLibrary(m_handle)), IFXException );
  }


//IFXRendering.dll
#if defined( WIN32 )
  m_handle = IFXLoadLibrary(L"Plugins/IFXRendering.dll");
#elif defined( LINUX )
  m_handle = IFXLoadLibrary(L"Plugins/IFXRendering.so");
#elif defined( MAC32 )
  m_handle = IFXLoadLibrary(L"Plugins/IFXRendering.dylib");
#else
  IFXTRACE_GENERIC(L"%s:%i Platform is not supported", __FILE__, __LINE__);
#endif

  TEST( m_handle != NULL, IFXException );

  if( m_handle != NULL )
  {
    pRegister=(IFXPluginRegisterFunction)IFXGetAddress( m_handle, "IFXPluginRegister");
    TEST( pRegister != NULL, IFXException );
    pCanUnloadNow = (IFXPluginCanUnloadNowFunction)IFXGetAddress(m_handle,"IFXPluginCanUnloadNow" );
    TEST( pCanUnloadNow != NULL, IFXException );

    componentNumber = 0;
    TEST( IFXSUCCESS(pRegister( &componentNumber, &pComponentDescriptorList)),IFXException );
    printf(" \t %d \n",componentNumber);
#if defined( WIN32 )
    TEST(componentNumber == RENDERING_WIN32_COMPONENT_COUNT, IFXException );
#elif defined( LINUX )
    TEST(componentNumber == RENDERING_LINUX_COMPONENT_COUNT, IFXException );
#elif defined( MAC32 )
    TEST(componentNumber == RENDERING_MAC32_COMPONENT_COUNT, IFXException );
#else
  IFXTRACE_GENERIC(L"%s:%i Platform is not supported", __FILE__, __LINE__);
#endif
    TEST( IFXSUCCESS(pCanUnloadNow()),IFXException );
    TEST( IFXSUCCESS( IFXReleaseLibrary(m_handle)), IFXException );
  }

  //IFXScheduling.dll
#if defined( WIN32 )
  m_handle = IFXLoadLibrary(L"Plugins/IFXScheduling.dll");
#elif defined( LINUX )
  m_handle = IFXLoadLibrary(L"Plugins/IFXScheduling.so");
#elif defined( MAC32 )
  m_handle = IFXLoadLibrary(L"Plugins/IFXScheduling.dylib");
#else
  IFXTRACE_GENERIC(L"%s:%i Platform is not supported", __FILE__, __LINE__);
#endif

  TEST( m_handle != NULL, IFXException );

  if( m_handle != NULL )
  {
    pRegister=(IFXPluginRegisterFunction)IFXGetAddress( m_handle, "IFXPluginRegister");
    TEST( pRegister != NULL, IFXException );
    pCanUnloadNow = (IFXPluginCanUnloadNowFunction)IFXGetAddress(m_handle,"IFXPluginCanUnloadNow" );
    TEST( pCanUnloadNow != NULL, IFXException );

    componentNumber = 0;
    TEST( IFXSUCCESS(pRegister( &componentNumber, &pComponentDescriptorList)),IFXException );
    printf(" \t %d \n",componentNumber);
    TEST(componentNumber == SCHEDULING_COMPONENT_COUNT, IFXException );
    TEST( IFXSUCCESS(pCanUnloadNow()),IFXException );
    TEST( IFXSUCCESS( IFXReleaseLibrary(m_handle)), IFXException );
  }

  IFXCOMUninitialize();

  return result;
}
