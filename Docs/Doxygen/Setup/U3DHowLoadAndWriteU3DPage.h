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
	@file	U3DHowLoadAndWriteU3DPage.h

			This header is used solely to contain the doxygen commands for 
			the How To Load And Write U3D documentation page.
*/


#ifndef U3DHowLoadAndWriteU3DPage_H
#define U3DHowLoadAndWriteU3DPage_H


//***************************************************************************
//	Doxygen pages
//***************************************************************************


//---------------------------------------------------------------------------
/**
	@page	PageHowLoadAndWriteU3D	How To Load & Write U3D

	This page describes some common procedures for how to load and write U3D 
	files.

	<hr noshade>
	@section SectionLoadingAFile	Loading a File
	This section describes the process of loading a file.  Here is a brief 
	overview and diagram:
	-#	Create and setup a Read Buffer component
	-#	Create and setup a Load Manager component
	-#	Call @c pLoadManager->Load()

	@image html "DiagramLoadFile.png"

	@subsection SectionLoadingStep1	Loading:  ReadBuffer
	1.  Create and setup a Read Buffer component
	-	@c IFXReadBufferX interface abstracts underlying file system
	-	Available read buffer objects
		-	Stdio Read Buffer wraps @c fread, @c fopen 
			(@c CID_IFXStdioReadBufferX, implemented by @c CIFXStdioReadBufferX)
		-	Internet Read Buffer wraps Internet usage 
			(@c CID_IFXInternetReadBuffer, implemented by @c CIFXInternetReadBufferX)
		-	@c CIFXDataBlockX wraps @c memcpy (@c CID_IFXDataBlockX)

	@subsection SectionLoadingStep2	Loading:  Load Manager
	2.  Create and setup a Load Manager component
	-	Main interface is @c IFXLoadManager
		(@c CID_IFXLoadManager, implemented by @c CIFXLoadManager)
	-	Load Manager initialized with reference to Core Services interface 
		IFXCoreServices
		(@c CID_IFXCoreServices, implemented by @c CIFXCoreServices)
		-	Core Services provides access to common services such as the 
			scene graph, palettes, scheduler, name map, ...

	@subsection SectionLoadingStep3	Loading:  Call Load
	3.  Call the Load Manager's Load function
	-	@c IFXLoadManager::Load parameters
		-	@c pReadBuffer – Interface pointer to buffer to load from
		-	@c atomicLoad - Specify if load should be atomic or non-atomic 
			and sectioned
			-	For atomic loading, Load Manager will attempt to load the 
				entire buffer before returning (if possible, depends upon 
				Read Buffer type)
			-	For non-atomic loading, Load Manager will schedule the load 
				with the Scheduler (if available) and return
		-	@c scope - Specify scope identification string used for name 
			collision resolution during the load depending upon the policy 
			specified
		-	@c collisionPolicy - Specify the object name collision 
			resolution policy of type IFXCollisionPolicy:
			-	Replace old with new
			-	Keep old and discard new
			-	Prepend scope string (always or on collision only)
			-	Append sequence number on collision

	@subsection SectionLoadingSample	Sample:  Load Manager
	Here is a simple code example with no error checking:

@code
// Create Load Manager and get main interface
IFXLoadManager *pLM = NULL;
IFXCreateComponent( CID_IFXLoadManager, IID_IFXLoadManager, ( void** ) &pLM ); 

// Initialize Load Manager with Core Services
pLM->Initialize( pCoreServices );

// Create Read Buffer and get buffer interface
IFXReadBufferX *pRB = NULL;
IFXCreateComponent( CID_IFXStdioReadBufferX, IID_IFXReadBufferX, ( void** ) &pRB );

// Get IFXStdio interface
IFXStdio *pStdio = NULL;
pRB->QueryInterface( IID_IFXStdio, ( void** ) &pStdio );

 // Open file
pStdio->Open( pFileName );

// Load file
pLM->Load( 
		pRB,		// Load information from this Read Buffer
		loadAtomic, // Attempt to load all at once?
		"myScope",	// Scope used for names collision resolution
		IFXCOLLISIONPOLICY_PREPENDALL ); // Policy used for handling object names	@endcode

	<hr noshade>
	@section SectionWritingAFile	Writing a File
	This section describes the process of writing a file.  Here is a brief 
	overview and diagram:
	-#	Create the scene
	-#	Mark objects for writing
	-#	Create and setup Write Manager
	-#	Create and setup Write Buffer object
	-#	Call @c pWriteManager->Write()

	@image html "DiagramWriteFile.png"

	@subsection SectionWritingStep1	Writing:  Create Scene
	1.  Create the scene
	-	By creating objects and adding them to the palettes
	-	By loading other files

	@subsection SectionWritingStep2	Writing:  Mark
	2.  Mark objects for writing
	-	Marking is done using the @c IFXMarkerX interface
	-	Marking is recursive and all objects used by that object are also marked
	-	Also use marking interface to
		-	Set quality factors (quantization levels)
			-	Quality factors can also be set using Write Manager
		-	Set block priority
			-	Priority setting can be recursive or not

	Marked objects are depicted in the diagram with an asterick (*).

	@subsection SectionWritingStep3	Writing:  Write Manager
	3.  Create and setup Write Manager
	-	Main interface is @c IFXWriteManager
		(@c CID_IFXWriteManager, implemented by @c CIFXWriteManager)
	-	Write Manager initialized with Core Services
		(@c CID_IFXCoreServices, implemented by @c CIFXCoreServices)
		-	Similar to Load Manager for the same reasons

	@subsection SectionWritingStep4	Writing:  Write Buffer
	4.  Create and setup Write Buffer object
	-	@c IFXWriteBufferX interface abstracts underlying file system
	-	Available write buffer objects
		-	Stdio Write Buffer wraps @c fwrite, @c fopen
			(@c CID_IFXStdioWriteBufferX, implemented by @c CIFXStdioWriteBufferX)
		-	@c CIFXDataBlockX wraps @c memcpy (@c CID_IFXDataBlockX)

	@subsection SectionWritingStep5	Writing:  Write
	5.  Call @c pWriteManager->Write()
	-	@c IFXWriteManager::Write parameters
		-	@c pWriteBuffer – Interface pointer to buffer to write to
		-	@c exportOptions – Specify which palettes should be written
	-	Only objects that have been marked and are in palettes selected by 
		@c exportOptions are written to the write buffer

	@subsection SectionWritingSample	Sample:  Write Manager
	Here is a simple code example with no error checking:

@code
// Create Write Manager and get main interface
IFXWriteManager *pWM = NULL;
IFXCreateComponent( CID_IFXWriteManager, IID_IFXWriteManager, ( void** ) &pWM );

// Initialize Write Manager with Core Services
pWM->Initialize( pCoreServices );

// Create Write Buffer and get buffer interface
IFXWriteBufferX *pWB = NULL;
IFXCreateComponent( CID_IFXStdioWriteBufferX, IID_IFXWriteBufferX, ( void** ) &pWB );

// Get IFXStdio interface
IFXStdio *pStdio = NULL;
pWB->QueryInterface( IID_IFXStdio, ( void** ) &pStdio );

 // Open file
pStdio->Open( pFileName );

// Mark all Scene Graph resources
pSceneGraph->Mark();

// Write the file
pWM->Write( pWB, IFXEXPORT_EVERYTHING );

// Close buffer
pStdio->Close();	@endcode

	<hr noshade>
	@section SectionRelatedInformation	Related Information
	-	@ref PageUsage "Usage"
	-	@ref PageHowMakeCLODMesh "How To Make A CLOD Mesh"
	-	@ref SectionExampleApplications "Example Applications"
	-	@ref PageCodingConventionsIFXCOM "IFXCOM Coding Conventions"
*/


#endif


