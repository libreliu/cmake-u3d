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

$global_root = "D:/Users/U3D/UnitTesting/".$ARGV[0]."/".$ARGV[1];
$test_name = $ARGV[1];
$test_category = $ARGV[0];

sub test_name
{
  return $test_name;
}

sub u3dsp
{
  my $u3dsp = $ENV{U3D_SOURCE};
  $u3dsp =~ s/ //g;
  $u3dsp =~ s/\\/\//g;
  return $u3dsp;
}

sub compare
{
  my( $cat, $test ) = @_;
  open EXE, "<$global_root/$cat/$test/$cfg/$test.exe";
  my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks) = stat(EXE);
  my $exetime = $mtime;
  close EXE;
  opendir CUR, "$global_root/$cat/$test";
  my @files = sort (readdir CUR);
  foreach( @files )
  {
    if( /\.c$/ || /\.cpp$/ || /\.h$/ )
    {
      open SRC, "$global_root/$cat/$test/$_";
      ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks) = stat(SRC);
      if( $mtime > $exetime )
      {
        return 1;
      }
      close SRC;
    }
  }
  closedir CUR;
  return 0;
}

sub compare_lin
{
  my( $cat, $test ) = @_;
  open EXE, "<$global_root/$cat/$test/$cfg/$test.out";
  my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks) = stat(EXE);
  my $exetime = $mtime;
  close EXE;
  opendir CUR, "$global_root/$cat/$test";
  my @files = sort (readdir CUR);
  foreach( @files )
  {
    if( /\.c$/ || /\.cpp$/ || /\.h$/ )
    {
      open SRC, "$global_root/$cat/$test/$_";
      ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks) = stat(SRC);
      if( $mtime > $exetime )
      {
        return 1;
      }
      close SRC;
    }
  }
  closedir CUR;
  return 0;
}


$global_root = ".";
$cfg = $ARGV[0];
chomp $cfg;
if( !$cfg )
{
  $cfg = "release";
}
#my $ostype = $ENV{OS};
@wcat = addf $global_root;
@cat = ();
foreach( @wcat )
{
  @wtest = addf $_;
  /(^.*)[\/\\]([^\/\\]*$)/;
  $cat = $2;
  push( @cat, $cat );
  foreach( @wtest )
  {
    /(^.*)[\/\\]([^\/\\]*$)/;
    print "\n\nBUILD STARTED: $2 ($cat), configuration: $cfg\n";
    my $fname = $2;
    if( $^O =~ m/Win32/i ||  $^O =~ m/CYGWIN/i )
    {
        $c = compare $cat, $fname;
    }else {
        $c = compare_lin $cat, $fname;
    }
    if( $c )
    {
      system "perl create_make.pl $cat $fname $cfg";
      system "make -s -C \"$global_root/$cat/$fname\"";
      $answer = $?;
      if( $answer )
      {
        $dst = "../TestSystem/Results/builderr.txt";
        my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks) = stat($dst);
        my ($lnsec,$lnmin,$lnhour,$lnmday,$lnmon,$lnyear,$lnwday,$lnyday,$lnisdst)=localtime($mtime);
        my ($lnsecc,$lnminc,$lnhourc,$lnmdayc,$lnmonc,$lnyearc,$lnwdayc,$lnydayc,$lnisdstc)=gmtime(time);
        if( $lnmday == $lnmdayc && $lnmon == $lnmonc && $lnyear == $lnyearc )
        {
          open DST, ">>$dst";
        }
        else
        {
          open DST, ">$dst";
        }
        print DST "Test $fname ($cat) build failed. Make return code: $answer\r\n";
        close DST;
        close HTML;
      }
       system "mkdir -p \"$global_root/../TestSystem/Tests/$cat\"";
       if( $^O =~ m/Win32/i ||  $^O =~ m/CYGWIN/i)
       {
           system "cp -f \"$global_root/$cat/$fname/$cfg/$fname.exe\" \"$global_root/../TestSystem/Tests/$cat/$fname.exe\"";
       }
       else
       {
            system "cp -f \"$global_root/$cat/$fname/$cfg/$fname.out\" \"$global_root/../TestSystem/Tests/$cat/$fname.out\"";
       }
    }
    else
    {
      print "$fname: up-to-date\n";
    }
  }
}
