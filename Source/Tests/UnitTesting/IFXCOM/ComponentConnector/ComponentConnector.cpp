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

/**
	@file	ComponentConnector.cpp

			This module defines the ComponentConnector unit test class.

			It's used to test the CIFXConnector component.
*/


//***************************************************************************
//	Includes
//***************************************************************************


// Note:  This include must be first so that IFX_INIT_GUID is defined so 
// that all CIDs and IIDs are defined for proper linkage.
#include "UTHeaders.h"


#include "ComponentConnector.h"
#include "IFXConnection.h"
#include "IFXConnectionServer.h"
#include "IFXCoreServices.h"

#define  SIZE 1024
//***************************************************************************
//	Public methods
//***************************************************************************


//---------------------------------------------------------------------------
IFXRESULT ComponentConnector::Run()
{
	IFXRESULT	result	= IFX_OK;

	
	try
	{
		result = IFXCOMInitialize();

		if ( IFXSUCCESS( result ) )
		{
			TestCreationAndUsage();

			IFXCOMUninitialize();
//			result = IFXCOMUninitialize();
		}
	}
	catch( IFXException& exception	)
	{
		IFXCHAR	pMessage[ SIZE ];

		swprintf(
			pMessage,
			SIZE,
			L"Received an exception with error return code "
			L"0x%08lX with description\n\"%ls\"\n",
			exception.GetIFXResult(),
			exception.GetText().Raw() );

		result = exception.GetIFXResult();
	}
	catch( ... )
	{
		Log( "Received an unknown exception\n" );

		result = IFX_E_UNDEFINED;
	}


	return result;
}


//***************************************************************************
//	Private methods
//***************************************************************************


//---------------------------------------------------------------------------
void ComponentConnector::TestCreationAndUsage()
{
	IFXRESULT			result;
	IFXUnknown			*pPseudoServerUnknown	= NULL;

	result = IFXCreateComponent(
				CID_IFXCoreServices,
				IID_IFXUnknown,
				( void** ) &pPseudoServerUnknown );

	if ( IFXSUCCESS( result ) )
	{
		IFXConnectionServer	*pConnectionServer	= NULL;

		result = IFXCreateComponent(
					CID_IFXConnector,
					IID_IFXConnectionServer,
					( void** ) &pConnectionServer );

		IFXConnection	*pConnection	= NULL;

		if ( IFXSUCCESS( result ) )
		{
			result = pConnectionServer->QueryInterface(
											IID_IFXConnection,
											( void** ) &pConnection );
		}

		if ( IFXSUCCESS( result ) )
		{
			IFXCoreServices	*pCoreServices;

			result = pConnection->GetReference(
									IID_IFXCoreServices,
									( void** ) &pCoreServices );

			if ( result == IFX_E_CONNECTION_SEVERED )
			{
				result = pConnectionServer->SetReference( pPseudoServerUnknown );

				if ( IFXSUCCESS( result ) )
				{
					result = pConnection->GetReference(
											IID_IFXCoreServices,
											( void** ) &pCoreServices );

					if ( IFXSUCCESS( result ) )
					{
						IFXRELEASE( pCoreServices );

						result = pConnectionServer->SetReference( NULL );

						if ( IFXSUCCESS( result ) )
						{
							result = pConnection->GetReference(
													IID_IFXCoreServices,
													( void** ) &pCoreServices );

							if ( result == IFX_E_CONNECTION_SEVERED )
							{
								IFXRELEASE( pPseudoServerUnknown );

								result = IFX_OK;
							}
							else
								result = IFX_E_UNDEFINED;
						}
					}
				}
			}
			else
				result = IFX_E_UNDEFINED;
		}

		IFXRELEASE( pConnection );
		IFXRELEASE( pConnectionServer );
		IFXRELEASE( pPseudoServerUnknown );
	}

	TEST( IFXSUCCESS( result ), IFXException );
//	TEST( IFXSUCCESS( result ), IFXException( result ) );
}


