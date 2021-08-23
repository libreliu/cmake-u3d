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
@file  CIFXCLODModifier2.h

  The header file that defines the base implementation class of the
  CIFXCLODModifier2.
*/

#ifndef __CIFXCLODModifier2_H__
#define __CIFXCLODModifier2_H__

#include "IFXCLODModifier.h"
#include "CIFXModifier.h"
#include "IFXCLODManagerInterface.h"
#include "IFXNeighborResControllerIntfc.h"
#include "IFXRenderable.h"
#include "IFXCoreCIDs.h"

class CIFXCLODModifier2 :  private CIFXModifier,
                 virtual public   IFXCLODModifier
{
            CIFXCLODModifier2();
  virtual  ~CIFXCLODModifier2();
  friend
  IFXRESULT IFXAPI_CALLTYPE CIFXCLODModifier2_Factory( IFXREFIID iid, void** ppv );

public:
  // IFXUnknown
  U32 IFXAPI        AddRef ();
  U32 IFXAPI        Release ();
  IFXRESULT IFXAPI  QueryInterface (   IFXREFIID  riid,
                              void**     ppv );
  // IFXMarker
  void IFXAPI     GetEncoderX (IFXEncoderX*& rpEncoderX);

  // IFXModifier
  IFXRESULT IFXAPI  GetOutputs ( IFXGUID**& rpOutOutputs,
                        U32&       rOutNumberOfOutputs,
              U32*&    rpOutOutputDepAttrs );
  IFXRESULT IFXAPI  GetDependencies (    IFXGUID*   pInOutputDID,
                                IFXGUID**& rppOutInputDependencies,
                                  U32&       rOutNumberInputDependencies,
                                  IFXGUID**& rppOutOutputDependencies,
                                U32&       rOutNumberOfOutputDependencies,
                  U32*&    rpOutOutputDepAttrs );

  IFXRESULT IFXAPI  GenerateOutput ( U32    inOutputDataElementIndex,
                            void*& rpOutData, BOOL& rNeedRelease );

  IFXRESULT IFXAPI  SetDataPacket ( IFXModifierDataPacket* pInInputDataPacket,
               IFXModifierDataPacket* pInDataPacket );
  IFXRESULT IFXAPI  Notify ( IFXModifierMessage eInMessage,
                    void*               pMessageContext );

  // IFXCLODModifier
  virtual IFXRESULT IFXAPI  GetCLODScreenSpaceControllerState(BOOL* pbOutOn);
  virtual IFXRESULT IFXAPI  SetCLODScreenSpaceControllerState(BOOL bInOn);
  virtual IFXRESULT IFXAPI  GetLODBias(F32* pfOutCLODScreenSpaceBias);
  virtual IFXRESULT IFXAPI  SetLODBias(F32 fInCLODScreenSpaceBias);
  virtual IFXRESULT IFXAPI  GetCLODLevel(F32* pfOutCLODLevel);
  virtual IFXRESULT IFXAPI  SetCLODLevel(F32 fInCLODLevel);

private:
  // IFXUnknown
  U32                     m_uRefCount;

  // IFXModifier
  U32 m_uMeshGroupIndex;
  U32 m_uMeshGroupChangeCount;

  U32 m_uFrustrumIndex;
  U32 m_uViewSizeIndex;
  U32 m_uTransformIndex;
  U32 m_uMeshGroupBoundsIndex;

  U32 m_uCLODManagerItfcIndex;
  U32 m_uCLODManagerItfcCount;

  // IFXCLODModifier
  BOOL m_bCLODScreenSpaceMetricOn;
  F32  m_fCLODScreenSpaceBias;
  F32  m_fCLODLevel;

  IFXMeshGroup* m_pMeshGroup;
  IFXCLODManager* m_pCLODManager;

  IFXRESULT ChangeMeshResolution();
};

#endif
