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

//------------- INCLUDES ----------------------------------------------------

#include "Exporter.h"
#include "ObjectExporter.h"

//------------- DEFINES -----------------------------------------------------

//------------- CONSTANTS ---------------------------------------------------

//------------- ENUMERATIONS ------------------------------------------------

//------------- CLASSES, STRUCTURES AND TYPES -------------------------------

//------------- LOCAL FUNCTION PROTOTYPES -----------------------------------

//------------- PUBLIC METHODS ----------------------------------------------

// destructor
IFXObjectExporter::~IFXObjectExporter()
{
	m_pNodeExp = NULL;
	m_pState = NULL;
	m_pWriter = NULL;
}

// constructor
IFXObjectExporter::IFXObjectExporter( const IFXNodeExporter* pNodeExp ): m_pNodeExp( pNodeExp )
{
	// define world state for object
	if( m_pNodeExp && m_pNodeExp->getNode() )
	{
		m_pState = m_pNodeExp->getNode()->EvalWorldState(0);
		m_pWriter = m_pNodeExp->getWriter();
	}
	else
	{
		m_pState = NULL;
		m_pWriter = NULL;
	}
}

//------------- PROTECTED METHODS -------------------------------------------

//------------- PRIVATE METHODS ---------------------------------------------

//------------- GLOBAL FUNCTIONS --------------------------------------------

//------------- LOCAL FUNCTIONS ---------------------------------------------