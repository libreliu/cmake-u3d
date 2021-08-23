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

/**
	@file	CIFXTextureModifier.cpp

			This module defines the CIFXTextureModifier component.
*/


//***************************************************************************
//	Includes
//***************************************************************************


#include "CIFXTextureModifier.h"
#include "IFXDids.h"
#include "IFXTextureModifierPluginCIDs.h"

//***************************************************************************
//	Defines
//***************************************************************************


//***************************************************************************
//	Constants
//***************************************************************************


//***************************************************************************
//	Enumerations
//***************************************************************************


//***************************************************************************
//	Classes, structures and types
//***************************************************************************


//***************************************************************************
//	Global data
//***************************************************************************


//***************************************************************************
//	Local data
//***************************************************************************


const IFXGUID* CIFXTextureModifier::m_scpOutputDIDs[] =
{
	&DID_IFXTexture
};

const IFXGUID* CIFXTextureModifier::m_scpInputDIDs[] =
{
	&DID_IFXTexture
};


//***************************************************************************
//	Local function prototypes
//***************************************************************************


//***************************************************************************
//	Public methods
//***************************************************************************


//---------------------------------------------------------------------------
U32 CIFXTextureModifier::AddRef()
{
	return ++m_refCount;
}

//---------------------------------------------------------------------------
U32 CIFXTextureModifier::Release()
{
	if ( !( --m_refCount ) )
	{
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return m_refCount;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXTextureModifier::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXTextureModifier )
			*ppInterface = (IFXTextureModifier*)this;
		else if ( interfaceId == IID_IFXModifier )
			*ppInterface = (IFXModifier*)this;
		else if ( interfaceId == IID_IFXUnknown )
			*ppInterface = (IFXUnknown*)this;
		else if ( interfaceId == IID_IFXMarker )
			*ppInterface = (IFXMarker*)this;
		else if ( interfaceId == IID_IFXMarkerX )
			*ppInterface = (IFXMarkerX*)this;
		else if ( interfaceId == IID_IFXMetaDataX )
			*ppInterface = (IFXMetaDataX*)this;
		else if ( interfaceId == IID_IFXSubject )
			*ppInterface = (IFXSubject*)this;
		else
		{
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( result ) )
			AddRef();
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

//---------------------------------------------------------------------------
void CIFXTextureModifier::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
	CIFXMarker::GetEncoderX(CID_IFXTextureModifierEncoder, rpEncoderX);
}


IFXRESULT CIFXTextureModifier::GetOutputs( IFXGUID**& rpOutOutputs,
										   U32&       rOutNumberOfOutputs,
										   U32*&      rpOutOutputDepAttrs )
{
	rOutNumberOfOutputs = sizeof(m_scpOutputDIDs) / sizeof(IFXGUID*);
	rpOutOutputs = (IFXGUID**)&m_scpOutputDIDs;
	rpOutOutputDepAttrs = NULL;
	return IFX_OK;
}

IFXRESULT CIFXTextureModifier::GetDependencies( IFXGUID*   pInOutputDID,
												IFXGUID**& rppOutInputDependencies,
												U32&       rOutNumberInputDependencies,
												IFXGUID**& rppOutOutputDependencies,
												U32&       rOutNumberOfOutputDependencies,
												U32*&      rpOutOutputDepAttrs )
{
	IFXRESULT result = IFX_OK;

	if (pInOutputDID == &DID_IFXTexture)
	{
		rppOutInputDependencies = (IFXGUID**)&m_scpInputDIDs;
		rOutNumberInputDependencies = sizeof(m_scpInputDIDs) / sizeof(IFXGUID*);
		rppOutOutputDependencies = NULL;
		rOutNumberOfOutputDependencies = 0;
		rpOutOutputDepAttrs = NULL;
	}
	else
	{
		IFXASSERTBOX( 1, "CIFXTextureObject::GetDependencies() called with unknown output!" );
		result = IFX_E_UNDEFINED;
	}

	return result;
}

