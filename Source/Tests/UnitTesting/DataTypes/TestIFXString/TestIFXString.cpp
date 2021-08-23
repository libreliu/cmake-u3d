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

#ifndef TESTIFXSTRING_H
#include "TestIFXString.h"
#endif

#include "UTHeaders.h"
#include "IFXString.h"
#include "IFXOSUtilities.h"

IFXRESULT TestIFXString::Run()
{
	IFXRESULT result = IFX_OK;
	FILE* stream;
	U32 uLength = 0;
	U8* pValue = NULL;

	IFXCHAR w_name[] =
	{
		0x5944,
		0x7e4a,
		0x4f0a,
		0x5944,
		0x7e4a,
		0x4f0a,
		0x5944,
		0x7e4a,
		0x4f0a,
		L'\0'
	};

	TEST(IFXSUCCESS(IFXSetDefaultLocale()), IFXException);

	char HtmlHead[] = "<html><head><META http-equiv=Content-Type content='text/html; charset=UTF-8'></head><body>";
	char HtmlEnd[]  = "</body></html>";
	U8 FrenchU8[] = "Je peux manger du verre, ça ne me fait pas de mal<br>";

	IFXString EnglishString = L"I can eat glass and it doesn't hurt me<br>";
	IFXString RussianString = "Я могу есть стекло, оно мне не вредит<br>";
	IFXString RussianString2 = "Я могу есть СТЕКЛО, оно МНЕ не вредит<br>";
	IFXString FrenchString;
	IFXString GreekString = "Μπορώ να φάω σπασμένα γυαλιά χωρίς να πάθω τίποτα<br>";
	IFXString UkrainianString = "Я можу їсти шкло, й воно мені не пошкодить<br>";
	IFXString KoreanString( w_name );
	IFXString TestString;

	stream = fopen( "teststring.html", "w" );
	TEST(stream != NULL, IFXException);
	TEST(fwrite((void*)HtmlHead, 90, 1, stream) == 1, IFXException);

	uLength = EnglishString.LengthU8();
	TEST(uLength == 42, IFXException);
	pValue = new U8[uLength+1];
	TEST(pValue != NULL, IFXException);
	TEST(IFXSUCCESS(EnglishString.ConvertToRawU8(pValue, uLength+1)), IFXException);
	TEST(fwrite((void*)pValue, uLength, 1, stream) == 1, IFXException);
	delete [] pValue;

	TEST(IFXSUCCESS(RussianString.GetLengthU8(&uLength)), IFXException);
	TEST(uLength == 70, IFXException);
	pValue = new U8[uLength+1];
	TEST(pValue != NULL, IFXException);
	TEST(IFXSUCCESS(RussianString.ConvertToRawU8(pValue, uLength+1)), IFXException);
	TEST(fwrite((void*)pValue, uLength, 1, stream) == 1, IFXException);
	delete [] pValue;

	TEST(IFXSUCCESS(FrenchString.Assign(FrenchU8)), IFXException);
	TEST(IFXSUCCESS(FrenchString.GetLengthU8(&uLength)), IFXException);
	pValue = new U8[uLength+1];
	TEST(pValue != NULL, IFXException);
	TEST(IFXSUCCESS(FrenchString.ConvertToRawU8(pValue, uLength+1)), IFXException);
	TEST(fwrite((void*)pValue, uLength, 1, stream) == 1, IFXException);
	delete [] pValue;

	TEST(IFXSUCCESS(TestString.Assign(&GreekString)), IFXException);
	TEST(IFXSUCCESS(TestString.GetLengthU8(&uLength)), IFXException);
	pValue = new U8[uLength+1];
	TEST(pValue != NULL, IFXException);
	TEST(IFXSUCCESS(TestString.ConvertToRawU8(pValue, uLength+1)), IFXException);
	TEST(fwrite((void*)pValue, uLength, 1, stream) == 1, IFXException);
	delete [] pValue;

	TEST(IFXSUCCESS(UkrainianString.GetLengthU8(&uLength)), IFXException);
	pValue = new U8[uLength+1];
	TEST(pValue != NULL, IFXException);
	TEST(IFXSUCCESS(UkrainianString.ConvertToRawU8(pValue, uLength+1)), IFXException);
	TEST(fwrite((void*)pValue, uLength, 1, stream) == 1, IFXException);
	delete [] pValue;

	TEST(IFXSUCCESS(KoreanString.GetLengthU8(&uLength)), IFXException);
	pValue = new U8[uLength+1];
	TEST(pValue != NULL, IFXException);
	TEST(IFXSUCCESS(KoreanString.ConvertToRawU8(pValue, uLength+1)), IFXException);
	TEST(fwrite((void*)pValue, uLength, 1, stream) == 1, IFXException);
	delete [] pValue;

	/*----------------------------------------------------------------------------*/
	fwrite((void*)"<br>", 4, 1, stream);
	I32 CompareResult = TestString.Compare("Μπορώ να φάω σπασμένα γυαλιά χωρίς να πάθω τίποτα<br>");
	TEST(CompareResult == 0, IFXException);

	CompareResult = RussianString.CompareNoCase(RussianString2);
	TEST(CompareResult == 0, IFXException);

	IFXCHAR Letter[2] = {0};
	U32 length = UkrainianString.Length();
    
	U32 i;
	for( i = 0; i < length; ++i )
	{
		TEST(IFXSUCCESS(UkrainianString.GetAt(i, Letter)), IFXException);
		TEST(IFXSUCCESS(TestString.Concatenate(Letter)), IFXException);
	}
	
	TEST(IFXSUCCESS(TestString.Concatenate(RussianString)), IFXException);
	TEST(IFXSUCCESS(TestString.Concatenate(FrenchU8)), IFXException);
	TEST(IFXSUCCESS(TestString.Concatenate(KoreanString)), IFXException);

	/*----------------------------------------------------------------------------*/

	uLength = TestString.LengthU8();
	pValue = new U8[uLength+1];
	TEST(IFXSUCCESS(TestString.ConvertToRawU8(pValue, uLength+1)), IFXException);
	TEST(fwrite((void*)pValue, uLength, 1, stream) == 1, IFXException);
	delete [] pValue;

	TEST(IFXSUCCESS(TestString.Clear()), IFXException);

	TEST(fwrite((void*)HtmlEnd, 14, 1, stream) == 1, IFXException);
	TEST(fclose(stream) == 0, IFXException);

	// ConvertToRawU8 crash test1
	{
		IFXString string = (U8*)"";
		char* pEmpty = "";

		TEST( IFX_E_BAD_PARAM == string.ConvertToRawU8( (U8*)pEmpty, 0 ), IFXException );
	}

	// ConvertToRawU8 crash test2
	{
		IFXString string = "";
		U8* pEmpty = (U8*)"";

		TEST( IFX_E_NOT_INITIALIZED == string.ConvertToRawU8( pEmpty, 10 ), IFXException );
	}

	// ConvertToRawU8 crash test3
	{
		IFXString string = "qwert";
		U8* pEmpty = (U8*)"";

		TEST( IFX_E_BAD_PARAM == string.ConvertToRawU8( pEmpty, 0 ), IFXException );
	}

	// ConvertToRawU8 crash test4
	/*{
		IFXString empty = "qwerty";
		U8* pEmpty = (U8*)"00";

		empty.ConvertToRawU8( pEmpty, 6 );
	}*/

	// ConvertToRawU8 crash test5
	{
		IFXString string;
		U8* pEmpty = (U8*)"aaaaaa";

		TEST( IFX_E_NOT_INITIALIZED == string.ConvertToRawU8( pEmpty, 6 ), IFXException );
	}

	// ConvertToRawU8 crash test6
	/*{
		IFXString string = "qwert";
		U8* pEmpty = (U8*)"";

		TEST( IFX_E_BAD_PARAM == string.ConvertToRawU8( pEmpty, 4 ), IFXException );
	}*/

	{
		IFXString RussianFilter = "есть*оно";
		TEST(RussianString.Match(RussianFilter, 0) != 0, IFXException );
	}

	IFXString fullPath(L"\\plugins\\IFXCore.dll"), pPath;
	U32 index = 0;

	TEST( IFXSUCCESS(fullPath.FindSubstring( L"IFXCore.dll", &index )), IFXException);
	TEST( IFXSUCCESS(fullPath.Substring( &pPath, 0, index )), IFXException);

	IFXString digits;
	TEST( IFXSUCCESS(digits.ToString(23412, 10)), IFXException);
	TEST( digits == L"23412", IFXException);

	return result;
}

