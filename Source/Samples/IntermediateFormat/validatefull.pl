#usr/bin/perl/
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

use strict;
use IO::Dir;
use File::Find;
use File::Path;
use File::Copy;

#
#  0. convert all idtf files from ./TestScene dir
#  played all converted *.u3d files  with the folowing options
#  1.  OGL, default options
#  2.  OGL SCENEGRAPH NONATOM
#  3.  OGL OCTREE ATOM
#  4.  DX8 SCGRAPH NONATOMIC
#  5.  DX8 Atomic Culling
#  6.  DX7 ATOMIC, OCTREE CULLING
#  7.  DX7 SCENEGRAPH NONATOM
#
#   all results will be colleted into _VALIDATION_RESULT.txt file
#

my $errcount=0;

my $path=$ENV{U3D_SOURCE};
$path   =~ s/\\/\//g;
my $TestScenePath =$path."Samples/IntermediateFormat/TestScenes/";
my $run =$path."Build/U3D/IDTFConverter.exe";

if ( !-e $run ) {
    print " couldn't find $run \n\n";
    exit 1;
}

    my @all_files=();
    opendir SRC, $TestScenePath;
    my @all_files = readdir SRC;
    closedir SRC;
    my @idtf_files = grep( $_ =~ m/\.idtf$/, @all_files );

#
#
#
my $filelog = "_VALIDATION_RESULT.log";
open FH, ">$filelog";
my @u3d_arr=();

print FH "\n\n";
print FH "\t********************************************* \n";
print FH "\t\t  CONVERTION PROCEDURE START \n";

# 0
#
# launching convertation all *.idtf files to *.u3d
#
my $command;
my $returnCode;
my $dstname;
chdir($TestScenePath);
foreach my $srcname (@idtf_files)
{
    $dstname=$srcname;
    $dstname=~ s/idtf$/u3d/; #print"\t $dstname\n";
    push(@u3d_arr,$dstname);

    $command = "$run -dl 1 -input $TestScenePath$srcname -output $TestScenePath$dstname";
    print " \t\t command -- $command\n";
    system "$command";

   $returnCode=$?;
   if( $returnCode != 0 )
   {
          print FH "\t\t convertion failed during $srcname \n";
          $errcount++;
   }

}#_foreach_
print FH "\t\t  CONVERTION PROCEDURE FINISH \n";
print FH "\t********************************************* \n";
print FH "\n\n";

#1
#
#  validate_default.bat  OGL, default options
#
    print FH "\n\n";
    print FH "\t********************************************* \n";
    print FH "\tOGL DEFAULT TESTING START \n";

    my $command;
    my $returnCode;
    my $run = $path."Build/U3D/SamplePlayer.exe";
    foreach my $srcname (@u3d_arr)
    {
        $command = "$run -i$TestScenePath$srcname -t3";
        print " \t\t command -- $command\n";
        system "$command";
       $returnCode=$?;
       print " $command\n";
       if( $returnCode != 0 )
       {
          print FH "\t\t OGL validation failed : $srcname, $returnCode \n";
          $errcount++;
       }
    }#_foreach_
    print FH "\tOGL DEFAULT TESTING FINISH \n";
    print FH "\t********************************************* \n";
    print FH "\n\n";

#2
#
#  validate_default.bat  OGL, scenegraph options
#
    print FH "\n\n";
    print FH "\t********************************************* \n";
    print FH "\tOGL SCENEGRAPH NONATOM TESTING START \n";

    my $command;
    my $returnCode;
    foreach my $srcname (@u3d_arr)
    {
        $command = "$run -i$TestScenePath$srcname -t3 -lnatom";
        print " \t\t command -- $command\n";
        system "$command";
       $returnCode=$?;
       print " $command\n";
       if( $returnCode != 0 )
       {
          print FH "\t\t OGL validation failed : $srcname, $returnCode \n";
          $errcount++;
       }
    }#_foreach_
    print FH "\tOGL SCENEGRAPH NONATOM  TESTING FINISH \n";
    print FH "\t********************************************* \n";
    print FH "\n\n";

