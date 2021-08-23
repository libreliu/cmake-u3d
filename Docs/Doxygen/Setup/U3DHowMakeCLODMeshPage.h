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
	@file	U3DHowMakeCLODMeshPage.h

			This header is used solely to contain the doxygen commands for 
			the How To Make A CLOD Mesh U3D documentation page.
*/


#ifndef U3DHowMakeCLODMeshPage_H
#define U3DHowMakeCLODMeshPage_H


//***************************************************************************
//	Doxygen pages
//***************************************************************************


//---------------------------------------------------------------------------
/**
	@page	PageHowMakeCLODMesh	How To Make A CLOD Mesh (Using AuthorGeomCompiler)

	This page describes the process of make a CLOD mesh and how to use the 
	AuthorGeomCompiler component.

	<hr noshade>
	@section SectionAuthorGeomCompiler	About AuthorGeomCompiler
	What is it?
	-	Utility class used to create an author CLOD resource from an author 
		mesh

	Why use it?
	-	Makes creating an author CLOD resource easier
	-	Encapsulates steps like mesh scrubbing and using 
		@c CID_IFXAuthorCLODGen

	Where is it?
	-	In @ref SectionExporting "IFXExporting DL" and associated project
	-	Source code exists physically in 
		@c Source\\RTL\\Component\\Exporting directory

	Component
	-	CID - @c CID_IFXAuthorGeomCompiler
	-	Class - @c CIFXAuthorGeomCompiler

	Interface
	-	IID - @c IID_IFXAuthorGeomCompiler
	-	Class - @c IFXAuthorGeomCompiler

	<hr noshade>
	@section SectionCreatingAuthorCLODResource	Creating an AuthorCLODResource
	Here is a summary of the steps that will follow which describe how to 
	create an AuthorCLODResource:
	-#	Setup an author mesh
	-#	Setup compiler parameters
	-#	Compile
	-#	Put the author CLOD resource in palette
	-#	Optional: Add skeleton, shading modifier, ...

	<hr noshade>
	@section SectionStep1	1.  Setup an author mesh
	-#	Create author mesh component (@c CID_IFXAuthorMesh) and get its 
		@c IFXAuthorMesh interface
	-#	Call @c IFXAuthorMesh::Allocate to allocate space for maximum 
		resolution mesh
	-#	Populate arrays for faces, positions, normals, texture coordinates, 
		etc.
		-	Operate on whole array or individual element

	@image html "DiagramMakeCLODMeshStep1.png"

	<hr noshade>
	@section SectionStep2	2.  Setup compiler parameters
	All parameters are either Booleans or values, plus one callback function 
	pointer.  They are all contained by the @c IFXAuthorGeomCompilerParams 
	structure.

	-#	Scrub params (@c IFXAuthorMeshScrubParams)
		-	Remove unused or invalid
		-	Zero area faces
		-	Normalize normals
	-#	CLOD params (@c IFXAuthorCLODGenParam)
		-	Merge unconnected
		-	Normals mode (none, no updates, parent update, track surface)
	-#	Compress params (@c IFXAuthorCLODCompressionParams)
		-	Quality factors
		-	Minimum resolution
		-	Normals parameters (crease angle, exclude ...)
		-	Streaming priority
	-#	Progress callback function param (@c IFXProgressCallback)

	@image html "DiagramMakeCLODMeshStep2.png"

	<hr noshade>
	@section SectionStep3	3.  Compile
	-#	Create component @c CID_IFXAuthorGeomCompiler and get its 
		@c IFXAuthorGeomCompiler interface
	-#	Prepare component by specifying the Scene Graph with the 
		@c IFXAuthorGeomCompiler::SetSceneGraph function
	-#	Call @c IFXAuthorGeomCompiler::Compile
		-#	Input -	Name of resource
		-#	Input -	Author mesh
		-#	Output - Author CLOD resource
		-#	Input - Encode blocks now flag (can be omitted but it is not 
			recommended)
		-#	Input - Compiler parameters @c IFXAuthorGeomCompilerParams 
			structure (can be omitted but it is not recommended)

	@image html "DiagramMakeCLODMeshStep3.png"

	<hr noshade>
	@section SectionStep4	4.  Put the author CLOD resource in palette
	-#	Obtain model @c GENERATOR palette from Scene Graph with 
		@c IFXSceneGraph::GetPalette
	-#	Use @c IFXPalette::Add function to add a new palette entry to hold 
		the author CLOD resource
		-	Use the exact same name for the resource as during the compile 
			step!
	-#	Store resource in palette entry with the 
		@c IFXPalette::SetResourcePtr function
		-	Must use author CLOD resource’s @c IFXUnknown interface pointer 
			to store it in a palette entry

	@image html "DiagramMakeCLODMeshStep4.png"

	<hr noshade>
	@section SectionRelatedInformation	Related Information
	-	@ref PageUsage "Usage"
	-	@ref PageHowLoadAndWriteU3D "How To Load & Write U3D"
	-	@ref SectionExampleApplications "Example Applications"
	-	@ref PageCodingConventionsIFXCOM "IFXCOM Coding Conventions"
*/


#endif


