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
#  convert all {U3D_SOURCE}Samples/IntermediateFormat/TestScenes/*.idtf 
#

cd ./TestScenes/

# clean all previously generated u3d files as all core dump files
rm -f *.u3d
rm -f core.*;

fails=0;
pass=0;
all=0;      

declare -a U3D
      
FILELIST=`ls *.idtf`
for I in $FILELIST;
do
   
   echo "";         
   echo "............. . converting    $I  .................";
   echo "";
   perl ../convert.pl $I
   answer=$?
   if [ "$answer" == "1" ]; then
       
       echo ""
       echo "===========================================================";
       echo "      	ERROR DURING \" $I \"  convertion               ";
       echo "==========================================================="; 
       echo ""
       
      
      fails=$[fails+1]
      U3D[fails]=$I;
   
   else
   
       echo "";							
       echo "	$I was converted successfully ! ";			
       echo "";
       
       pass=$[pass+1]   
   fi   
   
   all=$[all+1];
        		    
done

#  
# found and report about all failures
#

if [ "$fails" != "0" ]; then

   echo " ****************** the following tests failed ***************"; 
   for I in “${U3D[@]}”
   do
      echo "  error : $I ";
   done  
   echo " *************************************************************"; 
fi


   
   echo "  THE COMMON RESULTS :"        
   echo "  failures -------------------	: $fails";
   echo "  passed   -------------------	: $pass";
   echo "  total processed files ------	: $all";
   
   
exit 0;
