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
	@file	U3DCodingConventionsPage.h

			This header is used solely to contain the doxygen commands for 
			the Coding Conventions documentation page.
*/


#ifndef U3DCodingConventionsPage_H
#define U3DCodingConventionsPage_H


//***************************************************************************
//	Doxygen pages
//***************************************************************************


//---------------------------------------------------------------------------
/**
	@page	PageCodingConventions	Coding Conventions

	<hr noshade>
	@section	SectionMusts	Musts

	-#	Follow the IFXCOM guidelines when defining, implementing or using 
		IFXCOM components.  The guidelines for CIL languages such as C# are 
		TBD. 
	-#	Use the boiler-plate code templates for defining new source files 
		(including IFXCOM interfaces and components).  Of particular 
		importance is the inclusion of the common copyright notice and 
		disclaimer.
	<br><br><hr noshade><br>
	-#	Use header file #ifdef's to prevent multiple inclusions of the same 
		header file. 
	-#	No paths in #include statements.  Use local project settings to 
		specify paths. 
	-#	Watch case sensitivity in system header include statements for 
		portability (use stdlib.h instead of StdLib.h). 
	-#	Public interface header files must not include or require private 
		implementation header files. 
	<br><br><hr noshade><br>
	-#	Document module contents, interfaces, classes, methods and other 
		significant items with doxygen comments in the JavaDoc style (/// 
		and /** where asterisks on intermediate new lines are optional).  
		Specify doxygen commands using lower-case characters.  Prefix 
		commands with the "@" symbol instead of a "\".
	-#	Use comments to describe the meaning of global and class data members 
		(including when valid/initialized). 
	-#	Use comments prefixed with doxygen @c todo commands and date to 
		mark all uncompleted work or issues that need to be investigated.
	-#	All function and method names begin with an UPPERCASE letter.
		Subsequent words begin with an UPPERCASE letter.
	-#	Names of all functions and methods must end with the suffix "X" if 
		they are known to throw exceptions.  This is necessary even if they 
		call another method or function that throws exceptions.  However, it 
		is not necessary if such called functionality is wrapped in a try 
		catch block and the method or function itself does not directly 
		throw any exceptions.
	-#	All variable names, after prefixes (such as "m_"), begin with a 
		lowercase letter.  Subsequent words begin with an UPPERCASE letter.  
		For example, m_myValue or m_pMyValue when the required prefixes 
		and/or Hungarian is being used.
	-#	Macro and #define names should be all UPPERCASE letters separated by 
		underscores.  Use an underscore to prefix module locals.
	-#	Prefix public, globally scoped classes, structs and functions (not 
		methods) with "IFX" and no underscore.  For CIL languages such as C# 
		and Managed C++ use "U3D".
	-#	Required prefixes are listed in the following table (the U3D 
		containing prefixes are used for CIL languages such as C# and Managed 
		C++): <br><br>
		<table width="50%" border="1" align="center">
		  <tr> 
			<td>Class or structure definition</td>
			<td>IFX / U3D</td>
		  </tr>
		  <tr> 
			<td>Interface class definition</td>
			<td>IFX / IU3D</td>
		  </tr>
		  <tr> 
			<td>Component definition</td>
			<td>CIFX / CU3D</td>
		  </tr>
		  <tr> 
			<td>Template definition</td>
			<td>TIFX / TU3D</td>
		  </tr>
		  <tr> 
			<td>Enumeration definition</td>
			<td>EIFX / EU3D</td>
		  </tr>
		  <tr> 
			<td>Function (not for a class-scoped method)</td>
			<td>IFX / U3D</td>
		  </tr>
		</table><br>
	-#	Required variable naming prefixes are listed in order of usage 
		precedence in the following table: <br><br>
		<table width="50%" border="1" align="center">
		  <tr> 
			<td>Data Member</td>
			<td>m_</td>
		  </tr>
		  <tr> 
			<td>Data member static</td>
			<td>ms_</td>
		  </tr>
		  <tr> 
			<td>Global</td>
			<td>g_</td>
		  </tr>
		  <tr> 
			<td>Global static</td>
			<td>gs_</td>
		  </tr>
		  <tr> 
			<td>Local static</td>
			<td>s_</td>
		  </tr>
		  <tr> 
			<td>Reference</td>
			<td>r</td>
		  </tr>
		  <tr> 
			<td>Pointer</td>
			<td>p</td>
		  </tr>
		  <tr> 
			<td>Pointer to pointer</td>
			<td>pp</td>
		  </tr>
		  <tr> 
			<td>Handle</td>
			<td>h</td>
		  </tr>
		</table><br>
	-#	In new modules, consistently place block brackets on their own new 
		lines, immediately below associated selection or iteration statement 
		keywords, and consistently use only the above variable naming 
		prefixes. 
	-#	In existing modules, adhere to the established common block bracket 
		placement style and variable naming style. 
	-#	Use tabs for indenting with 4 character interval tab stops (setup 
		editor to NOT insert characters when tab is pressed). 
	-#	File names must not include white space.
	<br><br><hr noshade><br>
	-#	Validate all external interface inputs (i.e. function/method 
		parameters) for major subsystems in all builds. 
	-#	Check function/method results and return meaningful errors. 
	-#	Assert assumptions and correctness. 
	-#	No assignments to non-debug variables in debug only code. 
	<br><br><hr noshade><br>
	-#	Class constructors should fully qualify all calls they make to their 
		virtual methods. 
	-#	Make class destructors virtual unless a class has no virtual methods 
		and is optimized for size. If not, explicitly document why. 
	-#	Provide copy constructors for non-trivial classes not intended to be 
		IFXCOM components. 
	-#	Classes should not have any public data members. 
	-#	Use basic types defined in IFXDataTypes.h (i.e. avoid unconstrained 
		types and types we don't have control over). 
	-#	Create flexible data structures (i.e. no arbitrary limits). 
	-#	No magic numbers. Use descriptive constants or enumerations instead. 
	-#	The last enumeration in an enumerated type definition must not be 
		followed by a comma. 
	-#	All enumerated types must be stored as 32-bit values. This can be 
		ensured with the proper project build settings. 
	-#	Isolate and abstract platform/OS specific code into functions and 
		modules named with "IFXOS" prefix (OS specific #ifdef's belong here). 
	-#	No C++ stream I/O usage. Instead, use standard C library I/O. 
	<br><br><hr noshade><br>
	-#	Result codes shall be returned by all non-trivial functions and 
		methods using the IFXResult type. Alternatively, when unexpected 
		errors occur that are not a part of normal execution an 
		IFXException may be thrown if the function or method's name has the 
		suffix "X". 
	-#	The names of functions, interfaces and classes that throw exceptions 
		shall have the suffix "X". 
	-#	Source code must check the result codes of all called functions and 
		methods that return them using the macros defined in IFXResult.h and 
		IFXCheckX.h. When an exception is made and this is not done, comments 
		must be provided that describe the reason why. 
	<br><br><hr noshade><br>
	-#	Prior to code check-in, it must compile and link with no level-3 
		warnings (with approved pragma suppressions) in all builds. 
	-#	Prior to code check-in before Beta, do reasonable unit testing. After 
		Beta, first check all branches in the debugger. 
	-#	Design and code inspections should be used for significant modules and 
		code (schedule them early). 
	-#	Before shipping, create a certificate of origin for all code. 
	-#	Before shipping, verify the correct copyright was used for all new and 
		modified files. 
	-#	Before shipping, make sure all executables and dynamic libraries have 
		an appropriate, fairly consistent version resource. 
	-#	Before shipping, address all sections in the code marked with @c todo 
		prefixes. Ideally, add the comment text @c FUTURE if postponed. 

	<hr noshade>
	@section	SectionGoodPractice	Good Practice

	-#	Every class/component/interface has its own header (.h) and body (.cpp) 
		source file. 
	-#	Header (.h) and body (.cpp) source file names are exactly the name of 
		the class. 
	-#	Source code line length should be limited to ~80 characters. Adhere to 
		the established style of the file being edited. 
	-#	Header (.h) files include all and only required headers (except 
		precompiled headers). Avoid forward class declarations. 
	-#	Use the project defined function/method return value system (refer to 
		IFXOSDebug.h). 
	-#	Use the project defined debug messaging, assertion and trace system 
		(refer to IFXOSDebug.h). 
	-#	Initialize all local variables before usage. 
	-#	Validate inputs and outputs as early as possible. 
	-#	Give all class data members private visibility (except consts). Provide 
		protected accessor methods for derived classes. 
	-#	All intrinsic data types are passed by value unless they are used to 
		return a value. Other data types are never passed by value unless they 
		are <= the size of a pointer. 
	-#	Minimize conditional compilation. 
	-#	Minimize casting. 
	-#	Minimize the scope of code and data. 
	-#	Minimize block depth/levels of indentation. 
	-#	Minimize function/method exit points. 
	-#	Minimize operator overloading to reduce cryptic and deceptive code. 
	-#	Use consts and enums (class scoped when possible) over #defines. 
	-#	When #define declarations are used, enclose the value/calculation in 
		parens and cast it to the desired type. 
	-#	Avoid excessive commenting of end of block "}" statements and #endif's. 
	-#	Don't put end-of-file "EOF" comments at the end of any source files. 
	-#	Don't duplicate code in a comment unless the comment is providing an 
		example. 
	-#	Code behavior is more clear if "NULL" is explicitly used when checking 
		for null or non-null pointers. 
	-#	When comparing against a constant, unintended assignments can be 
		prevented by placing the const on the left side of the comparison. 

	<hr noshade>
	@section SectionRelatedInformation	Related Information
	- @ref PageCodingConventionsIFXCOM "IFXCOM Coding Conventions"
*/


#endif


