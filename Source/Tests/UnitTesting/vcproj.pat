<?xml version="1.0" encoding="windows-1251"?>
<VisualStudioProject
	ProjectType="Visual C++"
	Version="7.10"
	Name="<!--Perl: test_name-->"
	SccProjectName=""
	SccLocalPath="">
	<Platforms>
		<Platform
			Name="Win32"/>
	</Platforms>
	<Configurations>
		<Configuration
			Name="Debug|Win32"
			OutputDirectory=".\Debug"
			IntermediateDirectory=".\Debug"
			ConfigurationType="1"
			UseOfMFC="0"
			ATLMinimizesCRunTimeLibraryUsage="FALSE"
			CharacterSet="2"
			ReferencesPath="">
			<Tool
				Name="VCCLCompilerTool"
				Optimization="0"
				ExceptionHandling="TRUE"
				AdditionalIncludeDirectories="<!--Perl: includedirs-->"
				PreprocessorDefinitions="_LIB;_WIN32;_WINDOWS;_WINVER=0x0400;IFXAPI_EXPORT;DEBUG;_DEBUG;UNICODE;WIN32;WIN32_LEAN_AND_MEAN;WINVER=0x0400"
				WarningLevel="3"
				DebugInformationFormat="3"
				RuntimeLibrary="1"				
				RuntimeTypeInfo="TRUE"
				ObjectFile="./debug/"
				ProgramDataBaseFileName="./debug/"
				BufferSecurityCheck="FALSE"
				CompileAs="0"/>
			<Tool
				Name="VCCustomBuildTool"/>
			<Tool
				Name="VCLinkerTool"
				AdditionalDependencies="IFXCoreStatic.lib SceneHelpers.lib SampleViewerWin32.lib UnitTesting.lib winmm.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib uuid.lib comdlg32.lib  comctl32.lib"
				OutputFile="./debug/<!--Perl: test_name-->.exe"
				SuppressStartupBanner="TRUE"
				AdditionalLibraryDirectories="$(U3D_SOURCE)RTL\Build\All\debug\CoreStatic;$(U3D_SOURCE)RTL\Build\CoreStatic\debug;$(U3D_SOURCE)RTL\Build\All\debug\SceneHelpers;$(U3D_SOURCE)RTL\Build\SceneHelpers\debug;$(U3D_SOURCE)RTL\Build\All\debug\SampleViewerWin32;$(U3D_SOURCE)RTL\Build\SampleViewerWin32\debug;$(U3D_SOURCE)Tools\UnitTesting\Lib\Win32\debug;"
				IgnoreDefaultLibraryNames="LIBC.lib"
				GenerateDebugInformation="TRUE"				
				SubSystem="<!--Perl: subsystem-->"				
				TargetMachine="1"/>
			<Tool
				Name="VCMIDLTool"
				TypeLibraryName=".\Debug/<!--Perl: test_name-->.tlb"
				HeaderFileName=""/>
			<Tool
				Name="VCPostBuildEventTool"/>
			<Tool
				Name="VCPreBuildEventTool"/>
			<Tool
				Name="VCPreLinkEventTool"/>
			<Tool
				Name="VCResourceCompilerTool"
				PreprocessorDefinitions="DEBUG;_DEBUG"
				AdditionalIncludeDirectories="$(U3D_SOURCE)RTL/Kernel/Include"
				Culture="1033"
				ResourceOutputFileName="./debug/SamplePlayer.RES"/>
			<Tool
				Name="VCWebServiceProxyGeneratorTool"/>
			<Tool
				Name="VCXMLDataGeneratorTool"/>
			<Tool
				Name="VCWebDeploymentTool"/>
			<Tool
				Name="VCManagedWrapperGeneratorTool"/>
			<Tool
				Name="VCAuxiliaryManagedWrapperGeneratorTool"/>			
		</Configuration>
		<Configuration
			Name="Release|Win32"
			OutputDirectory=".\Release"
			IntermediateDirectory=".\Release"
			ConfigurationType="1"
			UseOfMFC="0"
			ATLMinimizesCRunTimeLibraryUsage="FALSE"
			CharacterSet="2">
			<Tool
				Name="VCCLCompilerTool"
				Optimization="0"
				ExceptionHandling="TRUE"
				AdditionalIncludeDirectories="<!--Perl: includedirs-->"
				PreprocessorDefinitions="_LIB;_WIN32;_WINDOWS;_WINVER=0x0400;IFXAPI_EXPORT;NDEBUG;UNICODE;WIN32;WIN32_LEAN_AND_MEAN;WINVER=0x0400"
				WarningLevel="3"
				DebugInformationFormat="3"
				RuntimeLibrary="0"				
				RuntimeTypeInfo="TRUE"
				ObjectFile="./release/"
				ProgramDataBaseFileName="./release/"
				BufferSecurityCheck="FALSE"
				CompileAs="0"/>
			<Tool
				Name="VCCustomBuildTool"/>
			<Tool
				Name="VCLinkerTool"
				AdditionalDependencies="IFXCoreStatic.lib SceneHelpers.lib SampleViewerWin32.lib UnitTesting.lib winmm.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib uuid.lib comdlg32.lib  comctl32.lib"
				OutputFile=".\Release/<!--Perl: test_name-->.exe"
				SuppressStartupBanner="TRUE"
				AdditionalLibraryDirectories="$(U3D_SOURCE)RTL\Build\All\release\CoreStatic;$(U3D_SOURCE)RTL\Build\CoreStatic\release;$(U3D_SOURCE)RTL\Build\All\release\SceneHelpers;$(U3D_SOURCE)RTL\Build\SceneHelpers\release;$(U3D_SOURCE)RTL\Build\All\release\SampleViewerWin32;$(U3D_SOURCE)RTL\Build\SampleViewerWin32\release;$(U3D_SOURCE)Tools\UnitTesting\Lib\Win32\release;"
				IgnoreDefaultLibraryNames="LIBC.lib"
				SubSystem="<!--Perl: subsystem-->"
				TargetMachine="1"/>
			<Tool
				Name="VCMIDLTool"
				TypeLibraryName=".\Release/<!--Perl: test_name-->.tlb"
				HeaderFileName=""/>
			<Tool
				Name="VCPostBuildEventTool"/>
			<Tool
				Name="VCPreBuildEventTool"/>
			<Tool
				Name="VCPreLinkEventTool"/>
			<Tool
				Name="VCResourceCompilerTool"				
				PreprocessorDefinitions="NDEBUG"
				AdditionalIncludeDirectories="$(U3D_SOURCE)RTL/Kernel/Include"
				Culture="1033"
				ResourceOutputFileName="./release/SamplePlayer.RES"/>
			<Tool
				Name="VCWebServiceProxyGeneratorTool"/>
			<Tool
				Name="VCXMLDataGeneratorTool"/>
			<Tool
				Name="VCWebDeploymentTool"/>
			<Tool
				Name="VCManagedWrapperGeneratorTool"/>
			<Tool
				Name="VCAuxiliaryManagedWrapperGeneratorTool"/>
		</Configuration>
		<Configuration
			Name="DebugCygwin|Win32"
			OutputDirectory=".\DebugCygwin"
			IntermediateDirectory=".\DebugCygwin"
			ConfigurationType="1"
			UseOfMFC="0"
			ATLMinimizesCRunTimeLibraryUsage="FALSE"
			CharacterSet="2"
			ReferencesPath="">
			<Tool
				Name="VCCLCompilerTool"
				Optimization="0"
				AdditionalIncludeDirectories="<!--Perl: includedirs-->"
				PreprocessorDefinitions="WIN32;_DEBUG;_CONSOLE;UNICODE"
				BasicRuntimeChecks="3"
				RuntimeLibrary="3"
				RuntimeTypeInfo="TRUE"
				UsePrecompiledHeader="2"
				PrecompiledHeaderFile=".\DebugCygwin/<!--Perl: test_name-->.pch"
				AssemblerListingLocation=".\DebugCygwin/"
				ObjectFile=".\DebugCygwin/"
				ProgramDataBaseFileName=".\DebugCygwin/"
				BrowseInformation="1"
				WarningLevel="3"
				SuppressStartupBanner="TRUE"
				DebugInformationFormat="4"
				CompileAs="0"/>
			<Tool
				Name="VCCustomBuildTool"/>
			<Tool
				Name="VCLinkerTool"
				AdditionalDependencies="winmm.lib comctl32.lib IFXCoreStatic.lib SampleViewerWin32.lib SceneHelpers.lib UnitTesting.lib"
				OutputFile=".\DebugCygwin/<!--Perl: test_name-->.exe"
				LinkIncremental="2"
				SuppressStartupBanner="TRUE"
				AdditionalLibraryDirectories="$(U3D_SOURCE)RTL\Build\CoreStatic\Debug;$(U3D_SOURCE)RTL\Build\SampleViewerWin32\Debug;$(U3D_SOURCE)RTL\Build\SceneHelpers\Debug;$(U3D_SOURCE)Tools\UnitTesting\Lib\Win32\Debug"
			IgnoreDefaultLibraryNames="libcd.lib;libcmtd.lib"
				GenerateDebugInformation="TRUE"
				ProgramDatabaseFile=".\DebugCygwin/<!--Perl: test_name-->.pdb"
				SubSystem="<!--Perl: subsystem-->"
				TargetMachine="1"/>
			<Tool
				Name="VCMIDLTool"
				TypeLibraryName=".\DebugCygwin/<!--Perl: test_name-->.tlb"
				HeaderFileName=""/>
			<Tool
				Name="VCPostBuildEventTool"/>
			<Tool
				Name="VCPreBuildEventTool"/>
			<Tool
				Name="VCPreLinkEventTool"/>
			<Tool
				Name="VCResourceCompilerTool"
				PreprocessorDefinitions="_DEBUG"
				Culture="1049"/>
			<Tool
				Name="VCWebServiceProxyGeneratorTool"/>
			<Tool
				Name="VCXMLDataGeneratorTool"/>
			<Tool
				Name="VCWebDeploymentTool"/>
			<Tool
				Name="VCManagedWrapperGeneratorTool"/>
			<Tool
				Name="VCAuxiliaryManagedWrapperGeneratorTool"/>
		</Configuration>
		<Configuration
			Name="ReleaseCygwin|Win32"
			OutputDirectory=".\ReleaseCygwin"
			IntermediateDirectory=".\ReleaseCygwin"
			ConfigurationType="1"
			UseOfMFC="0"
			ATLMinimizesCRunTimeLibraryUsage="FALSE"
			CharacterSet="2">
			<Tool
				Name="VCCLCompilerTool"
				Optimization="2"
				AdditionalIncludeDirectories="<!--Perl: includedirs-->"
				InlineFunctionExpansion="1"
				PreprocessorDefinitions="WIN32;NDEBUG;_CONSOLE;UNICODE"
				StringPooling="TRUE"
				RuntimeLibrary="2"
				RuntimeTypeInfo="TRUE"
				EnableFunctionLevelLinking="TRUE"
				UsePrecompiledHeader="2"
				PrecompiledHeaderFile=".\ReleaseCygwin/<!--Perl: test_name-->.pch"
				AssemblerListingLocation=".\ReleaseCygwin/"
				ObjectFile=".\ReleaseCygwin/"
				WarningLevel="3"
				SuppressStartupBanner="TRUE"/>
			<Tool
				Name="VCCustomBuildTool"/>
			<Tool
				Name="VCLinkerTool"
				AdditionalDependencies="winmm.lib comctl32.lib IFXCoreStatic.lib SampleViewerWin32.lib SceneHelpers.lib UnitTesting.lib"
				OutputFile=".\ReleaseCygwin/<!--Perl: test_name-->.exe"
				LinkIncremental="1"
				SuppressStartupBanner="TRUE"
				AdditionalLibraryDirectories="$(U3D_SOURCE)RTL\Build\CoreStatic\Release;$(U3D_SOURCE)RTL\Build\SampleViewerWin32\Release;$(U3D_SOURCE)RTL\Build\SceneHelpers\Release;$(U3D_SOURCE)Tools\UnitTesting\Lib\Win32\Release"
			IgnoreDefaultLibraryNames="libcmt.lib;libcmtd.lib;msvcrtd.lib"
				SubSystem="<!--Perl: subsystem-->"
				TargetMachine="1"/>
			<Tool
				Name="VCMIDLTool"
				TypeLibraryName=".\ReleaseCygwin/<!--Perl: test_name-->.tlb"
				HeaderFileName=""/>
			<Tool
				Name="VCPostBuildEventTool"/>
			<Tool
				Name="VCPreBuildEventTool"/>
			<Tool
				Name="VCPreLinkEventTool"/>
			<Tool
				Name="VCResourceCompilerTool"
				PreprocessorDefinitions="NDEBUG"
				Culture="1049"/>
			<Tool
				Name="VCWebServiceProxyGeneratorTool"/>
			<Tool
				Name="VCXMLDataGeneratorTool"/>
			<Tool
				Name="VCWebDeploymentTool"/>
			<Tool
				Name="VCManagedWrapperGeneratorTool"/>
			<Tool
				Name="VCAuxiliaryManagedWrapperGeneratorTool"/>
		</Configuration>
	</Configurations>
	<References>
	</References>
	<Files>
		<Filter
			Name="Source Files"
			Filter="cpp;c;cxx;rc;def;r;odl;idl;hpj;bat">
                        <File
				RelativePath="<!--Perl: u3dsp-->Samples/SampleViewerWin32/SamplePlayer.rc">
			</File>
			<!--Perl: source "cpp"-->
		</Filter>
		<Filter
			Name="Header Files"
			Filter="h;hpp;hxx;hm;inl">
			<!--Perl: source "h"-->
			<File
				RelativePath="../../../../Tools/UnitTesting/Include/IFXSuite.h">
			</File>
			<File
				RelativePath="../../../../Tools/UnitTesting/Include/IFXTest.h">
			</File>
			<File
				RelativePath="../../../../Samples/SceneHelpers/U3DCameraNavigation.h">
			</File>
			<File
				RelativePath="../../../../Samples/SceneHelpers/U3DSceneUtilities.h">
			</File>
			<File
				RelativePath="../../../../Samples/SampleViewerWin32/U3DViewer.h">
			</File>
			<File
				RelativePath="../../../../Samples/SampleViewerWin32/resource.h">
			</File>
		</Filter>
		<Filter
			Name="Resource Files"
			Filter="ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe">
		</Filter>
	</Files>
	<Globals>
	</Globals>
</VisualStudioProject>