IFXRESULT CIFXTextureModifier::GenerateOutput( U32 inOutputDataElementIndex, void*& rpOutData, BOOL& rNeedRelease )
{
	IFXRESULT result = IFX_OK;

	if (inOutputDataElementIndex == m_textureDataElementIndex)
	{
		STextureOutputInfo* pImageInfo = NULL;
		m_pInputDataPacket->GetDataElement(m_textureDataElementIndex, (void**)&pImageInfo);
		IFXDeallocate(m_ImageInfo.m_pData);
		m_ImageInfo = *pImageInfo;

		U8* pNewData = m_ImageInfo.m_pData = (U8*)IFXAllocate(pImageInfo->m_height*pImageInfo->m_pitch/4);

		U32 y, x;
		for (y = 0; y < pImageInfo->m_height/2; y++)
		{
			for (x = 0; x < pImageInfo->m_width/2; x++) 
			{
				U8* p1 = &pImageInfo->m_pData[y*2*pImageInfo->m_pitch + x*2*3];
				U8* p2 = &pNewData[y*pImageInfo->m_pitch/2 + x*3];
				U8 q1 = p1[0], q2 = p1[1], q3 = p1[2];
				if ((x&7)==2 && (y&7)==2) q1 = ~q1, q2 = ~q2, q3 = ~q3;
				p2[0] = q1, p2[1] = q2, p2[2] = q3;
			}
		}

		m_ImageInfo.m_height = pImageInfo->m_height / 2;
		m_ImageInfo.m_width = pImageInfo->m_width / 2;
		m_ImageInfo.m_pitch = pImageInfo->m_pitch / 2;
		m_ImageInfo.m_pData = pNewData;

		rpOutData = &m_ImageInfo;
	}

	return result;
}

IFXRESULT CIFXTextureModifier::SetDataPacket( IFXModifierDataPacket* pInInputDataPacket,
											  IFXModifierDataPacket* pInOutputDataPacket )
{
	IFXRESULT result = IFX_OK;

	IFXRELEASE(m_pModifierDataPacket);
	IFXRELEASE(m_pInputDataPacket);

	if (pInOutputDataPacket && pInInputDataPacket)
	{
		pInOutputDataPacket->AddRef();
		m_pModifierDataPacket = pInOutputDataPacket;
		pInInputDataPacket->AddRef();
		m_pInputDataPacket = pInInputDataPacket;
	} 
	else
		result = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(result))
		result = pInInputDataPacket->GetDataElementIndex( DID_IFXTexture, m_textureDataElementIndex );

	return result;
}

IFXRESULT CIFXTextureModifier::Notify( IFXModifierMessage eInMessage, void* pMessageContext )
{
	return IFX_OK;
}

//***************************************************************************
//	Protected methods
//***************************************************************************


//***************************************************************************
//	Private methods
//***************************************************************************


//---------------------------------------------------------------------------
/**
	This is the component constructor.  It doesn't do anything significant.

	@note	Significant state initialization is done by the RobustConstructor
			method if it's available or an interface specific Initialize
			method if one's supported.
*/
CIFXTextureModifier::CIFXTextureModifier() :
	m_refCount( 0 )
{
	memset(&m_ImageInfo, 0, sizeof(m_ImageInfo));
}

//---------------------------------------------------------------------------
/**
	This is the component destructor.  After all references to the component
	are released, it performs any necessary cleanup activities.
*/
CIFXTextureModifier::~CIFXTextureModifier()
{
	IFXDeallocate(m_ImageInfo.m_pData);
}


//***************************************************************************
//	Global functions
//***************************************************************************

//---------------------------------------------------------------------------
/**
	This is the CIFXTextureModifier component factory function.  It's used by
	IFXCreateComponent to create an instance of the component and hand back
	a pointer to a supported interface that's requested.

	@note	This component can be instaniated multiple times.

	@param	interfaceId	Reference to the interface id that's supported by the
						component that will be handed back.
	@param	ppInterface	Pointer to the interface pointer that will be
						initialized upon successful creation of the
						component.

	@return	Upon success, IFX_OK is returned.  Otherwise, one of the
			following failures are returned:  IFX_E_COMPONENT,
			IFX_E_INVALID_POINTER, IFX_E_OUT_OF_MEMORY or
			IFX_E_UNSUPPORTED.
*/
IFXRESULT IFXAPI_CALLTYPE CIFXTextureModifier_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		// Create the CIFXTextureModifier component.
		CIFXTextureModifier	*pComponent	= new CIFXTextureModifier;

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


//***************************************************************************
//	Local functions
//***************************************************************************
