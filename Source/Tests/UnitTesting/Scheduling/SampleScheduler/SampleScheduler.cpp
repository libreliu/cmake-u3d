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

#ifndef SAMPLESCHEDULER_H
#include "SampleScheduler.h"
#endif

#include "UTHeaders.h"
#include "IFXScheduler.h"
#include "IFXNotificationManager.h"
#include "IFXTask.h"

const U32 TestTaskID = 0x00000001;

class TestTask: public IFXTask
{
	friend IFXRESULT TestTask_Factory( IFXREFIID interfaceId, void** ppInterface );
public:
	TestTask():m_refcount(0){};
	virtual ~TestTask(){};
	// IFXUnknown
	virtual U32 IFXAPI	 AddRef(void);
	virtual U32 IFXAPI	 Release(void);
	virtual IFXRESULT IFXAPI	 QueryInterface( IFXREFIID riid, void **ppv);

	// IFXTask
	virtual IFXRESULT IFXAPI	 Execute(IFXTaskData * pTaskData);
private:
	U32 m_refcount;
};

U32 TestTask::AddRef()
{
	return ++m_refcount;
}

U32 TestTask::Release()
{
	if (!(--m_refcount))
	{
		delete this;
		return 0;
	}

	return m_refcount;
}

IFXRESULT TestTask_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT result  = IFX_OK;

	if ( ppInterface )
	{
		// Create the CIFXComponentName component.
		TestTask *pComponent = new TestTask;

		if ( pComponent )
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			if ( IFXSUCCESS( result ) )
				result = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the RobustConstructor or QI fails,
			// this will cause the component to be destroyed.
			pComponent->Release();
		}
		else
			result = IFX_E_OUT_OF_MEMORY;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

IFXRESULT TestTask::QueryInterface(
								   IFXREFIID riid, void** ppv )
{
	IFXRESULT result = IFX_OK;

	IFXASSERT( ppv );

	if ( !ppv ) {
		return IFX_E_INVALID_POINTER;
	}
	else if (riid == IID_IFXUnknown) {
		*ppv = (IFXUnknown*) this;
		AddRef();
		return IFX_OK;
	} else if (riid == IID_IFXTask) {
		*ppv = (IFXTask*) this;
		AddRef();
		return IFX_OK;
	} else {
		*ppv = NULL;
		return IFX_E_UNSUPPORTED;
	}
}

IFXRESULT TestTask::Execute(IFXTaskData * pTaskData)
{
	int x = 2+2;
	return IFX_OK;
}


IFXRESULT SampleScheduler::Run()
{
	IFXRESULT result = IFXCOMInitialize();

	if( IFXSUCCESS( result ) )
	{
		IFXDECLARELOCAL( IFXCoreServices, pCoreServicesMain );
		IFXDECLARELOCAL( IFXCoreServices, pCoreServices );
		IFXDECLARELOCAL( IFXScheduler, pScheduler );

		// Initialize the core services
		if( IFXSUCCESS( result ) )
		{
			result = IFXCreateComponent( 
							CID_IFXCoreServices, 
							IID_IFXCoreServices,
							(void**)&pCoreServicesMain );
		}

		if( IFXSUCCESS( result ) )
		{
			result = pCoreServicesMain->Initialize( 0 );

			if(IFXSUCCESS(result)) 
				result = pCoreServicesMain->GetWeakInterface(&pCoreServices);
		}

		// main test body
		{
			IFXTaskHandle m_TestTaskHandle;
			IFXDECLARELOCAL(IFXSystemManager, pSystemManager);

			TEST(IFXSUCCESS(pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler)), IFXException);

			TEST(IFXSUCCESS(pScheduler->GetSystemManager(&pSystemManager)), IFXException);

			{
				IFXDECLARELOCAL(IFXTask,pTask);
				m_TestTaskHandle = IFXTASK_HANDLE_INVALID;

				TestTask_Factory( IID_IFXTask, (void**) &pTask );

				TEST(IFXSUCCESS(pSystemManager->RegisterTask(pTask, 1, (void*)TestTaskID, &m_TestTaskHandle)), IFXException);
				TEST(IFXSUCCESS(pScheduler->Service()), IFXException);
				TEST(IFXSUCCESS(pScheduler->Service()), IFXException);
				TEST(IFXSUCCESS(pSystemManager->UnregisterTask(m_TestTaskHandle)), IFXException);
				m_TestTaskHandle = IFXTASK_HANDLE_INVALID;
			}
		}
	}

	if( IFXSUCCESS( result ) )
		result = IFXCOMUninitialize();

	return result;
}
