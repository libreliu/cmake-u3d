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


sub source
#
# returns all files in the @files array that have specified extension
# $_[0] - needed extension
#
{
  my $extf = $_[0];
  my @res = ();
  foreach( @files )
  {
    if( index( $_, ".$extf" ) == ((length $_) - (length ".$extf")) && index( $_, ".$extf" ) > -1 )
    {
      push( @res, $_ );
    }
  }
  return @res;
}


#$utdir = "D:/_test";
$utdir = $ARGV[0];
#$utname = "SampleUnitTest";
$utname = $ARGV[1];
#$num = 2;
$num = $ARGV[2];
#$timeout = 10;
$timeout = $ARGV[3];

opendir cur, $utdir;
@files = sort (readdir cur);
closedir cur;

$isLauncher = 0;
foreach( @files )
{
  if( eval("/^".$utname."Launcher\\.cpp\$/") )
  {
    $isLauncher = 1;
  }
}

if( !$isLauncher )
{
  print "Test launcher does not exist!\n";
  exit 1;
}

open LNC, "<$utdir/$utname"."Launcher.cpp";
@lines = <LNC>;
close LNC;
if( $lines[0] !~ /^\/\/Category: (\w+)$/ )
{
  print "Wrong category!\n";
  exit 1;
}

$cat = $1;
$i = 1;
@cats = ();
while( $lines[$i] =~ /^\/\/Category: (\w+)$/ )
{
  push( @cats, $1 );
  $i++;
}

@src = source "cpp";
push( @src, "$utname.h" );

foreach( @src )
{
  print "$_\n";
}

system "hidden/md.bat $cat";
system "hidden/md.bat $cat/$utname";

foreach( @src )
{
  system "hidden/copy.bat \"$utdir\" $_ $cat/$utname";
}

system "hidden/dbin.bat";
open DB, "<C:/temp/test.tdb";
@lines = <DB>;
close DB;

$testdata = "$utname $cat";
$exists = 0;
foreach( @lines )
{
  if( index( $_, $testdata ) >= 0 )
  {
    $exists = 1;
  }
}
if( $exists )
{
  print "Test exists, overwriting...\n";
}
else
{
  $testdata .= " $num $timeout";
  foreach( @cats )
  {
    $testdata .= " $_";
  }
  $testdata .= "\n";
  push( @lines, $testdata );
  open DB, ">>C:/temp/test.tdb";
  print DB $testdata;
  close DB;
}
system "hidden/dbout.bat";
