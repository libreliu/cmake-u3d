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

$path = "test.tdb";
system "hidden/dbin.bat $path";
if( $ARGV[0] )
{
  system " cp -f $path ".$ARGV[0];
  if( -d $ARGV[0] )
  {
    if( $ARGV[0] =~ /[\\\/]$/ )
    {
      $path = $ARGV[0]."test.tdb";
    }
    else
    {
      $path = $ARGV[0]."/test.tdb";
    }
  }
  elsif( -e $ARGV[0] )
  {
    $path = $ARGV[0];
  }
  else
  {
    print "something\'s wrong...\n";
    exit 1;
  }
}

print "table was copied to $path\n";
open DB, "+<$path";
@lines = <DB>;
@nl = ();
foreach( @lines )
{
  ( $t1, $cat1, $t3, $t4, @cat ) = split;
  push( @cat, $cat1 );
  foreach $cat( @cat )
  {
    $new = 1;
    foreach $cur( @nl )
    {
      if( $cat eq $cur )
      {
         $new = 0;
      }
    }
    if( $new )
    {
      push( @nl, $cat );
    }
  }
}
foreach( @nl )
{
  $_ .= "\r\n";
}
seek DB, 0, 0;
print DB @nl;
truncate DB, tell DB;
close DB;