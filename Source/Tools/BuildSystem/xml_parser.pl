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
use XML::Simple;
use util;

my %global_settings = util::parse_global_settings;
my %platform_settings = util::parse_platform_settings;

my $cxx = $global_settings{U3D_COMPILER};
my $exec_linker = $global_settings{U3D_EXEC_LINKER};
my $dlib_linker = $global_settings{U3D_DLIB_LINKER};
my $slib_linker = $global_settings{U3D_SLIB_LINKER};
my $platform = $global_settings{U3D_PLATFORM};

my @arr=();
my $string=0;

my $file_name="projects.ini";
my $xml_file_name=$ARGV[0]."project_$platform.xml";

sub write_project_file(\$\$\@\$\@\@\@\@\$ );
sub write_workspace(\@);
sub cut_arr($);
sub cut($);
sub parse;

my $prj_description;

#
# parsing input  "project_win.xml" file
# copy projects.ini to ../RTL/Config/ catalog
#

my $tree = XMLin( $xml_file_name, suppressempty => "", forcearray => [ "project_description" ] );

open(FH, ">$file_name") ||  die(" cannot create such file : $file_name! \n ");
if( -e $file_name )
{
    my $project_descriptions = $tree->{ project_description };

    my @workspace;
    foreach my $project_description ( @$project_descriptions )
    {

        #print "***** $project_description->{ project_name } ";

        my $proj_name = cut( $project_description->{ project_name } );
        push(@workspace,$proj_name);
    }

    write_workspace(@workspace);

    foreach my $project_description ( @$project_descriptions )
    {
        my $project_name         = cut( $project_description->{ project_name         } );
        my $output_name          = cut( $project_description->{ output_name          } );
        my @source_dir           = cut_arr( $project_description->{ source_dir       } );
        my $export               = cut ( $project_description->{ export              } );
        my @source_exclude_files = cut_arr( $project_description->{ source_exclude_files } );
        my @include_dirs         = cut_arr( $project_description->{ include_dirs         } );
        my @dependencies         = cut_arr( $project_description->{ dependencies         } );
        my @options              =  $project_description->{ options              };

        $prj_description = $project_description;

        my @optnew=();
        my $i = 0;

        #for( $i = 0; $i < 2; $i++ )
        while( $options[0][$i]{cfg} )
        {
            push( @optnew, "CONFIG:".$options[0][$i]{cfg} );

            @optnew = ( @optnew, cut( parseexternalsettings( $i, $cxx, @options )." ".parseexternalsettings( $i, $cxx."_".$platform, @options ) ) );
            @optnew = ( @optnew, cut( parseexternalsettings( $i, "add_$cxx", @options )." ".parseexternalsettings( $i, "add_$cxx"."_$platform", @options ) ) );
            @optnew = ( @optnew, cut( parseexternalsettings( $i, $slib_linker."_slib", @options )." ".parseexternalsettings( $i, $slib_linker."_slib_".$platform, @options ) ) );
            @optnew = ( @optnew, cut( parseexternalsettings( $i, $dlib_linker."_dlib", @options )." ".parseexternalsettings( $i, $dlib_linker."_dlib_".$platform, @options ) ) );
            @optnew = ( @optnew, cut( parseexternalsettings( $i, $exec_linker."_exec", @options )." ".parseexternalsettings( $i, $exec_linker."_exec_".$platform, @options ) ) );
            $i++;
        }

        my $vs7name = cut( $project_description->{ vs7name } );

        write_project_file(
                 $project_name,
                 $output_name,
                 @source_dir,
                 $export,
                 @source_exclude_files,
                 @include_dirs,
                 @dependencies,
                 @optnew,
                 $vs7name );

    }
}
else
{
      print(" we can't find file : $file_name ");
}

close(FH);

#
#
#

