#!/bin/bash
#***************************************************************************
#
#  Copyright (c) 2003 - 2006 Intel Corporation
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

# START .........................................                                                                                                      
#  ..........  changing make.def ................

# parsing input config file
perl collect_project_data.pl;
answer=$?
   if [ "$answer" == "1" ]; then
       echo "=====================================";
       echo " ERROR, check workspace list         " 
       echo "=====================================";
       exit 1;
   fi

   perl xml_parser.pl ../../Config/
   cp -f projects.ini ../../Config/
   rm -f projects.ini

#
# launch the main command line parsing procedure
#
perl parsing.pl "$@"
answer=$?
if [ "$answer" == "1" ]; then
    echo "=====================================";
    echo " ERROR, build process is not complete" 
    echo "=====================================";
    exit 1;
fi   


exit 0;
