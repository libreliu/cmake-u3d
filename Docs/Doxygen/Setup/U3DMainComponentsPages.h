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
	@file	U3DMainComponentsPages.h

			This header is used solely to contain the doxygen commands for 
			the various Main Components documentation pages.
*/


#ifndef U3DMainComponentsPages_H
#define U3DMainComponentsPages_H


//***************************************************************************
//	Doxygen pages
//***************************************************************************


//---------------------------------------------------------------------------
/**
	@page	PageMainComponents	Main Components

	This page contains high-level descriptions of many of the main 
	components.  Here is a list of those that will be covered:
	-	@ref SectionCoreServices "Core Services" (@ref SectionCore "IFXCore DL")
	-	@ref SectionPalette "Palette" (@ref SectionCore "IFXCore DL")
	-	@ref SectionSceneGraph "Scene Graph" (@ref SectionCore "IFXCore DL")
	-	@ref SectionNode "Nodes" (@ref SectionCore "IFXCore DL")
	-	@ref SectionModifiers "Modifiers" (@ref SectionCore "IFXCore DL")
	-	@ref SectionModifierChain "Modifier Chain" (@ref SectionCore "IFXCore DL")
	-	@ref SectionLoadManager "Load Manager" (@ref SectionImporting "IFXImporting DL")
	-	@ref SectionWriteManager "Write Manager" (@ref SectionExporting "IFXExporting DL")
	-	@ref SectionScheduler "Scheduler" (@ref SectionScheduling "IFXScheduling DL")
	-	@ref SectionNotificationManager "Notification Manager" (@ref SectionScheduling "IFXScheduling DL")
	-	@ref SectionRenderServices "Render Services" (@ref SectionRendering "IFXRendering DL")

	This discussion does not focus on the shading and rendering aspects.

	<hr noshade>
	@section SectionKernel	Kernel
	Before diving into these various components, it should be mentioned that 
	the Kernel in the @ref SectionCore "IFXCore DL" contains a number of 
	useful classes for working with 3D data and a component framework to 
	drive the system.
	-	Data types
		-	Simple ones (@c U8, @c U32, @c F32, ...)
		-	@c IFXVector2, @c IFXVector3, @c IFXVector4
		-	@c IFXMatrix4x4
		-	@c IFXQuaternion
		-	@c IFXString
		-	@c IFXException
		-	@c IFXRESULT
	-	Component framework
		-	IFXCOM

	<hr noshade>
	@section SectionIFXCOM	IFXCOM Component Object Model
	-	Interface-oriented component programming paradigm
		-	Light overhead, portable and C++ based
		-	Made similar to Microsoft*’s COM, but 
			-	No registry
			-	IFXCOM Plug-in DLs are discovered at run-time and must be 
				located anywhere in the @c Plugins subdirectory tree located 
				where IFXCore DL resides
			-	Supports component versioning
	-	Good starting point @c IFXCOM.h, @c IFXUnknown.h
		-	Starts with @c IFXCOMInitialize
		-	Stops with @c IFXCOMUninitialize
			-	Returns a warning if not all components instantiated have 
				been released
			-	Cannot unload some DLs as a result and resources are leaked
		-	Create with @c IFXCreateComponent
			-	Creates both singleton & non-singleton components
		-	Clients statically link to @c IFXCoreStatic.lib wrapper which 
			dynamically binds to IFXCore DL
			-	@ref SectionStaticLibraries "Static LIBs" are provided for 
				IFXCore DL and some other plug-in DLs that each contain 
				compiled C++ classes used by various interfaces and 
				components
	-	Result codes are also similar @c IFXResult.h

	<hr noshade>
	@section SectionCoreServices	Core Services
	-	Highest-level component and subsystem container
		-	Scene Graph, Scheduler and Notification Manager
	-	Represents single instance of the U3D run-time
	-	U3D profile is specified and maintained here
	-	Also general meta-data, palette entry name maps, File Reference 
		palette and base URL
	-	@c CID_IFXCoreServices : @c IFXCoreServices, @c IFXMetaDataX, 
		@c IFXUnknown

	@subsection	SectionDiagramCoreServices Component Diagram

	@image html "DiagramComponentCoreServices.png"

	@note	There is a special forwarding version of Core Services existing 
			for preventing many reference count cycles.  Generally, 
			knowledge of this is not necessary, however if there is a need 
			to avoid a reference count cycle, use the interface handed back 
			from the @c IFXCoreServices::GetWeakInterface function as usual 
			instead of the regular @c IFXCoreServices interface pointer 
			handed back from the Core Services' @c QueryInterface function.

	<hr noshade>
	@section SectionPalette	Palette
	-	Resource database for storing components
		-	An ordered list of uniquely named entries + default
		-	Entry points to a common interface or NULL
	-	Facilitates resource sharing
	-	Components keep entry ids for their resources
		-	Advantages of not maintaining direct pointer references
			-	Don't need to refresh multiple component pointers when 
				changes are made
			-	Helps with file loading
			-	Allows entry to not be present
	-	Supports adding, deleting, setting, finding and iterating
		-	Implementation allows entries to be quickly referenced via name 
			hash map or id/index
		-	This is beyond what is required or described by the specification
	-	@c CID_IFXPalette : @c IFXPalette, @c IFXSubject, 
		@c IID_IFXUnknown

	@subsection	SectionDiagramPaletteExample	Palette Example

	@image html "DiagramComponentPaletteExample.png"

	<hr noshade>
	@section SectionSceneGraph	Scene Graph
	-	Container for most resource palettes
		-	Nodes (all types), Lights, Models (generators), Views, Shaders, 
			Materials, Textures, Motions
		-	Couple old ones exist that will be removed
	-	Also keeps some scene specific state
		-	Visual debug flags and animation enabled flag
	-	@c CID_IFXSceneGraph : @c IFXSceneGraph, @c IFXUnknown

	<hr noshade>
	@section SectionNode	Node
	-	Is a modifier that
		-	Has spatial information
			-	Local parent relative transform
			-	World transform
			-	World bounding sphere
		-	Hierarchical relationships to other nodes
			-	Allows multiple parents
	-	Light-weight through resource sharing
		-	@c CID_IFXLight -> @c CID_IFXLightResource
		-	@c CID_IFXModel -> @c CID_IFXAuthorCLODResource, 
			@c CID_IFXAuthorLineSetResource or 
			@c CID_IFXAuthorPointSetResource
		-	@c CID_IFXView	-> @c CID_IFXViewResource
		-	@c CID_IFXGroup
	-	Supports subgraph model picking
	-	@c IFXNode, ...

	@subsection	SectionDiagramNodeHierarchyExample	Node Hierarchy Example

	@image html "DiagramComponentNodeHierarchyExample.png"

	<hr noshade>
	@section SectionModelNode	Model Node
	-	Spatially instances geometry defined in the Model Palette
		-	Meshes -> @c CID_IFXAuthorCLODResource
			-	@c IFXModel -> @c IFXAuthorCLODResource -> 
				@c IFXAuthorCLODMesh
		-	Lines -> @c CID_IFXAuthorLineSetResource
			-	@c IFXModel -> @c IFXAuthorLineSetResource -> @c IFXAuthorLineSet
		-	Points -> @c CID_IFXAuthorPointSetResource
			-	@c IFXModel -> @c IFXAuthorPointSetResource -> @c IFXAuthorPointSet
	-	These Model Palette resources “generate” or convert geometry 
		formatted for easy authoring into one more optimized for rendering
		-	Another implementation could start with renderable format
	-	Author-formatted geometry stored in referenced helper components
		-	@c CID_IFXAuthorCLODMesh
		-	@c CID_IFXAuthorLineSet
		-	@c CID_IFXAuthorPointSet
	-	Model node starts @e model @e instance @e modifier @e chain
		-	Establishes world transform by applying model transform to 
			transform data element
		-	Chain is appended to @e model @e resource @e modifier @e chain 
			once model generator resource association is established
		-	Start of @e model @e resource @e modifier @e chain is generator 
			stored in Model (Generator) Palette
	-	@c CID_IFXModel : @c IFXModel, @c IFXNode, ...

	<hr noshade>
	@section SectionViewNode	View Node
	-	Spatially instances a view (camera) defined in the View Palette
		-	@c CID_IFXViewResource
	-	Supports definition of presentation window specific information
		-	Near/far clip planes, perspective/orthographic, field of view, 
			window viewport, scaling, and pixel aspect ratio
	-	View resource defines
		-	Root node whose subgraph is potentially visible
		-	Fog and multipass render 
			usage/configuration
	-	Additional View Node functionality
		-	Layering with backdrops and overlays
		-	2D screen space model picking from root node’s subgraph
		-	Culling subsystem used and rendering (if 
			@ref SectionRendering "IFXRendering DL" is available)
	-	@c CID_IFXView : @c IFXView, @c IFXNode, ...

	<hr noshade>
	@section SectionModifiers	Modifiers
	-	Modifiers process some input information and produce output 
		information
		-	Modifier Data Packets (DPs) hold that information (interface is @c IFXDataPacket)
		-	Contain multiple Modifier Data Elements (DEs) identified by 
			Modifier Data Element Ids (DIDs) (defined in @c IFXDids.h)
		-	Input Modifier Data Packet is read-only
	-	Identify what DEs are output and for each DE output what input DEs 
		it depends upon
	-	Interface supported by all modifiers
		-	@c IFXModifier, @c IFXMarker, @c IFXMetaDataX, @c IFXUnknown

	@subsection SectionGeneralModifiers	General Modifiers
	Are not used at the start of a modifier chain
	-	Animation -> @c CID_IFXAnimationModifier : @c IFXAnimationModifier
		-	Performs node-based and bones-based key frame animation 
			(currently only the complete transform may be animated)
	-	Bone Weights -> @c CID_IFXBoneWeightsModifier : 
		@c IFXBoneWeightsModifier
		-	Bones modifier changes positions and normals based on bones 
			vertex weights
	-	CLOD -> @c CID_IFXCLODModifier : @c IFXCLODModifier
		-	Manually or automatically configures the CLOD resolution of 
			CLOD mesh
	-	Glyph 2D -> @c CID_IFXGlyph2DModifier : IFXGlyph2DModifier
		-	Generates a renderable mesh from input glyph provided to it
	-	Shading -> @c CID_IFXShadingModifier : @c IFXShadingModifier
		-	Shading modifier changes shading properties of renderable group
	-	Subdivision -> @c CID_IFXSubdivModifier : @c IFXSubdivModifier
		-	Subdivision modifier depends upon neighbor mesh data element
		-	Neighbor mesh DE originates from author CLOD resource generator 
			if one was created

	@subsection SectionOtherModifiers	Other Styles of Modifiers
	-	@ref SectionNode "Nodes (light, model, view and group)"
	-	Model Resources (Generators)
	-	Textures
		-	Available in 2st Edition ECMA-363
		-	Only as extension plug-ins

	<hr noshade>
	@section SectionModifierChain	Modifier Chain
	-	Coordinates the input requirements and dependencies between modifiers
		-	Ensures modifier have a valid chain relationship
	-	Directly executes modifiers based on output requests
		-	Propagates unused input data elements and ones not output
		-	Allows modifiers further in the chain to use that data
		-	Lazy output evaluation helps eliminate redundant computation
		-	For efficiency, output data packets and data elements are cached
	-	Created when components supporting modifier interface are stored in 
		a palette
	-	Accessible from @c IFXModifier interface
	-	@c CID_IFXModifierChain : @c IFXModifierChain, @c IFXObserver, 
		@c IFXSubject, @c IFXUnknown

	<hr noshade>
	@section SectionLoadManager	Load Manager
	-	Loads U3D formatted data and establishes its run-time component 
		representation
		-	Can embed relative and absolute file references
		-	Data can be loaded from memory, local files or remote files 
			identified via URLs
	-	Synchronous or multiple asynchronous load operations
		-	Each instance represents a single root load
	-	Progress and state monitoring
		-	Header loading, header loaded, media loading and media loaded
		-	Header loaded means all declaration block are loaded and fully 
			decoded including those in referenced files
		-	Media loaded means all blocks are loaded and fully decoded 
			including those in referenced files
	-	Load cancellation
	-	@c CID_IFXLoadManager : @c IFXLoadManager, @c IFXUnknown

	<hr noshade>
	@section SectionWriteManager	Write Manager
	-	Writes the marked U3D run-time components to a U3D formatted data 
		store
		-	Can embed relative and absolute file references
	-	Control provided over exactly what run-time state aspects are saved
	-	Quality factors configurable for all run-time state
	-	Interleaves continuation data blocks for progressive transmission 
		and decoding
	-	@c CID_IFXWriteManager : @c IFXWriteManager, @c IFXUnknown

	<hr noshade>
	@section SectionScheduler	Scheduler
	-	Provides system and simulation services useful for progressively 
		loading U3D data and for controlling simulation time to produce 
		animation
		-	System Manager
		-	Simulation Manager
		-	Time Manager
	-	Basic usage for asynchronous loading and animation
		-	Set simulation and clock behavior with 
			@c IFXScheduler::SetSimulationState
			-	Recommend setting state to @c FALSE and auto reset to @c TRUE
			-	Simulation clock will begin upon first call to service the 
				Scheduler
			-	Avoids missing parts of an animation sequence when start-up 
				activities are being performed
		-	Service the Scheduler once per frame before rendering
	-	Additional usage involves creating and registering tasks to perform 
		work activities
		-	Scheduler uses task paradigm internally for its subsystems
		-	Collision detection and resolution solutions would be registered 
			as simulation tasks
	-	@c CID_IFXScheduler : @c IFXScheduler, @c IFXUnknown

	<hr noshade>
	@section SectionNotificationManager	Notification Manager
	-	Provides way for system-wide and limited-use notifications to be 
		managed and easily broadcast
		-	Submitter has no reference to interested clients
	-	General or specific notification events possible
	-	Flexible registration of task handlers
		-	May handle only specific types of notifications or all of them
	-	@c CID_IFXNotificationManager : @c IFXNotificationManager, 
		@c IFXUnknown

	<hr noshade>
	@section SectionRenderServices	Render Services
	-	Singleton that tracks registered renderer types, allocated Render 
		Devices and the associated rendering state
		-	Each Render Device instance represents encapsulation of a single 
			video card
		-	Only Render Style available currently is traditional local 
			illumination, fixed-function rendering pipeline
	-	Renderers are a render layer abstraction
		-	Can query to determine if one is supported and if its usage is 
			recommended
		-	Renderers provided for OpenGL*, DirectX* 7 and DirectX* 8
		-	NULL renderer is an empty renderer, can be used as template
		-	Software renderer not currently provided
	-	Capability and configuration querying
	-	Anti-aliasing and multiple monitor control
	-	@c CID_IFXRenderServices : @c IFXRenderServices, @c IFXUnknown

	<hr noshade>
	@section SectionRelatedInformation	Related Information
	-	@ref PageCodingConventionsIFXCOM "IFXCOM Coding Conventions".
	-	@ref PageUsage "Usage"
*/

