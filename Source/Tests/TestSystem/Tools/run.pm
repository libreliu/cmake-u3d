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

use lib "../../../../Source/Tools/BuildSystem";
require util;

package run;
use strict;
use trun;
use IO::File;
use File::Copy;
use File::Basename;
use Text::ParseWords;

#
#input data : $arr_test - array test names: []  -> all tests as default
#                                           -a  -> all tests from ../Config/suite list
#                                           -t  -> the single name in the "../Tests/" dir
#                                           -s  -> array all test in the "../Tests/1exe" dir
#                                           -st -> test name  "../Tests/1exe/atom.exe"
#             $option - the first commamd line options like "-t","-s","-st"
#             $suitename - suite name
#             $opt2 - execute time (sec)
#

my $tests_path = $ENV{U3D_SOURCE}."Tests/TestSystem/";

sub run(\@\$\$\$\$) {

   my $arr_test=shift(@_);
   my $option=shift(@_);
   my $suitename=shift(@_);
   my $opt2 = shift(@_);
   my $RunningTimesDefault=shift(@_);
   my $RunningTimes;

   my $test;
   my $tmDefault=$$opt2;
   my $tm;
   my @out;
   my $prefix="../Tests/";

   #----finding all u3d files------

   my @files_list = util::GetFilesRecursive($tests_path);

   foreach my $file( @files_list )
   {
      if ( $file =~ /.u3d\Z/ )
      {
         system "rm -f '$file'";
      }
   }

   my $player = "";
   if( $^O =~ m/Win32/i ||  $^O =~ m/CYGWIN/i )
   {
        $player = "SamplePlayer.exe -i";
   }
   elsif( $^O =~ m/linux/i )
   {
        $player = "winXPlayer.out ";
   }
   elsif( $^O =~ m/darwin/i )
   {
        $player = "QrtzPlayer.out ";
   }

   foreach my $PrimaryName (@$arr_test)
   {
        $RunningTimes = $RunningTimesDefault;
        $tm = $tmDefault;
        $PrimaryName =~ /([^\\\/]*)[\\\/]([^\\\/\.]*)/;
        my $testcategory = $1;
        my $testname = $2;
        if( $testcategory && $testname )
        {
            open DB, "<../../UnitTesting/test.tdb";
            my @lines = <DB>;
            close DB;
            foreach( @lines )
            {
              if( /$testname $testcategory (\d*) (\d*)/ )
              {
                $tm = $1;
                $$RunningTimes = $2;
              }
            }
        }

      my $CurrName = $prefix . $PrimaryName;
      my $result_arr = [];

      my $ind;
      for($ind=0; $ind < $$RunningTimes; $ind++ )
      {
        print "        running $CurrName test ($tm seconds given)";

        #
        # catch output to $out variable
        #
        @out = trun::trun ($tm, $CurrName);
        find_and_get_trace($CurrName,$PrimaryName,@out);

        my @files_list = util::GetFilesRecursive($tests_path);

        foreach my $file( @files_list )
        {
            if ( $file =~ /.u3d\Z/ )
            {
                $tm = $tmDefault;
                $CurrName = $ENV{U3D_SOURCE}."Build/U3D/".$player.$file;
                $CurrName =~ tr%\\%\/%;
                print "\n        running $CurrName test ($tm seconds given)";
                $file =~ /([^\/\\]+)$/;
                my $newPrimaryName = $player." + ".$1." created by ".$PrimaryName;
                @out = trun::trun ($tm, $CurrName);
                find_and_get_trace($CurrName,$newPrimaryName,@out);
                system "rm -f '$file'";
            }
        }
        print "\n";
      }# for ($RunningTimes)

   }# foreach

}# run


#
# Search tracefile "trace.txt" in the current dir and validate it.
# If tracefile dosn't normal or it dosn't exist this function creates it.
#