#3
#
#  validate_default.bat  OGL, OCTREE ATOM options
#
    print FH "\n\n";
    print FH "\t********************************************* \n";
    print FH "\t\OGL OCTREE ATOM TESTING START \n";

    my $command;
    my $returnCode;
    foreach my $srcname (@u3d_arr)
    {
        $command = "$run -i$TestScenePath$srcname -t3 -coct";
        print " \t\t command -- $command\n";
        system "$command";
       $returnCode=$?;
       print " $command\n";
       if( $returnCode != 0 )
       {
          print FH "\t\t OGL convertion failed during $srcname \n";
          $errcount++;
       }
    }#_foreach_
    print FH "\tOGL OCTREE ATOM TESTING FINISH \n";
    print FH "\t********************************************* \n";
    print FH "\n\n";

#4
#
#  validate_dx8 NONATOMIC, default options
#
    print FH "\n\n";
    print FH "\t********************************************* \n";
    print FH "\tDX8 SCGRAPH NONATOMIC TESTING START \n";

    foreach my $srcname (@u3d_arr)
    {
       $command = "$run -i$TestScenePath$srcname -t3 -rdx8 -lnatom";
       system "$command";
       $returnCode=$?;
       print " $command\n";

       if( $returnCode != 0 )
       {
            print FH "\t\t DX8 convertion failed during $srcname \n";
            $errcount++;
       }
    }#_foreach_
    print FH "\tDX8 SCGRAPH NONATOMIC TESTING FINISH \n";
    print FH "\t********************************************* \n";
    print FH "\n\n";

#5
#
#  validate_dx8 NONATOMIC, default options
#
    print FH "\n\n";
    print FH "\t********************************************* \n";
    print FH "\tDX8 Atomic Culling TESTING START \n";

    foreach my $srcname (@u3d_arr)
    {
       $command = "$run -i$TestScenePath$srcname -t3 -rdx8 -coct";
       system "$command";
       $returnCode=$?;
       print " $command\n";

       if( $returnCode != 0 )
       {
            print FH "\t\t DX8 convertion failed during $srcname \n";
            $errcount++;
       }
    }#_foreach_
    print FH "\tDX8 Atomic Culling TESTING FINISH \n";
    print FH "\t********************************************* \n";
    print FH "\n\n";



#6
#
#  validatE DX7 ATOMIC, OCTREE CULLING options
#

    print FH "\n\n";
    print FH "\t********************************************* \n";
    print FH "\tDX7  ATOMIC Culling TESTING START \n";

    foreach my $srcname (@u3d_arr)
    {
      $command = "$run -i$TestScenePath$srcname -t3 -rdx7 -coct";
      system "$command";
      $returnCode=$?;
      print " $command\n";

       if( $returnCode != 0 )
       {
            print FH "\t\t DX7 convertion failed during $srcname \n";
            $errcount++;
       }
    }#_foreach_
    print FH "\tDX7 Atomic Culling TESTING FINISH \n";
    print FH "\t********************************************* \n";
    print FH "\n\n";

#7
#
#  validatE DX7 SCGRAPH NONATOMIC CULLING options
#

    print FH "\n\n";
    print FH "\t********************************************* \n";
    print FH "\tDX7  SCENEGRAPH NONATOM TESTING START \n";

    foreach my $srcname (@u3d_arr)
    {
      $command = "$run -i$TestScenePath$srcname -t3 -rdx7 -lnatom";
      system "$command";
      $returnCode=$?;
      print " $command\n";

       if( $returnCode != 0 )
       {
            print FH "\t\t DX7 convertion failed during $srcname \n";
            $errcount++;
       }
    }#_foreach_
    print FH "\tDX7 SCENEGRAPH NONATOM TESTING FINISH \n";
    print FH "\t********************************************* \n";
    print FH "\n\n";

close FH;

#
#  output result report
#
if( $errcount > 0 ){
    print"\n\n\n";
    print"\t\t\t  VALIDATION FAILED with  $errcount ERRORS \n";
    print"\t more detail info you can find into _VALIDATION_RESULT.log file ";
    print"\n\n\n";
} else {
    print"\n\n\n";
    print"\t\t\t  VALIDATION PASSED SUCCESSFULLY\n";
    print"\n\n\n";
}

exit(0);