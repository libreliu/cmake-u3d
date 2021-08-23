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

use lib "../../Tools/BuildSystem";
require util;

use IO::File;
my $groupTestsName;
my $TestName;

sub addf
#
# adds all subdirectories found in directory
# into the @files array
#
# $_[0] - directory name
#
{
  my @files = ();
  my $curdir = $_[0];
  opendir cur, $curdir;
  my @localfiles = sort (readdir cur);
  closedir cur;
  for(my $j=2; $j<@localfiles; $j++)
  {
    $subdir = $curdir."/".$localfiles[$j];
    $status = -d($subdir);

     if($status == 1)
    {
      @files = ( @files, $subdir );
      @files = ( @files, &addf( $subdir ) );
    }
  }
  return @files;
}

sub filter( \$\@ )
#
# excludes all directories from list that don't contain files
# with specified extension
# $_[0] - needed extension
# $_[1] - list of directories
#
{
  my $expf = shift( @_ );
  my $dirlist = shift( @_ );
  my @files;
  my $expmatch;
  my @matched = ();
  foreach( @$dirlist )
  {
    opendir cur, $_;
    @files = sort (readdir cur);
    closedir cur;

    $expmatch = 0;
    for(my $j=2; $j<@files; $j++)
    {
      if( !( -d "$_/".$files[$j] ) && ( index( $files[$j], ".$$expf" ) > -1 ) )
      {
        $expmatch = 1;
      }
    }
    if( $expmatch == 1 )
    {
      push( @matched, $_ );
    }
  }
  return @matched;
}

sub includedirs
#
# returns the list of all source directories that contain header files
#
{
  if( -e "_vcproj.inc" )
  {
    open INC, "<_vcproj.inc";
    my @lines = <INC>;
    close INC;
    return $lines[0];
  }
  my $u3dsp = $ENV{U3D_SOURCE};
  $u3dsp =~ s/ //g;
  $u3dsp =~ s/\\/\//g;
  my @include = ();
  push( @include, $u3dsp."RTL/Component/Include" );
  push( @include, $u3dsp."RTL/Component/SceneGraph" );
  push( @include, $u3dsp."RTL/Platform/Include" );
  push( @include, $u3dsp."RTL/Kernel/Include" );
  push( @include, $u3dsp."Samples/SceneHelpers" );
  push( @include, $u3dsp."Samples/SampleViewerWin32" );
  push( @include, $u3dsp."Tools/UnitTesting/Include" );
  push( @include, $u3dsp."Tools/UnitTesting/Shapes/Include" );
  $res = "";
  foreach( @include )
  {
    $res .= "$_; ";
  }
  return $res;
}

sub includedirsformakefile
#
# returns the list of all source directories that contain header files
#
{
    if( -e "_make.inc" )
    {
        open INC, "<_make.inc";
        my @lines = <INC>;
        close INC;
        return $lines[0];
    }
    my $u3dsp = $ENV{U3D_SOURCE};
    $u3dsp =~ s/ //g;
    $u3dsp =~ s/\\/\//g;
    my @include = ();
    push( @include, $u3dsp."RTL/Component/Include" );
    push( @include, $u3dsp."RTL/Component/SceneGraph" );
    push( @include, $u3dsp."RTL/Platform/Include" );
    push( @include, $u3dsp."RTL/Kernel/Include" );
    push( @include, $u3dsp."Samples/SceneHelpers" );
    if( $^O =~ m/Win32/i ||  $^O =~ m/CYGWIN/i )
    {
        push( @include, $u3dsp."Samples/SampleViewerWin32" );
    }
    elsif ( $^O =~ m/linux/i )
    {
        push( @include, $u3dsp."Samples/SampleViewerX" );
    }
    elsif ( $^O =~ m/darwin/i )
    {
        push( @include, $u3dsp."Samples/SampleViewerQrtz" );
    }
    push( @include, $u3dsp."Tools/UnitTesting/Include" );
    push( @include, $u3dsp."Tools/UnitTesting/Shapes/Include" );
    $res = "";
    foreach( @include )
    {
        $res .= " -I $_";
    }
    return $res;
}

sub source
#
# returns all files in the $global_root directory that have specified extension
# $_[0] - needed extension
#
{
  my $extf = $_[0];
  opendir cur, $global_root;
  my @files = sort (readdir cur);
  my $res = "";
  foreach( @files )
  {
    if( index( $_, ".$extf" ) == ((length $_) - (length ".$extf")) && index( $_, ".$extf" ) > -1 )
    {
      $res .= "<File\n\t\t\t\tRelativePath=\"".$_."\">\n\t\t\t</File>\n\t\t\t";
    }
  }

  closedir cur;
  return $res;
}

