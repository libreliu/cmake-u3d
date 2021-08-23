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

package util;
use strict;
use XML::Simple;
use Data::Dumper;
use XML::Parser::Expat;

my @include = ();
my @linker  = ();
my $output  = 0;
my $source  = 0;
my @add_source = ();
my $path2makedef = 0;
my $path2build   = 0;
my @options = ();
my @exclude=();
my $component_name;
my @tmp_source_arr_exclude=();

#---------------------------------------------------------------------------------------------------
#
# find all project components and put into @ component_list
#
sub pars_components(\$) {

my $file_name=shift(@_);
my $flag = 0;
my $tmp = 0;
my $component_name;

my @component=();
open( FH, "<$$file_name" ) || die "can not open $$file_name file";
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


#---------------------------------------------------------------------------------------------------
#
#  input - name of component which will be parsing for dependences determinate
#
sub pars_include(\@\$) {

   my $arr_file=shift(@_);
   my $name =shift(@_);

   @include=();
   my $flag_component = 0;
   my $flag_include   = 0;
   foreach my $str (@$arr_file) {

      if( $str =~ /\[$$name - Settings\]/ ) {
         $flag_component = 1;
      }
      if( $str =~ /"Include Directories"/ ) {
         $flag_include = 1;
      }
         # write all dependencies between "Include Directories" - "end Include ..."
         if( $flag_component eq 1 && $flag_include eq 1 )
         {
            if ( $str =~ /"Include Directories"/ || $str =~ /"end Include Directories"/ )
            {
               # miss this string
            } else {
               $str=~s/^ +//; # remove all the fisrt spaces

               $str=~ s/\s+//;
               $str=~ s/\s+\z//;
               $str=~ s/\s+//;
               push(@include,$str);
            }
         }

      if( $str =~ /"end Include Directories"/ ) {
         $flag_include = 0;
      }
      if ( $str =~ /\[$$name - end Settings\]/ ) {
        $flag_component = 0;
        return @include;
      }

   }; # foreach

}; # sub pars_include()


#---------------------------------------------------------------------------------------------------
#
#  input - name of component which will be parsing for linking determinate
#
sub pars_linker(\@\$) {

   my $arr_file=shift(@_);
   my $name =shift(@_);

   @linker=();
   my $flag_component = 0;
   my $flag_linker   = 0;
   foreach my $str (@$arr_file) {

      if( $str =~ /\[$$name - Settings\]/ ) {
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
                  push(@linker,$str);
               } else {
                  #print(" found the empty linker dependencis  \n"); # print debug
               }
            }# if - else
         }

      if( $str =~ /"end Dependencies"/ ) {
         $flag_linker = 0;
      }
      if ( $str =~ /\[$$name - end Settings\]/ ) {
        $flag_component = 0;
        return @linker;
      }

   };  # foreach
}; # sub pars_linker()

#---------------------------------------------------------------------------------------------------
#
#
#

sub pars_output_name(\@\$) {

   my $arr_file=shift(@_);
   my $name =shift(@_);
   my $flag_component = 0;
   my $flag_output = 0;

   foreach my $str (@$arr_file) {
      if( $str =~ /\[$$name - Settings\]/ ) {
         $flag_component = 1;
      }
      if( $str =~ /"Output"/ ) {
         $flag_output = 1;
      }

         if( $flag_component eq 1 && $flag_output eq 1 )
         {
            if ( $str =~ /"Output"/ || $str =~ /"End Output"/ )
            {
               # miss this string

            } else {
               return $str;
            }
         }

      if( $str =~ /"End Output"/ ) {
         $flag_output = 0;
      }
      if ( $str =~ /\[$$name - end Settings\]/ ) {
        $flag_component = 0;
      }

   }; # foreach

}; # pars_output_name()

#---------------------------------------------------------------------------------------------------
#
#
#

sub pars_sorce_path(\@\$) {

   my $arr_file=shift(@_);
   my $name =shift(@_);
   my $flag_component = 0;
   my $flag_source = 0;
   my @tmp_source_arr=();

   foreach my $str (@$arr_file) {
      if( $str =~ /\[$$name - Settings\]/ ) {
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
               $str =~ s/\$\(([^\)]*)\)/$ENV{$1}/eg;
               push(@tmp_source_arr,$str);
            }
         }

      if( $str =~ /"end Source Files"/ ) {
         $flag_source = 0;
      }
      if ( $str =~ /\[$$name - end Settings\]/ ) {
        $flag_component = 0;
        return @tmp_source_arr;
      }
   }; # foreach

   return @tmp_source_arr;
}; # sub pars_sorce_path

#---------------------------------------------------------------------------------------------------
#
#
#

