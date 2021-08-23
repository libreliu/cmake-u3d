#!/usr/bin/perl

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



use IO::File;



sub addf

#

# adds all subdirectories found in directory

# into the @files array

#

# $_[0] - directory name

#

{

  my @files = ();

  my $curdir = $_[0];

  opendir cur, $curdir;

  my @localfiles = sort (readdir cur);

  closedir cur;

  for(my $j=2; $j<@localfiles; $j++)

  {

    $subdir = $curdir."/".$localfiles[$j];

    $status = -d($subdir);

    if($status == 1)

    {

      @files = ( @files, $subdir );

    }

  }

  return @files;

}



sub delf

{

  my $file = $_[0];

  my $answer = unlink $file;

  if ( $answer != 1 )

  {

#    $FAIL = 1;

#    print "deleting $file... ERROR!\n";

  }

  else

  {

#    print "deleting $file... OK\n";

  }

}



$FAIL = 0;



$global_root = ".";

#unlink "./_make.inc";

#unlink "./_vcproj.inc";

#unlink "./../TestSystem/Results/daily.txt";

#unlink "./../TestSystem/Results/builderr.txt";

print "CLEAN STARTED\n";

delf "./_make.inc";

delf "./_vcproj.inc";

delf "./../TestSystem/Results/daily.txt";

delf "./../TestSystem/Results/build.txt";

delf "./../TestSystem/Results/builderr.txt";



@wcat = addf "./../TestSystem/Tests";



foreach( @wcat )

{

  opendir CUR, "$_";

  @files = sort (readdir CUR);

  closedir CUR;

  foreach $file( @files )

  {

    delf "$_/$file";

  }

  rmdir "$_";

}





@wcat = addf $global_root;



foreach( @wcat )

{

  @wtest = addf $_;

  foreach( @wtest )

  {

    opendir CUR, "$_/debug";

    @files = sort (readdir CUR);

    closedir CUR;

    foreach $file( @files )

    {

      delf "$_/debug/$file";

    }

    rmdir "$_/debug";

    opendir CUR, "$_/release";

    @files = sort (readdir CUR);

    closedir CUR;

    foreach $file( @files )

    {

      delf "$_/release/$file";

    }

    rmdir "$_/release";

  }

}

if( $FAIL == 1 )

{

  print "CLEAN FAILED\n";

}

else

{

  print "CLEAN SUCCEEDED\n";

}

