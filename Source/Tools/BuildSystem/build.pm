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

package build;
use strict;
use IO::Dir;
use File::Find;
use File::Copy;
use File::Path;
use util;

sub build_project(\$\$\$\@);

my $build_component_name=0;
my $VERBOSE=0;
my $CONFIG=0; # debug,release and etc
my $components_list;

my $ROOTDIR=$ENV{U3D_SOURCE};
$ROOTDIR=~ s/\s+\z//;
$ROOTDIR=~ tr%\\%/%; # D:/perforce/CWG/C3D/

my @linker_order=();
my @linker=();
my @linker_tmp=();

my $INIT_TIME=0;
my $linker_flag=0;  # as stub

# read "*.ini" file
my $project_ini_name = $ROOTDIR."Config/projects.ini";
my @arr_file=();

open( FH, "<$project_ini_name" ) || die "can not open $project_ini_name file";
@arr_file = <FH>;
close(FH);

my @includes = ();
my $include = "";
my @include_files = ();
my @include_ftimes = (); #massive of max time of header file and header files included in it recursively.
my $base_time = 0;
my $path = "";

sub build_project(\$\$\$\@) #return 1 if all is ok, else return 0
{
   $build_component_name = shift(@_);
   $VERBOSE = shift(@_);
   $CONFIG = shift(@_);
   $components_list = shift(@_);

   $build_component_name = $$build_component_name;
   $VERBOSE = $$VERBOSE;
   $CONFIG = $$CONFIG;

#   print "components_list = @$components_list\n";

    print "\n Building $build_component_name \n";
    my $res = BuildRecursive( $build_component_name );

    return $res;
}

#
#   the main recursive build procedure
#

sub BuildRecursive() { #return 1 if all is ok, else return 0

   my $TargetName = @_[0];
   my $target_exist = 0;
   my $res = 1;

   if ( util::GetIndex(@$components_list, $TargetName) != -1 )
   {
#        print "\t\t $TargetName has been validated \n";
        print("\t\t $TargetName is up-to-date \n");
        return $res;
   }
   else
   {
        push(@$components_list, $TargetName);
   }

   $target_exist = ExistTarget( $TargetName, $CONFIG );

   my $target_time          = 0;
   my $target_force_rebuild = "FALSE";

   if ( $target_exist == 1 ) {
      $target_time = GetModificationTime($TargetName);
      $target_force_rebuild = "FALSE";

   } else {
      $target_force_rebuild = "TRUE";
   }
   my @dep_name = GetDependencies($TargetName);
   my $size = @dep_name;

   if ( $size == 0 ) {

      if( $target_exist == 0 ) {
        $res = build_output_project( $TargetName );
      }
      else {
         my $count = GetFilesForBuild($TargetName, $CONFIG);

         if( $count > 0 ) {
            $res = build_output_project( $TargetName );
         }
         else {
            print("\t\t $TargetName is up-to-date \n");
         }
      }
   }
   else { # $size >= 1

      foreach my $curr_dep_name (@dep_name) {

         $res = BuildRecursive( $curr_dep_name );
         if ($res == 0) { return $res; }
         if ( $target_exist == 1 ) { # exist

            my $dep_name_time = GetModificationTime( $curr_dep_name );

            if ( $dep_name_time < $target_time )
            {
#                print "$dep_name_time < $target_time!!!\n";
               $target_force_rebuild = "TRUE";
            }
         }

      }#foreach

      #print " $TargetName, $target_exist, $target_force_rebuild  \n";
      if ( $target_force_rebuild eq "TRUE" ) {
         $res = build_output_project( $TargetName );
      }
      else {
         my $count = GetFilesForBuild($TargetName, $CONFIG);

         if( $count > 0 ) {
            $res = build_output_project( $TargetName );
         }

         else {
            print("\t\t $TargetName is up-to-date \n");
         }
      }
   } # if - else dependencies size

   return $res;
} # BuildRecursive

#--------------------------------------------------------------------------------------------------
#
# find linker dependencies number for "TargetName" component
# return dependencies array
#

