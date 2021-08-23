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
	@file	U3DSoftwareContentsPage.h

			This header is used solely to contain the doxygen commands for 
			the Software Contents documentation page.
*/


#ifndef U3DSoftwareContentsPage_H
#define U3DSoftwareContentsPage_H


//***************************************************************************
//	Doxygen pages
//***************************************************************************


//---------------------------------------------------------------------------
/**
	@page	PageSoftwareContents	Software Contents

	The Universal 3D Sample Software consists of a number of 
	@ref SectionDynamicLibraries "dynamic libraries", 
	@ref SectionStaticLibraries "static libraries" and 
	@ref SectionExampleApplications "example applications" that use those 
	libraries.  Details about these and their usage is provided below and 
	in referenced pages.

	<hr noshade>
	@section SectionDynamicLibraries	Universal 3D Dynamic Libraries
	Here is a list of the Universal 3D dynamic libraries for the Win32* 
	platform:
	-	@b @c IFXCore.dll - U3D Core dynamic library.  This is where it all 
		starts.  It contains the IFXCOM cross-platform component object 
		model and common U3D functionality used by most clients that use 
		the U3D base profile (or also the extensible profile when a future 
		edition of the U3D specification is implemented).
		-	There's a client specific static library called 
			@c IFXCoreStatic.lib for clients to use for interacting with it 
			without having to bind statically to the core.
		-	Generally, for client applications to find the IFXCore DL, it is 
			useful to put the common execution directory @c Source\\U3D in 
			the @c PATH system environment variable.
		-	Detailed IFXCOM information (start-up, shut-down, usage and 
			designing new components) is provided in the 
			@ref PageCodingConventionsIFXCOM "IFXCOM Coding Conventions".
	-	@b @c IFXExporting.dll - U3D Exporting dynamic library that contains 
		IFXCOM plug-ins and services to prepare and encode geometry to U3D.  
		It is used by clients that export U3D data.
		-	It must exist in a directory called @c Plugins that exists in 
			the same directory as @c IFXCore.dll.
	-	@b @c IFXImporting.dll - U3D Importing dynamic library that contains 
		IFXCOM plug-ins and services to decode a U3D data stream.  It is used 
		by clients that import U3D data.
		-	It must exist in a directory called @c Plugins that exists in 
			the same directory as @c IFXCore.dll.
	-	@b @c IFXRendering.dll - Optional U3D Rendering dynamic library that 
		contains IFXCOM plug-ins and 3D rendering services (Direct3D* 7/8 and 
		OpenGL*, possibly software in the future).  Used by clients needing a 
		pre-built solution to render imported or dynamically generated U3D 
		data.
		-	If present, it should exist in a directory called @c Plugins that 
			exists in the same directory as @c IFXCore.dll.
	-	@b @c IFXScheduling.dll - U3D Scheduling dynamic library that 
		contains IFXCOM plug-ins and scheduling services.  Used by clients 
		needing a pre-built solution to manage asynchronous U3D data 
		streaming, its decompression and for controlling simulation time.
		-	If present, it should exist in a directory called @c Plugins that 
			exists in the same directory as @c IFXCore.dll.

	@note	The IFXRendering and IFXScheduling DLs are @e not a part of the 
			U3D specification.

	<hr noshade>
	@section SectionStaticLibraries	Universal 3D Static Libraries
	Here is a list of the Universal 3D static libraries for the Win32* 
	platform:
	-	@b @c IFXCoreStatic.lib - Client specific static library to use for 
		interacting with the IFXCore DL without having to bind statically to 
		it.  Besides including the @c IFXCOMInitialize, @c IFXCreateComponent 
		and @c IFXCOMUnitialize functions, it also includes a variety of the 
		compiled non-component objects/classes referenced by the IFXCOM 
		interfaces associated with the IFXCore DL.
	-	@b @c IFXImportingStatic.lib - Client specific static library to use for 
		interacting with the IFXImporting DL.  IFXCOM plug-in DLs are always 
		dynamically bound to by the IFXCore DL, so this library does no form 
		of binding at all.  It simply includes a variety of the compiled 
		non-component objects/classes referenced by the IFXCOM interfaces 
		associated with the IFXImporting DL.
	-	@b @c IFXCorePluginStatic.lib - Static library for developing additional 
		IFXCOM plug-in DLs.  All IFXCOM plug-in DLs link to this library that 
		contains common needed functionality.

	<hr noshade>
	@section SectionExampleApplications	Example Applications
	Here is a list of the example client applications for the Win32* 
	platform:
	- 	@b @c BaseMeshConverter.exe - Example code that converts mesh in u3d 
		files to alternative representation 
	-	@b @c HelloWorld.exe - Example code in a single module that uses the 
		U3D DLs to create a simple hard-coded scene that is directly exported 
		to a U3D file called @c output.u3d in the current or active directory.
	-	@b @c IDTFConverter.exe - Example code that uses the U3D DLs to convert 
		an intermediate data text formatted file to a U3D file.  The format 
		of IDTF files are defined in a document in the 
		@c Docs\\IntermediateFormat directory.  If not created by default, 
		the project name is @c IDTFConverter.  Example IDTF files to get 
		started with are included in the 
		@c Source\\Tools\\IntermediateFormat\\TestScenes directory.
	-	@b @c IDTFExporter.dle - Example exporter plug-in for Discreet* 3ds 
		max* 7 that exports a scene to the IDTF format.  The IDTF Converter 
		can then be used to create a U3D file.
	-	@b @c SamplePlayer.exe - Example code that uses the U3D DLs to perform 
		basic inspect and examine style viewing.  If not created by default, 
		the project name is @c SamplePlayer.  Example U3D files to use are 
		included in the @c Source\\Samples\\Data directory.  The 
		@c IDTFConverter can also be used to generate new U3D files.
	-	@b @c TrueType.exe - Example code that uses the U3D DLs to create a 
		Glyph 2D modifier by setting up glyphs that represent a string using 
		various TrueType fonts.  At run-time to simplify the code, this 
		example depends upon the @c shading.u3d example U3D file which it 
		currently locates by using the @c U3D_SOURCE environment variable to 
		load it from the @c Source\\Samples\\TrueType directory.
	-	@b @c TextureModifier.dll - Example texture modifier extension plug-in.
		It is not provided in binary form, but can be built from Cygwin* shell 
		and Microsoft Visual Studio .NET 2003*.

	<hr noshade>
	@section SectionRelatedInformation	Related Information
	- @ref PageUsage "Usage"
*/


#endif


