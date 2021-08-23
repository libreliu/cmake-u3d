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
use create_make;
use util;
use File::Path;


#
# read from "project.ini" file all current project components,
# all components dependecies.
#

my $CONFIG=$ARGV[0]; # debug,release and etc

my $ROOTDIR=$ENV{U3D_SOURCE};
$ROOTDIR=~ s/\s+\z//;
$ROOTDIR=~ tr%\\%/%; # D:/perforce/CWG/C3D/Source/

my $file_name = $ROOTDIR."Config/projects.ini";

my @component_list = ();   # all project component list
my @include_arr = ();      # array for inlude file dependencies
my @linker_arr  = ();      # array for linker

my @include = ();      # temporarily array for inlude file dependencies
my @linker  = ();      # temporarily array for linker
my $output  = 0;       # temporarily array for all components output names
my $source  = 0;       # temporarily array : where you can find sources of this component
my @add_source = ();   # additional ".cpp" files which should be compile in the current project
my $path2makedef = 0;  # relative path to the ../Config/make.def file
my $path2build   = 0;  # relative path to the ../Config/make.def file
my @options = ();      # additional options for CFLAGS and for LINKER FLAGS
my @exclude=();        # contains files which should remove from compilation
my @export=();         # all export files, like IFXDLL.def

# tmp array
my @tmp_source_arr_exclude = ();

my @arr_file = ();     # write all .ini file into array
my $component_name;
my @tmp_source_arr = ();

   @ component_list = util::pars_components($file_name);
   # write "*.ini" file
   open( FH, "<$file_name" ) || die "can not open $file_name file";
   @arr_file = <FH>;
   close(FH);
   #
   my $size=0;
   my @link_targets = ();
   foreach $component_name ( @component_list ) {

      @include = util::pars_include( @arr_file, $component_name );
      @linker = util::pars_linker( @arr_file, $component_name );
      @link_targets = ();
      foreach( @linker )
      {
         s/^\W+//;
         chomp $_;
         util::pars_output_name( @arr_file, $_ ) =~ /^\W+(.*$)/;
         push( @link_targets, $1 )
      }

      $output  = 0;
      $output = util::pars_output_name( @arr_file, $component_name );

      $source  = 0;
      @add_source = ();
      @tmp_source_arr = ();
      @tmp_source_arr = util::pars_sorce_path( @arr_file, $component_name );

         # extract $source and @add_source from @tmp_source_arr
         $source = @tmp_source_arr[0];
         #$source=cut($source);

         #2
         $size=0;
         $size = @tmp_source_arr;
         for(my $i=1; $i < $size; $i++ ) {
            my $line = @tmp_source_arr[$i];
            $line=cut($line);
            push(@add_source,$line);
         }

      @export=();
      @export=util::pars_export( @arr_file, $component_name );

      @options = ();
      @options = pars_make_options( $component_name );

      $path2makedef=get_path2makedef($source);

      $path2build=get_path2build($source,$component_name,$CONFIG); #$path2build="../../Build/IFXCollision/";

      # find all exlude from compilation procedure files
      @exclude=();
      @tmp_source_arr_exclude=();
      @exclude=util::pars_exclude_path( @arr_file, $component_name );
      my $sz=@exclude;
      my $answer=0;
      $answer=create_make::collect_data(@include,
                                        @linker,
                                        $component_name,
                                        $output,
                                        $source,
                                        $path2makedef,
                                        $path2build,
                                        @options,
                                        @add_source,
                                        @exclude,
                                        @export,
                                        $CONFIG,
                                        @link_targets);

      if ($answer == "1" ) {
         print "===========================================================================================\n";
         print "==================== ERROR - cannot create $component_name Makefile   =====================\n";
         print "===========================================================================================\n";
         exit 1;
      }
   }; # foreach $component_name ( @component_list )

#--------------------------------------------------------------------------------------------------
# input : path to $source like path$==CWG/C3D/Source/RTL/Kernel/Common/
#       : $component_name
# return : relative path from $source to =="../../Build/IFXCollision/";
#
sub get_path2build(\$\$\$) {
   my $src=shift(@_);  # where we can find source files, for example, Source/MOA/Tasks
   my $component_name=shift(@_);
   my $CONFIG=shift(@_);

   my $cfg=$CONFIG."/";
   my $mydst="../../RTL/Build/".$component_name. "/". $cfg;
   File::Path::mkpath($mydst);

   my $dst = $ROOTDIR . "RTL/Build/";
   return $dst;
}#sub get_path2build

#--------------------------------------------------------------------------------------------------
# input path to $source like path$==CWG/C3D/Source/RTL/Kernel/Common/
# return : relative path from $source to /RTL/Config/
#
sub get_path2makedef(\$) {
   my $src=shift(@_);  # where we can find source files: for example, Source/MOA/Tasks

   my $dst;
   my $name="make.def";

   $dst = $ROOTDIR . "Config/" . $name;

   return $dst;
}#get_path2makedef

sub cut() {
   my $string = @_[0];
   $string=~ s/^\s*//;
   $string==~ s/\s*$//;
   chomp($string);
   return $string;
};

#
# extract all options
#
sub pars_make_options( $component_name ) {

   my $name = @_[0];
   my $flag_component = 0;
   my $flag_options   = 0;
   my $flag_config    = 0;
   foreach my $str (@arr_file) {

      # search [pfxdebug - Settings] ... [pfxdebug - end Settings]
      # search [$name - Settings] ... [$name - end Settings]
      if( $str =~ /\[$name - Settings\]/ ) {
         $flag_component = 1;
      }
      if( $str =~ /"options"/ ) {
         $flag_options = 1;
      }
      if( $str =~ /CONFIG:$CONFIG/i ) {
         $flag_config = 1;
      }

         # write all dependencies between "Include Directories" - "end Include ..."
         if( $flag_component eq 1 && $flag_options eq 1 && $flag_config eq 1 )
         {
            if ( $str =~ /"options"/ || $str =~ /"end options"/ || $str =~ /CONFIG:/i )
            {
            }
            else
            {
               $str=~s/^ +//; # remove all the fisrt spaces
               push(@options,$str);
            }
         }

      if( $str =~ /CONFIG:/i && $str !~ /CONFIG:$CONFIG/i ) {
         $flag_config = 0;
      }
      if( $str =~ /"end options"/ ) {
         $flag_options = 0;
         $flag_config = 0;
      }
      if ( $str =~ /\[$name - end Settings\]/ ) {
        $flag_component = 0;
        $flag_config = 0;
        return @options;
      }

   }; # foreach

}#pars_options
