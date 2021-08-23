//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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

#ifndef SAMPLEMETADATA_H
#include "SampleMetaData.h"
#endif

#include "UTHeaders.h"
#include "IFXNameMap.h"
#include "IFXMetaDataX.h"

IFXRESULT SampleMetaData::Run()
{
	IFXRESULT result = IFX_OK;
	
	result = IFXCOMInitialize();
	
	if( IFXSUCCESS( result ) )
	{
		IFXCoreServices* pCoreServices = NULL;
		IFXCoreServices* pCoreServicesMain = NULL;

		TEST(IFXSUCCESS(result = InitializeSceneGraph_CoreService( &pCoreServices, &pCoreServicesMain )), IFXException);
		
		if( IFXSUCCESS( result ) )
		{
			IFXDECLARELOCAL(IFXMetaDataX, pMetaDataX);
			U8 str[] = "Value of key N2";
			IFXString s1, s2, s3, sa1, sa2;
			IFXString *ps1 = NULL, *ps2 = NULL;
			U32 i, j, s;
			U8* p = NULL;

			TEST(IFXSUCCESS(pCoreServices->QueryInterface(IID_IFXMetaDataX, (void**)&pMetaDataX)), IFXException);

			pMetaDataX->SetStringValueX(IFXString("Key N1"), IFXString("Value of key N1"));
			pMetaDataX->SetBinaryValueX(IFXString("Key N2"), sizeof(str), str);
			pMetaDataX->SetStringValueX(IFXString("Key N3"), IFXString("Value of key N3"));

			TEST(IFXSUCCESS(pMetaDataX->GetIndex(IFXString("Key N1"), i)), IFXException);
			pMetaDataX->GetStringX(i, s1);
			TEST(IFXSUCCESS(pMetaDataX->GetIndex(IFXString("Key N2"), i)), IFXException);
			pMetaDataX->GetBinarySizeX(i, s);
			p = new U8[s];
			pMetaDataX->GetBinaryX(i, p);
			s2.Assign(p);
			TEST(IFXSUCCESS(pMetaDataX->GetIndex(IFXString("Key N3"), i)), IFXException);
			pMetaDataX->GetStringX(i, s3);
			TEST(s1 == "Value of key N1", IFXException);
			TEST(s2 == "Value of key N2", IFXException);
			TEST(s3 == "Value of key N3", IFXException);

			pMetaDataX->DeleteAll();
			delete []p;

			pMetaDataX->SetStringValueX("key1#","");
			pMetaDataX->SetStringValueX("key2#attr1","");
			pMetaDataX->SetStringValueX("key3#attr1=","");
			pMetaDataX->SetStringValueX("key4##attr1=val1","");
			pMetaDataX->SetStringValueX("key5   #attr1=\"value 1\"","");
			pMetaDataX->SetStringValueX("key6#attr1=\"value#1\"   #attr2=\"value 2\"\"","");

			TEST(IFXSUCCESS(pMetaDataX->GetIndex(L"key1", i)), IFXException);
			TEST(IFXSUCCESS(pMetaDataX->GetSubattributeIndex(i, L"", j)), IFXException);
			pMetaDataX->GetSubattributeNameX(i, j, s1);
			ps1 = &sa1;
			pMetaDataX->GetSubattributeValueX(i, j, ps1);
			TEST(s1 == L"", IFXException);
			TEST(ps1 == NULL, IFXException);

			TEST(IFXSUCCESS(pMetaDataX->GetIndex(L"key2", i)), IFXException);
			TEST(IFXSUCCESS(pMetaDataX->GetSubattributeIndex(i, L"attr1", j)), IFXException);
			pMetaDataX->GetSubattributeNameX(i, j, s1);
			ps1 = &sa1;
			pMetaDataX->GetSubattributeValueX(i, j, ps1);
			TEST(s1 == L"attr1", IFXException);
			TEST(ps1 == NULL, IFXException);

			TEST(IFXSUCCESS(pMetaDataX->GetIndex(L"key3", i)), IFXException);
			TEST(IFXSUCCESS(pMetaDataX->GetSubattributeIndex(i, L"attr1", j)), IFXException);
			pMetaDataX->GetSubattributeNameX(i, j, s1);
			ps1 = &sa1;
			pMetaDataX->GetSubattributeValueX(i, j, ps1);
			TEST(s1 == L"attr1", IFXException);
			TEST(sa1 == L"", IFXException);

			TEST(IFXSUCCESS(pMetaDataX->GetIndex(L"key4", i)), IFXException);
			TEST(IFXSUCCESS(pMetaDataX->GetSubattributeIndex(i, L"", j)), IFXException);
			pMetaDataX->GetSubattributeNameX(i, j, s1);
			ps1 = &sa1;
			pMetaDataX->GetSubattributeValueX(i, j, ps1);
			TEST(IFXSUCCESS(pMetaDataX->GetSubattributeIndex(i, L"attr1", j)), IFXException);
			pMetaDataX->GetSubattributeNameX(i, j, s2);
			ps2 = &sa2;
			pMetaDataX->GetSubattributeValueX(i, j, ps2);
			TEST(s1 == L"", IFXException);
			TEST(ps1 == NULL, IFXException);
			TEST(s2 == L"attr1", IFXException);
			TEST(sa2 == L"val1", IFXException);

			TEST(IFXSUCCESS(pMetaDataX->GetIndex(L"key5   ", i)), IFXException);
			TEST(IFXSUCCESS(pMetaDataX->GetSubattributeIndex(i, L"attr1", j)), IFXException);
			pMetaDataX->GetSubattributeNameX(i, j, s1);
			ps1 = &sa1;
			pMetaDataX->GetSubattributeValueX(i, j, ps1);
			TEST(s1 == L"attr1", IFXException);
			TEST(sa1 == L"value 1", IFXException);

			TEST(IFXSUCCESS(pMetaDataX->GetIndex(L"key6", i)), IFXException);
			TEST(IFXSUCCESS(pMetaDataX->GetSubattributeIndex(i, L"attr1", j)), IFXException);
			pMetaDataX->GetSubattributeNameX(i, j, s1);
			ps1 = &sa1;
			pMetaDataX->GetSubattributeValueX(i, j, ps1);
			TEST(IFXSUCCESS(pMetaDataX->GetSubattributeIndex(i, L"attr2", j)), IFXException);
			pMetaDataX->GetSubattributeNameX(i, j, s2);
			ps2 = &sa2;
			pMetaDataX->GetSubattributeValueX(i, j, ps2);
			pMetaDataX->GetEncodedKeyX(i, s3);
			TEST(s1 == L"attr1", IFXException);
			TEST(s2 == L"attr2", IFXException);
			TEST(s3 == L"key6#attr1=\"value#1\"#attr2=\"value 2\"\"\"", IFXException);
			TEST(sa1 == L"value#1", IFXException);
			TEST(sa2 == L"value 2\"", IFXException);
		}

		if( IFXSUCCESS( result ) )
		{
			IFXRELEASE(pCoreServicesMain);
			IFXRELEASE(pCoreServices);
		}

		result = IFXCOMUninitialize();
	}

	return result;
}
