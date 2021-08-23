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
# adds all files found in directory
# into the @files array
#
# $_[0] - directory name
#
{
  my $curdir = $_[0];
  opendir cur, $curdir;
  my @localfiles = sort (readdir cur);
  closedir cur;
  for(my $j=2; $j<@localfiles; $j++)
  {
    $subdir = $curdir."/".$localfiles[$j];
    $status = -d($subdir);
    if($status == 1) {&addf($subdir);}
    else {@files = (@files,$subdir);}
  }
}

$global_root = "../TestSystem/Tests";
@files = ();
@tests = ();
addf $global_root;
foreach( @files )
{
    if( $^O =~ m/Win32/i ||  $^O =~ m/CYGWIN/i )
    {
        if(/^.*[\/\\]([^\/\\]*[\/\\][^\/\\]*\.exe$)/)
        {
          push( @tests, $1 );
        }
    } else {
        if(/^.*[\/\\]([^\/\\]*[\/\\][^\/\\]*\.out$)/)
        {
          push( @tests, $1 );
        }
    }
}

open ALL, ">$global_root/../Config/all";
foreach( @tests )
{
    print ALL "$_\n";
}
