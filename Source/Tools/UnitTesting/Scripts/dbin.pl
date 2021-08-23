#! /usr/bin/perl
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

$dst = $ARGV[0];
$dst =~ s/\\/\//g;
if( $dst )
{
#  system " cp -f $path ".$dst;
  system "hidden/dbin.bat ".$dst;
  if( -d $dst )
  {
    if( $dst =~ /[\\\/]$/ )
    {
      $path = $dst."test.tdb";
    }
    else
    {
      $path = $dst."/test.tdb";
    }
  }
  elsif( -e $dst )
  {
    $path = $dst;
  }
  else
  {
    print "something\'s wrong...\n";
    exit 1;
  }
}
else
{
  $path = "C:/temp/test.tdb";
  system "dbin.bat $path";
}
print "table was copied to $path\n";
open DB, "+<$path";
@lines = <DB>;
@nl = ();
foreach( @lines )
{
  /(^[^ ]*) ([^ ]*)/;
  $line = "$2/$1.exe\r\n";
  push( @nl, $line );
}
seek DB, 0, 0;
print DB @nl;
truncate DB, tell DB;
close DB;