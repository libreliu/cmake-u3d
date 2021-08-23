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

cd %U3D_SOURCE%Samples\IntermediateFormat\TestScenes
%U3D_SOURCE%Build\u3d\IDTFConverter.exe -debuglevel 0 -input shading.idtf -output %U3D_SOURCE%Tests\UnitTesting\Modifiers\SampleTextureModifier\shading.u3d
%U3D_SOURCE%Build\u3d\IDTFConverter.exe -debuglevel 0 -input shading.idtf -output %U3D_SOURCE%Tests\UnitTesting\Shaders\UVTransform\shading.u3d
%U3D_SOURCE%Build\u3d\IDTFConverter.exe -debuglevel 0 -input sphere_shading.idtf -output %U3D_SOURCE%Tests\UnitTesting\Shaders\UVWrap\sphere_shading.u3d
%U3D_SOURCE%Build\u3d\IDTFConverter.exe -debuglevel 0 -input simpleBox.idtf -output %U3D_SOURCE%Tests\UnitTesting\Modifiers\SampleSkeleton\simpleBox.u3d
%U3D_SOURCE%Build\u3d\IDTFConverter.exe -debuglevel 0 -input simpleBox.idtf -output %U3D_SOURCE%Tests\UnitTesting\Modifiers\SampleCLODModCLODMod\simpleBox.u3d
%U3D_SOURCE%Build\u3d\IDTFConverter.exe -debuglevel 0 -input mp_box.idtf -output %U3D_SOURCE%Tests\UnitTesting\Modifiers\SampleCLODMod\mp_box.u3d

cd %U3D_SOURCE%Tests\Scenes
%U3D_SOURCE%Build\u3d\IDTFConverter.exe -debuglevel 0 -input box_ba_r.idtf -output %U3D_SOURCE%Tests\UnitTesting\SamplePlayer\SimpleTestNR.u3d
%U3D_SOURCE%Build\u3d\IDTFConverter.exe -debuglevel 0 -input mp_boxes.idtf -output %U3D_SOURCE%Tests\UnitTesting\SamplePlayer\SimpleTestMP.u3d
%U3D_SOURCE%Build\u3d\IDTFConverter.exe -debuglevel 0 -input threeLevelHierarchy.idtf -output %U3D_SOURCE%Tests\UnitTesting\Modifiers\AddRemoveModifier\threeLevelHierarchy.u3d
%U3D_SOURCE%Build\u3d\IDTFConverter.exe -debuglevel 0 -input AppXform2N.idtf -output %U3D_SOURCE%Tests\UnitTesting\MultiParent\ApplyXform2Node\AppXform2N.u3d
%U3D_SOURCE%Build\u3d\IDTFConverter.exe -debuglevel 0 -input GetSubgraphBound.idtf -output %U3D_SOURCE%Tests\UnitTesting\MultiParent\GetSubgraphBound\GetSubgraphBound.u3d
