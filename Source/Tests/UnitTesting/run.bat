@echo off
rem ***************************************************************************
rem 
rem   Copyright (c) 2004 - 2006 Intel Corporation
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

if "%1" == "debug" set DEBUG="debug"
if "%2" == "debug" set DEBUG="debug"
if "%1" == "clean" goto clean
if "%2" == "clean" goto clean
if "%1" == "rebuild" goto rebuild
if "%2" == "rebuild" goto rebuild
  perl daily.pl %DEBUG% 2>&1 | tee ../TestSystem/Results/build.txt
  goto eof
:clean
  perl clean.pl
  goto eof
:rebuild
  perl clean.pl | tee ../TestSystem/Results/build.txt
  perl daily.pl %DEBUG% 2>&1 | tee -a ../TestSystem/Results/build.txt
:eof
