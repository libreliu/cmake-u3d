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

use IO::File;

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
  my $u3dsp = $ENV{U3D_SOURCE};
  $u3dsp =~ s/ //g;
  $u3dsp =~ s/\\/\//g;
#  my @files = addf($u3dsp."Prototype");
  my @files = addf($u3dsp."Samples");
  @files = ( @files, addf( $u3dsp."RTL" ) );
  @files = ( @files, addf( $u3dsp."Tests" ) );
  my $exp = "h";
  my @include = filter( $exp, @files );
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
  my $u3dsp = $ENV{U3D_SOURCE};
  $u3dsp =~ s/ //g;
  $u3dsp =~ s/\\/\//g;
  my @files = addf($u3dsp."Samples");
  @files = ( @files, addf( $u3dsp."RTL" ) );
  @files = ( @files, addf( $u3dsp."Tests" ) );
  my $exp = "h";
  my @include = filter( $exp, @files );
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

sub source_lib
#
# returns all files in the UnitTesting/Source directory that have specified extension
# $_[0] - needed extension
#
{
  my $u3dsp = $ENV{U3D_SOURCE};
  $u3dsp =~ s/ //g;
  $u3dsp =~ s/\\/\//g;
  $u3dsp .= "Tools/UnitTesting/Source";
  my $extf = $_[0];
  opendir cur, $u3dsp;
  my @files = sort (readdir cur);
  my $res = "";
  foreach( @files )
  {
    if( index( $_, ".$extf" ) == ((length $_) - (length ".$extf")) && index( $_, ".$extf" ) > -1 )
    {
      $res .= "<File\n\t\t\t\tRelativePath=\"".$u3dsp."/".$_."\">\n\t\t\t</File>\n\t\t\t";
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
      chomp $tmpline;
      push( @vcproj, $tmpline."\n" );
    }
    else
    {
      push( @vcproj, $_ );
    }
  }
  close PAT;
  my $vc = IO::File->new( ">$dest_dir/$dest" );
  print ">$dest_dir/$dest";
  print $vc @vcproj;
  truncate $vc, tell DEST;
  close $vc;
}

sub create_test_launcher( \$\@\$\$ )
{
  my $test_name = shift( @_ );
  my $test_category = shift( @_ );
  my $exec = shift( @_ );
  my $timeout = shift( @_ );

  if( -e "$global_root/$$test_name"."Launcher.cpp" )
  {
    print "\nFile \"$$test_name"."Launcher.cpp\" already exist in folder \"$global_root\"\n\n";
    return 1;
  }
  print "Creating $global_root/$$test_name"."Launcher.cpp ...";
  my $launcher = IO::File->new( ">$global_root/$$test_name"."Launcher.cpp" );
  my $var2;
  print $launcher "//Exec: $$exec\n";
  print $launcher "//Timeout: $$timeout\n";
  foreach( @$test_category )
  {
    /(^.)(.*$)/;
    $_ = $1;
    $_ = uc $_;
    $var2 = $2;
    $var2 = lc $var2;
    $_ .= $var2;
    print $launcher "//Category: $_\n";
  }
  print $launcher "#include \"IFXTest.h\"\n";
  print $launcher "#include \"IFXSuite.h\"\n";
  print $launcher "#include \"$$test_name.h\"\n\n";
  print $launcher "using namespace U3DTestSuite;\n\n";
  print $launcher "int main()\n{\n\t$$test_name test;\n";
  print $launcher "\ttest.NewLog();\n";
  print $launcher "\ttest.Run();\n";
  print $launcher "\ttest.Report();\n";
  print $launcher "\ttest.ReleaseLog();\n";
  print $launcher "\tif( test.GetNumFailed() ) return 1;\n";
  print $launcher "\t\telse return 0;\n";
  print $launcher "}\n";
  close $launcher;
}

