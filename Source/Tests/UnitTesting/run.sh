#!/bin/bash
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
     
     if [ "$1" == "debug" ]; then
	    
	    echo " ============= debug version =================== ";							
	    perl daily.pl $1 2>&1 | tee ../TestSystem/Results/build.txt;
	    exit 0;
	
     elif [ "$1" == "clean" ]; then
     
	    echo " ============= cleaning =================== ";
	    perl clean.pl;
	    exit 0;
     
     elif [ "$1" == "rebuild" ]; then
	 
	 echo " ========== rebuild (debug version )====================";
	 perl clean.pl | tee ../TestSystem/Results/build.txt;
         perl daily.pl debug 2>&1 | tee -a ../TestSystem/Results/build.txt;
	 
     elif [ "$1" == "" ]; then  	     
      
      echo " ============= release version =================== ";  		       
      perl daily.pl $1 2>&1 | tee ../TestSystem/Results/build.txt;
			    
     else
     
echo " input parameter isn't allowed 		";
echo "";
echo " legal input parameters: debug, clean, rebuild or empty parameter "; 
echo " EXAMPLES :"
echo " ./run.sh  		---	build RELEASE  version";
echo " ./run.sh debug		---	build DEBUG    version";
echo " ./run.sh rebuild 	---	rebuild DEBUG  version";
echo " ./run.sh clean   	---	all debug & release configurations will be removed  "; 
echo "";
echo " keep input parameters in lowercase ";

     fi
     
exit 0;

