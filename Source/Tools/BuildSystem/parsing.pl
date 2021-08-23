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
use File::Copy;
use util;
use build;

my @components_list = ();

my $cur_dir=`pwd`;
$cur_dir=~ s/\s+\z//;
$cur_dir=~ tr%\\%/%;

my $build_os_type = util::get_global_setting( "U3D_PLATFORM" );
my @default_build;
my $sample;
if( $build_os_type eq "Win32") {
    @default_build = ( "Core", "ExportingPlugin", "ImportingPlugin", "RenderingPlugin", "SchedulingPlugin", "CoreStatic", "IDTFConverter", "TextureModifier" );
    $sample = "SamplePlayer";
    push(@default_build,$sample);
} elsif ( $build_os_type eq "Lin32" ) {
    @default_build = ( "Core", "ExportingPlugin", "ImportingPlugin", "RenderingPlugin", "SchedulingPlugin", "CoreStatic", "IDTFConverter", "TextureModifier" );
    $sample = "HelloWorld";
    push(@default_build,$sample);
    $sample = "SampleViewerX";
    push(@default_build,$sample);
    $sample = "winXPlayer";
    push(@default_build,$sample);
} elsif ( $build_os_type eq "Mac32" ) {
    @default_build = ( "Core", "ExportingPlugin", "ImportingPlugin", "SchedulingPlugin", "CoreStatic", "IDTFConverter", "TextureModifier" );
    $sample = "HelloWorld";
    push(@default_build,$sample);
    $sample = "RenderingPlugin";
    push(@default_build,$sample);
    $sample = "SampleViewerQrtz";
    push(@default_build,$sample);
    $sample = "QrtzPlayer";
    push(@default_build,$sample);
} else {
    print "  unknown OS type was found\n";
    exit 0;
}
my @OPTIONS=@ARGV;

my $build_os_type = util::get_global_setting( "U3D_PLATFORM" );
my @LO = ();
my $rebfl = 0;
my $relfl = 0;

for( my $i = 0; $i < @OPTIONS; $i++ )
{
   if( $OPTIONS[$i] ne "rebuild" )
   {
      push( @LO, $OPTIONS[$i] );
   }
   else
   {
      $rebfl = 1;
   }
   if( $OPTIONS[$i] eq "release" )
   {
      $relfl = 1;
   }
}
if( $rebfl )
{
   if( $relfl )
   {
      system "bash build.sh @LO clean release";
   }
   else
   {
      system "bash build.sh @LO clean debug";
   }
   exec "bash build.sh @LO";
}
@LO = ();
my $log = 0;
for( my $i = 0; $i < @OPTIONS; $i++ )
{
   if( $OPTIONS[$i] ne "--log" )
   {
      push( @LO, $OPTIONS[$i] );
   }
   else
   {
      $log = $OPTIONS[$i+1];
      $i++;
   }
}
$log =~ s/\\/\//g;
if( $log )
{
   exec "bash build.sh @LO | tee \"$log\"";
}
my $opt;
my $project="ALL";
my $verbose="yes"; # silentium as default
my $configuration;
# set input options flags
my $debug_flag    = "FALSE";
my $release_flag  = "FALSE";
my $clean_flag    = "FALSE";
my $rebuild_flag  = "FALSE";
my $rembin_flag   = "FALSE";
my $noerr_flag    = "FALSE";
my $build_specific_flag  = "FALSE"; # if will build the specific project
# 0 step : initialize input options flags
   foreach my $curr_opt ( @OPTIONS ) {
      if( $curr_opt eq "release" ) {
         $release_flag =  "TRUE";
      } elsif ($curr_opt eq "debug") {
         $debug_flag   = "TRUE";
      } elsif ($curr_opt eq "--verbose") {
         $verbose="no"; # we wiil have the full compile and link time output
      } elsif ($curr_opt eq "clean") {
         $clean_flag = "TRUE";
      } elsif ($curr_opt eq "rebuild") {
         $rebuild_flag = "TRUE";
      } elsif ($curr_opt eq "--bin") {
         $rembin_flag = "TRUE";
      } elsif ($curr_opt eq "NO_ERR_OUT") {
         $noerr_flag = "TRUE";
      } elsif ($curr_opt) {
         my @component_list = ();
         my $found = 0;
         @component_list = parse_components();
         foreach (@component_list) {
           chomp($_);
           chomp($curr_opt);
           if($_ eq $curr_opt)
            {
              $found = 1;
           }
         }
         if ( !$found ) {
            if( -e "../../Config/Projects/$curr_opt.xml" )
            {
                print "\n\nWARNING: looks like \"$curr_opt\" is a project name\nthat was not added to the workspace file.\n";
                print "If yes, please add it to workspace_*.xml\n";
            }
            print "\n\n";
            print " *********** ERROR : unknown command line options : $curr_opt ************* \n";
            print "\n\n";
            exit 1;
         } else {
            $project=$curr_opt;
            $build_specific_flag="TRUE";
         }
      }
   }
#0 -- incorrect input : ./build.sh --verbose debug release Core
   if ($release_flag eq "TRUE" && $debug_flag eq "TRUE" ) {
      print " incorrect point configuration !\n";
      exit 1;
   }
