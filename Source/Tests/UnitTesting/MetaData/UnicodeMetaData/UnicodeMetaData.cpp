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
#ifndef UNICODEMETADATA_H
#include "UnicodeMetaData.h"
#endif

#include "UTHeaders.h"
#include "IFXLoadManager.h"
#include "IFXWriteManager.h"
#include "IFXStdio.h"
#include "IFXReadBuffer.h"
#include "IFXWriteBuffer.h"
#include "IFXMetaDataX.h"
#include "IFXOSUtilities.h"

IFXRESULT UnicodeMetaData::Run()
{
    IFXRESULT result = IFXCOMInitialize();

  if( IFXSUCCESS( result ) )
  {
    IFXCoreServices* pCoreServices = NULL;
    IFXCoreServices* pCoreServicesMain = NULL;
    IFXSceneGraph* pSceneGraph = NULL;
    EIFXLoadState eState = IFX_LOAD_STATE_UNLOADED;
    IFXString s1, s1_res, s1_val;
    wchar_t w_str[100];
	IFXString src;
    U32 i;

    // locale should be set for Linux and Mac OS X
	TEST(IFXSUCCESS(IFXSetDefaultLocale()), IFXException);

    w_str[0] = 0x3092;
    w_str[1] = 0x697d;
    w_str[2] = 0x5929;
    w_str[3] = 0x3067;
    w_str[4] = 0x691c;
    w_str[5] = 0x7d22;
    w_str[6] = L'\0';
    s1.Assign(w_str);

    w_str[0] = 0x30d7;
    w_str[1] = 0x30ed;
    w_str[2] = 0x30e2;
    w_str[3] = 0x30fc;
    w_str[4] = 0x30b7;
    w_str[5] = 0x30e7;
    w_str[6] = 0x30f3;
    w_str[7] = 0x30d3;
    w_str[8] = 0x30c7;
    w_str[9] = 0x30aa;
    w_str[10] = L'\0';
    s1_val.Assign(w_str);

    result = InitializeSceneGraph_CoreService( &pCoreServices, &pCoreServicesMain, &pSceneGraph );

    //1: load file
    if( IFXSUCCESS( result ) )
    {
      IFXDECLARELOCAL(IFXScheduler, pScheduler);
      IFXDECLARELOCAL(IFXMetaDataX, pMetaDataX);
      IFXDECLARELOCAL(IFXStdio, pIFXStdio1);
      IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
      IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);

      src = getenv("U3D_SOURCE");
      src+="Tests/UnitTesting/SamplePlayer/SimpleTestNR.u3d";

      result = pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler);
      if( IFXSUCCESS( result ) )
        result = IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager);
      if( IFXSUCCESS( result ) )
        result = pIFXLoadManager->Initialize(pCoreServices);
      if( IFXSUCCESS( result ) )
        result = IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer);
      if( IFXSUCCESS( result ) )
        result = pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio1);
      if( IFXSUCCESS( result ) )
        result = pIFXStdio1->Open( (IFXCHAR*) src.Raw() );
      if( IFXSUCCESS( result ) )
        result = pIFXLoadManager->Load(pIFXReadBuffer, TRUE);
      if( IFXSUCCESS( result ) )
      {
        while (eState < IFX_LOAD_STATE_MEDIA_LOADED)
        {
          result = pScheduler->Service();
          result = pIFXLoadManager->GetAggregateLoadState(eState);
        };
      }
      if( IFXSUCCESS( result ) )
        result = pIFXStdio1->Close();

      if( IFXSUCCESS( result ) )
        result = pCoreServices->QueryInterface(IID_IFXMetaDataX, (void**)&pMetaDataX);


      if( IFXSUCCESS( result ) )
      {
        pMetaDataX->SetStringValueX(s1, s1_val);

        TEST(IFXSUCCESS(pMetaDataX->GetIndex(s1, i)), IFXException);
        pMetaDataX->GetStringX(i, s1_res);
        TEST(s1_val == s1_res, IFXException);
      }
    }

    // 2: save file
    if( IFXSUCCESS( result ) )
    {
      IFXDECLARELOCAL(IFXStdio, pIFXStdio2);
      IFXDECLARELOCAL(IFXWriteManager, pIFXWriteManager);
      IFXDECLARELOCAL(IFXWriteBuffer, pIFXWriteBuffer);

      TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXWriteManager, IID_IFXWriteManager, (void**)&pIFXWriteManager)), IFXException);
      TEST(IFXSUCCESS(pIFXWriteManager->Initialize(pCoreServices)), IFXException);
      TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioWriteBuffer, IID_IFXWriteBuffer, (void**)&pIFXWriteBuffer)), IFXException);
      TEST(IFXSUCCESS(pIFXWriteBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio2)), IFXException);

      src = getenv("U3D_SOURCE");
      src+="Tests/UnitTesting/SamplePlayer/SimpleTestNR_unicode_output.u3d";

      TEST(IFXSUCCESS(pIFXStdio2->Open( (IFXCHAR*) src.Raw() )), IFXException);
      TEST(IFXSUCCESS(pSceneGraph->Mark()), IFXException);
      TEST(IFXSUCCESS(pIFXWriteManager->Write(pIFXWriteBuffer, IFXEXPORT_EVERYTHING)), IFXException); // ^ IFXEXPORT_FILEREFERENCES

      TEST(IFXSUCCESS(pIFXStdio2->Close()), IFXException);
    }

    // 3: load saved file
    if( IFXSUCCESS( result ) )
    {
      IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManagerCheck);
      IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBufferCheck);
      IFXDECLARELOCAL(IFXMetaDataX, pMetaDataX);
      IFXDECLARELOCAL(IFXStdio, pIFXStdio3);
      IFXDECLARELOCAL(IFXScheduler, pScheduler);

      TEST(IFXSUCCESS(pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler)), IFXException);

      TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManagerCheck)), IFXException);
      TEST(IFXSUCCESS(pIFXLoadManagerCheck->Initialize(pCoreServices)), IFXException);

      TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBufferCheck)), IFXException);
      TEST(IFXSUCCESS(pIFXReadBufferCheck->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio3)), IFXException);

      TEST(IFXSUCCESS(pIFXStdio3->Open( (IFXCHAR*) src.Raw() )), IFXException);

      TEST(IFXSUCCESS(pIFXLoadManagerCheck->Load(pIFXReadBufferCheck, TRUE)), IFXException);

      eState = IFX_LOAD_STATE_UNLOADED;

      while (eState < IFX_LOAD_STATE_MEDIA_LOADED)
      {
        result = pScheduler->Service();
        result = pIFXLoadManagerCheck->GetAggregateLoadState(eState);
      };

      if( IFXSUCCESS( result ) )
        result = pIFXStdio3->Close();

      if( IFXSUCCESS( result ) )
        result = pCoreServices->QueryInterface(IID_IFXMetaDataX, (void**)&pMetaDataX);

      if( IFXSUCCESS( result ) )
      {
        TEST(IFXSUCCESS(pMetaDataX->GetIndex(s1, i)), IFXException);
        pMetaDataX->GetStringX(i, s1_res);
        TEST(s1_val == s1_res, IFXException);
      }
    }

    IFXRELEASE(pSceneGraph);
    IFXRELEASE(pCoreServicesMain);
    IFXRELEASE(pCoreServices);
  }

  if( IFXSUCCESS( result ) )
      result = IFXCOMUninitialize();

  return result;
}
