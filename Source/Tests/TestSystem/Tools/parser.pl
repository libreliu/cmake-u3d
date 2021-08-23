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

use strict;
use IO::File;
use IO::Dir;
use File::Find;
use run;

   my @options_arr=@ARGV;

   my $opt0=@options_arr[0];
   my $opt1=@options_arr[1];
   my $opt2=@options_arr[2];
   my $opt3=@options_arr[3];

   my $GLOB=0;
   my $options_size=@options_arr;
   if (1 < $options_size ) {
         print "\n";
         print " ERROR :  COMMAND LINE SYNTAX \n";
         print "\n";
         exit 1;
   } else {
         $opt2 = 5;   # as default  "Exec. time == 5 sec"
         $opt3 = 1;    # as default  " run exec times == 1 "
   }
   print "\n\t input command line options : \n";
   print "\t\t exec time = $opt2 \n";
   print "\t\t run times = $opt3 \n";

my @testlist=(); # global scope resoulution, contains test suite for running

my $name=0;
my $parsingresult=0;
my $sz=@options_arr;
if($sz == 0 ) {

   # give all tests from ../Tests/ dir,
   #     put these test in the @array with full pathes and
   #
   # get all test name list and pass this list to run::runfull()
   #

   GetWholeTestSuite();
   $opt0="NULL";
   $name="All Test Suite";
   run::run( @testlist,$opt0, $name, $opt2, $opt3 );#!!!NOT_CHECKED!!!

} else {

      #
      # 1. go to the ../Config/suites.cfg files,
      # 2. find "$opt1" suite name,
      # 3. open  "../Config/$opt1" file
      # 4. read from this file all pointed test full names ( must be pointed the pathes relative ../Tests/ directory)
      #    ( 2exe/ATOM.exe, win32/Texture.exe, or trun.exe )
      #
      # 5. collect all this file names to @testlist

      # check $name : if that $name suite don't exist - return 0;
      #               if exist, then check all tests existing - if don't exist - return 0
      #               if all OK, collct all these test to the @testlist and return 1
      $name=$ARGV[0];
      $parsingresult=fill_testsuite($name);
      _errmsg($parsingresult,$opt1);
      run::run( @testlist, $opt0, $name, $opt2, $opt3);

}# if-else ($sz)

# -------------------------------------------------------------------------------------------------
#  check the testname only in the ../Tests/ dir, don't search in the ../Tools/ifcom and etc
#  return "1" if this test 's been found between all ../Tests/ directory tree
#  return "0" in the contrary case
#

sub check_test_name($) {
   my $name=$_[0];
   my $result=0;
   my $path="../Tests/";

    use IO::Dir;
    my $dir = new IO::Dir $path;
    if (defined $dir) {
         my @files=$dir->read();
         foreach my $str ( @files ) {
            if( $str=~ m/\.exe\b/ ) {

               my @arr=split(/\./,$name); # requirement - the test name must contains onlu 1 "." as delimiter
                                          # eg 1.exe - OK, AA.DD.exe - BAD name
               $name=@arr[0];
               $str=~ s/.exe//;
               if($name=~ m/\b$str\b/) {
                  #print "\n";
                  #print " $name is correct test name \n";
                  #print "\n";
                  $result = 1;
                  last;
               }
            }#if
         }#foreach(@files)
   }#if(defined)
   return $result;
}#check_test

# ------------------------------------------------------------------------------
# check $name:if that $name suite don't exist -- return 0;
#             if exist, then check all tests existing - if don't exist-return 0
#             if all OK, collct all these test to the @testlist -- return 1

sub fill_testsuite() {
   my $namesuite=@_[0];

# 1 - goto ../Config/ dir search $namesuite files return 0 if no
# 2 - open $namesuite file and read and search all test names : return 0 if NO
# 3 - fill @testlist and return 1;
# requirement - we don't support nesting test suite list
# requirement - test or dir name don't allowed direct slach symbol "/"

#1
   my @files=();
   my $path="../Config/";
   use IO::Dir;
   my $dir = new IO::Dir $path;
   if (defined $dir) {
      @files=$dir->read();
   }
   my $flag=0;
   foreach my $str ( @files ) {
      if($str=~ m/\b$namesuite\b/) {
         $flag=1;
         last;
      }
   }
   if ($flag != 1 ) {
      print "\n";
      print " ERROR : can't find the ../Config/$namesuite \n";
      print "\n";
      return 0;
   }

#2  open and read test name from $namesuite
   my @arr=();
   my $full_test_name=$path.$namesuite;
   open( FH, "<$full_test_name" ) || die "can not open $full_test_name file";
      @arr = <FH>;
   close(FH);

   @testlist=();
   foreach (@arr) {
      my $str=$_;

      if($str =~ m/^\/\//) {
      }else {
#         $str=~ s/^\s*//; $str=~ s/\s+\z*//;
         $str=~ s/\r\n//;
         $str=~ s/\n//;
         if ($str!~ m/^\s*$/ ) {  # all except empty string

            my $res=0;
            if($str=~ m/\//) { # eg linu32/Projections.exe
               $res=check_suite_a ($str);
            } else {          # eg ATOM.exe
               $res=check_test_name ($str);
            }
            _errmsg($res,$str);

            push(@testlist, $str);
         }
      }# if-else

   }# foreach
   return 1;
}#fill_testsuite

#
#  check input $directory name existing
#  return 1 -> OK
#         0 -> FALSE
#
sub check_suite_a() {
   my $name=$_[0];
   $name =~ /^([^ ]*) /;
   if( $1 )
   {
      $name = $1;
   }
   $GLOB = 0;

   chdir "../Tests/";
   my @arr=();
   push(@arr,$name);
   find( \&test_suitename_a, @arr );
   chdir "../Tools/";
   return $GLOB;
}
sub test_suitename_a {
   my $name = $_;
   $GLOB = 1;
};


#
# this simple error message routine
#
sub _errmsg($$) {
   my $result = @_[0];
   my $option = @_[1];
   if($result == 0 ){
      print " ===================================================\n";
      print "             unknown input $option name             \n";
      print " ===================================================\n";
      exit 1;
   }
}

#
#
#
sub  GetWholeTestSuite() {
   my @files=();
   my $path="../Tests/";

   # read from root directive
   use IO::Dir;
   my $dir = new IO::Dir $path;
   if (defined $dir) {
      @files=$dir->read();
   }

   my $slach = "/";
   foreach my $str ( @files ) {

      $str=$path.$str;
      if ( -d $str  ) {
         $str=~ s/\z+//;
         if( $str =~ m/\.$/ || $str =~ m/\.\.$/){
         } else {
            # read from current ../Tests/ifxcom (as example ) directory
            my $newdir = new IO::Dir $str;
            my @newfiles=();
               if ( defined $newdir ) {
                  @newfiles=$newdir->read();
               }
               foreach my $name (@newfiles) {
                  if( $name !~ m/\.$/ && $name !~ m/\.\.$/){ # remove all "." ".." dir's

                     $name = $str . $slach .$name; # ../Tests/Dummy/test_good.exe
                     $name =~ s/^..\/Tests\///; # like Dummy/test_good.exe
                           #print " \\\\ $name \n";
                     push(@testlist,$name);
                  }
               }#foreach (@newfiles)
         }#

      } elsif ( -x $str ) {    # matching the all executable file
            push(@testlist,$str);
      } else {
      }
   } #foreach ( @files )
} #GetWholeTestSuite
