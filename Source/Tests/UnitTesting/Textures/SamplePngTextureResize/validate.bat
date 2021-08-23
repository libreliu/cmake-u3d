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
cd TestScenes

rem 
rem OGL
rem 
set SRC=%U3D_SOURCE%Build\u3d\SamplePlayer.exe
    
set TST=Flag_128.u3d
if exist %TST% (
   %SRC% -i%TST%  -rogl -t1
) 
 
set TST=Flag_256.u3d
if exist %TST% (
   %SRC% -i%TST%  -rogl -t1
)
    
set TST=Flag_512.u3d
if exist %TST% (
   %SRC% -i%TST%  -rogl -t1
)
 
