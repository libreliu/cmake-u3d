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

#ifndef SAMPLENOTIF_H
#include "SampleNotif.h"
#endif

#include "UTHeaders.h"
#include "IFXScheduler.h"
#include "IFXNotificationManager.h"
#include "IFXTask.h"

const U32 TestTaskID = 0x00000001;

// {407713E2-FD94-419f-8809-8017211C6498}
IFXDEFINE_GUID(IID_IFXTestTask,
0x407713e2, 0xfd94, 0x419f, 0x88, 0x9, 0x80, 0x17, 0x21, 0x1c, 0x64, 0x98);

class IFXTestTask: virtual public IFXUnknown
{
public:
	virtual void SetTest( SampleNotif* test ) = 0;
};

class TestTask: public IFXTask, public IFXTestTask
{
public:
	TestTask();
	~TestTask();

	// IFXUnknown
	virtual U32  IFXAPI	AddRef(void);
	virtual U32  IFXAPI	Release(void);
	virtual IFXRESULT  IFXAPI	QueryInterface( IFXREFIID riid, void **ppv);

	// IFXTask
	virtual IFXRESULT  IFXAPI	 Execute(IFXTaskData * pTaskData);

	// IFXTestTask
	void 	SetTest( SampleNotif* test ) {m_test = test;}

private:
	U32 m_refCount;
	SampleNotif* m_test;
};

TestTask::TestTask()
: m_refCount( 0 )
{
}

TestTask::~TestTask()
{
}

U32 TestTask::AddRef()
{
  return ++m_refCount;
}

U32 TestTask::Release()
{
  if (!(--m_refCount))
  {
    delete this;
    return 0;
  }

  return m_refCount;
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
  else if (riid == IID_IFXUnknown) 
  {
    *ppv = (IFXUnknown*) this;
    AddRef();
    return IFX_OK;
  } 
  else if (riid == IID_IFXTask) 
  {
    *ppv = (IFXTask*) this;
    AddRef();
    return IFX_OK;
  } 
  else if (riid == IID_IFXTestTask) 
  {
    *ppv = (IFXTestTask*) this;
    AddRef();
    return IFX_OK;
  } 
  else 
  {
    *ppv = NULL;
    return IFX_E_UNSUPPORTED;
  }
}

IFXRESULT TestTask::Execute(IFXTaskData * pTaskData)
{
  int x = 2+2;
  m_test->Log("Notification procedure execution successful\n");
  return IFX_OK;
}


IFXRESULT SampleNotif::Run()
{
	IFXRESULT result = IFX_OK;

	result = IFXCOMInitialize();

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
		if(IFXSUCCESS(result)) 
		{
			IFXTaskHandle m_TestTaskHandle = IFXTASK_HANDLE_INVALID;
			IFXDECLARELOCAL(IFXTestTask,pTestTask);

			result = TestTask_Factory( IID_IFXTestTask, (void**) &pTestTask );

			if( IFXSUCCESS(result) )
			{
				IFXTask* pTask = NULL;
				IFXNotificationManager* pNotificationManager = NULL;
				IFXNotificationInfo* pInfo = NULL;

				TEST(IFXSUCCESS(pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler)), IFXException);

				TEST(IFXSUCCESS(pScheduler->GetNotificationManager(&pNotificationManager)), IFXException);

				pTestTask->SetTest( this );

				TEST(IFXSUCCESS(pTestTask->QueryInterface( IID_IFXTask, (void**)&pTask)), IFXException);

				TEST( 
					IFXSUCCESS(pNotificationManager->RegisterTask( 
															pTask, 
															IFXNotificationType_Collision, 
															IFXNotificationId_CollisionDetected, 
															&m_TestTaskHandle )), 
								IFXException);

				TEST(
					IFXSUCCESS(IFXCreateComponent(CID_IFXNotificationInfo, IID_IFXNotificationInfo, (void**) &pInfo)), 
					IFXException);

				TEST(IFXSUCCESS(pInfo->Initialize(pCoreServices)), IFXException);
					
				TEST(IFXSUCCESS(pInfo->SetType(IFXNotificationType_Collision)), IFXException);
				TEST(IFXSUCCESS(pInfo->SetId(IFXNotificationId_CollisionDetected)), IFXException);

				U32 n = 666;
				TEST(IFXSUCCESS(pInfo->SetNotificationData( &n )), IFXException);
				TEST(IFXSUCCESS(pNotificationManager->SubmitEvent(pInfo)), IFXException);

				TEST(IFXSUCCESS(pScheduler->Service()), IFXException);
				TEST(IFXSUCCESS(pNotificationManager->UnregisterTask(m_TestTaskHandle)), IFXException);
				
				IFXRELEASE( pInfo );
				IFXRELEASE( pTask );
				IFXRELEASE( pNotificationManager );
			}
		}
	}
		
	if( IFXSUCCESS( result ) )
		result = IFXCOMUninitialize();

	return result;
}
