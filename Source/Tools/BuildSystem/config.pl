#!/usr/bin/perl
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

my $root=$ENV{U3D_SOURCE};
$root=~ s/\\/\//;
$conf = $root."Config/u3d.conf";
@settings = ( "U3D_PLATFORM", "U3D_COMPILER", "U3D_SLIB_LINKER", "U3D_DLIB_LINKER", "U3D_EXEC_LINKER", "U3D_OUTPUT" );

if( !$ARGV[0] )
{
  print "\n\nConfiguration script for the Universal 3D build system. Usage:\n\nperl config.pl <arg0> ... <argN>\n\nWhere\n";
  for $i( 0..@settings-1 ) {print "<arg$i> is the new value of $settings[$i] setting\n" }
  print "\nAdditional info about these parameters can be found in the u3d.conf file.\nIf a certain value is not intended to be changed";
  print " then the corresponding argument should be set to \"-\".\nFor example, to switch to Linux from the default settings";
  print " use the following command line:\n\nperl config.pl Lin32 gcc ar g++ g++ -\n\nThe trailing -\'s can be omitted so this command line can be also typed as\n\n";
  print "perl config.pl Lin32 gcc ar g++ g++\n\nAnother example shows how to switch to Intel compiler\n\nperl config.pl - icl\n\nand back:\n\nperl config.pl - cl\n";
}

# remove read-only attribute from u3d.conf
chmod 0777, $conf;

open CONF, "<$conf";
@lines = <CONF>;
close CONF;

open CONF, ">$conf";
foreach $line( @lines )
{
  for $i( 0..@settings-1 )
  {
    #if it's a proper setting
    if( $line =~ /^([^\w\#]*$settings[$i]\s*=\s*)(\S+[\x20-\xFF]*)/ && $ARGV[$i] && $ARGV[$i] ne "-" )
    {
      #then apply command-line argument here
      print CONF "$1$ARGV[$i]\n";
      $count = 1;
      last;
    }
  }
  if( !$count ) { print CONF $line }
  else { $count = 0 }
}
close CONF;