sub write_project_file(\$\$\@\$\@\@\@\@\$ ) {

      my $project_name         = shift(@_);
      my $output_name          = shift(@_);
      my $source_dir           = shift(@_);
      my $export               = shift(@_);
      my $source_exclude_files = shift(@_);
      my $include_dirs         = shift(@_);
      my $dependencies         = shift(@_);
      my $options              = shift(@_);
      my $vs7name              = shift(@_);


      print FH "# ------------------------------------------------------------------------------\n";
      print FH "#\n";
      print FH "#       $$project_name component description\n";
      print FH "#\n";
      print FH "# ------------------------------------------------------------------------------\n";

      print FH "[$$project_name - Settings]\n";
      print FH "\n\n";

      print FH "\"Output\"\n";
      print FH "\t".parse $$output_name."\n";
      print FH "\"End Output\"\n";
      print FH "\n";

      print FH "\"Source Files\"\n";
               foreach(@$source_dir) {
                  print FH "\t".parse $_."\n";
               }
      print FH "\"end Source Files\"\n";
      print FH "\n";


      print FH "\"Export Files\"\n";
               print FH "\t".parse $$export."\n";
      print FH "\"end Export Files\"\n";
      print FH "\n";

      print FH "\"Source Files exclude\"\n";
               foreach(@$source_exclude_files) {
                  print FH "\t".parse $_."\n";
               }
      print FH "\"end Source Files exclude\"\n";
      print FH "\n";

      print FH "\"Include Directories\"\n";
               foreach(@$include_dirs) {
                  print FH "\t".parse $_."\n";
               }
      print FH "\"end Include Directories\"\n";
      print FH "\n";

      print FH "\"Dependencies\"\n";
               foreach(@$dependencies) {
                  print FH "\t".parse $_."\n";
               }
      print FH "\"end Dependencies\"\n";
      print FH "\n";

      print FH "\"options\"\n";
               foreach(@$options) {
                  print FH "\t".parse $_."\n";
               }
      print FH "\"end options\"\n";
      print FH "\n";

      print FH "\"vs7name\"\n";
          print FH "\t$$vs7name\n";
      print FH "\"end vs7name\"\n";
      print FH "\n";

      my $tools_new=get_tools_path(@$source_dir);
      my $tools_new=cut($tools_new);
      print FH "\"tools_dir\"\n";
      print FH "\t$tools_new\n";
      print FH "\"end_dir\"\n";
      print FH "\n";

      print FH "[$$project_name - end Settings]\n";
      print FH "\n\n";

}#sub write_project_file


# -------------------------------------------------------------------------------------------------
#
#

sub write_workspace(\@) {

      my $workspace=shift(@_);
      print FH "\n";
      print FH "\n";

      print FH "[Workspace]\n";
      foreach my $str (@$workspace) {
         print FH "$str\n";
      }
      print FH "[End Workspace]\n";

      print FH "\n";
      print FH "\n";
      print FH "\n";
      print FH "# ==============================================================================\n";
      print FH "#\n";
      print FH "# Projects\n";
      print FH "#\n";
      print FH "# ==============================================================================\n";
      print FH "\n\n";
}



# -------------------------------------------------------------------------------------------------
#
#

