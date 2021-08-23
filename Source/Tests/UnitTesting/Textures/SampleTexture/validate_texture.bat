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
    
set TST=__shading_dst_1_PNG.u3d
if exist %TST% (
   %SRC% -i%TST%  -rogl -t3
) 
 
set TST=__shading_dst_1_jpeg24.u3d
if exist %TST% (
   %SRC% -i%TST%  -rogl -t3
)
       
set TST=__shading_dst_1_RGBA_PNG.u3d
if exist %TST% (
   %SRC% -i%TST%  -rogl -t3
) 

set TST=__shading_dst_2_RGBA_JPEG24_JPEG8.u3d        
if exist %TST% (
   %SRC% -i%TST%  -rogl -t3
) 

set TST=__shading_dst_2_RGBA_JPEG24_PNG.u3d        
if exist %TST% (
   %SRC% -i%TST%  -rogl -t3
) 

set TST=__shading_dst_2_RGBA_PNG_JPEG8.u3d
if exist %TST% (
   %SRC% -i%TST%  -rogl -t3
) 

set TST=__shading_dst_2_RGBA_PNG_PNG.u3d
if exist %TST% (
   %SRC% -i%TST%  -rogl -t3
) 

set TST=__shading_dst_3_jpeg8.u3d
if exist %TST% (
   %SRC% -i%TST%  -rogl -t3
) 

set TST=__shading_dst_3_png.u3d
if exist %TST% (
   %SRC% -i%TST%  -rogl -t3
) 

set TST=__shading_dst_4_RGBA_JPEG8.u3d
if exist %TST% (
   %SRC% -i%TST%  -rogl -t3
) 

set TST=__shading_dst_4_RGBA_PNG.u3d
if exist %TST% (
   %SRC% -i%TST%  -rogl -t3
) 

set TST=__shading_dst_4_RGBA_1JPEG8+3PNG.u3d
if exist %TST% (
   %SRC% -i%TST%  -rogl -t3
) 

set TST=__shading_dst_4_RGBA_1PNG+3JPEG8.u3d
if exist %TST% (
   %SRC% -i%TST%  -rogl -t3
) 

set TST=__shading_dst_4_RGBA_2JPEG8+2PNG.u3d
if exist %TST% (
   %SRC% -i%TST%  -rogl -t3
) 

set TST=__shading_dst_4_RGBA_2PNG+2JPEG8.u3d
if exist %TST% (
   %SRC% -i%TST%  -rogl -t3
) 

set TST=__shading_dst_4_RGBA_3JPEG8+1PNG.u3d
if exist %TST% (
   %SRC% -i%TST%  -rogl -t3
) 

set TST=__shading_dst_4_RGBA_3PNG+1JPEG8.u3d
if exist %TST% (
   %SRC% -i%TST%  -rogl -t3
) 

set TST=__shading_dst_1_RGB_JPEG24_1_FILE.u3d
if exist %TST% (
   %SRC% -i%TST%  -rogl -t3
) 

set TST=__shading_dst_1_RGB_PNG24_1_FILE.u3d
if exist %TST% (
   %SRC% -i%TST%  -rogl -t3
) 

set TST=__dst_4_RGBA_PNG_mixed1.u3d
if exist %TST% (
   %SRC% -i%TST%  -rogl -t3
) 

set TST=__dst_4_RGBA_PNG_mixed2.u3d
if exist %TST% (
   %SRC% -i%TST%  -rogl -t3
) 

set TST=__dst_4_RGBA_PNG_mixed3.u3d
if exist %TST% (
   %SRC% -i%TST%  -rogl -t3
) 

set TST=__dst_4_RGBA_PNG_mixed4.u3d
if exist %TST% (
   %SRC% -i%TST%  -rogl -t3
) 

set TST=__shading_dst_1_RGB_PNG32_1_FILE.u3d
if exist %TST% (
   %SRC% -i%TST%  -rogl -t3
) 