//---------------------------------------------------------------------------
/**
	@page	PageDebugOutput	Debug Output

	On Win32, full output debug messages can be enabled that display within 
	the Microsoft* Visual Studio* .NET* 2003 IDE output debug window by 
	importing the registry file called @c IFX.reg in the 
	@c Source\\RTL\\Platform\\Win32\\Common directory.

	Debug versions of the U3D software check the 
	@c HKEY_LOCAL_MACHINE\\SOFTWARE\\3DIF\\IFX registry key's DWORD value 
	named @c DebugValue.  The following values are recognized:
	- 	@c 0 - No debug messages are displayed.  Default.
	-	@c 1 - Error debug messages displayed.
	-	@c 2 - Warning debug messages displayed plus previous ones.
	-	@c 3 - Message debug messages displayed plus previous ones.

	Refer to the @c IFXOSDebug.h header file for additional details about 
	the debug system.

	<hr noshade>
	@section SectionRelatedInformation	Related Information
	- @ref PageUsage "Usage"
*/

//---------------------------------------------------------------------------
/**
	@page	PagePerformance	Performance Optimizations

	The U3D Sample Software has a variety of performance optimizations.  While 
	the architecture provides a crucial high-level form of optimization, there 
	are also lower-level optimizations included for IA-32 based platforms.  In 
	addition, if the 
	<a href="http://www.intel.com/software/products/ipp/index.htm">Intel&reg; 
	Integrated Performance Primitives (IPP)</a> DLs 
	(http://www.intel.com/software/products/ipp/index.htm) are installed and 
	present in the U3D DLs @c PATH there will be an additional automatic 
	run-time performance improvement for matrix functionality, texture image 
	reformatting & resizing, and basic memory management.

	<hr noshade>
	@section SectionRelatedInformation	Related Information
	- @ref PageUsage "Usage"
*/


#endif


