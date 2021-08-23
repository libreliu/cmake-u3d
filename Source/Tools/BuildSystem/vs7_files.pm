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


package vs7_files;
use strict;
use util;
use Cwd;
use IO::Dir;
use File::Find;
use File::Basename;


sub GetHeaderFiles(\@\@\$);

#
# This file converts the input XML project format ("project_win.xml") to Visual C++ project file format
#

#
# This module has been written specially for U3D project.
# all interace files will be search in the "../RTL/Component/Include/" directory
#

#
# extact all interface files from @cpp and @header files
#
# input : ------------------------------------------------------------------
#         $source - current project name location
#         @cpp    - all  @cpp files
#         @header - all  @header files list
#
# output : return all interfaces header files list
#


my $ROOTDIR="../../";

sub GetInterfaceFiles(\$\@\@\@) {
    my $srcpath = shift(@_);  # e.g : CWG\C3D\Source\RTL\Comonent\ImageCodec1\
    my $cpp_list= shift(@_);  # e.g : d:\perforce\CWG\C3D\Source\RTL\Component\ImageCodec1\*.cpp
    my $h_list  = shift(@_);  # e.g : d:\perforce\CWG\C3D\Source\RTL\Component\ImageCodec1\*.h
    my $include = shift(@_);

    my @InterfaceFiles=(); # output array
    my @curr_list=();      # temporary array

    # 1---------------------------------------------------------------------
    # parsing all cpp and header files ( @$cpp_list and @$h_list),
    # extract all records like "#include file.h",
    # insert this in the current @curr_list array

    my @arr=();
    foreach (@$cpp_list) {
       my $file=$_;
       
       @arr=parsing($file);

       # add to @curr_list
       foreach (@arr){
          push(@curr_list,$_);
       }
       @arr=();
    } #foreach (@$cpp_list)

    foreach (@$h_list) {
        my $file=$_;

        $file=~ tr%\\%/%;
        @arr=parsing($file);

        # add to @curr_list
        foreach (@arr){
                push(@curr_list,$_);
        }
        @arr=();
    }# foreach (@$h_list)

    # 2---------------------------------------------------------------------
    # search files with the same name into "RTL/Component/Include/" dir and into
    #                                      "RTL/Kernel/Include"
    #

    # path relative Tools dir ( without CWG/C3D/Source/ path )

    # merge into one array
    my @GlobalHeaderArray  = GetHeaderFiles ( @curr_list, @$include, $$srcpath );

    # 3 ---------------------------------------------------------------------
    # compare and remove the unnecessary files
    # @curr_list contains all header which has been extracted from *.cpp, *.h files
    #

    my $size1 = @GlobalHeaderArray;
    my $size2 = @$h_list;
        
    my $i = 0;
    while ($i < $size1)
    {
        my $file = @GlobalHeaderArray[$i];
        my $flag = 0;
        
        my $j = $i+1;
        while($j < $size1)
        {
                if (@GlobalHeaderArray[$j] eq $file)
                {
                        splice(@GlobalHeaderArray,$j,1);
                        $size1--;
                        next;
                }
                
                $j++;
        }
        

        for ($j = 0; $j < $size2; $j++)
        {
                if (@$h_list[$j] eq $file)
                {
                        splice(@GlobalHeaderArray,$i,1);
                        $size1--;
                        $flag = 1;
                        last;
                }
        }

        if ($flag == 1)
        {
                next;
        }
        
        $i++;
    }

    @InterfaceFiles = @GlobalHeaderArray;
    return @InterfaceFiles;

 }#GetInterfaceFiles


#-------------------------------------------------------------------------------
#
# extract all records like "#include file.h" and  save
#
sub parsing() {
   my $file=$ROOTDIR."RTL/Build/All/".@_[0];
   my @arr_file=();
   my @outarr=();
   open(FILEHANDLE, "<$file") || die(" cannot open $file file! \n ");
      @arr_file = <FILEHANDLE>;
   close(FILEHANDLE);

   foreach(@arr_file) {
      my $str=$_;  #  #include "CIFXImageCodec1.h"
      $str=~ s/^\s*//;
      $str=~ s/\s+\z//;

      if($str=~ m/^#include/ ) {

         $str=~ s/#include//;

         if( $str=~ m/\s+\"/) {
             $str=~ s/\s+\"//;
             if($str=~ m/"$/){

               $str=~ s/"$//;
               push(@outarr,$str);

            }# end

         } else {
            # we don't processing cases with the standart libraries
            # headers like <stdio.h>  and etc
         }

      }# start

   }# foreach

   return @outarr;
}#sub parsing()

#-------------------------------------------------------------------------------
#
#   $path1 = "../RTL/Component/Include/"; # all U3D project interface files located in this dir's
#   $path2 = "../RTL/Kernel/Include/";
#
#   return all file.h from $path1 & $path2 directories
#

