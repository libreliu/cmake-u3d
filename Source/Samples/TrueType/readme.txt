TrueType README
===============


This directory contains the Microsoft* Win32* TrueType Glyph 2D sample.
It shows a technique for adding adding text messages into U3D scenes.  It
demonstrates how to create glyph commands for the Glyph2D modifier using
TrueType font information extracted using the Microsoft* Windows* API.


Building
--------

To build the TrueType sample use Microsoft* Visual Studio* .NET* 2003 with
the TrueType.vcproj project file.

The only current U3D static library depended upon is IFXCoreStatic.lib
and SceneHelpers.lib.

IFXCore.DLL must be in your environment PATH variable and whereever that
is located a Plugins subdirectory must exist that contains the other U3D
DLs that are needed:  IFXImporting.DLL, IFXRendering.DLL and
IFXScheduling.DLL.

Launching
---------

To launch TrueType sample copy file called "shading.u3d" from the source
directory of the sample to the directory with TrueType binary. After that,
launch "TrueType.exe".


* Other names and brands may be claimed as the property of others.