#1 step -----------------------------------------------------------------------
   if($release_flag eq "TRUE" )
    {
      $configuration="release";
   } else {
      $configuration="debug";
   }
   File::Copy::copy("../../Config/make_$build_os_type.def","../../Config/make.def") or die "Copy failed: $!";
   if( $clean_flag ne "TRUE" )
   {
      # the common stage - parsin and create
      system ($^X,"parsing_and_create.pl", $configuration );
      if ( $?  ) {
         print "\n\n";
         print " *********** Command Line Parser ERROR  ************* \n";
         print "\n\n";
         exit 1;
      }
   }

# build all -----------------------------------------------------------------------

if ( $project eq "ALL" && $clean_flag ne "TRUE" )
{
   my $make_all = $cur_dir."/__make_all";
   my $make = $cur_dir."/__make";
   system "rm -f $make_all";
   system "rm -f $make";

   foreach $project( @default_build )
   {
      my $res = build::build_project($project,$verbose,$configuration,@components_list);
      if ( $res == 0  ) {
         print "\n\n";
         print " *********** BUILD $project ERROR  ************* \n";
         print "\n\n";
         &errout(1);
      }
   }
   &errout(0);
}

# 2  -> as default this is reserve version : if we will want to have some default version
# 3  -> build.sh  rebuild as default IFXDLL debug
# 4  -> build.sh  $project debug
# 5  -> build.sh  $project release
# 6  -> build.sh  clean - full clean (debug/release)
# 7  -> build.sh  clean debug   --verbose
# 8  -> build.sh  clean release  --verbose
# 9  -> build.sh  clean $project (debug/release) --verbose
# 10 -> build.sh  clean $project (debug/release)  --verbose

# 3 step -----------------------------------------------------------------------
if ($rebuild_flag eq "TRUE") {
   system ($^X,"clean_all.pl","ALL","ALL",$verbose,"FALSE" );
   if ( $?  ) {
      print "\n\n";
      print " *********** CLEAN ALL BUILD ERROR  ************* \n";
      print "\n\n";
      exit 1;
   }
   foreach( @default_build )
   {
      system "perl parsing.pl $_ @OPTIONS NO_ERR_OUT";
      if ( $?  ) {
         print " \n\n*********** ERROR building $_ ************* \n\n\n";
         &errout(1);
      }
   }
   &errout(0);
}

# 4,5 step ---------------------------------------------------------------------
if ($build_specific_flag eq "TRUE" && $clean_flag eq "FALSE" ) {
   my $make_all = $cur_dir."/__make_all";
   my $make = $cur_dir."/__make";
   system "rm -f $make_all";
   system "rm -f $make";

   my $res = build::build_project($project,$verbose,$configuration,@components_list);
   if ( $res == 0  ) {
      print "\n\n";
      print " *********** BUILD $project ERROR  ************* \n";
      print "\n\n";
      &errout(1);
   }
   &errout(0);
}

# 6 - 10 steps------------------------------------------------------------------
if($clean_flag eq "TRUE" ) {
   $configuration="ALL";
   if ($debug_flag eq "TRUE") {
      $configuration="debug";
   } elsif ( $release_flag eq "TRUE" ) {
      $configuration="release";
   }
   system ($^X,"clean_all.pl",$project,$configuration,$verbose,$rembin_flag );
   if ( $?  ) {
      print "\n\n";
      print " *********** CLEAN ALL BUILD ERROR  ************* \n";
      print "\n\n";
      exit 1;
   }
   exit 0;
}

sub errout
{
  my $make_all = $cur_dir."/__make_all";
  my $make = $cur_dir."/__make";

  if( $noerr_flag eq "TRUE" )
  {
    exit @_[0];
  }

  open FH, $make_all;
  my @lines = <FH>;
  close FH;
  my $succ = 0;
  my $fail = 0;

  print "\n\n";

  print "------------------- Build done -------------------\n";
  foreach( @lines )
  {
    / (\d+)( fatal)? error/;
    chomp;
    print;
    if( $1 )
    {
      $fail++;
      print ": failed\n";
    }
    else
    {
      $succ++;
      print ": succeeded\n";
    }
  }
  print "--------------------------------------------------\n";
  print "Entire build: $succ succeeded, $fail failed\n";
  system "rm -f $make_all";
  system "rm -f $make";

  exit @_[0];
}

sub parse_components()
{
   my $flag = 0;
   my $tmp = 0;
   my @component=();
   my $component_name;
   open( FH, "<../../Config/projects.ini" ) || die "can not open \"projects.ini\" file";
   while( defined ( $component_name = <FH> ) ){
       chomp $component_name;
       #[Workspace]  ... #[End Workspace]
       if( $component_name =~ /\[Workspace\]/ )
        {
          $flag = 1;
       }
       if ( $flag eq 1 ) {
          if( $component_name !~ /\[Workspace\]/ && $component_name !~ /\[End Workspace\]/){
           #  print(" :: $component_name \n");
             $tmp = length($component_name);
             if ( $tmp > 2 ) {
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
}
