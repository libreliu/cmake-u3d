Player README
=============


This directory contains the example U3D player (or viewer) for Microsoft* 
Win32*.  It demonstrates how to import U3D files, how they can be 
rendered and some ways in which they can be interacted with.


Command-line format
-------------------

SamplePlayer.exe [-base[ext]] [-base[ext]] ...

Options: 
	renderer type: -rogl           - OpenGL*
		    or -rdx7           - DirectX* 7
		    or -rdx8           - DirectX* 8
	culling mode:  -csgr           - SceneGraph Culler
		       -coct           - Octree Culler
	loading mode:  -latom          - Atomic
		       -lnatom         - Non-Atomic
	sec timeout:   -t10            - 10 second timeout
	input file:    -iFileName.ext


Building
--------

To build the example player use the BuildSystem (refer to the main 
documentation for details) or use Microsoft* Visual Studio* .NET* 2003 
with the SamplePlayer.sln solution file.

The only current U3D static library depended upon is IFXCoreStatic.lib 
and SceneHelpers.lib.

IFXCore.DLL must be in your environment PATH variable and whereever that 
is located a Plugins subdirectory must exist that contains the other U3D 
DLs that are needed:  IFXImporting.DLL, IFXRendering.DLL and 
IFXScheduling.DLL.


* Other names and brands may be claimed as the property of others.


