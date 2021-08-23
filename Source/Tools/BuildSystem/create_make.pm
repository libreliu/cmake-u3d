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

package create_make;
use strict;
use File::Spec;
use util;
use build;

my $ROOTDIR=$ENV{U3D_SOURCE};
$ROOTDIR=~ s/\s+\z//;
$ROOTDIR=~ tr%\\%/%; # example :/perforce/CWG/C3D/Source/

my $U3D_compiler = cut( util::get_global_setting( "U3D_COMPILER" ) );
my $exec_linker = cut( util::get_global_setting( "U3D_EXEC_LINKER" ) );
my $dlib_linker = cut( util::get_global_setting( "U3D_DLIB_LINKER" ) );
my $slib_linker = cut( util::get_global_setting( "U3D_SLIB_LINKER" ) );
my $platform = cut( util::get_global_setting( "U3D_PLATFORM" ) );
my $dep_folder = cut( util::get_global_setting( "U3D_LINKDEP" ) );

my $slib_link_postfix = cut( util::get_platform_setting( "SLIB_LINK_POSTFIX" ) );
my $dlib_link_postfix = cut( util::get_platform_setting( "DLIB_LINK_POSTFIX" ) );

my $slib = cut( util::get_platform_setting( "SLIB" ) );
my $dlib = cut( util::get_platform_setting( "DLIB" ) );
my $exec = cut( util::get_platform_setting( "EXEC" ) );
my $obj  = cut( util::get_platform_setting( "OBJ" ) );

sub second_parse_linkflags(\$\$);

# input : ------------------------------------------------------------------
#         @include - all include file dependencies for this current project
#         @linker  - all  linker dependencies for this current project
#         $project_name - current project name
#         $output  - output project name e.g.: pfxdebuq.so (linux) or pfxmemory.dll - win32
#         $source  - where we can find that sources
#         $path2makedef - path to default make.def file located
#         $options  - specifiec for current project compie and linking options
#         $add_source - additional for current project  compilation sourses, which locate in the others directories
#         $export - all export dependencies for current project, like IFXDLL.def
#         $CONFIG - debug, release and etc
#         $link_targets - target files for linked libraries
#
# output : -----------------------------------------------------------------
#         that routine will create Makefile and
#           put its into this project root directory
#

