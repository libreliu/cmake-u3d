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
	@file	U3DUsagePages.h

			This header is used solely to contain the doxygen commands for 
			the various Usage documentation pages.
*/


#ifndef U3DUsagePages_H
#define U3DUsagePages_H


//***************************************************************************
//	Doxygen pages
//***************************************************************************


//---------------------------------------------------------------------------
/**
	@page	PageUsage	Usage

	Information about the Universal 3D Sample Software and it's usage is 
	provided here.

	<hr noshade>
	@section SectionPackaging10000	Universal 3D From 10,000'
	This is a 10,000' diagram of the high-level packaging and usage of the 
	U3D dynamic libraries.  Descriptions are in pages referenced below.

	@image html "DiagramPackaging10000.png"

	Link to 
	@ref PagePackagingDiagrams "high-level packaging diagrams for each U3D DL".

	<hr noshade>
	@section SectionAdditionalUsageDocumentation	Additional Usage Documentation
	-	@ref PageSoftwareContents "Software Contents"
		-	@ref SectionDynamicLibraries "Universal 3D Dynamic Libraries"
		-	@ref SectionStaticLibraries "Universal 3D Static Libraries"
		-	@ref SectionExampleApplications "Example Applications"
	-	@ref PageMainComponents "Main Components"
		-	@ref PageCodingConventionsIFXCOM "IFXCOM Coding Conventions".
		-	@ref PageDebugOutput "Debug Output"
		-	@ref PagePerformance "Performance Optimizations"
	-	@ref PageHighLevelUsageApproaches "High-level Usage Approaches"
	-	@ref PageHowLoadAndWriteU3D "How To Load & Write U3D"
	-	@ref PageHowMakeCLODMesh "How To Make A CLOD Mesh"
	-	@ref PageFAQ "Frequently Asked Questions"

	<hr noshade>
	@section SectionSourceDocumentation	Source Documentation
	-	<a class="qindex" href="hierarchy.html">Class Hierarchy</a>
	-	<a class="qindex" href="annotated.html">Annotated Type List</a>
	-	<a class="qindex" href="classes.html">Type Index</a>
	-	<a class="qindex" href="functions.html">Function List</a> 
	-	<a class="qindex" href="globals.html">Global File Member List</a> 
	-	<a class="qindex" href="files.html">File List</a> 
	-	<a class="qindex" href="dirs.html">Directory List</a> 
*/

//---------------------------------------------------------------------------
/**
	@page	PagePackagingDiagrams	Dynamic Library Packaging Diagrams

	This page contains high-level packaging diagrams for the U3D dynamic 
	libraries.  Many key functionalities and components are represented in 
	each of these diagrams, however not all of them are shown.

	<hr noshade>
	@section SectionCore	Core DL
	The U3D Core dynamic library contains the IFXCOM cross-platform 
	component object model and common U3D functionality used by most clients 
	that use the U3D base profile (or also the extensible profile when a 
	future edition of the U3D specification is implemented).

	@image html "DiagramPackagingCore.png"

	<hr noshade>
	@section SectionExporting Exporting DL
	The U3D Exporting dynamic library contains IFXCOM plug-ins and services 
	to prepare and encode geometry to U3D.  It is used by clients that 
	export U3D data.

	@image html "DiagramPackagingExporting.png"

	<hr noshade>
	@section SectionImporting	Importing DL
	The U3D Importing dynamic library contains IFXCOM plug-ins and services 
	to decode a U3D data stream.  It is used by clients that import U3D data.

	@image html "DiagramPackagingImporting.png"

	<hr noshade>
	@section SectionRendering	Rendering DL
	The U3D Rendering dynamic library contains IFXCOM plug-ins and 3D 
	rendering services (Direct3D* 7/8 and OpenGL*, possibly software in the 
	future).  Used by clients needing a pre-built solution to render 
	imported or dynamically generated U3D data.

	@image html "DiagramPackagingRendering.png"

	<hr noshade>
	@section SectionScheduling	Scheduling DL
	The U3D Scheduling dynamic library contains IFXCOM plug-ins and 
	scheduling services.  Used by clients needing a pre-built solution to 
	manage asynchronous U3D data streaming, its decompression and for 
	controlling simulation time.

	@image html "DiagramPackagingScheduling.png"

	<hr noshade>

	@note	The IFXRendering and IFXScheduling DLs are @e not a part of the 
			U3D specification.

	@note	The IFXExtensibility DL is not included here because it is a 
			future dynamic library that does not exist yet.  It is expected 
			after the extensible profile is available in a future edition of 
			ECMA-363.

	<hr noshade>
	@section SectionRelatedInformation	Related Information
	-	@ref PageUsage "Usage"
*/


#endif


