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
      @files = ( @files, &addf( $subdir ) );
    }
  }
  return @files;
}

sub cat
{
  my $utdir = $_[0];
  my $utname = $_[1];
  opendir cur, $utdir;
  my @files = sort (readdir cur);
  closedir cur;

  my $isLauncher = 0;
  foreach( @files )
  {
    if( eval("/^".$utname."Launcher\\.cpp\$/") )
    {
      $isLauncher = 1;
    }
  }

  if( !$isLauncher )
  {
    print "$utname: test launcher does not exist!\n";
    return 1;
  }

  open LNC, "<$utdir/$utname"."Launcher.cpp";
  my @lines = <LNC>;
  close LNC;

  my $size = @lines;
  my $i = 0;

  for (; $i < $size; $i++)
  {
    if ( @lines[$i] =~ /^\/\/Exec: / )
    {
	last;
    }
  }

  if( ($i+2 >= $size) || ( $lines[$i+2] !~ /^\/\/Category: (\w+)/ ) )
  {
    print "$utname: wrong category!\n";
    return 1;
  }
  my $cat = $1;

  if( $lines[$i] !~ /^\/\/Exec: (\w+)/ )
  {
    print "$utname: wrong execution time!\n";
    return 1;
  }
  my $exec = $1;

  if( $lines[$i+1] !~ /^\/\/Timeout: (\w+)/ )
  {
    print "$utname: wrong timeout!\n";
    return 1;
  }
  my $timeout = $1;

  $i = $i+3;
  @cats = ( $cat, $exec, $timeout );
  while( ( $i < $size ) && ( $lines[$i] =~ /^\/\/Category: (\w+)/ ) )
  {
    push( @cats, $1 );
    $i++;
  }
  return @cats;
}

@dir = addf ".";

open DB, ">test.tdb";

foreach( @dir )
{
  /^\.[\\\/]([^\\\/]*)[\\\/]([^\\\/]*$)/;
  if( $1 && $2 )
  {
#    print "$_ 1: $1, 2: $2\n";
    cat $_, $2;
#    print @cats;
#    print "\n";
    $testdata = $2." ".$cats[0];

    for( $i = 1; $i < @cats; $i++ )
    {
      $testdata .= " ".$cats[$i];
    }
    $testdata .= "\n";
    print DB $testdata;
  }
}

close DB;
