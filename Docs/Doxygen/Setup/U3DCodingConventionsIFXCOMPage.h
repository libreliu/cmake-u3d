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
	@file	U3DCodingConventionsIFXCOMPage.h

			This header is used solely to contain the doxygen commands for 
			the IFXCOM Coding Conventions documentation page.
*/


#ifndef U3DCodingConventionsIFXCOMPage_H
#define U3DCodingConventionsIFXCOMPage_H


//***************************************************************************
//	Doxygen pages
//***************************************************************************


//---------------------------------------------------------------------------
/**
	@page	PageCodingConventionsIFXCOM	IFXCOM Coding Conventions

	This project uses a C++ based component model that is similar to 
	Microsoft* COM.  Both are interface-oriented programming paradigms, 
	however IFXCOM's overhead is light, it is portable and it only supports 
	C++.  This page describes the coding conventions for how to create 
	IFXCOM interfaces and components, as well as how to use them.  It is 
	considered an addendum to U3D's general 
	@ref PageCodingConventions "Coding Conventions".

	<hr noshade>
	@section SectionGoals	Goals
	The goals originally established for IFXCOM are:
	-#	@e Low-space @e and @e runtime @e costs.  No inter-process or 
		inter-thread marshalling or other function invocation overhead.  
		No frequent access to an object registry for object construction.  
		Aggregation is not supported so containment should be used instead.
	-#	@e Easy @e to @e use.  Familiar to programmers who are familiar with 
		Microsoft* COM.  Simple usage model.  Clearly documented rules for 
		reference counting and pointer copying.
	-#	@e Portable.   Straightforward to port to multiple operating systems 
		and platforms.  No need to implement complex subsystems simply to 
		port the code.

	<hr noshade>
	@section SectionDefinitions	Definitions
	This page will use the following definitions:
	-	An @e interface is an abstract collection of function prototypes 
		that advertise a set of functionality but does not provide any 
		implementation of that functionality and does not declare any 
		storage space.  A C++ class without data members that has all of its 
		functions publicly declared as "pure virtual" is an interface.
	-	A @e class is a collection of storage members and function 
		prototypes that provides implementation for at least some of its 
		advertised functions (which are usually part of an interface).
	-	An @e instantiable @e class is a class for which an instance can be 
		created.  There must be an implementation for each of the functions 
		in the class.
	-	A @e component is an instantiable class via a common creation 
		mechanism that supports one or more interfaces.
	-	An @e object is a component.

	<hr noshade>
	@section SectionComponentAndInterfaceDesign	Design of Components and Interfaces
	-#	All subsystems and all objects visible outside their subsystems must 
		be components and use the IFXCOM paradigm.  Objects only used 
		internally within a subsystem may use the IFXCOM paradigm, but are 
		not required to.
	-#	When creating new components, begin with the latest IFXCOM templates 
		located in the @c Source\\Templates folder.  The templates 
		demonstrate the proper way to implement the common boilerplate code 
		for creating components of both the non-singleton and singleton 
		variety.  Templates are also available for interfaces and for 
		creating new IFXCOM plug-in dynamic libraries.
	-#	All components will use IFXCID's for unique identification.  They 
		are simply an IFXGUID typedef that's equivalent to a GUID.  They are 
		commonly defined in a header file specific to the IFXCOM plug-in 
		dynamic library that they are associated with (for example 
		IFXCoreCIDs.h).  By convention they are prefixed with @c CID_.  Use 
		Microsoft*'s @c GUIDGEN.EXE tool to generate them.  Do not use the 
		standard Microsoft* COM and OLE header files.  Type and macro 
		definitions are provided in the header IFXGUID.h that are portable.
	-#	All interfaces will use IFXIID's for unique identification.  They 
		are simply an IFXGUID typedef that is equivalent to a GUID.  They 
		are commonly defined in the interface header file that they are 
		associated with.  By convention they are prefixed with @c IID_.  
		Use Microsoft*'s @c GUIDGEN.EXE tool to generate them.  Do not use 
		the standard Microsoft* COM and OLE header files.  Type and macro 
		definitions are provided in the header IFXGUID.h that are portable.
	-#	All components must support the IFXUnknown interface (minimally).  
		The IFXUnknown interface consists of three functions for lifetime 
		management and obtaining interfaces supported by the component.  It 
		is functionally equivalent to IUnknown.  Note that IFXREFIID is just 
		a reference to an IFXIID.
		@code
class IFXUnknown
{
	public:
		virtual U32 		AddRef() = 0;
		virtual U32 		Release() = 0;
		virtual IFXRESULT	QueryInterface( 
								IFXREFIID	interfaceId, 
								void**		ppInterface ) = 0;
};
		@endcode
	-#	Components may optionally support custom interfaces.  All custom 
		interfaces must derive solely from another interface (multiple 
		inheritance should be avoided, virtual inheritance is also 
		slower and generally is not needed unless implementation inheritance 
		is used in the component implementation).  The top-most interface 
		must always be IFXUnknown.
	-#	Names of custom interfaces must end with the suffix @c X if any of 
		their functions are known to throw exceptions.  This is necessary 
		even if a function calls another function that throws exceptions.  
		However, it is not necessary if such called functionality is wrapped 
		in a try-catch block and the function itself does not directly throw 
		any exceptions.
	-#	C++ references to interfaces and other types are allowed as 
		parameters in interface function prototypes.  Reference-style 
		parameters should be used solely for either input or output.  If 
		used for input, the parameter must be marked as constant in the 
		prototype.  Note:  Using references properly can reduce the need for 
		basic pointer parameter validation checking in function 
		implementations.
	-#	All components will have an IFXCOM compatible factory function 
		defined that is used by @c IFXCreateComponent indirectly for 
		instantiation.  The component class declaration must define the 
		factory function as a friend.  It will hand back a pointer to the 
		requested interface on the created object if it supports it.  Each 
		component's factory function must be registered with the Core DL for 
		this to work properly.  The factory function might look like the 
		following example:
		@code
friend IFXRESULT	CIFXMyComponent_Factory(
						IFXREFIID	interfaceId,
						void**		ppInterface );
		@endcode
	-#	All components will have private (or protected) constructors, 
		accessible only through the friend factory function.  The destructor 
		function should be declared private.
	-#	All component class constructors should be empty.  If a custom 
		interface with an @c Initialize function is not necessary, a call to 
		a @c RobustConstructor function should be made by the component's 
		factory function so that an error can be returned and handed back to 
		the client.  The templates demonstrate this technique.
	-#	Immediately before a reference to a component is returned by the 
		component's factory function, it should increase the newly 
		instantiated component's reference count by calling its @c AddRef 
		function.
	-#	The implementation of the IFXUnknown interface's @c QueryInterface 
		function must be reflexive (a QI for an interface from the requested 
		interface should succeed), symmetric (it should be possible to QI 
		back to the original interface used to QI for a different one) and 
		transitive (any interface supported by a component can be QIed from 
		any of its interfaces).  Additionally, @c QueryInterface must always 
		say yes or no for an interface for any particular instance of a 
		component.  @c QueryInterface must always return the same pointer 
		value when the IFXUnknown interface is requested.  However, it can 
		return a different pointer value each time other interfaces are 
		requested.  This is useful for dynamically allocating subcomponents.
	-#	To define a new component for the IFXCOM plug-in dynamic library it 
		is associated with:
		-	The new component description should be added to the 
			@c gs_componentDescriptorList of this plug-in dynamic library 
			(refer to the @c IFXPluginName.cpp template).
		-	To use this component outside this plug-in dynamic library the 
			appropriate IFXDEFINE_GUID for its IFXCID must be declared 
			(refer to the @c IFXPluginNameCIDs.h template).
		-	The header files for all externally exposed interfaces of this 
			component should be included (refer to the 
			@c IFXPluginNameInterfaces.h template).

	<hr noshade>
	@section SectionPluginDynamicLibraryDesign	Design of Plug-in Dynamic Libraries
	-#	The body of the plug-in dynamic library must be implemented.  The 
		@c IFXPluginNameDllMain.cpp file contains the template for Windows* and 
			Linux* specific implementations.
	-#	All required functions that must be exported from the plug-in 
		dynamic library (@c IFXPluginRegister and @c IFXPluginCanUnloadNow) 
		must be implemented (refer to the @c IFXPluginName.cpp template).
	-#	Cross-platform @c IFXPluginNameStartup and @c IFXPluginNameShutdown 
		functions must be implemented for plug-in dynamic library 
		initialization and uninitialization (refer to the 
		@c IFXPluginName.cpp template).
	-#	The IFXDEFINE_GUIDs for all IFXCIDs of components which are 
		considered to be used outside of the dynamic plug-in library must be 
		declared (refer to the @c IFXPluginNameCIDs.h template).
	-#	All header files for externally exposed interfaces of components 
		defined in the dynamic plug-in library must be included (refer to 
		the @c IFXPluginNameInterfaces.h template).
	-#	Templates for extension plug-in modifier can be found in 
		@c Source\\Templates\\ExtensionModifier folder.
	-#	The plug-in dynamic libraries must statically link with 
		@c IFXCorePluginStatic.lib.
	-#	All compiled plug-in dynamic libraries must be placed in a 
		@c Plugins folder that must exist in the same directory as the 
		IFXCore DL.  Note:  In the future release for Linux, this 
		requirement might be different.

	<hr noshade>
	@section SectionComponentAndInterfaceUsage	Usage of Components and Interfaces
	-#	The statically linked @c IFXCoreStatic.lib library provides an easy 
		way to use all available components.  It dynamically loads the 
		@c IFXCore plug-in dynamic library which contains the IFXCOM 
		component system.  This component system must be initialized by 
		calling the IFXCOMInitialize function before any creation or usage 
		of IFXCOM components.  When no IFXCOM component exist and the 
		component system is no longer needed, call the IFXCOMUninitialize 
		function to unitialize it.  During uninitialization the IFXCOM 
		component system checks the ability of plug-in dynamic libraries to 
		be unloaded.  If this is not possible for any of plug-in dynamic 
		library, which means that some of the component instances have not 
		been released or some dynamically allocated objects have not been 
		deleted, the IFXCOMUninitialize function returns the warning 
		IFX_W_CANNOT_UNLOAD.
	-#	Components may be created by calling the IFXCreateComponent function.  
		Just specify the desired component's IFXCID, the desired interface's 
		IFXIID (that is supported by the component) and a pointer to a void 
		pointer that will receive a pointer to the desired interface upon 
		successful return.  Note:  Upon successful return, the caller is 
		responsible for calling the requested interface's 
		@c Release function.  Note:  If a type of component is a singleton 
		and there are outstanding interface references to it, any subsequent 
		calls to IFXCreateComponent will hand back interface references to 
		the same object that was already instantiated.
        @code
IFXRESULT   IFXCreateComponent( 
				IFXREFCID   componentId, 
				IFXREFIID   interfaceId, 
				void**      ppInterface );
		@endcode
	-#	When passing an interface pointer to a function, the caller must not 
		release the interface pointer until the called function returns.  
		This needs to be specifically adhered to when multithreading or 
		reentrancy is involved.
	-#	When a copy of an interface pointer is made that will exist in a 
		wider scope than the original pointer, the interface's @c AddRef 
		function must be called.  When the copy is no longer necessary, the 
		interface's @c Release function must be called.  For example, if a 
		function @c "Register( IFXCoreServices* pCoreServices )" of a 
		component @c CIFXTest takes as an argument a pointer to the 
		interface IFXCoreServices and in the @c Register function 
		implementation, it copies that pointer into the @c CIFXTest data 
		member @c m_pCoreServices, the @c AddRef function for the 
		IFXCoreServices interface must be called.  Immediately before 
		destroying the instance of @c CIFXTest, @c m_pCoreServices should 
		have its @c Release function called.
	-#	When a client is finished using a component or interface that it has 
		ownership of, the client must call the interface's @c Release 
		function.  A client must never invoke the delete operator on a 
		component.  Note:  IFXCOM components, when implemented properly and 
		based on the templates, have a private delete operator to prevent 
		this from ever being possible.  Note:  A client will have ownership 
		of an interface if it ever called a function to receive an interface 
		pointer to a component or it called the @c AddRef method on the 
		interface (and hopefully kept a pointer to it).  For example, 
		calling IFXCreateComponent, calling an interface's @c QueryInterface 
		function or calling @c AddRef on an interface pointer received as a 
		parameter and keeping it in a data member.

	@note	When it is known ahead-of-time that a reference count cycle 
			between one or more components will occur (or it is discovered 
			that one exists), it can be prevented (or fixed) by using a 
			special common prevention component.  This component's id is 
			CID_IFXConnector.  Its main interfaces are IFXConnection and 
			IFXConnectionServer.  It is designed to manage a weak interface 
			reference to a component in a controlled manner so that no 
			clients need to knowingly keep weak references to interfaces and 
			break the IFXCOM rules.  Using this special component will make 
			for a more stable engine and won't require custom coding of 
			prevention components such as those already developed to deal 
			with this issue (such as CIFXCoreServicesRef used by 
			CIFXCoreServices).

	<hr noshade>
	@section SectionRelatedInformation	Related Information
	- U3D's general @ref PageCodingConventions "Coding Conventions"
*/


#endif