sub pars_options(\@\$) {

   my $arr_file=shift(@_);
   my $name =shift(@_);

   my $flag_component = 0;
   my $flag_options   = 0;
   my @options=();

   foreach my $str (@$arr_file) {

      if( $str =~ /\[$$name - Settings\]/ ) {
         $flag_component = 1;
      }
      if( $str =~ /"options"/ ) {
         $flag_options = 1;
      }
         # write all dependencies between "Include Directories" - "end Include ..."
         if( $flag_component eq 1 && $flag_options eq 1 )
         {
            if ( $str =~ /"options"/ || $str =~ /"end options"/ )
            {
            } else {
               $str=~s/^ +//; # remove all the fisrt spaces
               push(@options,$str);
            }
         }

      if( $str =~ /"end options"/ ) {
         $flag_options = 0;
      }
      if ( $str =~ /\[$$name - end Settings\]/ ) {
        $flag_component = 0;

        return @options;
      }

   }; # foreach

}#pars_options


#---------------------------------------------------------------------------------------------------
#
#
#
sub pars_export(\@\$) {

   my $arr_file=shift(@_);
   my $name =shift(@_);

   my $flag_component = 0;
   my $flag_export   = 0;
   my @export=();

   foreach my $str (@$arr_file) {

      if( $str =~ /\[$$name - Settings\]/ ) {
         $flag_component = 1;
      }
      if( $str =~ /"Export Files"/ ) {
         $flag_export = 1;
      }
         if( $flag_component eq 1 && $flag_export eq 1 )
         {
            if ( $str =~ /"Export Files"/ || $str =~ /"end Export Files"/ )
            {
            } else {
               $str =~ s/^ +//;
               $str =~ s/^\s*//;
               $str =~ s/\s+\z//;

               my $aaa = length($str);
               if( $aaa != 0 ) {
                  push(@export,$str);
               }

            }
         }

      if( $str =~ /"end Export Files"/ ) {
         $flag_export = 0;
      }
      if ( $str =~ /\[$$name - end Settings\]/ ) {
        $flag_component = 0;

        return @export;
      }

   }; # foreach


}#pars_export

#---------------------------------------------------------------------------------------------------
#
#
#
sub pars_exclude_path(\@\$) {

   my $arr_file=shift(@_);
   my $name =shift(@_);

   my $flag_component = 0;
   my $flag_source_exclude = 0;
   @tmp_source_arr_exclude = ();

   foreach my $str (@$arr_file) {
      if( $str =~ /\[$$name - Settings\]/ ) {
         $flag_component = 1;
      }
      if( $str =~ /"Source Files exclude"/ ) {
         $flag_source_exclude = 1;
      }
         # write all dependencies between "Include Directories" - "end Include ..."
         if( $flag_component eq 1 && $flag_source_exclude eq 1 )
         {
            if ( $str =~ /"Source Files exclude"/ || $str =~ /"end Source Files exclude"/ )
            {
               # miss this string
            } else {
               $str=cut($str);
               push(@tmp_source_arr_exclude,$str);
            }
         }

      if( $str =~ /"end Source Files exclude"/ ) {
         $flag_source_exclude = 0;
      }
      if ( $str =~ /\[$$name - end Settings\]/ ) {
        $flag_component = 0;
        return @tmp_source_arr_exclude;
      }
   }; # foreach

   return @tmp_source_arr_exclude;
}  #pars_exclude_path



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


# -------------------------------------------------------------------------------------------------
#
#  generate GUID for each project
#
#sub guid() {
#   my $key;
#   #chomp($key=`UUIDGEN`);
#   $key=`UUIDGEN`;
#   #chop($key);
#   chomp($key);
#   $key=uc($key);
#   return $key;
#}

sub guid() {
   my $key;
   $key=`UUIDGEN`;
   $key=~ s/^\s*//;
   $key=~ s/\s+\z//;
   $key=uc($key);
   return $key;
}


#
#
#
sub vs7_name(\@\@\% ) {
   my $arr_file=shift(@_);
   my $component_name=shift(@_);
   my $vs7_name=shift(@_);

   my $flag_component = 0;
   my $flag_vs7_name = 0;
   my @tmp_source_arr=();


   #
   # go throuch all array and fill hash
   #
   foreach my $name (@$component_name) {

$flag_component = 0;
$flag_vs7_name = 0;
@tmp_source_arr=();

      foreach my $str (@$arr_file) {
         if( $str =~ /\[$name - Settings\]/ ) {
            $flag_component = 1;
         }
         if( $str =~ /"vs7name"/ ) {
            $flag_vs7_name = 1;
         }
            if( $flag_component eq 1 && $flag_vs7_name eq 1 )
            {
               if ( $str =~ /"vs7name"/ || $str =~ /"end vs7name"/ )
               {
                  # miss this string
               } else {
                  $str=~ s/\s*//;
                  chop($str);
                  #print">>>>>>>>>>>>>> vs7 name:$name,>>>>$str>>>>>>>>>>>>>>> \n";
                  $$vs7_name{"$name"}=$str;
                  last();
               }
            }

         if( $str =~ /"end vs7name"/ ) {
            $flag_vs7_name = 0;
         }
         if ( $str =~ /\[$name - end Settings\]/ ) {
           $flag_component=0;
         }
      }; # foreach  @arr_file

   }# foreach my $name (@$component_name)

   return %$vs7_name;

}; # vs7_name()


