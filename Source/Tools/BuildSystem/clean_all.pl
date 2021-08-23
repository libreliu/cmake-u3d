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
use File::Find;
use File::Path;
use File::Copy;
use util;

my $clean_component_name=$ARGV[0]; # "ALL" or "project name"
my $CONFIG=$ARGV[1];               # "ALL", "debug", "release"
my $VERBOSE=$ARGV[2];              # "yes", "no"
my $rembin=$ARGV[3];

#
# read from "project.ini" file all current project components,
# all components dependecies.
#

# common for this file variables list
my $project_ini_name = "../../Config/projects.ini"; # ../CWG/C3D/Source/Templates/project.ini
my @arr_file = ();
my @component_list = pars_components();
my $component_name;
my $source;

my $slib = cut( util::get_platform_setting( "SLIB" ) );
my $dlib = cut( util::get_platform_setting( "DLIB" ) );
my $exec = cut( util::get_platform_setting( "EXEC" ) );
my $u3d_output = cut( util::get_global_setting( "U3D_OUTPUT" ) );

#------------------------------------------------------------------------------

   # write "*.ini" file
   open( FH, "<$project_ini_name" ) || die "can not open $project_ini_name file";
      @arr_file = <FH>;
   close(FH);
   #

   if( $rembin eq "TRUE" ) {
   print "Deleting all output binaries...\n...\n";
   }
   else {
   print "Deleting all intermediate and output files...\n...\n";
   }
   if ($clean_component_name ne "ALL") {

            #
            # remove $clean_component_name
            #

            if( $CONFIG ne "ALL" )
            {
                  remove_component($clean_component_name);
            }
            else
            {
               $CONFIG = "debug";
               remove_component($clean_component_name);
               $CONFIG = "release";
               remove_component($clean_component_name);
            }

   }
   else
   {
               #
               # remove ALL
               #
         if( $CONFIG ne "ALL" )
         {
               foreach my $project (@component_list) {
                  remove_component($project);
               }
         }
         else
         {
            $CONFIG = "debug";
            foreach my $project (@component_list) {
               remove_component($project);
            }
            $CONFIG = "release";
            foreach my $project (@component_list) {
               remove_component($project);
            }
         }
   }
   print "...\nCleaning complete.\n";
exit 0;

#------------------------------------------------------------------------------
#
# remove all objects file into $clean_component_name source directories
# and into output "build" directoriy
#
sub remove_component()
{
    my $clean_os_type = util::get_global_setting( "U3D_PLATFORM" );
    my $component_name=@_[0];
    print " Cleaning '$component_name',  configuration '$CONFIG|$clean_os_type'";
    my @arr=();
    my @dirs=();

    my $cfg=$CONFIG . "/";
    my $path_base="../../RTL/Build/";
    my $path=$path_base . $component_name . "/" . $cfg;

    my $output = cut( util::pars_output_name(@arr_file, $component_name) );
    if ( !( $output =~ /\.$slib/ ) )
    {
        if( -e "../../$u3d_output/$output" )
        {
            unlink "../../$u3d_output/$output";
        }
    }

    opendir BLD, $path;
    my @files = sort (readdir BLD);
    closedir BLD;

    for( my $i = 2; $i < @files; $i++ )
    {
        if( ( $files[$i] =~ /\.$dlib$/ || $rembin ne "TRUE" ) )
        {
          if( !( -d $path.$files[$i] ) )
          {
            push( @arr, $path.$files[$i] );
          }
          else
          {
            push( @dirs, $path.$files[$i]."/" );
          }
        }
    }

    foreach my $dir ( @dirs )
    {
        opendir DIR, $dir;
        my @dep_files = sort (readdir DIR);
        closedir DIR;

        for( my $i = 2; $i < @dep_files; $i++ )
        {
            if( $dep_files[$i] =~ /\.$dlib$/ || $rembin ne "TRUE" )
            {
                if( !( -d $dir.$dep_files[$i] ) )
                {
                    push( @arr, $dir.$dep_files[$i] );
                }
            }
        }
    }

    if($VERBOSE ne "yes" )
    {
        print "                                                      \n";
        print " \t\t Deleting the following files : @arr\n";
        print "                                                      \n";
        print "                                                      \n";
    }

    my $cnt = unlink @arr;
    if( $cnt != @arr )
    {
        print " ERROR, can't remove $component_name binaries\n";
    }

    my $make_path = GetSourcePath( $component_name );
    $make_path =~ s/\s//g;
    if ( -e $ENV{U3D_SOURCE}.$make_path."Makefile" )
    {
        $cnt = unlink $ENV{U3D_SOURCE}.$make_path."Makefile";
        if($cnt  != 1 )
        {
            print " ERROR, can't remove $make_path Makefile\n";
        }
    }

    print" - done.\n";
} # remove_component($clean_component_name);


#------------------------------------------------------------------------------
#
# find all project components and put into @ component_list
#
sub pars_components() {

my $flag = 0;
my $tmp = 0;
my @component=();
open( FH, "<$project_ini_name" ) || die "can not open $project_ini_name file";
   while( defined ( $component_name = <FH> ) ){
       chomp $component_name;
       #[Workspace]  ... #[End Workspace]
       if( $component_name =~ /\[Workspace\]/ ) {
          $flag = 1;
       }
       if ( $flag eq 1 ) {
          if( $component_name !~ /\[Workspace\]/ && $component_name !~ /\[End Workspace\]/){
             # this is very rough dummy method to remove all \n, \t, \s* and s.o symbols
             $tmp = length($component_name);
             if ( $tmp > 2 ) {
               #print("  ====$component_name \n");
               push(@component,$component_name);
             }
          }
       }
       if ( $component_name =~ /\[End Workspace\]/ ) {
         $flag = 0;
       }

   };# while

   return @component;
   close(FH);
}; # sub pars_components()

sub cut() {
   my $string = @_[0];
   $string=~ s/^\s*//;
   $string==~ s/\s*$//;
   chomp($string);
   return $string;
};

sub GetSourcePath($) {

   my $name = @_[0];
   my $flag_component = 0;
   my $flag_source = 0;

   my @arr_file=();
   my $project_ini_name = $ENV{U3D_SOURCE}."Config/projects.ini";

   open( FH, "<$project_ini_name" ) || die "can not open $project_ini_name file";
      @arr_file = <FH>;
   close(FH);


   foreach my $str (@arr_file) {
      if( $str =~ /\[$name - Settings\]/ ) {
         $flag_component = 1;
      }
      if( $str =~ /"Source Files"/ ) {
         $flag_source = 1;
      }
         # write all dependencies between "Include Directories" - "end Include ..."
         if( $flag_component eq 1 && $flag_source eq 1 )
         {
            if ( $str =~ /"Source Files"/ || $str =~ /"end Source Files"/ )
            {
               # miss this string
            } else {
               return $str;
            }
         }

      if( $str =~ /"end Source Files"/ ) {
         $flag_source = 0;
      }
      if ( $str =~ /\[$name - end Settings\]/ ) {
        return 0;
      }
   }
}
