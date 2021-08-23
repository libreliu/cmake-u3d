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

@opt = @ARGV;
$sync = "p4 sync ";
$src = "//depot/CWG/C3D/";
$log = "sync.log";
$out = " | tee -a $log";

$force = "";
$date = "";
$subdir = "";

   foreach( @opt )
   {
      if( /-h/ )
      {
         print "\nU3D Build Machine mainframe.\n";
         print "Usage: perl sync.pl [option]\n\n";
         print "Options:\n";
         print "-f\t\tforce sync\n";
         print "-h\t\thelp screen\n";
         exit 0;
      }
      elsif( /-f/ )
      {
         $force = "-f ";
      }
      elsif( /-d(\d\d\d\d\/\d\d\/\d\d)/ )
      {
         $date = "\@$1";
      }
      elsif( /-s(.*)/ )
      {
         $subdir = $1;
      }
      else
      {
         print "Error, wrong syntax!\n";
         exit 1;
      }
   }

   ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks) = stat($log);
   ($msec,$mmin,$mhour,$mmday,$mmon,$myear,$mwday,$myday,$misdst)=localtime($mtime);
   ($csec,$cmin,$chour,$cmday,$cmon,$cyear,$cwday,$cyday,$cisdst)=localtime(time);

   if( $mmday eq $cmday && $mmon eq $cmon && $myear eq $cyear )
   {
      $fh = IO::File->new(">> $log");
      print $fh "\n\n";
   }
   else
   {
      $fh = IO::File->new("> $log");
   }
   print $fh "Perforce sync started: $cmday.".($cmon+1).".".($cyear+1900).", $chour:$cmin:$csec GMT\n";
   print $fh "Executing: $sync$force$src$subdir...$date\n\n";
   close $fh;
   system "$sync$force$src$subdir...$date$out";
   $fh = IO::File->new(">> $log");
   close $fh;