# -------------------------------------------------------------------------------------------------
#
#

sub get_tools_path() {

   my @source=@_[0];

   my $path2tools=0;
   my $str=@source[0]; #the first element - path to sources

   my @tmp=split(/\//,$str);
   my $tmpsize=@tmp;

   $tmpsize=$tmpsize-3;
   $tmpsize = "../" x $tmpsize;
   $path2tools=$tmpsize;
   my $add_path = "Tools/";
   $path2tools=$path2tools.$add_path;

   return $path2tools;
}; # sub get_tools_path()


sub parse_global_settings
{
   my %settings = ();
   my $src = $ENV{U3D_SOURCE};
   $src =~ s/\\/\//g;
   open GL, "<$src"."Config/u3d.conf" or die "no global settings available...\n";
   my @gl = <GL>;
   foreach( @gl )
   {
      if( /^[^\w\#]*(\w*)\s*=\s*([^\s]*)/ )
      {
        %settings = ( %settings, $1 => $2 );
      }
   }
   close GL;
   return %settings;
}

sub get_global_setting
{
   my %settings = parse_global_settings;
   return $settings{$_[0]};
}


sub parse_platform_settings
{
   my %glob = parse_global_settings();
   my $platform = $glob{U3D_PLATFORM};
   my $src = $ENV{U3D_SOURCE};
   $src =~ s/\\/\//g;

   my %settings = ();
   open PL, "<$src"."Config/make_$platform.def" or die "no platform settings available...\n";
   my @pl = <PL>;
   foreach( @pl )
   {
      if( /^[^\w\#]*(\w*)\s*=\s*(\S+[\x20-\xFF]*)/ )
      {
        %settings = ( %settings, $1 => $2 );
      }
   }
   close PL;
   return %settings;
}

sub get_platform_setting
{
   my %settings = parse_platform_settings;
   return $settings{$_[0]};
}

sub SimplifyPath {

    my $path = @_[0];
    my $spliter = @_[1];
    my $res_path = "";
    my $i;
    my $j;
    my $size;

    $path =~ tr%\\%\/%;

    my @array = split(/\//, $path);

    $size = @array;
    $i = 0;
    while ($i < $size-1) # $i+1 must be < $size
    {
        if ( (@array[$i] ne "..") && (@array[$i+1] eq "..") )
        {
                for ($j = $i; $j < $size-2; $j++) # $j+2 must be < $size
                {
                        @array[$j] = @array[$j+2];
                }
                $size = $size-2;

                if ($i > 0)
                {
                        $i--;
                }
        }
        else
        {
                $i++;
        }
    }

    for ($i = 0; $i < $size-1; $i++)
    {
        if ( (@array[$i] ne "") and (@array[$i] ne ".") )
        {
                $res_path = $res_path . @array[$i] . $spliter;
        }
    }
    $res_path = $res_path . @array[$size-1];

    if ($path =~ /^\//)
    {
        $res_path = $spliter . $res_path;
    }

    if ($path =~ /\/$/)
    {
        $res_path = $res_path . $spliter;
    }

    return $res_path;
}

#--------------------------------------------------------------------------------------------------

my @files_list;

sub GetFilesRecursive
{
        my $path = @_[0];

        @files_list = ();
        RecursiveFilesSearch($path);

        return @files_list;
}

sub RecursiveFilesSearch
{
        my $path = @_[0];
        opendir source, $path;
        my @files = readdir source;
        closedir source;

        foreach my $file( @files )
        {
                if ( ($file ne ".") && ($file ne "..") )
                {
                        if (-d $path.$file)
                        {
                                RecursiveFilesSearch($path.$file."/");
                        }
                        else
                        {
                                push(@files_list, $path.$file);
                        }
                }
        }
}

#------------------------------------------------------------------------------

#
# Find index of transferred element in transferred array.
# If element was not found return -1.
#

sub GetIndex(\@\$)
{
        my $list = shift(@_);
        my $name = shift(@_);
        my $res = -1;
        my $size = @$list;

        for (my $i=0; $i < $size; $i++)
        {
                if ( @$list[$i] eq $$name )
                {
                        $res = $i;
                        last;
                }
        }

        return $res;
}

1;