sub create_suite_launcher( \$\@ )
{
  my $test_name = shift( @_ );
  my $tests = shift( @_ );
  my @test_names = ();
  foreach( @$tests )
  {
    /([^\/\\]*$)/;
    push( @test_names, $1 )
  }

  if( -e "$global_root/$$test_name"."Launcher.cpp" )
  {
    print "\nFile \"$$test_name"."Launcher.cpp\" already exist in folder \"$global_root\"\n\n";
    exit 1;
  }
  print "creating $global_root/$$test_name"."Launcher.cpp ...";
  my $launcher = IO::File->new( ">$global_root/$$test_name"."Launcher.cpp" );
  print $launcher "#include \"IFXTest.h\"\n";
  print $launcher "#include \"IFXSuite.h\"\n";
  print $launcher "#include \"IFXCategory.h\"\n";
  foreach( @$tests )
  {
    print $launcher "#include \"$_.h\"\n";
  }
  print $launcher "\nusing namespace U3DTestSuite;\n\n";
  print $launcher "\nint main()\n{\n\tIFXSuite suite( \"$$test_name\" );\n";
  print $launcher "\ttry\n\t{\n";
  foreach( @test_names )
  {
    print $launcher "\t\tsuite.AddTestX( new $_ );\n"
  }
  print $launcher "\t}\n\tcatch( IFXTestSuiteError& e )\n\t{\n\t\t";
  print $launcher "printf( e.GetText() );\n";
  print $launcher "\t\texit( 1 );\n\t}\n";
  print $launcher "\tsuite.Run();\n";
  print $launcher "\tU8 nFail = suite.Report();\n";
  print $launcher "\tsuite.Free();\n";
  print $launcher "\treturn nFail;\n";
  print $launcher "}\n";
  close $launcher;
}

$subsystem = $ARGV[0];
$global_root = $ARGV[1];
$test_name = $ARGV[2];
$exec = $ARGV[3];
$timeout = $ARGV[4];
for( $i = 5; $i < @ARGV; $i++ )
{
  push( @test_category, $ARGV[$i] );
}

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

sub header_info
{
  $result = "";
  $result .= "//Exec: $exec\n";
  $result .= "//Timeout: $timeout\n";
  foreach( @test_category )
  {
    /(^.)(.*$)/;
    $_ = $1;
    $_ = uc $_;
    $var2 = $2;
    $var2 = lc $var2;
    $_ .= $var2;
    $result .= "//Category: $_\n";
  }
  return $result;
}

sub utheaders_console
{
  if (subsystem == 1)
  {
    return "#include \"UTHeaders.h\""
  }
  return "";
}

sub utheaders_windows
{
  if (subsystem == 2)
  {
    return "#include \"UTHeaders.h\""
  }
  return "#include \"U3DSceneUtilities.h\"";
}


if( !( -e "$global_root/$test_name.h" ) )
{
  print "\nCan\'t find file \"$test_name.h\" in folder \"$global_root\", creating...\n\n";
  parser "hidden/header.pat", "$test_name.h", $global_root;
}

if( !( -e "$global_root/$test_name.cpp" ) )
{
  print "\nCan\'t find file \"$test_name.cpp\" in folder \"$global_root\", creating...\n\n";
  parser "hidden/cpp.pat", "$test_name.cpp", $global_root;
}

if( $subsystem eq "console" )
{
  create_test_launcher $test_name, @test_category, $exec, $timeout;
}
else
{
  if( -e "$global_root/$test_name"."Launcher.cpp" )
  {
    print "\nFile \"$test_name"."Launcher.cpp\" already exist in folder \"$global_root\"\n\n";
  }
  else
  {
    print "Creating $global_root/$$test_name"."Launcher.cpp ...";
    parser "hidden/launcherwin.pat", $test_name."Launcher.cpp", $global_root;
  }
}

parser "hidden/vcproj.pat", "$test_name.vcproj", $global_root;
parser "hidden/cl.pat", "cl.txt", $global_root;
parser "hidden/link.pat", "link.txt", $global_root;
parser "hidden/makefile.pat", "Makefile", $global_root;
