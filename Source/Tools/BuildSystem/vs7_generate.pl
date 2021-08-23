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
use vs7_vcproj;
use vs7_sln;
use util;
use File::Find;
#use XML::Simple;
use Data::Dumper;
use util;

my $slib = cut( util::get_platform_setting( "SLIB" ) );
my $dlib = cut( util::get_platform_setting( "DLIB" ) );
my $exec = cut( util::get_platform_setting( "EXEC" ) );

my %global_settings = util::parse_global_settings;
my %platform_settings = util::parse_platform_settings;

sub cut_arr($);
sub cut($);

my @OPTIONS=@ARGV;

my $uuidgen=`uuidgen.exe`;
 if($uuidgen eq "") {
   print "\n";
   print "ERROR, don't set path to \"...\\Microsoft Visual Studio\\Common\\Tools\"  \n";
   print "\n";
   exit 1;
}

vs7_generate();

# -------------------------------------------------------------------------------------------------
#

sub vs7_generate() {

   #
   # This file converts the input XML project format ("project_win.xml") to Visual C++ project file format
   #
   my @component_list = ();   # all project component list
   my @include_arr = ();      # array for include file dependencies
   my @linker_arr  = ();      # array for linker
   my @include = ();
   my @linker  = ();
   my $output  = 0;
   my $source  = 0;
   my @add_source=();
   my $path2makedef=0;
   my $path2build=0;
   my @options = ();
   my @exclude=();

   # tmp array
   my @tmp_source_arr_exclude = ();

   my @arr_file = ();     # write all .ini file into array
   my $component_name;
   my @tmp_source_arr=();

   #
   # will contains all "cpp" and "h" file name which contains into all CWG/C3D/Source/ dir's
   #
   my @global_cpp=();
   my @global_header=();
   my $answer=0; # local

   my %guid;  # will contains name and GUID
   my %vs7_name; # will contains project name and vs7name.vcproj name

   my $file_name="../../Config/projects.ini";

   #
   #2
   # fill all *.cpp files which were located into CWG/C3D/Source,
   #     so we should go to "cd ../" and start FindGlobal() routine
   #
   use Cwd;
   my $currpath=Cwd::cwd();
   my $tmp;
   my $tmp1;
   my $tmp2;
   my $sz;
   my @global_cpp;
   my @global_header;

   if ($currpath=~ m/cygdrive/ )
   {
         $sz=length("/cygdrive/");   # ==d/perforce/CWG/C3D/Source/Tools
         $currpath=substr($currpath,$sz);

         $currpath=~ s/Tools//;      # ==d/perforce/CWG/C3D/Source
         $currpath=~ s/tools//;      # ==d/perforce/CWG/C3D/Source

         #replaced "d" --> "d:"
         $tmp2=":";
         $sz=length($currpath);
         my $tmp=substr($currpath,0,-sz+1);
         $tmp1=$tmp;
         $tmp=$tmp.$tmp2; # == d:
         my $tmp1=substr($currpath,1);# ==/perforce/CWG/C3D/Source

         $tmp1=$tmp.$tmp1;            # == d:/perforce/CWG/C3D/Source
         $currpath=$tmp1;

   } else {

      $currpath=~ s/Tools//;      # ==d:/perforce/CWG/C3D/Source

   }

   my $cpp=qr/\.cpp$/;
   my $h=qr/\.h$/;
   File::Find::find(\&FindGlobal,$currpath);
   sub FindGlobal {
      my $src_name=$File::Find::name;
      if($src_name=~ $cpp || $src_name=~/\.c$/ || $src_name=~/\.cxx$/ ){
         push(@global_cpp,$src_name);
      }
      if($src_name=~ $h || $src_name=~/\.hxx$/){
         push(@global_header,$src_name);
      }
   }

   #
   # 3
   #  the current VS7 file project component generation
   #

   @component_list = util::pars_components($file_name);

   # write "*.ini" file
   open( FH, "<$file_name" ) || die "can not open $file_name file";
   @arr_file = <FH>;
   close(FH);
   #
   # fill %vs7_name
   #
   util::vs7_name(@arr_file,@component_list,%vs7_name);

   print "\n\n\n";
   # all the following flags will be extracted from win_make.def files
   # these are common for all projects

   my @CFLAGS_make=();     #CFLAGS = /nologo /W3 /MDd /Gm /GX /Zi /Od
   my @DEFINES_make=();     #DEFINES = /D DEBUG /D _DEBUG /D "IFXAPI_EXPORT" /D WINDOWS /D "_WINDOWS"
   my $CLINKFLAGS_make=(); #CLINKFLAGS = /LD
   my $CLIB_make=();       #C_LIB=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib

   my $configutaion;
   $configutaion="debug";
   @CFLAGS_make       = extract_cflags_frommake($configutaion,0);
   @DEFINES_make      = extract_cflags_frommake($configutaion,1);
   $CLINKFLAGS_make   = extract_clinkflags_frommake($configutaion);
   $CLIB_make         = extract_clib_frommake($configutaion);

   my $size=0;
   foreach $component_name ( @component_list ) {

         @include = ();
         @include = util::pars_include( @arr_file,$component_name );

         @linker = ();
         @linker = util::pars_linker( @arr_file,$component_name );

         $output  = 0;
         $output = util::pars_output_name( @arr_file,$component_name );

         $source  = 0;
         @add_source = ();
         @tmp_source_arr = ();
         @tmp_source_arr = util::pars_sorce_path( @arr_file,$component_name );

         # extract $source and @add_source from @tmp_source_arr
         $source = @tmp_source_arr[0];

         #2
         $size=0;
         $size = @tmp_source_arr;
         for(my $i=1; $i < $size; $i++ ) {
               my $line = @tmp_source_arr[$i];
               $line=~ s/^\s*//;
               $line==~ s/\s*$//;
               chomp($line);
               push(@add_source,$line);
         }

         my @export = util::pars_export( @arr_file,$component_name );
         foreach (@export){
            $_=~ s/^\s*// ;
            $_=~ s/^\t*// ;
            $_=~ s/\s+\z// ;
            chomp($_);
         }

         @options = ();
         @options = util::pars_options( @arr_file,$component_name );
         my @path2makedef  = split(/\//,$source);
         $path2makedef  = @path2makedef;

         $path2makedef = $path2makedef - 4;
         $path2makedef = "../" x $path2makedef;
         $path2build   = $path2makedef;

         # find all exlude from compilation procedure files
         @exclude=();
         @exclude=util::pars_exclude_path( @arr_file,$component_name );

         my $COMMONLINKFLAGS = $CLINKFLAGS_make;
         $COMMONLINKFLAGS =~ tr/=/   /;
         my $COMMONCLIB = $CLIB_make;

         my %__tree;
         my $flag_config = 0;
         my @optnew = ("","","");
         my @opt = ();
         my $cfg = "";
         my $content;
         foreach my $str( @options )
         {
           if( $cfg && $str =~ /CONFIG:/i && $str !~ /CONFIG:$cfg/i )
           {
             $flag_config = 0;
             @opt = map( cut( $_ ), @opt ); # Delete leading and trailing spaces.
             $optnew[0] = "ADD_CFLAGS=".filter_cflags( $opt[0] );

             if( $output =~ /\.$slib/ )
             {
                $optnew[1] = "ADD_LINKFLAGS=".filter_linkflags( $opt[2] );
             }
             elsif( $output =~ /\.$dlib/ )
             {
                $optnew[1] = "ADD_LINKFLAGS=".filter_linkflags( $opt[3] );
             }
             elsif( $output =~ /\.$exec/ )
             {
                $optnew[1] = "ADD_LINKFLAGS=".filter_linkflags( $opt[4] );
             }

             $optnew[2] = "ADD_LIB=".$opt[1];

             @optnew = grep( $_ ne "", @optnew ); # Remove empty lines.
             $content = join( "\n", @optnew );
             @opt = ();
             %__tree = ( %__tree, $cfg => $content );
           }
           if( $str =~ /CONFIG:(.*)/i )
           {
             $cfg = $1;
             $flag_config = 1;
           }
           if( $flag_config eq 1 && !( $str =~ /CONFIG:/i) )
           {
             $str=~s/^ +//; # remove all the fisrt spaces
             push(@opt,$str);
           }
         }
         @opt = map( cut( $_ ), @opt ); # Delete leading and trailing spaces.
         $optnew[0] = "ADD_CFLAGS=".filter_cflags( $opt[0] );

         if( $output =~ /\.$slib/ )
         {
            $optnew[1] = "ADD_LINKFLAGS=".$opt[2];
         }
         elsif( $output =~ /\.$dlib/ )
         {
            $optnew[1] = "ADD_LINKFLAGS=".$opt[3];
         }
         elsif( $output =~ /\.$exec/ )
         {
            $optnew[1] = "ADD_LINKFLAGS=".$opt[4];
         }
         $optnew[1] = filter_linkflags( $optnew[1] );

         $optnew[2] = "ADD_LIB=".$opt[1];
         @optnew = grep( $_ ne "", @optnew ); # Remove empty lines.
         $content = join( "\n", @optnew );
         %__tree = ( %__tree, $cfg => $content );

# create current project
         $answer=vs7_vcproj::create_vcproj (
                  @include,
                  @linker,
                  $component_name,
                  $output,
                  $source,
                  $path2makedef,
                  $path2build,
                  @options,
                  @add_source,
                  @exclude,
                  @global_cpp,
                  @global_header,
                  %guid,
                  %vs7_name,
                  @CFLAGS_make,
                  @DEFINES_make,
                  @export,
                  $COMMONLINKFLAGS,
                  $COMMONCLIB,
                  %__tree
         );


         if ($answer == "1" ) {
            print "===========================================================================================\n";
            print "=========  ERROR - cannot create $component_name file description project  ================\n";
            print "===========================================================================================\n";
            exit 1;
         }
         $COMMONLINKFLAGS=0;
         $COMMONCLIB=0;

      }; # foreach $component_name ( @component_list )

   #
   # 4
   # the VS7 file solution generation
   #

      print "\n";
      print " Rebuilding  \"U3D solution project\" \n";
      print "\n";

       #
       # fill configuration suite
       #
       my @configuration = ();
       foreach ( @options )
       {
          if( /CONFIG:(.*)/ ) {
               push(@configuration, $_ );
          }
       }

      $answer=vs7_sln::create_sln( @arr_file,@component_list, %guid, %vs7_name, @configuration  );
      if ($answer == "1" ) {
         print "===========================================================================================\n";
         print "======================       ERROR - cannot create solution  project       ================\n";
         print "===========================================================================================\n";
         exit 1;
      }

      my $inifile="../../Config/projects.ini";
      if ( -e $inifile ) {
         unlink ($inifile);
      }

}; #vs7_generate()

#------------------------------------------------------------------------------------------------------
# extract CFLAGS from make_win.def file
#
sub extract_cflags_frommake($$){

   my $configutation=$_[0];
   my $opt = $_[1];

   my @FLAGS;
   my @arr=();
   my $str;
   chdir("../../Config/");

   my $makefile = "make_Win32.def";

   open (FH, "<$makefile" ) ||
            die " cannot open and write from ../../Config/make.def file";
   while( defined ( $str = <FH> ) ) {

      if ( $str=~ m/^\CFLAGS/) {
         push(@FLAGS,$str);
      }
   }
   close FH;
   my @arr=__cflag_extract(@FLAGS, $opt);

   chdir("../Tools/BuildSystem/");

   return @arr;
}; #extract_make_flags

#
#  1. removed /c flag if there
#  2. remove all /D prefixes
#  3. insert ";" as delimiter ( _DEBUG;)
#
sub __cflag_extract(\@\$) {

   my @CFLAGS=shift(@_);
   my $mode = shift(@_);
   my @newarr=();
   my $str;

   my $str=@CFLAGS[0];  # $str == /c /nologo /MDd /W3 /Gm /GX /Zi /Od /D DEBUG /D _DEBUG /D "IFXAPI_EXPORT" /D WINDOWS /D "_WINDOWS"
   $str=~ s/CFLAGS//;
   $str=~ tr%"% %;
   my @arr=split("/",$str);

   my $delim=";";
   my $t;
   my $sz=@arr;
   for (my $ind=1; $ind < $sz; $ind++ ) {
      $t=@arr[$ind];
         $t=~ s/^\s*//;
         $t=~ s/\s+\z//;
      chomp($t);
      $t=$t.$delim;
#      if($t!~ m /\bc\b/){ # remove compiler options "/c"
      if( $t =~ /^D (.*$)/ && $mode==1 )
      {
         push(@newarr,$1);
      }
      if( $t !~ /^D / && $mode==0 )
      {
         push(@newarr,$t);
      }

   }

   return @newarr;  #== c; nologo; MTd; W3; Gm; GX; Zi; Od; DEBUG; _DEBUG; IFXAPI_EXPORT; WINDOWS; _WINDOWS;

} # cflag_convert

#------------------------------------------------------------------------------------------------------

sub extract_clinkflags_frommake($) {

   my $configutation=@_[0];
   my $FLAGS;
   my $str;
   chdir("../../Config/");

   my $makefile = "make_Win32.def";


   open (FH, "<$makefile" ) || die " cannot open and write from ../../Config/make.def file";
   while( defined ( $str = <FH> ) ) {
      if ( $str=~ m/^\CLINKFLAGS/) {
         $str=~ s/CLINKFLAGS//;
         $str=~ s/^\s*//;
         $str=~ s/\s+\z//;
         $FLAGS=$str;
         last;
      }
   }
   close FH;
   chdir("../Tools/BuildSystem/");
   return $FLAGS;
}#extract_clinkflags_frommake

#------------------------------------------------------------------------------------------------------

sub extract_clib_frommake($) {

   my $configutation=@_[0];
   my $FLAGS;
   my $str;
   chdir("../../Config/");

   my $makefile = "make_Win32.def";

   open (FH, "<$makefile" ) || die " cannot open and write from ../../Config/make.def file";
   while( defined ( $str = <FH> ) ) {
      if ( $str=~ m/^\C_LIB/) {
         $str=~ s/C_LIB//;
         $str=~ s/^\s*//;
         $str=~ s/\s+\z//;
         $FLAGS=$str;
         last;
      }
   }
   close FH;
   chdir("../Tools/BuildSystem/");
   return $FLAGS;
}#extract_clib_frommake

# -------------------------------------------------------------------------------------------------
#
#

sub cut_arr($) {

   my $string = shift( @_ );
   my @array;

   @array = split( m/\n/, $string );  # Cut text into lines.
   @array = map( cut( $_ ), @array ); # Delete leading and trailing spaces.
   @array = grep( $_ ne "", @array ); # Remove empty lines.

   return @array;

}; # sub cut_arr

# -------------------------------------------------------------------------------------------------
#
#

sub cut($) {
   my $string = shift(@_);
   $string =~ s/^\s*//;
   $string =~ s/\s*$//;
   chomp($string);
   return $string;
};


sub parseexternalsettings
{
   my $i = $_[0];
   my $set = $_[1];
   my @options = $_[2];
   my @compkeys = ();
   my $ckn = 0;

   my $optstr = join( " ", @compkeys );
   $optstr =~ s/\n//gm;
   if( !$optstr )
   {
      $optstr = " ";
   }
   return $optstr;
}



sub parse
{
   my $val = $_[0];
   $val =~ s/\$\((U3D_[^\)]*)\)/$global_settings{$1}/eg;
   $val =~ s/\$\(([^\)]*)\)/$platform_settings{$1}/eg;
   return $val;
}

