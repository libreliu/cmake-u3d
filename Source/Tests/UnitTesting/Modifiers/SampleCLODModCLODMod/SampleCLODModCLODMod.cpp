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

#include "UTHeaders.h"
#include "IFXModifierChain.h"
#include "U3DViewer.h"
#include "IFXCheckX.h"

#include "SampleCLODModCLODMod.h"


SampleCLODModCLODMod::SampleCLODModCLODMod()
: m_pSGUtils( NULL )
{
	TEST( IFXSUCCESS( IFXCOMInitialize() ), IFXException );
}

SampleCLODModCLODMod::~SampleCLODModCLODMod()
{
}

IFXRESULT SampleCLODModCLODMod::StartUp()
{
	IFXRESULT result = IFX_OK;

	m_pSGUtils = new U3DSceneUtilities;

	if( NULL == m_pSGUtils )
		result = IFX_E_OUT_OF_MEMORY;

	return result;
}

IFXRESULT SampleCLODModCLODMod::ShutDown()
{
	IFXRESULT result = IFX_OK;

	result =  IFXCOMUninitialize();

	return result;
}

IFXRESULT SampleCLODModCLODMod::Run()
{
	TEST(IFXSUCCESS(m_pSGUtils->InitializeScene()), IFXException);

	//load CLOD with bones from U3D file
	{
		IFXDECLARELOCAL(IFXLoadManager, pIFXLoadManager);
		IFXDECLARELOCAL(IFXReadBuffer, pIFXReadBuffer);
		IFXDECLARELOCAL(IFXStdio, pIFXStdio);

		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pIFXLoadManager)), IFXException);
		TEST(IFXSUCCESS(pIFXLoadManager->Initialize(m_pSGUtils->m_pCoreServices)), IFXException);
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pIFXReadBuffer)), IFXException);
		TEST(IFXSUCCESS(pIFXReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio)), IFXException);

		IFXString test_file( getenv("U3D_SOURCE") );
		test_file += L"Tests/UnitTesting/Modifiers/SampleCLODModCLODMod/simpleBox.u3d";

		IFXRESULT res = pIFXStdio->Open( (IFXCHAR*)test_file.Raw() );

		TEST(IFXSUCCESS( res ), IFXException );

		if( IFXSUCCESS( res ) )
		{
			TEST(IFXSUCCESS(m_pSGUtils->m_pSceneGraph->Mark()), IFXException);
			TEST(IFXSUCCESS(pIFXLoadManager->Load(pIFXReadBuffer, IFXEXPORT_EVERYTHING)), IFXException); // ^ IFXEXPORT_FILEREFERENCES
			TEST(IFXSUCCESS(pIFXStdio->Close()), IFXException);

		}

	}

	InitCLODModifier( L"Box01" );
	InitCLODModifier( L"Box01" );

	//saving to file
	{
		IFXDECLARELOCAL(IFXWriteManager, pIFXWriteManager);
		IFXDECLARELOCAL(IFXWriteBuffer, pIFXWriteBuffer);
		IFXDECLARELOCAL(IFXStdio, pIFXStdio);

		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXWriteManager, IID_IFXWriteManager, (void**)&pIFXWriteManager)), IFXException);
		TEST(IFXSUCCESS(pIFXWriteManager->Initialize(m_pSGUtils->m_pCoreServices)), IFXException);
		TEST(IFXSUCCESS(IFXCreateComponent(CID_IFXStdioWriteBuffer, IID_IFXWriteBuffer, (void**)&pIFXWriteBuffer)), IFXException);
		TEST(IFXSUCCESS(pIFXWriteBuffer->QueryInterface(IID_IFXStdio, (void**)&pIFXStdio)), IFXException);
		TEST(IFXSUCCESS(pIFXStdio->Open( L"output.u3d" ) ), IFXException);
		TEST(IFXSUCCESS(m_pSGUtils->m_pSceneGraph->Mark()), IFXException);
		TEST(IFXSUCCESS(pIFXWriteManager->Write(pIFXWriteBuffer, IFXEXPORT_EVERYTHING)), IFXException); // ^ IFXEXPORT_FILEREFERENCES
		TEST(IFXSUCCESS(pIFXStdio->Close()), IFXException);
	}

   return IFX_OK;
}

IFXRESULT SampleCLODModCLODMod::InitCLODModifier( const IFXCHAR* pModelName )
{
	/////////////////////////////////////////// create CLOD modifier
	IFXDECLARELOCAL(IFXUnknown,pObject);
	TEST(IFXSUCCESS(IFXCreateComponent( CID_IFXCLODModifier, IID_IFXUnknown, (void**)&pObject )), IFXException);
	IFXDECLARELOCAL(IFXMarker,pMarker );
	TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXMarker, (void**)&pMarker )), IFXException);
	IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
	TEST(IFXSUCCESS(m_pSGUtils->m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph )), IFXException);
	TEST(IFXSUCCESS(pMarker->SetSceneGraph( pSceneGraph )), IFXException);
	IFXDECLARELOCAL(IFXModifier, pMod);
	TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXModifier, (void**)&pMod)), IFXException);

    {
        U32 uResourceID = 0;

        IFXDECLARELOCAL(IFXPalette,pSGPalette);
        TEST(IFXSUCCESS(pSceneGraph->GetPalette( IFXSceneGraph::NODE, &pSGPalette )), IFXException);

        U32 iResultPaletteFind = pSGPalette->Find( pModelName, &uResourceID );
        if ( IFX_E_CANNOT_FIND == iResultPaletteFind  )
        {
			//TEST(IFXSUCCESS(IFX_E_CANNOT_FIND), IFXException);
			return IFX_E_CANNOT_FIND;
        }

        // If this is a chain object
        {
			IFXDECLARELOCAL(IFXModifierChain, pModifierChain);

            // get the modifier at the head of the chain
            IFXDECLARELOCAL(IFXModifier,pHeadModifier);
            U32 iResultGetResource = pSGPalette->GetResourcePtr(uResourceID,IID_IFXModifier,(void**)&pHeadModifier);

            // Get the modifier chain from the head modifier
            TEST(IFXSUCCESS(pHeadModifier->GetModifierChain(&pModifierChain)), IFXException);

            TEST(IFXSUCCESS(pModifierChain->AddModifier( *pMod, IFXModifierChain::END_OF_MODCHAIN, FALSE )), IFXException);

            U32 uModCount = 0;
            TEST(IFXSUCCESS( pModifierChain->GetModifierCount( uModCount )) , IFXException);
        }
    }

    IFXDECLARELOCAL(IFXCLODModifier, pCLODMod);
    TEST(IFXSUCCESS(pObject->QueryInterface( IID_IFXCLODModifier, (void**)&pCLODMod)), IFXException);
    pCLODMod->SetCLODScreenSpaceControllerState( FALSE );
    pCLODMod->SetLODBias( 0.002f );
    pCLODMod->SetCLODLevel( 0.3f );
    
	return IFX_OK;
}

IFXRESULT SampleCLODModCLODMod::View( char* pParams )
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