sub GetDependencies() {

   my $comp_name = @_[0];
   my $str;
   my $flag_linker = 0;
   my $flag_component = 0;
   my @links=();
   foreach $str (@arr_file) {
         if( $str =~ /\[$comp_name - Settings\]/ ) {
            $flag_component = 1;
         }
      if( $str =~ /"Dependencies"/ ) {
         $flag_linker = 1;
      }
      # write all dependencies between "Include Directories" - "end Inlude ..."
      if( $flag_component eq 1 && $flag_linker eq 1 )
      {
         if ( $str =~ /"Dependencies"/ || $str =~ /"end Dependencies"/ )
         {
            # miss this string
         } else {
            # if it's empty string we willn't add its into linker array
            if ( $str=~ /\w+/ ) {

               $str=~ s/^\s*//;
               $str==~ s/\s*$//;
               chomp($str);
               push(@links,$str);

            } else {
               #print(" found the empty linker dependencis  \n"); # print debug
            }
         }# if - else
      };#if -else

      if( $str =~ /"end Dependencies"/ ) {
         $flag_linker = 0;
      }
      if ( $str =~ /\[$comp_name - end Settings\]/ ) {
         $flag_component = 0;
      }
   }; #foreach $str (@arr_file)

   #print "@links\n"; exit 1;

   return @links;

} #GetDependencies

#--------------------------------------------------------------------------------------------------
#
# input - project name
# return 1 if this component has been built yet else return 0
#

sub ExistTarget
{
        my $comp_name = @_[0];
        $CONFIG = @_[1];
        my $name = "";

        $comp_name=cut($comp_name);

        my $path = $ROOTDIR . "RTL/Build/" . $comp_name . "/" . $CONFIG . "/";

        my $output_name = cut( util::pars_output_name( @arr_file, $comp_name ) );
        $output_name =~ /([^\/]*)$/;
        $output_name = $1;

        my $slib = cut( util::get_platform_setting("SLIB") );
        my $dlib = cut( util::get_platform_setting("DLIB") );

#        print "slib = $slib , dlib = $dlib\n";

        if ($output_name =~ /\.$slib/)
        {
                $name = $path . util::get_platform_setting("LIB_PREFIX") . $output_name;
        }
        else
        {
                $name = $path . $output_name;
        }

        if( -e $name )
        {
                return 1;
        }
        else
        {
#               print "Big Errror !!! $name not exist\n";
                return 0;
        }
}

