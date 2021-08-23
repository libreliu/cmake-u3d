rem ***************************************************************************
rem 
rem   Copyright (c) 2003 - 2006 Intel Corporation
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

rem   -------------------------------------------------------------------------
rem   convertion "project_win.xml" files description to .vsproj and .sln files
rem   -------------------------------------------------------------------------

   call cls

   perl collect_project_data.pl
   if errorlevel 1 goto :eof

   perl xml_parser.pl ../../Config/
   cp -f projects.ini ../../Config/
   rm -f projects.ini

   perl vs7_generate.pl

   :eof
