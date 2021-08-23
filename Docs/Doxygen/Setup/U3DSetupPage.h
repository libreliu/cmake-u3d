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
	@file	U3DSetupPage.h

			This header is used solely to contain the doxygen commands for 
			the Setup documentation page.
*/


#ifndef U3DSetupPage_H
#define U3DSetupPage_H


//***************************************************************************
//	Doxygen pages
//***************************************************************************


//---------------------------------------------------------------------------
/**
	@page	PageSetupProcedure	Setup Procedure

	The following styles of setup are available based on the expected 
	usage needs.

	<hr noshade>
	@section SectionPrecompiledDLs	For Precompiled Universal 3D Dynamic Libraries
	To use the existing U3D Sample Software without recompiling it, this 
	distribution includes a root @c Include and @c Lib directory that should 
	be added to your project and/or compiler configuration.  The precompiled 
	binaries are provided in the @c Bin directory.  Additional details are 
	described on the @ref PageUsage "Usage" page.

	The header files in the root @c Include directory are currently just 
	duplicated from @c Source\\RTL\\Component\\Include, 
	@c Source\\RTL\\Kernel\\Include and @c Source\\RTL\\Platform\\Include.  
	No serious attempt to reduce the headers to only those needed by clients 
	has been made yet.

	If planning to include @c IFXRenderingInterfaces.h, @c IFXDirectX7.h, 
	@c IFXDirectX8.h and/or @c IFXOpenGL.h, it will be necessary to have the 
	Microsoft* DirectX* 8.1 SDK headers or those from a newer version, and 
	the SGI* OpenGL* header @c glext.h in the available include paths.
	-	To find the available versions of the DirectX* SDK visit the 
		<a href="http://www.microsoft.com/downloads/search.aspx?displaylang=en&categoryid=2">DirectX*</a> 
		section of the Microsoft* Download Center and search using "SDK" 
		as the keyword and "DirectX" as the Product/Technology.
	-	A version of the SGI* OpenGL* header @c glext.h is provided in the 
		@c Source\\RTL\\Dependencies\\OpenGL\\Include directory.

	<hr noshade>
	@section SettingUpBuildSystem	For Setting Up Build System
	This step is required if Universal 3D dynamic libraries are to be compiled.
	Before compilation, the build system configuration should be set.
	The default values for configuration parameters are:
	@verbatim
U3D_PLATFORM = Win32
U3D_COMPILER = cl
U3D_LINKER   = link
U3D_OUTPUT   = Build/U3D@endverbatim
	These values are stored in file @c Source\\Config\\u3d.@c conf.
	They should be adjusted according to user configuration. This can be done
	by either editing file @c u3d.@c conf or by using script
	@c Tools\\BuildSystem\\config.@c pl.\n
	Example 1:
	@verbatim
perl config.pl - icl@endverbatim
	This command changes @c U3D_COMPILER value to @c icl.\n
	Example 2:
	@verbatim
perl config.pl Lin32 gcc ar@endverbatim
	This command changes @c U3D_PLATFORM value to @c Lin32,
	@c U3D_COMPILER value to @c gcc,
	@c U3D_LINKER value to @c ar.
	This script without parameters produces help screen.\n
	Currently supported values:
	-#	@c U3D_PLATFORM:
			- @c Win32		Microsoft* Win32
			- @c Lin32		32-bit Linux*
	-#	@c U3D_COMPILER:
			- @c cl			Microsoft* C++ compiler, supported on Win32 platform
			- @c icl		Intel&reg; C++ compiler, supported on Win32 and Lin32 platforms
			- @c gcc		GNU C++ compiler, supported on Lin32 platform
	-#	@c U3D_LINKER:
			- @c link		Microsoft* linker, supported on Win32 platform
			- @c ar			GNU archiver, supported on Lin32 platform
	-#	@c U3D_OUTPUT:
			- Directory for U3D binaries. Possible value is any path inside
			@c $U3D_SOURCE.
	@section SectionCompilingDLs	For Compiling Universal 3D Dynamic Libraries on Windows*
	To recompile the U3D Sample Software on a Microsoft* Win32 platform, it 
	is necessary to perform the following additional steps after unzipping the 
	U3D package:

	-#	Start setting up the U3D build system by downloading and installing 
		the Small Cygwin* Package (SCP) build tool.  It is a small, 
		easy-to-setup distribution of Red Hat* Cygwin* 2.416. The package is 
		zipped and is minimal, including just those portions of Cygwin* 
		required by the U3D build system, such as the Bash shell and Perl.
		This SCP package also contains Larry Wall and Clark Cooper's 
		XML::Parser 2.34, and Grant McLean's XML::Simple 2.09 that are 
		needed by the U3D build system and are not included with 
		any known Cygwin* distribution.
		-	CygWin_2.416_Minimal.zip can be obtained from the U3D Sample 
			Software project website on 
			<a href="http://sourceforge.net/projects/u3d/">Source Forge*</a> 
			(http://sourceforge.net/projects/u3d/) in the 
			<a href="http://sourceforge.net/project/showfiles.php?group_id=133107">files area</a>.
		-	Source code used for this minimal Cygwin* distribution can also 
			be obtained from the project website on 
			<a href="http://sourceforge.net/projects/u3d/">Source Forge*</a> 
			(http://sourceforge.net/projects/u3d/) in the 
			<a href="http://sourceforge.net/project/showfiles.php?group_id=133107">files area</a>.  
			These are named:  
			-	CygWin_2.416_Source.zip
			-	XML-Parser-2.34.tar.gz
			-	XML-Simple-2.09.tar.gz
		-	To learn more about Cygwin* or to obtain the complete 
			distribution visit http://www.cygwin.com/.  After a complete 
			distribution of Cygwin* is setup, make sure to install and test 
			both XML::Parser and XML::Simple using Cygwin*.  Do this by 
			typing in the following commands from the directory that both 
			packages were placed in:
			@verbatim
tar xvzf XML-Parser-2.34.tar.gz
cd XML-Parser-2.34
perl Makefile.PL
make test
make install
perl -MXML::Parser -e 1; echo $?;
cd ..

tar xvzf XML-Simple-2.09.tar.gz
cd XML-Simple-2.09
perl Makefile.PL
make test
make install
perl -MXML::Simple -e 1; echo $?;	@endverbatim
			There should be no error messages.  The above Perl* steps will 
			display zero if the installation was successful.
	-#	Add the Cygwin* @c Bin directory's complete path to your system's 
		@c PATH environment variable.  If you installed it in the typical 
		location, this would be @c C:\\Cygwin\\Bin.
	-#	Create a new user environment variable called @c U3D_SOURCE.  Set it 
		to the location of the U3D @c Source folder.
		-	Make sure to specify the complete path including the root and 
			drive letter, as well as terminating the path with a trailing 
			backslash.  For example, @c C:\\SDK\\U3D\\Source\\.
		-	The path specified should NOT contain any spaces due to the 
			current build system scripts.
	-#	In the initial build configuration, it is necessary to copy the 
		Microsoft* DirectX* 8.1 SDK header files or those from a newer 
		version, into the @c Source\\RTL\\Dependencies\\DirectX\\Include 
		directory.
		-	To find the available versions of the DirectX* SDK visit the 
			<a href="http://www.microsoft.com/downloads/search.aspx?displaylang=en&categoryid=2">DirectX*</a> 
			section of the Microsoft* Download Center and search using "SDK" 
			as the keyword and "DirectX" as the Product/Technology.
		-	The projects that depend upon these SDK header files include 
			the DirectX* 7 and 8 renderers.  These are defined by the 
			@c IFXRenderingDirectX7.xml and @c IFXRenderingDirectX8.xml 
			build system configuration files.
	-#	If building the example IDTF exporter plug-in for Discreet* 3ds max* 
		7 is desired, the following new user environment variables need to 
		be created to identify various directories:
		-	@c MAX_SDK must contain the path to the Discreet* 3ds max* 7 SDK 
			without a trailing backslash.  For example, 
			@c C:\\3dsmax7\\maxsdk.
		-	@c MAX_ROOT must contain the path to the Discreet* 3ds max* 
			directory where it was installed without a trailing backslash.  
			This is used for placing a copy of the IDTF exporter plugin .dle 
			file in the @c plugins subdirectory.  For example, 
			@c C:\\3dsmax7.

	<hr noshade>
	@section SectionCompilingDLsLinux	For Compiling Universal 3D Dynamic Libraries on Linux*
	To recompile the U3D Sample Software on a Linux* platform, it 
	is necessary to perform the following additional steps after unzipping the 
	U3D package:

	-# Install and test XML::Parser and XML::Simple.  See above.
	-# Create and export environment variables:
		- @c U3D_SOURCE. Set to the location of the U3D @c Source directory.  Include trailing slash.
		- @c U3D_LIBDIR. Set to  @c ${U3D_SOURCE}Build/U3D.
		- Add @c U3D_LIBDIR to @c PATH and @c LD_LIBRARY_PATH variables.

	<hr noshade>
	@section SectionNextStep	Next Step
	Proceed to @ref PageBuildProcedure "Build Procedure".
*/


#endif