sub GetFilesForBuild
{
   my $comp_name = @_[0];
   $CONFIG = @_[1];

   my @tmp_source_arr = ();
   my $file = "";
   my $obj_fname = "";
   my @files_for_build = ();

   $comp_name = cut($comp_name);

   $base_time = "";
   $path = "";

   @tmp_source_arr = util::pars_sorce_path( @arr_file, $comp_name );

   my @exclude = util::pars_exclude_path( @arr_file, $comp_name );

   @includes = util::pars_include( @arr_file, $comp_name );

   $base_time = GetModificationTime( $comp_name );

   $path = @tmp_source_arr[0];
   $path =~ s/^\s*//;
   $path =~ s/\s*$//;

   opendir source, $ROOTDIR.$path;
   my @files = readdir source;
   closedir source;

   my $out_dir = $ROOTDIR . "RTL/Build/" . $comp_name . "/" . $CONFIG . "/";

   my @files_post_excl = ();
   my $file;
   my $excl;

   foreach $file( @files )
   {
      my $present = 1;
      foreach $excl( @exclude )
      {
         if( $file eq $excl )
         {
            $present = 0;
         }
      }

      if( $present )
      {
         push( @files_post_excl, $file )
      }
   }

   my $size = @tmp_source_arr;
   for(my $i=1; $i < $size; $i++ )
   {
        $file = @tmp_source_arr[$i];
        $file = cut($file);
        push(@files_post_excl, $file);
   }

   foreach $file( @files_post_excl )
   {
      if($file =~ /\.(cpp|c)$/ )
      {
         my $fname = $ROOTDIR . $path . $file;
         my $ftime = -M$fname;
         my $is_modified = 0;

         if ($file =~ /(.*)\/\w+(\.\w*)?$/)
         {
                push(@includes, $1);
         }
         else
         {
                push(@includes, "");
         }

         if ( $ftime < $base_time )
         {
            $is_modified = 1;
         }

         if ( $is_modified == 0 )
         {
            open( FH, "<$fname" ) || die "can not open $fname file";
            my @strings = <FH>;
            close(FH);

            my $str;

            foreach $str( @strings )
            {
               if ($str =~ /^#include[\s\t]*\"(.*)\"/)
               {
                  my $mod_time = ModTimeRecursive($1);

                  if ( ($mod_time != -1) && ($mod_time < $base_time) )
                  {
                        $is_modified = 1;
                        last;
                  }
               }
            }
         }

         pop(@includes);

         if ( $is_modified == 1 )
         {
            $file =~ /([^\/\\]+)\.(cpp|c)$/;

            $obj_fname = $out_dir.$1.".obj";
            if ( -e $obj_fname )
            {
                system "rm $obj_fname";
            }
            else
            {
                $obj_fname = $out_dir.$1.".o";
                if ( -e $obj_fname )
                {
                   system "rm $obj_fname";
                }
            }

            push(@files_for_build, $file);
         }
      }
   }

   return @files_for_build;
}

#
# build output project
#

sub build_output_project() { #return 1 if all is ok, else return 0

   my $new_link_name=@_[0];
   my $res = 1;

   $new_link_name=cut($new_link_name);

#   my $path_for_build=get_path2build($new_link_name);
   my $path_for_build=(util::pars_sorce_path( @arr_file, $new_link_name ))[0];

   $path_for_build=cut($path_for_build);
   my $build_os_type = util::get_global_setting( "U3D_PLATFORM" );
   my $compiler = util::get_global_setting( "U3D_COMPILER" );

print "\n";
print "\n";
print " ----- Build started: Project: $new_link_name, Configuration: $CONFIG $build_os_type -----\n";
print "\n";

   #$path_for_build=$path_for_build;
   $path_for_build = $ROOTDIR . $path_for_build;
   chdir $path_for_build;

   my $make_name="Makefile";
   $make_name=cut($make_name);
   my $make_log = $ROOTDIR."Tools/BuildSystem/__make";
   my $make_all = $ROOTDIR."Tools/BuildSystem/__make_all";
   my $make_errors = 0;
   my $make_warnings = 0;
   my $make_crash = 0;
   if( -e $make_name ) {
      if($VERBOSE eq "yes" ){
         system ("make 2>&1 -s | tee ".$make_log );
      }else{
         system ("make 2>&1 | tee ".$make_log );
      }
      open MAKELOG, $make_log;
      my @make_log_lines = <MAKELOG>;

      foreach( @make_log_lines )
      {
         if( /\*\*\*/ ) { $make_crash = 1; }
      }
      foreach( @make_log_lines )
      {
         if ($compiler eq "cl")
         {
            if( /:.*error \w+\d\d\d\d:/ )
            {
               $make_errors++;
            }
            if( /:.*warning \w+\d\d\d\d:/ )
            {
               $make_warnings++;
            }
         }

         if ($compiler eq "gcc")
         {
            if( /(.*):\d+:( warning:)?/ )
            {
               if( -e $1 )
               {
                   if( $2 eq " warning:" )
                   {
                       $make_warnings++;
                   }
                   elsif( $_ !~ /instantiated from/ )
                   {
                       $make_errors++;
                   }
               }
            }
         }

         if ( $compiler ne "cl" && $compiler ne "gcc" )
         {
            if ( /warning/ )
            {
               $make_warnings++;
            }
         }
      }
      close MAKELOG;

      my $fh_make_all = IO::File->new( ">> $make_all" );
      if( $make_errors == 0 && $make_crash )
      {
            print("\n $new_link_name - 1 fatal error \n\n");
      }
      else
      {
            print("\n $new_link_name - $make_errors error(s), $make_warnings warning(s) \n\n");
      }
      if( $make_errors == 0 && $make_crash )
      {
            print $fh_make_all " $new_link_name - 1 fatal error \n";
      }
      else
      {
            print $fh_make_all " $new_link_name - $make_errors error(s), $make_warnings warning(s) \n";
      }
      close $fh_make_all;
      if ( $make_errors > 0 ||$make_crash || $? ) {
         $res = 0;
      }

   } else {

      print("Cannot find $make_name  in $new_link_name directory \n");
      print "Looks like \"$new_link_name\" is a project name that was not added to the workspace file.\n";
      print "If yes, please add it to workspace_w|lin32.xml\n";

      $res = 0;
   };# if - else

   my $back = $ROOTDIR . "Tools/";
   chdir $back;

   return $res;
}#build_output_project


#
#  find *.lib or something the same binaries and etc
#
sub extract_libs() {
   my $link_name=shift(@_);
   my $go2dir="../RTL/Build/$link_name";
   chdir($go2dir)|| die " cannot cd to $go2dir ($!)";

   my $binary=find_bin();

   my $dst="../";
   File::Copy::copy($binary,$dst);

   chdir("../../../Tools") || die " cannot cd to ../../../Tools ($!)";
}

#
# into current directory find output binaries lile IFXBase.dll and return it name
#
sub find_bin() {

   my $binary;
   use File::Glob ':globally';
   my @dll = <*.{dll}>;
   my @lib = <*.{lib}>;
   my @exe = <*.{exe}>;
   my @oo = <*.{o}>;
   my @so = <*.{so}>;
   my @aa = <*.{a}>;
   my @x32 = <*.{x32}>;
   my @arr=();
   foreach (@dll){ push(@arr,$_); }
   foreach (@lib){ push(@arr,$_); }
   foreach (@exe){ push(@arr,$_); }
   foreach (@oo) { push(@arr,$_); }
   foreach (@so) { push(@arr,$_); }
   foreach (@aa) { push(@arr,$_); }
   foreach (@x32) { push(@arr,$_); }
   foreach (@arr){
      if($_=~ m/.dll$/ || $_=~ m/.lib$/ || $_=~ m/.exe$/ || $_=~ m/.so$/ || $_=~ m/.a$/ || $_=~ m/.out$/ || $_=~ m/.x32 $/)
      {
         $binary=$_;
         last();
      }
   }
   return $binary;
}# sub find()

#
# return $component_name source path
#
sub get_source($component_name) {
   my $name = @_[0];
   my $flag_component = 0;
   my $flag_source = 0;
   my $source;

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
               $source = $str;
               last;
            }
         }

      if( $str =~ /"end Source Files"/ ) {
         $flag_source = 0;
      }
      if ( $str =~ /\[$name - end Settings\]/ ) {
        $flag_component = 0;
        return $source;
      }
   }; # foreach
   return $source;
}# get_source()