sub get_tools_path(@) {

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


#------------------------------------------------------------------------------

#
# Return config string for transferred option name. If option name is not supprted by xml
# it's name can be set using such construction:
# <option name="OPTION_NAME">
# <value>
# OPTION_VALUE
# </value>
# </option>
#

sub parseexternalsettings
{
   my $i = $_[0];
   my $set = $_[1];
   my @options = $_[2];
   my @compkeys = ();
   my $ckn = 0;

   if( $options[0][$i]{$set} =~ /ARRAY/ )
   {
      $ckn = 0;

      while( $options[0][$i]{$set}[$ckn] )
      {
        my $index = util::GetIndex(@compkeys, $options[0][$i]{$set}[$ckn]);

        if ($index == -1)
        {
            push( @compkeys, $options[0][$i]{$set}[$ckn] );
        }

        $ckn++;
      }
   }
   else
   {
      if ($options[0][$i]{$set} ne "")
      {
        push( @compkeys, $options[0][$i]{$set} );
      }
   }

   if( $options[0][$i]{$set} eq "" )
   {
     if( $options[0][$i]->{option}{$set}{value} =~ /ARRAY/ )
     {
        $ckn = 0;

        while( $options[0][$i]->{option}{$set}{value}[$ckn] )
        {
            my $index = util::GetIndex(@compkeys, $options[0][$i]->{option}{$set}{value}[$ckn]);

            if ($index == -1)
            {
                push( @compkeys, $options[0][$i]->{option}{$set}{value}[$ckn] );
            }

            $ckn++;
        }
     }
     else
     {
        if ($options[0][$i]->{option}{$set}{value} ne "")
        {
            push( @compkeys, $options[0][$i]->{option}{$set}{value} );
        }
     }
   }

   my $optstr = join( " ", @compkeys );
   $optstr =~ s/\n//gm;
   if( !$optstr )
   {
      $optstr = " ";
   }
   return $optstr;
}

#------------------------------------------------------------------------------

#
# Make modifications in transferred config string
# using project, global and platform settings.
#

sub parse
{
   my $val = $_[0];
   my @val_array = ();

   $val =~ s/\$\((U3D_[^\)]*)\)/$global_settings{$1}/eg;

   while ( $val =~ /\$\(([^\)]*)\)/ )
   {
        my $fnd = $1;
        my $rep = "\\\$\\\($fnd\\\)";
        my $res = $platform_settings{ $fnd };

        if ($res eq "" )
        {
            $res = cut ( $prj_description->{ $fnd } );

            if ( $res ne "")
            {
                $res =~ s/\$\((U3D_[^\)]*)\)/$global_settings{$1}/eg;
                #$res =~ s/\$\(([^\)]*)\)/$platform_settings{$1}/eg;

                if ( $fnd eq "output_name" )
                {
                    $res =~ /([^\/]*)$/;
                    $res = $1;
                }
            }
            else
            {
                # If unable to correctly decypher option $(***) due to
                # absence of corresponding definition in XML file
                # then delete this option.

                # Delete $(export)
                if ( $fnd eq "export" )
                {
                    $val =~ /(\s*\S*\$\(export\)\S*)/;
                    $val = $`.$';
                }
            }

            $val =~ s/$rep/$res/g;
        }
        else
        {
            if ( ($fnd eq "DLIB") || ($fnd eq "SLIB") || ($fnd eq "EXEC") )
            {
                $val =~ s/$rep/$res/g;
            }
            else
            {
                $val =~ /(\$\([^\)]*\))/;

                push( @val_array, $`.$1 );
                $val = $';
            }
        }
   }
   push(@val_array, $val);

   $val = join( "", @val_array );

   #Delete all unneeded options basing on custom property of XML file.

   #Delete resource file inclusion if XML file doesn't contain .rc file.
   if ( $val =~ /\.RES\s/ )
   {
        my $ROOTDIR = $ENV{U3D_SOURCE};

        my @source_dir = cut_arr( $prj_description->{ source_dir } );
        my $size = @source_dir;

        for ( my $i=0; $i<$size; $i++ )
        {
            @source_dir[$i] =~ s/\$\((U3D_[^\)]*)\)/$global_settings{$1}/eg;
            #@source_dir[$i] =~ s/\$\(([^\)]*)\)/$platform_settings{$1}/eg;
        }

        my $path = @source_dir[0];

        my @rc_files = grep( $_ =~ m/\.rc$/, @source_dir );

        opendir SRC, $ROOTDIR.$path;
        my @src_files = readdir SRC;
        closedir SRC;
        @rc_files = (@rc_files, grep( $_ =~ m/\.rc$/, @src_files ) );

        if(@rc_files == 0 )
        {
            if ($val =~ /(\s\S+\.RES)/)
            {
                $val = $`.$';
            }
        }
   }

   $val =~ s/evaluate\(([^\)\;]*)\;\s*([^\)]*)\)/eval qq\/$1\/? $2 : ""/eg;

   return $val;
}
