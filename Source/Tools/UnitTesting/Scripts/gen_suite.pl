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

#$path = "C:/temp/test.ts";
#system "dbin.bat $path";

#$path = "C:/temp/test.tdb";
$path = "test.tdb";
system "hidden/dbin.bat $path";

print "table was copied to $path\n`";
open DB, "+<$path";
@lines = <DB>;
@nl = ();
if( !$ARGV[0] )
{
  print "parameter missing\n\n";
  exit 1;
}

print "table was copied to $path\n";
open DB, "+<$path";
@lines = <DB>;
@nl = ();
$cat = $ARGV[0];
foreach( @lines )
{
  /(^[^ ]*) (.*$)/;
  $f = $2;
  if ( $f =~ /$cat/i )
  {
    /(^[^ ]*) ([^ ]*)/;
    $line = "$2/$1.exe\n";
    push( @nl, $line );
  }
}
seek DB, 0, 0;
print DB @nl;
truncate DB, tell DB;
close DB;
system "confout.bat $path -i ".$ARGV[0];