#-------------------------------------------------------------------------------
sub find_links() {

   my $comp_name = @_[0];
   my $str;
   my $flag_linker = 0;
   my $flag_component = 0;
   my @linker_tmp=();
   foreach $str (@arr_file)
   {
      if( $str =~ /\[$comp_name - Settings\]/ ) {
         $flag_component = 1;
      }
      if( $str =~ /"Dependencies"/ ) {
         $flag_linker = 1;
      }
      # write all dependencies between "Include Directories" - "end Inlude ..."
      if( $flag_component eq 1 && $flag_linker eq 1 )
      {
         if ( $str =~ /"Dependencies"/ || $str =~ /"end Dependencies"/ )
         {
            # miss this string
         } else {
            # if it's empty string we willn't add its into linker array
            if ( $str=~ /\w+/ ) {

               $str=cut($str);
               push(@linker_tmp,$str);

            } else {
               #print(" found the empty linker dependencis  \n"); # print debug
            }
         }# if - else
      };#if -else

      if( $str =~ /"end Dependencies"/ ) {
         $flag_linker = 0;
      }
      if ( $str =~ /\[$comp_name - end Settings\]/ ) {
         $flag_component = 0;
      }
   }; #foreach $str (@arr_file)

   #print"linker_tmp  @linker_tmp \n";
   return @linker_tmp;
};#sub find_links

