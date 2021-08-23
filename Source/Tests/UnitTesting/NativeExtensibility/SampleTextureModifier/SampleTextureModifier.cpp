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
#ifndef SAMPLETEXTUREMODIFIER_H
#include "SampleTextureModifier.h"
#endif

#include "UTHeaders.h"
#include "IFXModifierChain.h"
#include "../../../../Samples/TextureModifier/IFXTextureModifier.h"
#include "../../../../Samples/TextureModifier/IFXTextureModifierPluginCIDs.h"
#include "U3DViewer.h"
#include "IFXCheckX.h"

SampleTextureModifier::SampleTextureModifier()
: m_pSGUtils( NULL )
{
	TEST( IFXSUCCESS( IFXCOMInitialize() ), IFXException );
}

SampleTextureModifier::~SampleTextureModifier()
{
}

IFXRESULT SampleTextureModifier::StartUp()
{
	IFXRESULT result = IFX_OK;

	m_pSGUtils = new U3DSceneUtilities;

	if( NULL == m_pSGUtils )
		result = IFX_E_OUT_OF_MEMORY;

	return result;
}

IFXRESULT SampleTextureModifier::ShutDown()
{
	IFXRESULT result = IFX_OK;

	result =  IFXCOMUninitialize();

	return result;
}

IFXRESULT SampleTextureModifier::Run()
{
	IFXRESULT result = IFX_OK;

	IFXString dst( L"shading_tm.u3d" );
	IFXString src( getenv("U3D_SOURCE") );
	src += L"Tests/UnitTesting/NativeExtensibility/SampleTextureModifier/shading.u3d";
	
	if( NULL != m_pSGUtils)
		result = m_pSGUtils->InitializeScene(IFXPROFILE_EXTENSIBLE);
	else
		result = IFX_E_NOT_INITIALIZED;

	if( IFXSUCCESS(result) )
	{
		// load file
		IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
		IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);
		IFXDECLARELOCAL(IFXStdio, pIFXStdio);

		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager)), IFXException);
		TEST(IFXSUCCESS(pIFXLoadManager->Initialize(m_pSGUtils->m_pCoreServices)), IFXException);
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer)), IFXException);
		TEST(IFXSUCCESS(pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio)), IFXException);

		TEST(IFXSUCCESS(pIFXStdio->Open( (IFXCHAR*)src.Raw() )), IFXException);
		TEST(IFXSUCCESS(pIFXLoadManager->Load(pIFXReadBuffer)), IFXException);
		TEST(IFXSUCCESS(pIFXStdio->Close()), IFXException);
	}

	if( IFXSUCCESS(result) )
	{
		// add extension texture modifier
		IFXTextureModifier* pTextureModifier1 = NULL;
		IFXTextureModifier* pTextureModifier2 = NULL;

		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXTextureModifier, IID_IFXTextureModifier, (void**)&pTextureModifier1)), IFXException);
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXTextureModifier, IID_IFXTextureModifier, (void**)&pTextureModifier2)), IFXException);

		IFXMarker *pMarker = NULL;
		TEST(IFXSUCCESS(pTextureModifier1->QueryInterface( IID_IFXMarker, (void**)&pMarker )), IFXException);
		TEST(IFXSUCCESS(pMarker->SetSceneGraph( m_pSGUtils->m_pSceneGraph )), IFXException);
		pMarker->Release();
		TEST(IFXSUCCESS(pTextureModifier2->QueryInterface( IID_IFXMarker, (void**)&pMarker )), IFXException);
		TEST(IFXSUCCESS(pMarker->SetSceneGraph( m_pSGUtils->m_pSceneGraph )), IFXException);
		pMarker->Release();

		IFXDECLARELOCAL(IFXModifier, pMyModifier1);
		pTextureModifier1->QueryInterface(IID_IFXModifier, (void**)&pMyModifier1);
		IFXDECLARELOCAL(IFXModifier, pMyModifier2);
		pTextureModifier2->QueryInterface(IID_IFXModifier, (void**)&pMyModifier2);

		IFXDECLARELOCAL(IFXModifier, pModifier);
		U32 id = 0;
		m_pSGUtils->m_pTexturePalette->Find(L"lines", &id);
		m_pSGUtils->m_pTexturePalette->GetResourcePtr(id, IID_IFXModifier, (void**)&pModifier);

		IFXDECLARELOCAL(IFXModifierChain, pModChain);
		pModifier->GetModifierChain(&pModChain);
		pModChain->AddModifier(*pMyModifier1);
		pModChain->AddModifier(*pMyModifier2);

		IFXRELEASE(pTextureModifier1);
		IFXRELEASE(pTextureModifier2);
	}

	if( IFXSUCCESS(result) )
	{
		// write file with extension texture modifier
		IFXDECLARELOCAL(IFXWriteManager, pWriteManager);
		IFXDECLARELOCAL(IFXWriteBuffer, pWriteBuffer);
		IFXDECLARELOCAL(IFXStdio, pStdio);

		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXWriteManager, IID_IFXWriteManager, (void**)&pWriteManager)), IFXException);
		TEST(IFXSUCCESS(pWriteManager->Initialize(m_pSGUtils->m_pCoreServices)), IFXException);
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioWriteBuffer, IID_IFXWriteBuffer, (void**)&pWriteBuffer)), IFXException);
		TEST(IFXSUCCESS(pWriteBuffer->QueryInterface(IID_IFXStdio, (void**)&pStdio)), IFXException);

		TEST(IFXSUCCESS(pStdio->Open( (IFXCHAR*)dst.Raw() ) ), IFXException);

		TEST(IFXSUCCESS(m_pSGUtils->m_pSceneGraph->Mark()), IFXException);
		TEST(IFXSUCCESS(pWriteManager->Write(pWriteBuffer, IFXEXPORT_EVERYTHING)), IFXException); // ^ IFXEXPORT_FILEREFERENCES
		TEST(IFXSUCCESS(pStdio->Close()), IFXException);
	}

	return result;
}

IFXRESULT SampleTextureModifier::View( char* pParams )
{
	IFXRESULT result = IFX_OK;
	wchar_t* ptr = NULL;

	if( NULL != pParams )
	{
		U32 s = mbstowcs(NULL, pParams, 0);

		if( s > 0 )
		{
			ptr = new wchar_t[s+1];

			if( NULL != ptr )
			{
				mbstowcs(ptr, pParams, s);
				ptr[s]=L'\0';
			}
			else
				result = IFX_E_OUT_OF_MEMORY;
		}
	}

	if( IFXSUCCESS(result) )
	{
		try
		{
			U3DViewer viewer;

			if( NULL != ptr )
				IFXCHECKX( viewer.ParseArguments( ptr ) );

			IFXCHECKX( viewer.SetupWindows() );
			IFXCHECKX( viewer.InitializeSG( m_pSGUtils ) );

			// Render until the user is done
			IFXCHECKX( viewer.EventLoop() );
		}
		catch(...)
		{
			IFXDELETE_ARRAY( ptr );
			IFXTRACE_GENERIC(L"Viewing failed\n");
			result = IFX_OK;
		}
	}

	IFXDELETE_ARRAY( ptr );

	return result;
}
