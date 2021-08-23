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
#include "ExternalTextureReference.h"

#include "UTHeaders.h"

#include "IFXCheckX.h"
#include "U3DViewer.h"

ExternalTextureReference::ExternalTextureReference()
: m_pSGUtils( NULL )
{
	TEST( IFXSUCCESS( IFXCOMInitialize() ), IFXException );
}

ExternalTextureReference::~ExternalTextureReference()
{
}

IFXRESULT ExternalTextureReference::StartUp()
{
	IFXRESULT result = IFX_OK;

	m_pSGUtils = new U3DSceneUtilities;

	if( NULL == m_pSGUtils )
		result = IFX_E_OUT_OF_MEMORY;

	return result;
}

IFXRESULT ExternalTextureReference::ShutDown()
{
	IFXRESULT result = IFX_OK;

	result = IFXCOMUninitialize();

	return result;
}

IFXRESULT ExternalTextureReference::Run()
{
	IFXRESULT result = IFX_OK;

	// Used texture is located at http://nnsu3d1.ccr.corp.intel.com/u3d/Images
	IFXString http_tex_ref( getenv("U3D_SOURCE") );
	http_tex_ref += L"Tests/UnitTesting/InternetStreaming/ExternalTextureReference/tex_ref.u3d";

	// Used texture is located at ftp://nnsu3d1.ccr.corp.intel.com/u3d/Images
	IFXString ftp_tex_ref( getenv("U3D_SOURCE") );
	ftp_tex_ref += L"Tests/UnitTesting/InternetStreaming/ExternalTextureReference/tex_ref_ftp.u3d";

	result = Load( http_tex_ref );

	if( IFXSUCCESS(result) )
	{
		IFXDELETE( m_pSGUtils );
		m_pSGUtils = new U3DSceneUtilities;

		if( NULL == m_pSGUtils )
			result = IFX_E_OUT_OF_MEMORY;
	}

	if( IFXSUCCESS(result) )
	{
		result = Load( ftp_tex_ref );
	}

	return result;
}

IFXRESULT ExternalTextureReference::Load( IFXString& rFile )
{
	IFXRESULT result = IFX_OK;

	IFXCoreServices* pCoreServices = NULL;

	result = m_pSGUtils->InitializeScene();

	if( IFXSUCCESS(result))
	{
		pCoreServices = m_pSGUtils->m_pCoreServices;
		IFXADDREF( pCoreServices );

		IFXDECLARELOCAL(IFXScheduler, pScheduler);
		IFXDECLARELOCAL(IFXLoadManager, pLoadManager);
		IFXDECLARELOCAL(IFXReadBuffer, pReadBuffer);
		IFXDECLARELOCAL(IFXStdio, pStdio);

		TEST(IFXSUCCESS(pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler)), IFXException);
        TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pLoadManager)), IFXException);
        TEST(IFXSUCCESS(pLoadManager->Initialize(pCoreServices)), IFXException);
        TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pReadBuffer)), IFXException);
        TEST(IFXSUCCESS(pReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pStdio)), IFXException);

		TEST(IFXSUCCESS(pStdio->Open( (IFXCHAR*)rFile.Raw() )), IFXException);

        result = pLoadManager->Load(pReadBuffer, FALSE);
		IFXASSERT(result == IFX_OK);

		EIFXLoadState eState = IFX_LOAD_STATE_UNLOADED;
		while (eState < IFX_LOAD_STATE_MEDIA_LOADED)
		{
			result = pScheduler->Service();
			IFXASSERT(result == IFX_OK);
			result = pLoadManager->GetAggregateLoadState(eState);
			IFXASSERT(result == IFX_OK);
		};

		TEST(IFXSUCCESS(pStdio->Close()), IFXException);
	}

    IFXRELEASE(pCoreServices);

	return result;
}

IFXRESULT ExternalTextureReference::View( char* pParams )
{
	IFXRESULT result = IFX_OK;
	U32 s = mbstowcs(NULL, pParams, 0);
	wchar_t* ptr = NULL;

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

