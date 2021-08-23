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
use IO::Dir;
use util;

my $U3DSOURCE = $ENV{U3D_SOURCE};
$U3DSOURCE =~ s/\\/\//g;

if ($U3DSOURCE eq "")
{
	print "Please set U3D_SOURCE environment variable...\n";
	exit 1;
}

my %global_settings = util::parse_global_settings;
my %platform_settings = util::parse_platform_settings;

my @localcomp_arr = ();
my $errorfl = 0;
my $os_type = $ENV{OS};

my $project_tail = $global_settings{U3D_PLATFORM};

my $plugpath = $ENV{U3D_SOURCE}.util::get_global_setting( "U3D_OUTPUT" )."/Plugins";
$plugpath =~ s/\\/\//g;
if ( !( -e $plugpath ) )
{
   system "mkdir -p $plugpath";
}

sub check_lists(\@\@);


   my $path="../../Config/Projects/";
   my @component_list=();
   my @full_array=();

# 1.
# open project xml file and fill @component_list
#
   @component_list=write_workspace();

   my $xmlfile="../../Config/project_$project_tail.xml ";
   open( FH, ">$xmlfile" ) || die "can not open $xmlfile file";

print FH "<?xml version=\"1.0\"?>\n"; #<?xml version="1.0"?>
print FH "<!DOCTYPE options [\n";

opendir KEYS, $U3DSOURCE."Config/Options";
my @keys = sort (readdir KEYS);
closedir KEYS;
for( my $i = 2; $i < @keys; $i++ )
{
	print FH "<!ENTITY $keys[$i] SYSTEM \"$U3DSOURCE"."Config/Options/$keys[$i]\" >\n";
}

print FH "]>\n";
print FH "<workspace>\n";
print FH "<!--\n";
print FH "# ==============================================================================\n";
print FH "# This section should Include all the workspaces.\n";
print FH "#\n";
print FH "# Workspaces\n";
print FH "#\n";
print FH "# ==============================================================================\n";
print FH "-->\n\n";
print FH "<workspace_component>\n\n\n";

foreach (@component_list) {  print FH "$_\n";  }

print FH "\n\n</workspace_component>\n";

   close(FH);

# 2.
# read all *.xml files and fill common array file
#
   my $dir = IO::Dir->new($path) or die( "can't open $path: $!" );
   my @files = sort ($dir->read());
   #foreach (@files){print">>> $_ \n";} exit 1;

   # 2.1
   # check all workspace_win.xml or *lin.xml list and @files list : the both list must be identical,
   # in the other case - print ERROR message and exit 1;
   #

   my $answer= check_lists(@files,@component_list);
   if($answer == 1) {
      print " \n";
      print "     ERROR, check workspace list      \n";
      exit 1;
   }

   my @component_arr=();
   my $existing_flag=0;
   my $name;
   my $list_name;
   my $file_name;
   foreach $list_name ( @component_list ) {
                           # example - DataTypes.xml
      foreach $file_name ( @files ) {
            $name=$file_name;
            $name=~ s/.xml//;
            @component_arr=();
            if( $name=~ /\b$list_name\b/ ){

               $existing_flag=1;
               $file_name=$path.$file_name;

               open( FHFILE, "<$file_name" ) || die "can not open $file_name file";
                  while(<FHFILE>) {
                     $_=~ s/\s+\n/\n/;
                     push(@localcomp_arr,$_);
                  }
               close(FHFILE);
               if (mandatory ("project_name") != 1 ||
                   mandatory ("output_name") != 1 ||
                   mandatory ("source_dir") == 0)
                   #these attributes are mandatory
               {
                  print "Mandatory parameter missing in file $file_name\n";
                  $errorfl++;
               }

               my @vals = mandatory ("project_name");
               $vals[0] =~ /^\s+(.*)\n$/;
               my $val = $1;
               #this attribute should be equal to file name
               if ($val ne $list_name)
               {
                  print "Wrong project name in file $file_name\n";
                  $errorfl++;
               }
               $errorfl += iserror ("project_name",0,$file_name);
               $errorfl += iserror ("output_name",1,$file_name);
               @vals = mandatory ("source_dir");
               $vals[0] =~ /^\s+(.*)\n$/;
               $val = $1;
                #this attribute should start with RTL/ or Tests/ or Prototype/ and be completed by /
                if (!($val =~ /^RTL\/.*\/$/) && !($val =~ /^Tests\/.*\/$/) && !($val =~ /^Prototype\/.*\/$/) && !($val =~ /^Samples\/.*\/$/)  && !($val =~ /^Tools\/.*\/$/))
                {
 print "Wrong path to source directory in file $file_name\n";
 $errorfl++;
                }
               else
               {
                  my $env_u3d_source = $ENV{U3D_SOURCE};
                  $val =~ s/\$\((U3D_[^\)]*)\)/$global_settings{$1}/eg;
                  $val =~ s/\$\(([^\)]*)\)/$platform_settings{$1}/eg;
                  $env_u3d_source = $env_u3d_source.$val;
                  $env_u3d_source =~ s/\s//g;
                  $env_u3d_source =~ s/\\/\//g;
                  if( !( -d "$env_u3d_source" ) )
                  {
                     print "$env_u3d_source not exists, creating...\n";
                     system "mkdir \"$env_u3d_source\"";
                  }
               }
               $errorfl += iserror ("source_dir",2,$file_name);
               $errorfl += iserror ("export",2,$file_name);
               $errorfl += iserror ("source_exclude_files",2,$file_name);
               $errorfl += iserror ("include_dirs",2,$file_name);
               $errorfl += iserror ("dependencies",2,$file_name);
               $errorfl += iserror ("options",3,$file_name);
               $errorfl += iserror ("vs7name",0,$file_name);

               foreach (@localcomp_arr)
               {
                  push(@component_arr,$_);
               }
               @localcomp_arr = ();
               $file_name=~ s/^$path//;

               last();
            }#if

      } # foreach ( @files )

      if($existing_flag == 0 ) {

      } else {
         # @full_array = <-- @component_arr
         foreach (@component_arr) {
            push(@full_array,$_);
         }
         @component_arr=();
      }
      $existing_flag=0;

   } # foreach(@component_list)



 if ($errorfl != 0)
 {
    print "\nErrors total: $errorfl\n";
    exit 1;
 }