sub  GetGlobalHeaderFiles() {

   my $path2include=@_[0];  # ../RTL/Component/Include/
   my @tmp_headerArray=();
   my @headerArray=();
   my $glupath=0; # take a full path and glue to search header file name

   #
   # get full path to Include dir's
   #
   $glupath=GetPath2Include($path2include);

   # read from root directive
    use IO::Dir;
    my $dir = new IO::Dir $path2include;
    if (defined $dir) {
        @tmp_headerArray=$dir->read();
    }

    my $slash="/";
    my $new_glupath;
    my @new_headerArray=();
    #TODO this one nested level. We should write code for arbitrary nested levels
    foreach (@tmp_headerArray){

       if ( -d $path2include.$_  ) {

          $_=~ s/^\s*//;
          $_=~ s/\s_\z//;
          #$new_glupath=$glupath.$_.$slash; # as $glupath + $new directory path
          $new_glupath=$glupath.$_.$slash;

          my $newpath=$path2include.$_;

           # debug
           # print " >>> $newpath\n";

          @new_headerArray=();
          my $newdir = new IO::Dir $newpath;
          if (defined $newdir) {
             if( $_ !~ m/\.$/ && $_ !~ m/\.\.$/){ # remove all "." ".." dir's
                @new_headerArray=$newdir->read();
             }
          }

          foreach(@new_headerArray){
             if($_=~ m/\.h$/ || $_=~ m/\.hxx$/) {
                  my $name=$_;
                  $name=$new_glupath.$name;
                  push(@headerArray,$name); # added header files form the nested dir's
             }
          }
          @new_headerArray=();

       } else {
          if($_=~ m/\.h$/ || $_=~ m/\.hxx$/) {
             my $name=$_;
             #print "!!!!! $glupath      \n";
             #$glupath as common for all files
             $name=$glupath.$name;  # glue the  full path from root dir, like "d:/"
             push(@headerArray,$name); # only Header files
          }
       }

       $new_glupath=$glupath;

    }#foreach (@tmp_headerArray)

     #debug
     #foreach (@headerArray){ print "!!!!! $_      \n"; } exit 1;

   return @headerArray;
} #GetGlobalHeaderFiles

#
#
#
sub GetPath2Include() {

   my $path2include=@_[0];
   my $currpath=getcwd;

   return $path2include;


   #..................................................................
   my $sz=length("/cygdrive/");   # ==d/perforce/CWG/C3D/Source/Tools
   $currpath=substr($currpath,$sz);

   $currpath=~ s/Tools//;      # ==d/perforce/CWG/C3D/Source

   #replaced "d" --> "d:"
   my $tmp2=":";
   $sz=length($currpath);
   my $tmp=substr($currpath,0,-sz+1);
   my $tmp1=$tmp;
   $tmp=$tmp.$tmp2; # == d:
   $tmp1=substr($currpath,1);# ==/perforce/CWG/C3D/Source

   $tmp1=$tmp.$tmp1;            # == d:/perforce/CWG/C3D/Source
   $currpath=$tmp1;

   $currpath=~ s/\s+ \z//;
   chomp($currpath);

         #debug
         #print ">>>>>> currpath $currpath\n"; exit 1;
         #$path1 = "../RTL/Component/Include/"
         #$path2 = "../RTL/Kernel/Include/";

   if($path2include=~ m/RTL\/Component\/Include\//) {

      my $dst="RTL/Component/Include/";
      $currpath=$currpath.$dst;

   } elsif ($path2include=~ m/RTL\/Kernel\/Include\// ) {

      my $dst="RTL/Kernel/Include/";
      $currpath=$currpath.$dst;

   }# elsif ($path2include=~ m/Tests\/MOA\/Xtras\/Include\//) {
    #  my $dst = "Tests/MOA/Xtras/Include/";
    #  $currpath = $currpath.$dst;

   #} else {
   #}

   #print " ####### 354 $currpath \n";
   return $currpath;
}# GetPath2Include


sub GetHeaderFiles(\@\@\$) {

    my $headfiles = shift(@_);
    my $include   = shift(@_);
    my $path      = shift(@_);

    my @OutputHeaderArray = ();

    # debug
    #1 foreach(@$headfiles){ print"   >>>>>> $_ \n";}
    #2 foreach(@$include){ print"    >>>>>> $_ \n";}
    #3 print"   >>>>>>>>>>>> $$path \n"; exit 1;

    foreach my $incl ( @$include ) {
    # get full path 2 include directory
        my $newpath = $ROOTDIR . $$path . $incl;
    #read from root directive
        use IO::Dir;
        my $dir = new IO::Dir $newpath;
        my @tmp_arr = ();
        if (defined $dir) {
            @tmp_arr = $dir->read();
        }
        my @tmp_h_arr = ();
        foreach(@tmp_arr) {
            if ($_ =~ m/\.h$/ || $_ =~ m/\.hxx$/) {
                push(@tmp_h_arr,$_);
            }
        }

        #search "h" file from @$headfiles list if yes - then add this file to @OutputHeaderArray array
        
        foreach my $headerfilename (@$headfiles)
        {
            my $i = 0;
            my $size = @tmp_h_arr;
            
            while ($i < $size)
            {
                if( @tmp_h_arr[$i] eq  $headerfilename )
                {
                    my $res_path = "../../../" . $$path . $incl . "/" . @tmp_h_arr[$i];
                    my $spliter = "\\";
                    $res_path = util::SimplifyPath( $res_path , $spliter );
                    push ( @OutputHeaderArray , $res_path );
                    splice(@tmp_h_arr,$i,1);
                    next;
                }
                
                $i++;
            }
        }
    }# include


    return @OutputHeaderArray;

} # GetHeaderFiles

1;
