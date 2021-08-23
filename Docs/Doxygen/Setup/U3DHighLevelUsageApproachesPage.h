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
	@file	U3DHighLevelUsageApproachesPage.h

			This header is used solely to contain the doxygen commands for 
			the High-level Usage Approaches documentation page.
*/


#ifndef U3DHighLevelUsageApproachesPage_H
#define U3DHighLevelUsageApproachesPage_H


//***************************************************************************
//	Doxygen pages
//***************************************************************************


//---------------------------------------------------------------------------
/**
	@page	PageHighLevelUsageApproaches	High-level Usage Approaches

	Some common U3D Sample Software usage activities are described below for 
	exporters and viewers/players.

	@section SectionExporterActivities	Exporter Activities
	Exporters will typically perform the following steps:

	-#	Initialize and setup the system
		-	Component system
		-	Core Services
		-	Scene Graph
	-#	Read source CAD scene and model data, and put into the U3D Sample 
		Software's internal run-time format by
		-	Constructing a scene graph
		-	Creating and setting up author-meshes and other geometry 
			components such as line sets, point sets and 2D glyphs
		-	Creating corresponding material and shader definitions
		-	Alternative available to generate intermediate data text format 
			(IDTF) files and invoke 
			@ref SectionExampleApplications "example U3D converter"
	-#	Compile each of the author-meshes to generate Continuous Level of 
		Detail (CLOD) author-meshes
		-	Define what mesh clean-up or scrubbing operations are performed
	-#	Use the Write Manager to output a U3D file based on the current 
		run-time state and the desired configuration
		-	Quality levels for different attributes
		-	Minimum resolution
		-	Streaming priority for various resource
		-	Which resource types to include in the generated U3D file

	Additional details are described in 
	@ref PageHowLoadAndWriteU3D "How To Load & Write U3D" and 
	@ref PageHowMakeCLODMesh "How To Make A CLOD Mesh".  The 
	@ref SectionExampleApplications "example applications" also include a 
	simple exporter in the @e Hello @e World style, an intermediate data 
	text format to U3D @e Converter and a 2D glyph creation demonstration 
	using @e TrueType.

	@section SectionViewerActivities	Viewer/Player Activities
	Viewers/players will typically perform the following steps (a subset of 
	these are also done by importers):

	-#	Initialize and setup the system
		-	Component system
		-	Core Services
		-	Scene Graph
		-	Rendering Device
		-	Renderers
	-#	Use Load Manager to read U3D file(s)
		-	Automatically instantiates palette resources (essentially the 
			scene database) and restores aspects of the run-time environment 
			that were selected in the files for serialization
		-	Multiple simultaneous file loads may be performed, synchronously 
			or asynchronously
			-	May be performed even after rendering has begun
			-	Asynchronous loads require periodic execution of Load 
				Manager tasks
			-	Done by IFXScheduling DL if available using the Simulator’s 
				System Manager, or clients may periodically execute the Load 
				Manager
	-#	Manually perform any desired additional programmatic creation of 
		geometry, scene graph nodes or other types of resources
	-#	Perform scripted or interactive scene modifications and animation by 
		using Scheduling subsystem
		-	Facilitated by Simulation Manager, built-in or custom Simulator 
			Tasks, and Time-based Tasks
		-	Notification Manager is another important ingredient
	-#	Render frames using the Device which is configured to render one or 
		more Scene Graph View Nodes
		-	Performs culling and executes modifier chains as needed
		-	Shaders then consume and draw the renderable elements

	Additional details are described in 
	@ref PageHowLoadAndWriteU3D "How To Load & Write U3D".  The 
	@ref SectionExampleApplications "example applications" also include a 
	basic @e Player and a 2D glyph creation demonstration using @e TrueType.

	<hr noshade>
	@section SectionRelatedInformation	Related Information
	-	@ref PageUsage "Usage"
	-	@ref PageHowLoadAndWriteU3D "How To Load & Write U3D"
	-	@ref PageHowMakeCLODMesh "How To Make A CLOD Mesh"
	-	@ref SectionExampleApplications "Example Applications"
*/


#endif