# 3.
# write to ../Config/project_(w|l)in.xml
#
   open( FH, ">>$xmlfile" ) || die "can not open $xmlfile file";
   print FH @full_array;
   print FH "</workspace>";
   close(FH);


#
#
#
sub write_workspace() {

   my @list=();
   my @arr_file=();
   my $_workspacefile="../../Config/workspace_$project_tail.xml";


   open( FH, "<$_workspacefile" ) || die "can not open $_workspacefile file";
      @arr_file = <FH>;
   close(FH);

   my $flag_in  =0;
   my $flag_out =0;
   my $var=0;
   foreach (@arr_file) {

      if ($_=~ /workspace/ ) {
         $flag_in=1;
      }
      if ( $_=~/\/workspace/ ) {
         $flag_out=1;
      }

      if ($flag_in == 1 && $flag_out == 0) {

         if ( $_=~ /workspace/ || $_=~ /\/workspace/ ) {
         } else {
            if($_=~ m/\w/){  # remove all empty strings
               $var=$_;
               $var=~ s/s\*$//;
               chomp($var);
               push(@list,$var);
            }
         }
      }
   }#foreach

   #
   #
   foreach (@list) {
    $_=~ s/^\s+//;  # remove all first whitespaces
    $_=~ s/\s+\z//; # remove all backending whitespaces or tabulate symbols
   }

   return @list;
}#write_workspace()

#
#
#
sub check_lists(\@\@) {

   my $files=shift(@_);
   my $project_list=shift(@_);

   my $answer=0;
   my $name;

   my $sz=@$project_list;

   foreach my $list_name ( @$project_list ) {

      foreach my $file_name ( @$files ) {

            $name=$file_name;
            if( $name =~ m/xml$/) {

                  $name=~ s/.xml//;

                  if( $name=~ /\b$list_name\b/ ){
                     $answer++;
                     #print ">>> $file_name, $name, $list_name, $answer, $sz  <<< \n";
                  }#if

            }

      } # foreach ( files )

   }# foreach ( project_list  )

   if ($answer == $sz ) {
      $answer = 0;
   } else {
      $answer = 1;
   }
   return $answer;
}

sub mandatory
{
   my $name = $_[0];
   my $start;
   my @res = ();
   for (my $i = 0; $i < @localcomp_arr; $i++)
   {
      if ($localcomp_arr[$i] =~ /^\s*<$name>\s*\n$/)
      {
         $start = $i;
         $i = @localcomp_arr;
      }
   }
   for (my $end = $start+1; $end < @localcomp_arr; $end++)
   {
      if ( !($localcomp_arr[$end] =~ /\w/) )
      {
         return 0;
      }
      if ($localcomp_arr[$end] =~ /<\/$name>/)
      {
#         return splice(@localcom_arr$end-$start-1);
         for (my $i = $start+1; $i < $end; $i++)
         {
            push (@res,$localcomp_arr[$i]);
         }
         return @res;
      }
   }
}

sub notallowed
{
   if (($_[1] == 0 && !($_[0] =~ /^[\w\$\;\(\)\ \"]+$/)) ||
       ($_[1] == 1 && !($_[0] =~ /^[\w\/\$\(\)\;\ \"]+\.[\w\$\(\)\;\ \"]*$/)) ||
       ($_[1] == 2 && !($_[0] =~ /^[\w\/\.\$\(\)\;\ \"]*?$/)) ||
       ($_[1] == 3 && !($_[0] =~ /^[\w\/\.\=\-\:\"\s\;\$\(\)\ \"]*?$/)))
   {
      print "Some symbols not allowed in file $_[2]:";
      print "\n\t$_[0]\n";
      return 1;
   }
   return 0;
}

sub iserror
{
   my @vals = mandatory ($_[0]);
   my $val;
   foreach (@vals)
   {
      /^\s+(.*)\n$/;
      $val = $1;
      if (notallowed ($val, $_[1], $_[2])) {return 1;}
   }
   return 0;
}
