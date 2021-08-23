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

my $fname=$ARGV[0];

my $path=$ENV{U3D_SOURCE};

$path=$path."Samples/IntermediateFormat/TestScenes/";

my $dstname=$fname;

$dstname=~ s/idtf$/u3d/;

$dstname=$path.$dstname;

my $srcname = $path.$fname;

$run = $ENV{U3D_SOURCE} . "Build/U3D/IDTFConverter.out";

if ( !-e $run ) {

    print " ***************************************************** \n\n";

    print " couldn't find $run \n\n";

    print " ***************************************************** \n";

    exit 1;

}

my $command = "$run -input $srcname -output $dstname";



my $code;



   system "$command";

   $code=$?;

   my $exit_code =$code >> 8;

   my $signal_code =$code & 127;

   my $dumped_code=$code & 128;



   if( $exit_code != 0 || $dumped_code != 0 )

   {

        print "\t\t return code          ==  $code\n";

        print "\t\t return exit_code     ==  $exit_code\n";

        print "\t\t return signal_code   ==  $signal_code\n";

        print "\t\t return dumped_code   ==  $dumped_code\n";

        exit 1;

   }



 exit 0;