#--------------------------------------------------------------------------------------------------
#
# input - component name
# open $path_to_build ("../Build/") directory and read all file names from there
# check if $link_name.(dll/lib/o/so/exe/out) has been built yet
#
# return "yes"  if this component has been built yet
#
sub read_check_directory() {

   my $comp_name=@_[0];
   if($VERBOSE ne "yes" ) {
      print "\n ======= $comp_name ========= \n";
   }

   my $path_to_build= $ROOTDIR . "RTL/Build/" . $comp_name . "/" . $CONFIG . "/";

   my @files=();
   my $file=0;
   my $out=0;

   $comp_name=cut($comp_name);
   my $dll=".dll";
   $dll=$comp_name.$dll;
   my $lib=".lib";
   $lib=$comp_name.$lib;
   my $exe=".exe";
   $exe=$comp_name.$exe;
   my $a=".a";
   $a=$comp_name.$a;
   my $so=".so";
   $so=$comp_name.$so;
   my $out=".out";
   $out=$comp_name.$out;
   if($VERBOSE ne "yes" ){
      #print("()path2build: $path_to_build\n");
   }

   my $dir = IO::Dir->new($path_to_build) or
                           die( "can't open $path_to_build: $!" );
   my @files = $dir->read();

   my @file_tmp=();
   foreach $file ( @files ) {
      if ( $file =~ /$comp_name/) {
          push(@file_tmp,$file);
      };
   };

   my $flag=0;
   foreach $file ( @file_tmp )
   {
      if ( $file =~ /$dll/ ) {
         $flag++;
      } elsif ($file =~ /$lib/) {
         $flag++;
      } elsif ($file =~ /$exe/) {
         $flag++;
      } elsif ($file =~ /$a/) {
         $flag++;
      } elsif ($file =~ /$so/) {
         $flag++;
      } elsif ($file =~ /$out/) {
         $flag++;
      }
   };

   if( $flag ge 1) {
      $out = 1;
   } else {
      $out = 0;
   };

   return $out;
};#sub read_directory

sub ModTimeRecursive($) # return -1 if not found
{
        my $name = @_[0];
        my $full_name;
        my $ftime;
        my $relative_ftime;
        my $index;
        my $found = 0;

        foreach $include( @includes )
        {
                $full_name = $ROOTDIR . $path . $include . "/" . $name;

                if ( -e $full_name )
                {
                        $found = 1;
                        last;
                }
        }

        if ($found == 0)
        {
                return -1;
        }

        $full_name = util::SimplifyPath( $full_name , "/" );

        $ftime = -M$full_name;

        $index = util::GetIndex(@include_files, $full_name);

        if ($index != -1)
        {
                return @include_ftimes[$index];
        }

        push(@include_files, $full_name);
        push(@include_ftimes, $ftime);

        $index = @include_ftimes - 1;

        open( FH, "<$full_name" ) || die "can not open $full_name file";
        my @strings = <FH>;
        close(FH);

        my $str;
        foreach $str( @strings )
        {
                if ($str =~ /^#include[\s\t]*\"(.*)\"/)
                {
                        $relative_ftime = ModTimeRecursive($1);

                        if ( ($relative_ftime != -1) && ($relative_ftime < $ftime) )
                        {
                                @include_ftimes[$index] = $relative_ftime;
                                $ftime = $relative_ftime;
                        }
                }
        }

        return @include_ftimes[$index];
}

#--------------------------------------------------------------------------------------------------



#--------------------------------------------------------------------------------------------------

sub GetModificationTime($)
{
        my $comp_name = @_[0];
        my $timemodification = 0;
        my $name;

        $comp_name=cut($comp_name);

        my $path = $ROOTDIR . "RTL/Build/" . $comp_name . "/" . $CONFIG . "/";

        my $output_name = cut( util::pars_output_name( @arr_file, $comp_name ) );
        $output_name =~ /([^\/]*)$/;
        $output_name = $1;

        my $slib = cut( util::get_platform_setting("SLIB") );
        my $dlib = cut( util::get_platform_setting("DLIB") );

        if ($output_name =~ /\.$slib/)
        {
                $name = $path . util::get_platform_setting("LIB_PREFIX") . $output_name;
        }
        else
        {
                $name = $path . $output_name;
        }

        if( -e $name )
        {
                $timemodification = -M$name;
        }

        return $timemodification;
}

#-------------------------------------------------------------------------------

# utility
sub cut() {
   my $string = @_[0];
   $string=~ s/^\s*//;
   $string=~ s/\s*$//;
   chomp($string);
   return $string;
};

#-------------------------------------------------------------------------------

1;