#
# extract all options
#
sub pars_make_options(\$\@)
{
   my $CONFIG = shift(@_);
   my $strings =shift(@_);
   print @$strings;
   my @options = ();
   my $flag_config = 0;
   foreach my $str( @$strings )
   {
      if( $str =~ /CONFIG:$$CONFIG/i ) {
         $flag_config = 1;
      }

         # write all dependencies between "Include Directories" - "end Include ..."
         if( $flag_config eq 1 && !( $str =~ /CONFIG:/i || $str =~ /^\s$/ ) )
         {
            $str=~s/^ +//; # remove all the fisrt spaces
            push(@options,$str);
         }

      if( $str =~ /CONFIG:/i && $str !~ /CONFIG:$$CONFIG/i ) {
         $flag_config = 0;
      }
   }
   return @options;
}#pars_options

sub filter_linkflags
{
    my $linkflags = $_[0];

    $linkflags =~ s/(\S*\$\[\S+\]\S*)//g;
    $linkflags =~ s/(\S*\$\(\S+\)\S*)//g;
    $linkflags =~ s/(\S+\.def\S+)//g;
    $linkflags =~ s/"  "/" "/g;

    return $linkflags;
} #filter_linkflags

sub filter_cflags
{
    my $cflags = $_[0];

    $cflags =~ s/(\S*\$\[\S+\]\S*)//g;
    $cflags =~ s/(\S*\$\(\S+\)\S*)//g;
    $cflags =~ s/"  "/" "/g;

    return $cflags;
} #filter_cflags
