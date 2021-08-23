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

system "rm -f ../Results/daily.txt";
system "rm -f ../Results/builderr.txt";

system "perl gen_db.pl";
system "perl build_rec.pl ".$ARGV[0];
system "perl gen_all.pl";

$fail_lim = 10; #limit of failures allowed

opendir CUR, "../TestSystem/Config";
@files = sort (readdir CUR);
closedir CUR;
chdir "../TestSystem/Tools";

for( $i = 2; $i < @files; $i++ )
{
        system "perl parser.pl ".$files[$i];   # all _ExternalExecutables
}
open DL, "<../Results/daily.txt";
my @lines = <DL>;
my $result_line = "";
my $str = "";
close DL;
my $res = 0;
my $script_failed = 0;
my $failed = 0;
my $sum_failed = 0;
my $sfailed = 0;
my $terminated = 0;
my $with_assertions = 0;
my $unr_exeptions = 0;
my $timeout_wnc = 0;
my $timeout = 0;
my $passed_ws = 0;
my $passed = 0;
my $bln_failed = 0;
my $no_trace = 0;

my $i = 0;
my $ib = 0;
my $ibn = 0;
my $ie = 0;
my $size = 0;

$ib = 1;
$i = $ib;
$ie = $ib;

$size = @lines;

while($i < $size)
{
        $bln_failed = 0;

        while( !(@lines[$i] =~ /Test \"/) && ($i < $size) )
        {
                $i++;
        }
        if ($i == $size) { break; }

        $str = cut(@lines[$i+1]);
        $ibn = $i+2;

        if ( $str =~ /Script Failed/ )
        {
            $script_failed++;
            $bln_failed = 1;
        }
        elsif( $str =~ /Segmentation Fault/ )
        {
            $sfailed++;
            $bln_failed = 1;
        }
        elsif( $str =~ /Terminate/ )
        {
            $terminated++;
            $bln_failed = 1;
        }
        elsif( $str =~ /With Assertion/ )
        {
            $with_assertions++;
            $bln_failed = 1;
        }
        elsif( $str =~ /Unrecognized Exception/ )
        {
            $unr_exeptions++;
            $bln_failed = 1;
        }
        elsif( $str =~ /No Normal Trace File/ )
        {
            $no_trace++;
            $bln_failed = 1;
        }
        elsif ( $str =~ /Failed: (\d+)/ && $1 )
        {
            $failed++;
            $str = "\tResult: Failed";
            $bln_failed = 1;
        }
        elsif ( $str =~ /Timeout/ )
        {
            $timeout++;
            $bln_failed = 1;
        }
        elsif ( $str =~ /Passed - with warnings/ )
        {
            $passed_ws++;
            $bln_failed = 1;
        }
        elsif ( $str =~ /Passed/ )
        {
            $passed++;
        }

        if ($bln_failed == 1)
        {
                $result_line = $result_line.cut(@lines[$i])."\r\n".$str."\r\n";

                $ie = $i-1;
                for ($i=$ib; $i<=$ie; $i++)
                {
                        $str = cut(@lines[$i]);

                        if ($str ne "")
                        {
                                $result_line = $result_line.$str."\r\n";
                        }
                }
                $result_line = $result_line."\r\n\r\n";
        }

        $ib = $ibn;
        $i = $ib;
        $ie = $ib;
}

system "cp -f ../Results/daily.txt ../Results/daily.tmp";

open DL, ">../Results/daily.txt";
print DL "";
close DL;

open DL, ">>../Results/daily.txt";

if ( $passed > 0 )
{
        print "\r\n\r\n\tPassed: $passed\r\n";
        print DL "\r\n\r\n\tPassed: $passed\r\n";
}

if ( $passed_ws > 0 )
{
        print "\tPassed with warnings: $passed_ws\r\n";
        print DL "\tPassed with warnings: $passed_ws\r\n";
}

if ( $failed > 0 )
{
        print "\tFailed: $failed\r\n";
        print DL "\tFailed: $failed\r\n";
}

if ( $timeout > 0 )
{
        print "\tFailed witht timeout: $timeout\r\n";
        print DL "\tFailed with timeout: $timeout\r\n";
}

if ( $sfailed > 0 )
{
        print "\tFailed with segmentation fault: $sfailed\r\n";
        print DL "\tFailed with segmentation fault: $sfailed\r\n";
}

if ( $terminated > 0 )
{
        print "\tFailed with terminate: $terminated\r\n";
        print DL "\tFailed with terminate: $terminated\r\n";
}

if ( $with_assertions > 0 )
{
        print "\tFailed with assertion: $with_assertions\r\n";
        print DL "\tFailed with assertion: $with_assertions\r\n";

}

if ( $unr_exeptions > 0 )
{
        print "\tFailed with unknown reason: $unr_exeptions\r\n";
        print DL "\tFailed with unknown reason: $unr_exeptions\r\n";
}

if ( $no_trace > 0 )
{
        print "\tFailed with no normal trace file: $no_trace\r\n";
        print DL "\tFailed with no normal trace file: $no_trace\r\n";
}

if ( $script_failed > 0 )
{
        print "\tFailed with script faile: $script_failed\r\n";
        print DL "\tFailed with script faile: $script_failed\r\n";
}

print "\r\n";
print DL "\r\n";

$sum_failed = $failed + $sfailed + $terminated + $with_assertions + $unr_exeptions + $script_failed + $timeout + $no_trace;

print "\tTotal failed: $sum_failed\r\n";
print DL "\tTotal failed: $sum_failed\r\n";

if( $sum_failed < $fail_lim && $sum_failed > 0 )
{
    $res = 1;
    print "\nDaily testing warning: $sum_failed tests failed\n";
}
if( $sum_failed >= $fail_lim )
{
    $res = 8;
    print "\nDaily testing error: $sum_failed tests failed. Too many failures\n";
}
if( -e "../Results/builderr.txt" )
{
  open BLD, "<../Results/builderr.txt";
  @bld = <BLD>;
  $bld_scalar = @bld;
  print "\tNumber of tests not built: $bld_scalar\r\n";
  print DL "\tNumber of tests not built: $bld_scalar\r\n";

  if( $bld_scalar + $sum_failed < $fail_lim )
  {
    $res = 1;
    print "\nDaily testing warning: $bld_scalar tests broken\n";
  }
  else
  {
    $res = 8;
    print "\nDaily testing error: $bld_scalar tests broken. Too many failures\n";
  }
}

print DL "\r\n\r\n\r\n".$result_line;
close DL;
exit $res;

#-------------------------------------------------------------------------------

sub cut() {
   my $string = @_[0];
   $string =~ s/\r//g;
   $string =~ s/\n//g;
   return $string;
};

#-------------------------------------------------------------------------------