sub find_and_get_trace
{
    my $fullpath=$_[0];     # path from Tools to the test place : ../Tests/Dummy/test_bad.exe
    my $PrimaryName=$_[1];  # like Dummy/test_bad.exe
    my @out = $_[2];        # trun report

    my $tracefile="trace.txt";
    my @files=();

    use IO::Dir;
    my $srcdir = "."; # current directory
    my $dir = new IO::Dir $srcdir;
    if (defined $dir)
    {
        @files=$dir->read();
    }

    # checking that the tracefile exist.
    my $istrace = 0;
    foreach(@files)
    {
        if ($_ =~ m/$tracefile/)
        {
            $istrace = 1;
        }
    }

    my $is_normal_trace = 0;

    # checking that if the tracefile exist it contains information about test.
    if( $istrace )
    {
        open TRACE, "$tracefile";
        my @lines = <TRACE>;
        my $syze = @lines;
        my @res_lines = ();
        close TRACE;

        open TR, ">$tracefile";

        for (my $i=0; $i<$syze; $i++)
        {
            my $str = @lines[$i];
            $str=~ s/\r//g;
            $str=~ s/\n//g;

            push( @res_lines, $str );

            if ( $str =~ /Test \"/ )
            {
                $is_normal_trace = 1;
            }
        }

        foreach( @res_lines )
        {
            print TR "\r\n$_";
        }

        close TR;
    }

    # checking if test is a simple validation of created u3d file with using of ***Player.
    my $isPlayer = 0;

    if  ( ( $fullpath =~ /SamplePlayer.exe/ ) || ( $fullpath =~ /winXPlayer.out/ ) || ( $fullpath =~ /QrtzPlayer.out/ ) )
    {
        $isPlayer = 1;
    }

    # if it is so creating simple tracefile for it.
    if ( $isPlayer )
    {
        open TR, ">$tracefile";
        print TR "\r\n";
        print TR "Test \"$PrimaryName\"\r\n\tResult: @out[0]";
        close TR;
    }
    else
    {
        # if test has normal tracefile and tracefile contains information about test.
        if ( $is_normal_trace )
        {
            #checking if tracefile contain information about failures or not.
            open TRACE, "<$tracefile";
            my @lines = <TRACE>;
            close TRACE;

            my $failed = 0;

            foreach(@lines)
            {
                if ( /Failed: (\d*)/ )
                {
                    if ( $1 > 0 )
                    {
                        $failed = 1;
                        last;
                    }
                }

            }

            # if tracefile doesn't contain information about failures and result of trun is not PASSED
            # creating tracefile with trun result.
            if ( $failed == 0 )
            {
                if ( @out[0] ne "Passed" )
                {
                    open TR, ">$tracefile";
                    print TR "\r\n";
                    print TR "Test \"$PrimaryName\"\r\n\tResult: @out[0]";
                    close TR;
                }
            }
        }
        else # if test hasn't normal tracefile creating it.
        {
            open TR, ">>$tracefile";
            print TR "\r\n";

            if ( @out[0] =~ /Failed/ )
            {
                print TR "Test \"$PrimaryName\"\r\n\tResult: @out[0]";
            }
            else
            {
                print TR "Test \"$PrimaryName\"\r\n\tResult: Failed - No Normal Trace File";
            }
            close TR;
        }
    }


               #rename
               my $name=basename($PrimaryName);

               my $OSTYPE=$^O; # MSWin32 - Windows, cygwin - under cygwin
               if($OSTYPE=~ m/MSWin32/i || $OSTYPE =~ m/CYGWIN/i )
               {
                  $name=~ s/.exe//;
               } else {
                  $name=~ s/.out//; #assumption - unix like OS, may be Linux Red Hat *.*
               }

               my $dst="../Results/daily.txt";
               my $html="../Results/daily.html";
#               my $list="../Results/list.inc";
               my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks) = stat($dst);
               my ($lnsec,$lnmin,$lnhour,$lnmday,$lnmon,$lnyear,$lnwday,$lnyday,$lnisdst)=localtime($mtime);
               my ($lnsecc,$lnminc,$lnhourc,$lnmdayc,$lnmonc,$lnyearc,$lnwdayc,$lnydayc,$lnisdstc)=localtime(time);
               my $htmlmon = ($lnmonc>8)?($lnmonc+1):("0".($lnmonc+1));
               my $htmlmday = ($lnmdayc>9)?($lnmdayc):("0".$lnmdayc);
               my $htmlyr = $lnyearc-100;
               $htmlyr = ($htmlyr>9)?($htmlyr):("0".($htmlyr));
               my $htmlname = $htmlmday.$htmlmon.$htmlyr.".html";
#               $html .= $htmlname;
               my @lines=();
               if( $lnmday == $lnmdayc && $lnmon == $lnmonc && $lnyear == $lnyearc )
               {
                 open DL, ">>$dst";
                 open HTML, "<$html";
                 my @ol = <HTML>;
                 for( my $i=0; $i<@ol-1; $i++ )
                 {
                   push( @lines, $ol[$i] );
                 }
                 close HTML;
                 open HTML, ">$html";
               }
               else
               {
                 $lnmonc = ($lnmonc>8)?($lnmonc+1):("0".($lnmonc+1));
                 $lnmdayc = ($lnmdayc>9)?($lnmdayc):("0".$lnmdayc);
                 $lnhourc = ($lnhourc>9)?($lnhourc):("0".($lnhourc));
                 $lnminc = ($lnminc>9)?($lnminc):("0".($lnminc));
                 $lnsecc = ($lnsecc>9)?($lnsecc):("0".($lnsecc));
                 open DL, ">$dst";
                 open HTML, ">$html";
                 print DL "Daily testing, ".$lnmdayc.".".$lnmonc.".".($lnyearc+1900).", $lnhourc:$lnminc:$lnsecc GMT\r\n\r\n";
                 print HTML "<table border=0 cellspacing=1 cellpadding=0 width=\"100%\"><tr><td nowrap>";
                 print HTML "Daily testing, ".$lnmdayc.".".$lnmonc.".".($lnyearc+1900).", $lnhourc:$lnminc:$lnsecc GMT<br>\n";
                 print HTML "</td></tr></table>";
                 print HTML "<table border=0 cellpadding=0 cellspacing=0><tr><td style=\"background-color = '#a0a0a0'; padding = 0px;\">\n";
               }
               open TRACE, $tracefile;
               print HTML @lines;
               @lines = <TRACE>;
               print DL @lines;
               print HTML "<table border=0 cellspacing=1 cellpadding=0 width=\"100%\"><tr><td>";
               foreach( @lines )
               {
                 if( /(.*Test ")([^"]*)(".*)$/ )
                 {
                    print HTML "$1<b>$2</b>$3<br>";
                 }
                 elsif( /(.*Failed: )(\d+)(.*)$/ )
                 {
                    print HTML "$1<b>$2</b>$3<br>";
                 }
                 else
                 {
                    print HTML "$_<br>";
                 }
               }
               print HTML "</td></tr></table>";
               print DL "\r\n\r\n";
               close TRACE;
               close DL;
               print HTML "\n</td></tr></table></body></html>";
               close HTML;
                unlink ($tracefile);


}# _find


1;