sub collect_data(\@\@\$\$\$\$\$\@\@\@\@\$\@ )
{
  my $include         = shift(@_);
  my $linker          = shift(@_);
  my $project_name  = shift(@_);
  my $output          = shift(@_);
  my $source          = shift(@_);
  my $path2makedef    = shift(@_);
  my $path2build      = shift(@_);
  my $options         = shift(@_);
  my $additional_source = shift(@_);
  my $excl_source     = shift(@_);
  my $export          = shift(@_);
  my $CONFIG          = shift(@_);
  my $link_targets    = shift(@_);

   chomp($$output);
   chomp($$project_name);
   $$output=~ s/^\s*//;
   $$project_name=~ s/^\s*//;

   # removed spaces
   foreach (@$include){$_=~ s/^\s*//;}

   my $ans=0;

   $ans=create_make($include,
                    $linker,
                    $project_name,
                    $output,
                    $source,
                    $path2makedef,
                    $path2build,
                    $options,
                    $additional_source,
                    $excl_source,
                    $export,
                    $CONFIG,
                    $link_targets);


   if ($ans == 1) {
      print " =====================================================================================\n";
      print " =================ERROR during $$project_name Makefile generation=====================\n";
      print " =====================================================================================\n";
      exit 1;
   }

   return 0;

}; # sub collect_data()

#
# this routine dynamically create Makefile for Windows binaries
# input : ($include,$linker,$project_name,$output,$source,$path2makedef,$path2build);
# output: Makefile for current project
#
sub create_make(\@\@\$\$\$\$\$\@\@\@\@\$\@ )
{
    my $include         = shift(@_);
    my $linker          = shift(@_);
    my $project_name    = shift(@_);
    my $output          = shift(@_);
    my $source          = shift(@_);
    my $path2makedef    = shift(@_);
    my $path2build      = shift(@_);
    my $options         = shift(@_);
    my $add_source      = shift(@_);
    my $excl_source     = shift(@_);
    my $export          = shift(@_);
    my $CONFIG          = shift(@_);
    my $link_targets    = shift(@_);

    my $out_bin = $$output;
    $$output =~ /([^\/]*)$/;
    $$output = $1;

    my $CFG=$$CONFIG."/";
    $$output =~ /([^.]*)/;
    my $out_name = $1;
    my $path = $$source;
    my $i;
    my $var;

    $path =~ s/^\s*//;
    $path =~ s/\s*$//;
    # finish path for creation Makefiles ( e.g. : ../source/rtl/platform/win32/Makefile )
    my $fullname = $ROOTDIR.$path."Makefile";

    my $cflags = cut( @$options[0] );
    my $add_lib = cut( @$options[1] );
    my $slib_linkflags = cut( @$options[2] );
    my $dlib_linkflags = cut( @$options[3] );
    my $exec_linkflags = cut( @$options[4] );

    # additional processing to $add_lib : if we met Source/.../*.lib dependencies we will transform this
    # *.lib to full path
    # it is specially for VS7 generation

    $add_lib = addlib_modifying($add_lib);

    # modified include pathes add -I ,so : -I ../inc/pfxdebug.... -I ../inc/ifxobjects
    modified_include_path($include);

    # open and create if there isn't such file
    unless (open(FH, ">$fullname") )
    {
      print "\n fulname = \"$fullname\" cann't be open\n";
      return 1;
    }

    my $link_dependence;

    my $newpath2makedef=$ROOTDIR . "Config/make.def";
    my $new_path2build=$ROOTDIR . "RTL/Build/";

    if( -e $fullname )
    {
        print FH "include $newpath2makedef\n ";
        print FH "\n";

        print FH "# -------------- path to build directory ---------------\n";
        # like  out_dir=../../../Build/

        print FH "out_dir=$new_path2build\n";
        print FH "\n";

        my $addpath=$$project_name."/";
        print FH "out_dir_root=$new_path2build$addpath$$CONFIG\n";
        print FH "\n";

        my $u3d_out_dir = $ROOTDIR . util::get_global_setting( "U3D_OUTPUT" );
        print FH "u3d_out_dir=$u3d_out_dir\n";
        print FH "\n";

        print FH "# -------------- include dependences ---------------\n";
        print FH "inc_dep= ";
        foreach( @$include )
        {
           s/-I /-I/;
           print FH $_." ";
        }
        print FH "\n";

        print FH "# -------------- linking dependences ---------------\n";

        for( $i=0; $i < @$linker; $i++ )
        {
            chomp(@$linker[$i]);

            print FH "link_dep_$i=".@$linker[$i]."/$CFG\n";
        }

        print FH "slib_link_dep=";
        for( $i=0; $i < @$linker; $i++ )
        {
            if ( $$link_targets[$i]=~/\.$slib/ )
            {
                my $target = "\$(LIB_PREFIX)".$$link_targets[$i];
                print FH "\$(out_dir)\$(link_dep_$i)$target ";
            }
        }
        print FH "\n";

        my $config_name = $ROOTDIR."Config/projects.ini";
        open( CFH, "<$config_name" ) || die "can not open $config_name file";
        my @arr_file = <CFH>;
        close(CFH);

        print FH "dlib_link_dep=";
        if ( $dlib_link_postfix ne "" )
        {
            for( $i=0; $i < @$linker; $i++ )
            {
                if ( $$link_targets[$i]=~/\.$dlib/ )
                {
                    my $target = $$link_targets[$i];
                    $target =~ s/\.$dlib/$dlib_link_postfix/;
                    print FH "\$(out_dir)\$(link_dep_$i)$target ";
                }
            }
        }
        else
        {
            for( $i=0; $i < @$linker; $i++ )
            {
                if ( $$link_targets[$i]=~/\.$dlib/ )
                {
                    my $component_name = @$linker[$i];
                    my $target = cut( util::pars_output_name( @arr_file, $component_name ) );
                    print FH "\$(u3d_out_dir)/$target ";
                }
            }
        }
        print FH "\n";

        print FH "link_dep=\$(slib_link_dep) \$(dlib_link_dep)\n";

        #print FH "#---------------- output name ---------------------------------------\n";
        $$output=~s/^ +//; # remove all the fisrt spaces

        print FH "# ------------------ compilation & linking  --------------------------\n\n";
        #print FH "compiler_opt=\$(CFLAGS)\n";

        print FH "all:\$(wildcard *.cpp)   \n";
        print FH "all:\$(wildcard *.c)   \n";

        # routine  : $(filter-out @$excl_source, \$(wildcard *.cpp)) --> is used for exclude from compilation files from @$excl_source array
        #             $(wildcard *.cpp) - compile only file with "cpp" extension
        # @$add_source  --> additional file list which should be include in the compilation procedure
        #

        my @rc_files = grep( $_ =~ m/\.rc$/, @$add_source ); # get all resource files
        @$add_source = grep( $_ !~ m/\.rc$/, @$add_source ); # remove from input array

        my $OUT_OBJ = $ROOTDIR . "RTL/Build/" . $addpath . $CFG;
        my $target_exist = build::ExistTarget($$project_name, $$CONFIG);

        my $string = "";

        my $base_cflags = parse_flags($cflags, "", "");

        if ( $target_exist )
        {
            my @files_for_build = build::GetFilesForBuild($$project_name, $$CONFIG);

            foreach( @files_for_build )
            {
               $cflags = second_parse_cflags($base_cflags, $_);
               $string = "$U3D_compiler $cflags\n";
               $string =~ s/^\s*/\t/;
               print FH "$string";
            }
        }
        else
        {
            opendir source, $ROOTDIR.$path;
            my @files = readdir source;
            closedir source;
            my @files_post_excl = ();
            my $file;
            my $excl;
            foreach $file( @files )
            {
               my $present = 1;
               foreach $excl( @$excl_source )
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
            foreach( @$add_source )
            {
               push( @files_post_excl, $_ );
            }

            foreach( @files_post_excl )
            {
               if( /\.(cpp|c)$/ )
               {
                  $cflags = second_parse_cflags($base_cflags, $_);
                  $string = "$U3D_compiler $cflags\n";
                  $string =~ s/^\s*/\t/;
                  print FH "$string";
               }
            }
        }

        opendir SRC, $ROOTDIR.$path;
        my @src_files = readdir SRC;
        closedir SRC;
        @rc_files = (@rc_files, grep( $_ =~ m/\.rc$/, @src_files ) );

        my $rc_output = "";

        if( (@rc_files != 0 ) &&
            ( ( ($$output =~ /\.$slib/) && ($slib_linkflags =~ /\.RES/) ) ||
              ( ($$output =~ /\.$dlib/) && ($dlib_linkflags =~ /\.RES/) ) ||
              ( ($$output =~ /\.$exec/) && ($exec_linkflags =~ /\.RES/) ) ) )
        {
            foreach my $rc (@rc_files)
            {

               $rc_output = $out_name . ".RES";
               my $RC_conf_defines = "/d NDEBUG";

               if( $$CONFIG =~ /debug/i )
               {
                    $RC_conf_defines = "/d DEBUG /d _DEBUG";
               }

               my $version = $ROOTDIR."RTL/Kernel/Include/";
               my $rc_string=" rc $RC_conf_defines /i $version /fo $rc_output /v $rc  \n";

               $rc_string=~ s/^\s*/\t/;

               if( !( -e "$rc_output" ) )
               {
                    print FH "$rc_string";
               }
            }
        }

        if ( $slib_link_postfix eq "\.$obj" )
        {
            print FH "\tmkdir -p \$(out_dir_root)\/$dep_folder\n";

            my $cnt1 = 0;

            for( $i=0; $i < @$linker; $i++ )
            {
                if ( $$link_targets[$i]=~/\.$slib/ )
                {
                    print FH "\tcp -rf \$(out_dir)\$(link_dep_$i)*.\$(OBJ) \$(out_dir_root)\/$dep_folder\/\n";

                    my $cnt2 = 0;
                    my $lnk1 = cut( @$linker[$i] );

                    my @other_linker = util::pars_linker( @arr_file, $lnk1 );

                    for( my $j=0; $j < @other_linker; $j++ )
                    {
                        my $lnk2 = cut( @other_linker[$j] );

                        my $target = cut( util::pars_output_name( @arr_file, $lnk2 ) );

                        if ( $target=~/\.$slib/ )
                        {
                            $cnt2 = 1;
                            last;
                        }
                    }

                    if ( $cnt2 == 1 )
                    {
                        print FH "\tcp -rf \$(out_dir)\$(link_dep_$i)$dep_folder\/*.\$(OBJ) \$(out_dir_root)\/$dep_folder\/\n";
                    }

                    $cnt1++;
                }
            }
            print FH "\n";

            if( $cnt1 == 0 )
            {
                $slib_linkflags =~ s/\S*$dep_folder\/\*\.\$\(OBJ\)//;
                $dlib_linkflags =~ s/\S*$dep_folder\/\*\.\$\(OBJ\)//;
                $exec_linkflags =~ s/\S*$dep_folder\/\*\.\$\(OBJ\)//;
            }
        }

        my $base_slib_linkflags = $slib_linkflags;

        if( $$output =~ /\.$dlib/ )
        {
            $dlib_linkflags = parse_flags($dlib_linkflags, $out_name, $add_lib);

            $string = second_parse_linkflags($dlib_linkflags, $base_slib_linkflags);
            $string = $string."$dlib_linker $dlib_linkflags\n";
        }
        elsif( $$output =~ /\.$exec/ )
        {
            $exec_linkflags = parse_flags($exec_linkflags, $out_name, $add_lib);

            $string = second_parse_linkflags($exec_linkflags, $base_slib_linkflags);
            $string= $string."$exec_linker $exec_linkflags\n";
        }
        elsif( $$output =~ /\.$slib/ )
        {
            $slib_linkflags = parse_flags($slib_linkflags, $out_name, $add_lib);

            $string= "$slib_linker $slib_linkflags\n";
        }

        $string=~ s/^\s*/\t/;
        print FH "$string\n";


        if( $$output =~ /\.$exec/ || $$output =~ /\.$dlib/ )
        {
            my $out_dir = $ROOTDIR."RTL/Build/" . $addpath . $$CONFIG;
            my $OUTCP = $ROOTDIR . util::get_global_setting( "U3D_OUTPUT" ) . "/$out_bin";
            print FH "\tcp -f $out_dir\/$$output $OUTCP";
        }
   }
   else
   {
      print(" we can't find file : $fullname ");
   }
   close (FH);
   return 0;
}; #create_make

sub FindFileRecursive(\$\$)
{
        my $dir = shift(@_);
        my $search_file = shift(@_);

        opendir DIR, $dir;
        my @sources = readdir DIR;
        closedir DIR;

        my $source;
        my $full_name;
        my @dirs = ();

        foreach $source(@sources)
        {
                if ( ( $source ne "." ) and ( $source ne ".." ) )
                {
                        $full_name = $dir . "/" . $source;

                        if ( -d $full_name )
                        {
                                push(@dirs, $full_name);
                        }
                        else
                        {
                                if ( $source eq $search_file )
                                {
                                        return $full_name;
                                }
                        }
                }
        }

        my $res;

        foreach $dir( @dirs )
        {
                $res = FindFileRecursive($dir, $search_file);

                if ( $res ne "" )
                {
                        return $res;
                }
        }

        return "";
};

# modified include pathes add -I ,so : -I ../inc/pfxdebug.... -I ../inc/ifxobjects
sub modified_include_path(\@) {

   my $include = shift(@_);
   my $str;
   my $begin_str="-I "; # add  the first symbol -> "-I " ...
   my @tmparray=();

   foreach $str (@$include) {
      chomp($str);
      $str = $begin_str.$str;
      push(@tmparray,$str);
   }

   @$include=();
   # reinitialization:
   @$include=@tmparray;

}; #add_include_path

sub cut() {
   my $string = @_[0];
   $string=~ s/^\s*//;
   $string==~ s/\s*$//;
   chomp($string);
   return $string;
};

sub addlib_modifying() {

   my $add_lib=shift(@_);
   #
   # Source/Tools/Lib/Debug/IFXRenderSW.lib
   #

   my $str=$add_lib;
   my @arr=split(/ +/,$str);

   $add_lib="";
   foreach( @arr) {

      my  $str=$_;
      if ($_=~ m/^Source\//) { #find and set the full path
           $str =~ s/^Source\///;
           $str = $ROOTDIR . $str;

      }
      $add_lib="$add_lib $str";
   }

   return $add_lib;

} #addlib_modifying

sub parse_flags
{
    my $flags = $_[0];
    my $out_name = $_[1];
    my $add_lib = $_[2];

    #$flags =~ s/\$\(([^\)]*)\)/\\\$\($1\)/g;
    $flags =~ s/\$\[out_dir\]/\$\(out_dir_root\)/g;
    $flags =~ s/\$\[out_name\]/$out_name/g;
    $flags =~ s/\$\[add_lib\]/$add_lib/g;
    $flags =~ s/\$\[inc_dep\]/\$\(inc_dep\)/g;
    $flags =~ s/\$\[link_dep\]/\$\(link_dep\)/g;
    $flags =~ s/\$\[link_dep\]/\$\(link_dep\)/g;
    $flags =~ s/\$\[link_dep\]/\$\(link_dep\)/g;
    $flags =~ s/\$\[slib_link_dep\]/\$\(slib_link_dep\)/g;
    $flags =~ s/\$\[implib_link_dep\]/\$\(implib_link_dep\)/g;
    $flags =~ s/\$\[dlib_link_dep\]/\$\(dlib_link_dep\)/g;

    return $flags;
} #parse_flags

sub second_parse_linkflags(\$\$)
{
    my $base_linkflags=shift(@_);
    my $slib_linkflags=shift(@_);
    my $res = "";

    $slib_linkflags = $$slib_linkflags;

    if ( $$base_linkflags =~ /\$\[slib_link_dep_as_single\]/ )
    {
        $$base_linkflags =~ s/\$\[slib_link_dep_as_single\]/\$\(out_dir_root\)\/$dep_folder\/all_dep_lib\.$slib/g;

        $slib_linkflags =~ s/\S+\.$obj\s//g;
        $slib_linkflags =~ s/\S+\.\$\(OBJ\)\s//g;

        if ( $slib_linkflags =~ /(\$\[out_dir\]\/\S+)/ )
        {
            $slib_linkflags = $` . "\$\(out_dir_root\)\/$dep_folder\/all_dep_lib\.$slib" . $';
        }
        else
        {
            print "Can't find string that is refer to output object\n";
            exit 1;
        }

        $slib_linkflags =~ s/\$\[out_dir\]/\$\(out_dir_root\)/g;
        $slib_linkflags =~ s/\$\[add_lib\]//g;
        $slib_linkflags =~ s/\$\[link_dep\]//g;
        $slib_linkflags =~ s/\$\[slib_link_dep\]//g;
        $slib_linkflags =~ s/\$\[implib_link_dep\]//g;
        $slib_linkflags =~ s/\$\[dlib_link_dep\]//g;

        $slib_linkflags =~ s/^\s*//;
        $slib_linkflags =~ s/\s*$//;

        $res = "$slib_linker $slib_linkflags \$\(out_dir_root\)\/$dep_folder\/\*\.\$(OBJ)\n\t";
    }

    return $res;
}

sub second_parse_cflags
{
    my $flags = $_[0];
    my $file_full_name = $_[1];
    my $file_name = "";

    $file_full_name =~ /([^\/\\]+)\.(cpp|c)$/;
    $file_name = $1;

    $flags =~ s/\$\[file_full_name\]/$file_full_name/g;
    $flags =~ s/\$\[file_name\]/$file_name/g;

    return $flags;
}

1;
