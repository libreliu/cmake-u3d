Exporter README
===============


This directory contains a Discreet* 3ds max* exporter plug-in for Win32 to
save scenes to Intermediate Data Text Format (IDTF) files that the
IDTFConverter can read and convert to U3D files.


Building
--------

To build the IDTF Exporter use Microsoft* Visual Studio* .NET* 2003 with 
the ..\Make\Exporter.vcproj project file.  This project depends upon the
Discreet* 3ds max* 7 SDK and the Biped header file (Bipexp.h).  Make sure 
that the SDK is installed and then setup the following environment variables 
used by this project file:

Variable      Value
--------      -----

MAX_SDK       Must contain the path to the Discreet* 3ds max* 7 SDK without 
              a trailing slash.  For example:

              C:\3dsmax7\maxsdk

MAX_ROOT      Must contain the path to the Discreet* 3ds max* directory where 
              it was installed without a trailing slash.  This is used for 
              placing a copy of the exporter plugin .dle file in the pluins
              directory.  For example:

              C:\3dsmax7


* Other names and brands may be claimed as the property of others.