sub parser
#
# generate .vcproj file for unit test project
# $_[0] - input file
# $_[1] - output file
# $_[2] - location of unit test source code
#
{
  my $src = $_[0];
  my $dest = $_[1];
  my $dest_dir = $_[2];
  open PAT, $src;
  my @lines = <PAT>;
  my @vcproj = ();
  my $tmpline;
  foreach( @lines )
  {
    if( /(^.*)<!--Perl: (.*)-->(.*$)/ )
    {
      $tmpline = $1.( eval $2 ).$3;
      while( $tmpline =~ /(^.*)<!--Perl: (.*)-->(.*$)/ )
      {
        $tmpline = $1.( eval $2 ).$3;
      }
      push( @vcproj, $tmpline."\n" );
    }
    else
    {
      push( @vcproj, $_ );
    }
  }
  close PAT;

  my $vc = IO::File->new( ">$dest_dir/$dest" );
  print $vc @vcproj;
  truncate $vc, tell DEST;
  close $vc;
}# parser.pl

$global_root    = "./".$ARGV[0]."/".$ARGV[1];
#$global_root = "D:/Users/U3D/perforce/CWG/C3D/Source/Tests/UnitTesting/".$ARGV[0]."/".$ARGV[1];
$test_name      = $ARGV[1];
$test_category  = $ARGV[0];
$cfg            = $ARGV[2];
$groupTestsName = $ARGV[0]."/".$ARGV[1];
$TestName       = $ARGV[1];

sub test_name
{
  return $test_name
}

sub u3dsp
{
  my $u3dsp = $ENV{U3D_SOURCE};
  $u3dsp =~ s/ //g;
  $u3dsp =~ s/\\/\//g;
  return $u3dsp;
}

sub subsystem
{
  if( $subsystem eq "console" )
  {
    return 1;
  }
  if( $subsystem eq "window" )
  {
    return 2;
  }
  return 0;
}

sub config
{
  return $cfg;
}

sub config_defs
{
  if( $cfg =~ /Release/i )
  {
    return "/D NDEBUG";
  }
  else
  {
    return "/D DEBUG /D _DEBUG";
  }
}

sub link_defs
{
  if( $cfg =~ /Release/i )
  {
    return "";
  }
  else
  {
    return "/DEBUG";
  }
}

sub cflags
{
    if ( $^O =~ m/linux/i )
    {
        return "-DLINUX";
    }
    elsif ( $^O =~ m/darwin/i )
    {
        return "-DMAC32";
    }
}

sub config_Lin32_defs
{
    if( $cfg =~ /Release/i || $cfg =~ /release/i || $cfg =~ /RELEASE/i)
    {
        return "-DNDEBUG";
    }
    elsif( $cfg =~ /Debug/i || $cfg=~ /debug/i || $cfg=~ /DEBUG/i )
    {
        return "-DDEBUG -D_DEBUG";
    }
    else
    {
        print " \t unknown configuration was found \n";
        print " \t currently TS support release and debug testing modes only  \n";
        exit 1;
    }
}

sub runtime
{
  if( $cfg =~ /Release/i )
  {
    return "/MT";
  }
  else
  {
    return "/MTd";
  }
}

sub runtimeLin32
{
  if( $cfg =~ /Release/i || $cfg =~ /release/i || $cfg =~ /RELEASE/i)
  {
    return "-D_MT";
  }
  elsif( $cfg =~ /Debug/i || $cfg=~ /debug/i || $cfg=~ /DEBUG/i )
  {
    return "-D_MTd";
  }
  else
  {
      print " \t unknown configuration was found \n";
      print " \t currently TS support release and debug testing modes only  \n";
      exit 1;
  }
}

if( !( -e "$global_root/$test_name.h" ) )
{
  print "\nCan\'t find file \"$test_name.h\" in folder \"$global_root\"\n\n";
  exit 1;
}

if( $^O =~ m/Win32/i ||  $^O =~ m/CYGWIN/i )
{
    unlink "$global_root/$test_name.sln";
    parser "vcproj.pat", "$test_name.vcproj", $global_root;
    parser "cl.pat", "cl.txt", $global_root;
    parser "link.pat", "link.txt", $global_root;
    parser "makefile.pat", "Makefile", $global_root;
}
elsif ( $^O =~ m/linux/i )
{
    parser "gcc.pat", "gcc.txt", $global_root;
    parser "linkLin.pat", "linkLin.txt", $global_root;
    parser "makeFileLin.pat", "Makefile", $global_root;
}
elsif ( $^O =~ m/darwin/i )
{
    parser "gcc.pat", "gcc.txt", $global_root;
    parser "linkMac.pat", "linkMac.txt", $global_root;
    parser "makeFileMac.pat", "Makefile", $global_root;
}
else
{
    print " the unknown OS was found \n";
    exit 1;
}

sub groupTestsName
{
    return $groupTestsName;
}

sub TestName
{
    return $TestName;
}

sub compiler
{
    my $compiler = util::get_global_setting( "U3D_COMPILER" );
    return $compiler;
}

sub OBJ
{
     my $dirname = $ARGV[0]."/".$ARGV[1];
     my $t = u3dsp();
     my $path = $t . "Tests/UnitTesting/" . $dirname;
     opendir cur, $dirname;
     my @allfiles = sort (readdir cur);
     closedir cur;
     my $src;
     foreach my $file (@allfiles){
         if($file =~ m/.cpp$/)
         {
             $file =~ s/.cpp$/.o/;
             $file = $path . "/" . $cfg ."/".  $file;
             $src = $src." ". $file;
         }
     }
     return $src;
}
