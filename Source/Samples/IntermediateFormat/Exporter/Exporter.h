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
	@file	Exporter.h

			This module contains the IFXExporter class.

	@note	As a starting point, portions of this module were based on the 
			Discreet* 3ds max* SDK sample exporters and may contain 
			materials owned by Autodesk*, Inc.  Copyright 1994 - 2004.

			* Other names and brands may be claimed as the property of 
			  others.
*/

#ifndef __EXPORTER_H__
#define __EXPORTER_H__

//------------- INCLUDES ----------------------------------------------------

#include "ShaderExporter.h"
#include "ObjectExporter.h"
#include "max.h"
#include "writer.h"
#include "IFXCOM.h"

//------------- DEFINES -----------------------------------------------------

#define NODETYPE_NUM 5

//------------- CONSTANTS ---------------------------------------------------

//------------- ENUMERATIONS ------------------------------------------------

//------------- CLASS, STRUCTURE AND TYPES ----------------------------------
/**
*  Exporter is used to initialize and start up export process
*/
class IFXExporter
{
public:
	~IFXExporter();
	IFXExporter( const TCHAR* );

	BOOL initialize( ExpInterface* pEi, Interface* pI );
	BOOL finalize();
	IFXRESULT export();

protected:
	IFXExporter() {};
private:
	IFXExporter( const IFXExporter& ) {};
	IFXExporter& operator= ( const IFXExporter& ) {};

	BOOL exportHeaderInfo();
	void exportGlobalInfo();

	BOOL enumerateNode( INode* );
	BOOL enumerateResource( INode*, U32 nodeCnt ) const;
	BOOL exportShadingModifier( INode* ) const;
	BOOL exportAnimationModifier( INode* ) const;
	BOOL exportBoneWeightModifier( INode* ) const;
	BOOL exportMotionResource( INode* );
	void countNodes( INode*, U32& rNodeCnt );
	void startResourceList( U32 nt, U32 nc );
	void endResourceList();

	IFXWriter*         m_pWriter;
	ExpInterface*   m_pEi;
	Interface*      m_pI;
	IFXShaderExporter* m_pShaderExporter;

	U32* m_pNodeCount;
	U32* m_pCurNodes;
	U32 m_trackCount;
	U32 m_curTrack;
}; 


//------------- GLOBAL FUNCTION PROTOTYPES ----------------------------------

//------------- GLOBAL DATA -------------------------------------------------

#endif


