#!/bin/sh
#***************************************************************************
#
#  Copyright (c) 2004 - 2006 Intel Corporation
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
#***************************************************************************

#
#  set write and executable mode for as for all perl (*.pl, *.pm) as for 
#  all shell ( *.sh) files    
#  work directory's : {U3D_SOURCE}Tests/UnitTesting/   and
#                     {U3D_SOURCE}/TestSystem/Tools/
#

       find . -name "*.sh" -exec chmod a+x \{\} \;
       find . -name "*.pl" -exec chmod a+r+x \{\} \;
       find . -name "*.pm" -exec chmod a+r+x \{\} \;
       
       cd ../TestSystem/Tools/
       
       find . -name "*.sh" -exec chmod a+x \{\} \;
       find . -name "*.pl" -exec chmod a+x \{\} \;
       find . -name "*.pm" -exec chmod a+x \{\} \;
       
       
exit 0;

