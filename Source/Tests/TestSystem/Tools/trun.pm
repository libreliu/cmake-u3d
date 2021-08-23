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

package trun;
use strict;
use warnings;
use Config;
use POSIX ":sys_wait_h";
use Text::ParseWords;

my $child_killed = 0;

sub trun
{
    my $tm = shift(@_);
    my $CurrName = shift(@_);
    my @res = ();

    if( $^O =~ m/Win32/i ||  $^O =~ m/CYGWIN/i )
    {
        @res = wtrun ($tm, $CurrName);
    }
    else
    {
        @res = ltrun ($tm, $CurrName);
    }
    return @res;
}

###############################################################################
#                                                                             #
#   Input Parameters :                                                        #
#    1 - execution time (sec)                                                 #
#    2 - execution file name ( don't forget to add "./" specially for linux ) #
#   Output results :                                                          #
#             Passed                                                          #
#             Passed - with warnings                                          #
#             Failed - Segmentation Fault                                     #
#             Failed - Terminate                                              #
#             Failed - Unrecognized Exception                                 #
#                                                                             #
###############################################################################
sub wtrun
{
    my $tm = shift(@_);
    my $CurrName = shift(@_);

    my $errcode  = -1;
    my $ext_code = -1;
    my $signal   = -1;
    my $core     = -1;
    my $result   = -1;
    my @res = ( "" , "OK" );

    my $out = `trun.exe $tm $CurrName`;
    my @words = &Text::ParseWords::quotewords(' ',0,$out );
    my $sz = @words;

    my $trun_out = $words[$sz-1];

    $trun_out =~ s/\s+\z$//;
    $trun_out =~ s/\.$//;
    $trun_out =~ /(\d+)$/;
    $errcode = $1;

    if ($errcode ne "")
    {
        $ext_code = ( $1 >> 8 );
        $signal = ( $1 & 127 );
        $core = ( $1 & 128 );
    }
    else
    {
        $errcode = -1;
        $signal  = -1;
        $core    = -1;
        $res[1] = "Timeout";
    }

    if ( $errcode == 0 && $signal == 0 && $core == 0 )
    {
        #test passed before timeout
        print "\n\tPassed\n";
        $result = "Passed";
    }
    elsif ( $errcode != 0 && $signal == 0 && $core == 0)
    {
        print "\n\tWarnings: errcode = $errcode\n";
        $result = "Passed - with warnings: errcode = $errcode";
    }
    elsif ( $errcode == 6 && $signal == 6 )
    {
        # example: terminate called after throwing an instance of 'IFXException'
        print "\n\tTerminate\n";
        $result = "Failed - Terminate";
    }
    elsif ( $errcode == 7 && $signal == 7 )
    {
        print "\n\tAssertion\n";
        $result = "Failed - With Assertion";
    }
    else
    {
        # print "\n\terrcode == $errcode, signal == $signal, core == $core\n";
        print "\n\tUnrecognized Exception: errcode = $errcode\n";
        $result = "Failed - Unrecognized Exception: errcode = $errcode";
    }

    $res[0] = $result;

    return @res;
}# sub wtrun

###############################################################################
#                                                                             #
#   Input Parameters :                                                        #
#    1 - execution time (sec)                                                 #
#    2 - execution file name ( don't forget to add "./" specially for linux ) #
#   Output results :                                                          #
#             Timeout                                                         #
#             Script Failed                                                   #
#             Passed                                                          #
#             Passed - with warnings                                          #
#             Failed - Segmentation Fault                                     #
#             Failed - Terminate                                              #
#             Failed - Unrecognized Exception                                 #
#                                                                             #
###############################################################################
sub ltrun
{
    my $tm     = shift(@_);
    my $file   = shift(@_);

    my $errcode  = -1;
    my $ext_code = -1;
    my $signal   = -1;
    my $core     = -1;
    my $result   = -1;
    my $pid;
    my $timeout1 = 0;
    my $timeout2 = 0;
    my $i = 0;
    my $ad_tm = 10;
    my @res = ( "" , "OK" );

    #set custom signal handler. If a child process
    #terminates then the $child_killed flag is set
    $SIG{ CHLD } = sub { $child_killed = 1; };

    if( !defined( $pid = fork() ) )
    {
        die(" fork() failed \n");
    }
    elsif ( $pid == 0 )
    {
        exec $file; #run test in the child process
        die "exec failed\n";
    }
    else
    {
        #if ( $tm > 30 ) { $tm = 10; }

        for( $i = 0; $i < $tm; $i++ )
        {
          sleep 1; #in the main process wait not more than $tm sec
          if( $child_killed ) { last; } #check if test finished
        }
        if ( $i == $tm ) { $timeout1 = 1; }

        kill 3, $pid; #if not - kill it

        if( $child_killed == 0 )
        {
            for( $i = 0; $i < $ad_tm; $i++ )
            {
                if( $child_killed ) { last; } #check if test finished
                sleep 1;
            }
        }

        if( $child_killed == 0 )
        {
            $timeout2 = 1;
            kill 9, $pid; #if not - kill it
        }

        wait;

        $errcode  = $?;
        $ext_code = ( $? >> 8 );
        $signal = ( $? & 127 );
        $core = ( $? & 128 );

        $SIG{ CHLD } = 'DEFAULT';
    }

    #----------------------------------------------------------

    if ( $timeout1 == 1 )
    {
        $res[1] = "Timeout";
    }

    if( $errcode == -1 || $signal == -1 || $core == -1 )
    {
        print "\n\tScript Failed\n";
        $result = "Script Failed";
    }
    elsif ( $errcode == 0 && $signal == 0 && $core == 0 )
    {
        #test passed before timeout
        print "\n\tPassed\n";
        $result = "Passed";
    }
    elsif ( $errcode != 0 && $signal == 0 && $core == 0)
    {
        print "\n\tWarnings: errcode = $errcode\n";
        $result = "Passed - with warnings: errcode = $errcode";
    }
    elsif ( $errcode == 11 && $signal == 11 )
    {
        # segmentation fault
        print "\n\tSegmentation Fault\n";
        $result = "Failed - Segmentation Fault";
    }
    elsif ( $errcode == 6 && $signal == 6 )
    {
        # example: terminate called after throwing an instance of 'IFXException'
        print "\n\tTerminate\n";
        $result = "Failed - Terminate";
    }
    else
    {
        # print "\n\terrcode == $errcode, signal == $signal, core == $core\n";
        print "\n\tUnrecognized Exception: errcode = $errcode\n";
        $result = "Failed - Unrecognized Exception: errcode = $errcode";
    }

    #
    # remove all core dump
    #
    if( $core != 0 )
    {
        my @files = ();
        my $curdir =".";
        opendir HDIR, $curdir;
        @files = readdir HDIR;
        closedir HDIR;
        foreach my $currfile (@files){
            if ( $currfile =~ m /^core*.*/ ) {
                unlink($currfile);
            }
        }
    }
    $child_killed = 0;

    $res[0] = $result;

    return @res;
}# sub ltrun

1;
