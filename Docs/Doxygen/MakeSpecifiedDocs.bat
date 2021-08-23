rem ***************************************************************************
rem 
rem   Copyright (c) 1999 - 2006 Intel Corporation
rem 
rem   Licensed under the Apache License, Version 2.0 (the "License");
rem   you may not use this file except in compliance with the License.
rem   You may obtain a copy of the License at
rem 
rem      http://www.apache.org/licenses/LICENSE-2.0
rem 
rem   Unless required by applicable law or agreed to in writing, software
rem   distributed under the License is distributed on an "AS IS" BASIS,
rem   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
rem   See the License for the specific language governing permissions and
rem   limitations under the License.
rem 
rem ***************************************************************************
@echo off

if "%1" == "" goto About
if not "%2" == "" goto About
if "%1" == "U3D" goto MakeDocs
if "%1" == "Test" goto MakeDocs
goto About

:MakeDocs
cls
echo Making docs:  %1
echo Start Time:
time /t
set DoxOutDir=Output\HTML_%1
rd /S /Q %DoxOutDir%
mkdir %DoxOutDir% >nul
mkdir %DoxOutDir%\ImagesCommon >nul
xcopy /y /s ..\Templates\ImagesCommon\*.* %DoxOutDir%\ImagesCommon >nul

:Generate
doxygen Setup\%1.cfg
echo Finish Time:
time /t
goto End

:About
echo :
echo : Usage:  MakeSpecifiedDocs Which
echo :
echo :         Which may be "U3D" or "Test", case is important!
echo :
echo : If Test is specified, make sure you've first created
echo : a doxygen subdirectory in the Source\Tests branch
echo : ("Source\Tests\doxygen") and placed all relavent source
echo : in it that you want to test out the generated doxygen
echo : for.
echo :
goto End

:End
