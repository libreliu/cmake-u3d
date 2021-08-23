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

package vs7_vcproj;
use strict;
use util;
use vs7_files;
use Cwd;
use IO::Dir;
use File::Find;
use File::Path;
use File::Basename;
use File::Spec;
use Text::ParseWords;

#
# This file converts the input XML project format ("project_win.xml") to Visual C++ project file format
#
# input : ------------------------------------------------------------------
#         @include         - all include file dependencies for this current project
#         @linker          - all linker dependencies for this current project
#         $project_name    - current project name
#         $output          - output project name
#         $source          - sources path, i.e. where we can find that sources
#         $path2makedef    - path to default make.def file located
#         $options         - specifiec for current project compile and linking options
#         $add_source      - additional for current project  compilation sourses, which locate in the others directories
#         @global_cpp      - contains all "cpp" file name which contains into all CWG/C3D/Source/ dir'
#         @global_header   - contains all "h" file name which contains into all CWG/C3D/Source/ dir'
#         %guid            - hash : "key-value" == "project name-GUID"
#         %vs7_name        - hash : "key-value" == "project name- VS7.NET name "
#         @ADD_CFLAGS      - additional flags for each project
#         @export          - list of the *.def files e.g - IFXDLL.def
#         $COMMONLINKFLAGS - the common (make_win.def + curr. project) ADD_LINKFLAGS
#         $COMMONCLIB      - the common (make_win.def + curr. project) libraries
#         %option_tree     - the whole project_win.xml file contain
#
# output : -----------------------------------------------------------------
#         that routine will create Makefile and
#         put its into this project root directory
#

my $ROOTDIR="../../";

my @rc_list=(); # resource file list
sub sort_name;
sub FillOptionsArray(\$\%\$);
sub _cflag_cleaning(\@\@);
sub _separate_cflags(\@);
sub _cleanarray(\@);

sub create_vcproj(\@\@\$\$\$\$\$\@\@\@\@\@\%\%\@\@\@\$\$\%)
{
    my $include           = shift(@_);
    my $linker            = shift(@_);
    my $project_name      = shift(@_);
    my $output            = shift(@_);
    my $source            = shift(@_);
    my $path2makedef      = shift(@_);
    my $path2build        = shift(@_);
    my $options           = shift(@_);
    my $add_source        = shift(@_);
    my $excl_source       = shift(@_);
    my $global_cpp        = shift(@_);
    my $global_header     = shift(@_);
    my $guid              = shift(@_);
    my $vs7_name          = shift(@_);
    my $ADD_CFLAGS        = shift(@_);
    my $DEFINES           = shift(@_);
    my $export            = shift(@_);
    my $COMMONLINKFLAGS   = shift(@_);
    my $COMMONCLIB        = shift(@_);
    my $option_tree       = shift(@_);

    my @ADD_CFL = ();


    my $answer=0;

    chomp($$output);
    chomp($$project_name);
    $$output=~ s/^\s*//;
        $$project_name=~ s/^\s*//;

        my $pch=".pch";

    @rc_list=();

#
# as example we have $source == CWG/C3D/Source/RTL/Kernel/Common/
# and we have to cut $source for create ../../RTL/Build/All/Kernel.vcproj file
#
    $$source=~ s/^\s*//;
    $$source=~ s/\s+\z//;

# 1 full path form root to CWG/C3D/Source/
    my $path1=get_path2source() ;  # e.g - d:/perforce/CWG/C3D/Source/

        my $tmppath=$$source;
        $tmppath=~ s/\s*CWG\/C3D\/Source\///;

    $$source=~ s/\s*CWG\/C3D\/Source/../; # replace "CWG/C3D/Source" --> ".."
    chomp($$source);

    my @source_list=(); # this array will contains all "*.cpp" names
# which have been included in current $project_name project
        my @header_list=(); # will contains all ".h" files into current directory

        my @new_header_list=(); # add array

        my @tmp_add_source=@$add_source; #remember add_source array

        @source_list=fill_source_lists($source,$add_source,$excl_source);

# all header files in the curr. directory
    @new_header_list=fill_header_list($source );
# pass this list to fill somethig like RelativePath="CIFXDataBlockQueueX.h">
    my @local_h_file_list=@new_header_list;

    my $extension=".vcproj";

    my $special_project_name;
# extract this name from $source name : $source=../../RTL/project/IFXDLL/ ---> $special_project_name==IFXDLL
    $special_project_name=$$source;
    $special_project_name=~ s/\/$//;
        $special_project_name=~ s#.*/([^/]*)$#$1#;

    my $dsp_file;

    if($special_project_name=~ m/Win32/ ) {
        $dsp_file="Platform.vcproj";
    }else {
        $dsp_file="$special_project_name$extension";
    }

    my $fullname = "$$source$dsp_file";

    my $newpath="../../RTL/Build/";
    my $delim="/";

# create dir : $newdir
#my $newdir=$newpath.$$project_name.$delim;
    my $all="All";
    my $newdir=$newpath.$all.$delim;

    File::Path::mkpath($newdir);

# @new_header_list will be contain header file list
#      relative path from "RTL/Build/All/" directory to "Header files location" directory's

    @new_header_list=relative_path_modication($$source,$include);

    my $val; my $key;
#while ( ($val,$key) = each (%$vs7_name )  ) { print"val=$val,,,,,,,,key=$key\n";} exit 1;

    my $new_name;
    my $new_key;
    while ( ($val,$key) = each %$vs7_name ) {

        if ($val=~ m/\b$$project_name\b/) {

            $new_key=$key;
            $new_name=$new_key;
            $new_key=$new_key.$extension;
        }
    }# while

        my $spec_path;
    $newpath=$newdir.$new_key;

    my $t = $ROOTDIR . "RTL/Build/ALL/" . $new_key;

    print " creating : $t  \n";
#print " creating >>> $newpath  \n";

    open(FH, ">$newpath") || die(" cannot create such file : $newpath ! \n ");

    print FH "<?xml version=\"1.0\" encoding=\"windows-1251\"?>\n";
    print FH "<VisualStudioProject\n";
    print FH "\tProjectType=\"Visual C++\"\n";
    print FH "\tVersion=\"7.10\"\n";

    print FH "\tName=\"$new_name\"\n";

    my $GUID=util::guid();
    $$guid{"$$project_name"}=$GUID;

    print FH "\tProjectGUID=\"{$GUID}\"\n";

    print FH "\tRootNamespace=\"$new_name\"\n";         #   RootNamespace="DataTypes"
    print FH "\tSccProjectName=\"\"\n";                 #   SccProjectName=""
    print FH "\tSccLocalPath=\"\">\n";                  #   SccLocalPath=""

    print FH "\t<Platforms>\n";
    print FH "\t\t<Platform\n";
    print FH "\t\t\tName=\"Win32\"/>\n";
    print FH "\t</Platforms>\n";

#=========================================================================================

#this compiler options extracted from common make files
    my @cmpl_flags;
    my @cmpl_flags_original  = _separate_cflags(@$ADD_CFLAGS);
    @ADD_CFL = @$ADD_CFLAGS;
# this is linker flag list extracted from common make file
    my @link_flags;
    my @link_flags_original = separate_COMMONLINKFLAGS($COMMONLINKFLAGS);

    my @lib_flags;
    my @lib_flags_original = $$COMMONCLIB;

    print FH "\t<Configurations>\n";

    foreach my $cfg ( keys( %$option_tree ) ) {
#print " 273 (vs7_vcproj.pm)   >>>>>>>>>>>>> $cfg  \n";
        print FH "\t\t<Configuration\n";

        @$ADD_CFLAGS = @ADD_CFL;

        @cmpl_flags = @cmpl_flags_original;
#
# step 1 : compiler options parsing
#          1.1 -> initialization @options array
#          1.2 -> separate @options[0]
#          1.3 -> add current CFLAGS to @cmpl_flags     ---> for the specific compiler opttions parsing
#          1.4 -> add  @$ADD_CFLAGS from @options[0]    ---> for the PreprocessorDefinitions

#1.1
        @$options = FillOptionsArray($cfg,%$option_tree,$$project_name);  #foreach(@$options){print "      options $_ \n";} exit 1;

# 1.2
        my @cfl = split(/ +/, @$options[0]);
        my @def = ();
        @cfl = _cflag_cleaning(@cfl,@def);
# 1.3
        my @tmp_opt_0 = _separate_cflags(@cfl);
        push(@cmpl_flags, @tmp_opt_0 );
# 1.4
        foreach (@$ADD_CFLAGS){
            push(@cfl, $_ );
        }
        foreach( @cfl )
        {
          /(^.*);$/;
          $_ = "\/$1";
        }
        foreach (@$DEFINES){
            push(@def, $_ );
        }
#
# step 2 : linker options parsing COMMONLINKFLAG
#
        @link_flags =  @link_flags_original;
        my $tmplink = @$options[1];
        push (@link_flags,@$options[1]);                   #print " >>>>>>>>> @link_flags <<<<<<<<<<\n";

#
# step 3 : additional libraries list parsing
#
        @lib_flags = @lib_flags_original;
        my $tmplib = @$options[2];
        push( @lib_flags, $tmplib );                       #print " >>>>>>>>> @lib_flags <<<<<<<<<<\n";


#
# the main current configuration stage
#
        my $tmpname = $cfg . "|Win32";
        print FH "\t\t\tName=\"$tmpname\"\n";    #print FH "\t\t\tName=\"Debug|Win32\"\n";


        print FH "\t\t\tOutputDirectory=\".\\$cfg\\$$project_name\"\n";

        print FH "\t\t\tIntermediateDirectory=\".\\$cfg\\$$project_name\"\n";

# ConfigurationType == 3 - exe, 2 - dll 4 - lib
        if($$output=~ m/exe$/){
            print FH "\t\t\tConfigurationType=\"1\"\n";
        }elsif($$output=~ m/dll$/){
            print FH "\t\t\tConfigurationType=\"2\"\n";
        }elsif($$output=~ m/lib$/){
            print FH "\t\t\tConfigurationType=\"4\"\n";
        }else{
            print " ---------------------------------------------------------------------------- \n";
            print " ERROR :Unknown Configuration type during  $$project_name vcproj generation \n";
            print " ---------------------------------------------------------------------------- \n";
            exit 1;
        }
        print FH "\t\t\tUseOfMFC=\"0\"\n";
        print FH "\t\t\tATLMinimizesCRunTimeLibraryUsage=\"FALSE\"\n";
        print FH "\t\t\tCharacterSet=\"2\">\n";

# Tools section beginning  to continue .....
        print FH "\t\t\t<Tool\n";
        print FH "\t\t\t\tName=\"VCCLCompilerTool\"\n";

        print FH "\t\t\t\tAdditionalIncludeDirectories=\"@new_header_list\"\n";
        print FH "\t\t\t\tPreprocessorDefinitions=\"@def\"\n";
        print FH "\t\t\t\tAdditionalOptions=\"@cfl\"\n";

# Release, Profile ==2
# Debug, Purify == 3

#print FH "\t\t\t\tRuntimeLibrary=\"3\"\n";
        $$output =~ /([^\/]*)\.[^\.\/]*$/;
        my $out_filename = $1;
        print FH "\t\t\t\tPrecompiledHeaderFile=\".\\$cfg\\$$project_name\\$out_filename$pch\"\n";
        print FH "\t\t\t\tAssemblerListingLocation=\".\\$cfg\\$$project_name\\\"\n";
        print FH "\t\t\t\tObjectFile=\".\\$cfg\\$$project_name\\\"\n";
        print FH "\t\t\t\tProgramDataBaseFileName=\".\\$cfg\\$$project_name\\\"\n";
        print FH "\t\t\t\tBrowseInformation=\"1\"\n";
        print FH "\t\t\t\tBrowseInformationFile=\"\$(IntDir)\\\"\n";
#print FH "\t\t\t\tSuppressStartupBanner=\"TRUE\"\n";

# remove all repeated @cmpl_arr array elements
        _cleanarray(@cmpl_flags); #print " 389 @cmpl_flags \n";
        foreach (@cmpl_flags) {   # from make_def
            if($_=~ m/\s*Mt;\s*/i) { print FH "\t\t\t\tRuntimeLibrary=\"0\"\n"; }
            elsif ($_=~ m/\s*MTd;\s*/i){print FH "\t\t\t\tRuntimeLibrary=\"1\"\n";}
            elsif ($_=~ m/\s*MD;\s*/i ){print FH "\t\t\t\tRuntimeLibrary=\"2\"\n";}
            elsif ($_=~ m/\s*MDd;\s*/i){print FH "\t\t\t\tRuntimeLibrary=\"3\"\n";}
            elsif ($_=~ m/\s*ML;\s*/i ){print FH "\t\t\t\tRuntimeLibrary=\"4\"\n";}
            elsif ($_=~ m/\s*MLd;\s*/i){print FH "\t\t\t\tRuntimeLibrary=\"5\"\n";}
            else{};

#Run-time error checks
            if($_=~ m/\s*RTC1;\s*/i) { print FH "\t\t\t\tBasicRuntimeChecks=\"3\"\n"; }
            elsif ($_=~ m/\s*RTCs;\s*/i){print FH "\t\t\t\tBasicRuntimeChecks=\"1\"\n";}
            elsif ($_=~ m/\s*RTCu;\s*/i){print FH "\t\t\t\tBasicRuntimeChecks=\"2\"\n";}
            elsif ($_=~ m/\s*RTCsu;\s*/i){print FH "\t\t\t\tBasicRuntimeChecks=\"3\"\n";}
            else{};

            if( $_=~ m/\s*W0;\s*/i ){ print FH "\t\t\t\tWarningLevel=\"0\"\n";}
            elsif( $_=~ m/\s*W1;\s*/i ){ print FH "\t\t\t\tWarningLevel=\"1\"\n"; }
            elsif( $_=~ m/\s*W2;\s*/i ){ print FH "\t\t\t\tWarningLevel=\"2\"\n"; }
            elsif( $_=~ m/\s*W3;\s*/i ){ print FH "\t\t\t\tWarningLevel=\"3\"\n"; }
            elsif( $_=~ m/\s*W4;\s*/i ){ print FH "\t\t\t\tWarningLevel=\"4\"\n"; }
            else {}
            if ( $_=~ m/\s*Disabled;\s*/i ) { print FH "\t\t\t\tDebugInformationFormat=\"0\"\n"; }
            elsif ( $_=~ m/\s*Z7;\s*/i ) { print FH "\t\t\t\tDebugInformationFormat=\"1\"\n"; }
            elsif ( $_=~ m/\s*Zd;\s*/i ) { print FH "\t\t\t\tDebugInformationFormat=\"2\"\n"; }
            elsif ( $_=~ m/\s*Zi;\s*/i ) { print FH "\t\t\t\tDebugInformationFormat=\"3\"\n"; }
            elsif ( $_=~ m/\s*ZI;\s*/i ) { print FH "\t\t\t\tDebugInformationFormat=\"4\"\n"; }
            else{}

            if ( $_=~ m/\s*Od;\s*/i ) { print FH "\t\t\t\tOptimization=\"0\"\n";}
            elsif ( $_=~ m/\s*O1;\s*/i ) { print FH "\t\t\t\tOptimization=\"1\"\n";}
            elsif ( $_=~ m/\s*O2;\s*/i ) { print FH "\t\t\t\tOptimization=\"2\"\n";}
            elsif ( $_=~ m/\s*Ox;\s*/i ) { print FH "\t\t\t\tOptimization=\"3\"\n";}
            elsif ( $_=~ m/\s*Og;\s*/i ) { print FH "\t\t\t\tOptimization=\"4\"\n";}
            else{}

            if ( $_=~ m/\s*Gm;\s*/i ) { print FH "\t\t\t\tMinimalRebuild=\"TRUE\"\n";}
            elsif ( $_=~ m/\s*Gt;\s*/i ) { print FH "\t\t\t\tEnableFiberSafeOptimizations=\"TRUE\"\n";}
            elsif ( $_=~ m/\s*Gy;\s*/i ) { print FH "\t\t\t\tEnableFunctionLevelLinking=\"TRUE\"\n";}
            elsif ( $_=~ m/\s*Gx;\s*/i ) { print FH "\t\t\t\tExceptionHandling=\"TRUE\"\n";}
            else { }

            if ( $_=~ m/\s*Za;\s*/i ) { print FH "\t\t\t\tDisableLanguageExtensions=\"TRUE\"\n"; }
        }#foreach (@cmpl_flags)

            print FH "\t\t\t\tCompileAs=\"0\"/>\n";

        print FH "\t\t\t<Tool\n";
        print FH "\t\t\t\tName=\"VCCustomBuildTool\"/>\n";


        my @AddDependencies=();
        @AddDependencies=get_add_dependencies($options);
        @AddDependencies=get_fullDependensPath(@AddDependencies);

        print FH "\t\t\t<Tool\n";
        print FH "\t\t\t\tName=\"VCLinkerTool\"\n";

        if( $$output !~ /\.lib$/ )
        {
            $$output =~ /([^\/]*)$/;
            print FH "\t\t\t\tOutputFile=\"./$cfg/\$(ProjectName)/$1\"\n";
        }
        print FH "\t\t\t\tProgramDatabaseFile=\"./$cfg/\$(ProjectName)/$out_filename.pdb\"\n";
        print FH "\t\t\t\tLinkIncremental=\"1\"\n";
        print FH "\t\t\t\tImportLibrary=\"./$cfg/\$(ProjectName)/$out_filename.lib\"\n";


        #foreach(@link_flags){print "link_flags = $_ \n";} #exit 1;
        my $linkflags = join (' ', @link_flags);
        if ($linkflags ne "") { print FH "\t\t\t\tAdditionalOptions=\"$linkflags\"\n"; }

#
# =============================================================
#
        my $exportsize=@$export;
        if($exportsize >= 1 ) {
            print FH "\t\t\t\tAdditionalDependencies=\"@AddDependencies\"\n"; #AdditionalDependencies="winmm.lib"/>

                print FH "\t\t\t\tTargetMachine=\"1\"\n";

            if( $linkflags =~ /\/DEBUG/ )
            {
                print FH "\t\t\t\tGenerateDebugInformation=\"TRUE\"\n";
            }
            else
            {
                print FH "\t\t\t\tGenerateDebugInformation=\"FALSE\"\n";
            }

            foreach (@$export){

                my $tmp=$$source;
                my $path1 = $ROOTDIR . "RTL/Build/All/"; #print " path1  $path1 \n";
                my $path2 = $ROOTDIR . $tmp; #print " path2  $path2 \n";
                my $path3 = @$export[0]; #print " path3  $path3 \n";
                my $path=File::Spec->rel2abs( $path3, $path2 );  #print " path  $path \n";
                my $path4=File::Spec->abs2rel( $path, $path1 );  #print " path4 $path4 \n";

                my $spliter = "\\";
                $path4 = util::SimplifyPath( $path4 , $spliter );

                print FH "\t\t\t\tModuleDefinitionFile=\"$path4\"/>\n";

            }# foreach  $@export

        } else {

                if($$output =~ m/.dll$/ || $$output =~ m/.exe$/ ) {
#                               print FH "\t\t\t\tOutputFile=\"$OUT\"\n";
                    print FH "\t\t\t\tTargetMachine=\"1\"\n";
                    if( $linkflags =~ /\/DEBUG/ )
                    {
                        print FH "\t\t\t\tGenerateDebugInformation=\"TRUE\"\n";
                    }
                    else
                    {
                        print FH "\t\t\t\tGenerateDebugInformation=\"FALSE\"\n";
                    }
                }

                print FH "\t\t\t\tAdditionalDependencies=\"@AddDependencies\"/>\n"; #AdditionalDependencies="winmm.lib"/>
} #  if -else ($exportsize >= 1 )

print FH "\t\t\t<Tool\n";
                print FH "\t\t\t\tName=\"VCLibrarianTool\"\n";

                if($$output =~ m/.lib$/ ) {
                    print FH "\t\t\t\tOutputFile=\".\\$cfg\\$$project_name\\$$output\"\n";
                    print FH "\t\t\t\tAdditionalOptions=\"/MACHINE:X86\"\n";
                }
                print FH "\t\t\t\tSuppressStartupBanner=\"TRUE\"/>\n";

                print FH "\t\t\t<Tool\n";
                print FH "\t\t\t\tName=\"VCMIDLTool\"/>\n";

                print FH "\t\t\t<Tool\n";
                print FH "\t\t\t\tName=\"VCPostBuildEventTool\"";
                if($$output =~ m/.(dll)$/ || $$output =~ m/.(exe)$/ ) {
                    my $ext = $1;
                    my $OUT = "../../../".util::get_global_setting( "U3D_OUTPUT" );
                    $OUT =~ s/\//\\/g;
                    $OUT =~ s/\\+$//;
                    my $outputwin = $$output;
                    $outputwin =~ s/\//\\/g;
                    $$output =~ /(.*)\/[^\/]*/;
                    print FH "\n\t\t\t\tCommandLine=\"mkdir $OUT\\$1";
                    print FH "\n\t\t\t\tcopy /Y $cfg\\$$project_name\\$out_filename.$ext $OUT\\$outputwin\"";
                }
                print FH "/>\n";


                print FH "\t\t\t<Tool\n";
                print FH "\t\t\t\tName=\"VCPreBuildEventTool\"/>\n";

                print FH "\t\t\t<Tool\n";
                print FH "\t\t\t\tName=\"VCPreLinkEventTool\"/>\n";

                print FH "\t\t\t<Tool\n";
                print FH "\t\t\t\tName=\"VCResourceCompilerTool\"\n";

                if( $cfg =~ /debug/i )
                {
                    print FH "\t\t\t\tPreprocessorDefinitions=\"DEBUG;_DEBUG\"\n";
                }
                else
                {
                    print FH "\t\t\t\tPreprocessorDefinitions=\"NDEBUG\"\n";
                }
                print FH "\t\t\t\tAdditionalIncludeDirectories=\"..\\..\\..\\RTL\\Kernel\\Include\"\n";
                my $size = @rc_list;
# TBD adjust Culture in accordance with resource kinds
# english resource Culture=\"1033\"
                if($size == 0 ){
                    print FH "\t\t\t\tCulture=\"1033\"/>\n";
                } else {
                    print FH "\t\t\t\tCulture=\"1033\"";
                    foreach( @rc_list )
                    {
                        print FH "\n\t\t\t\tResourceOutputFileName=\"..\\..\\..\\RTL\\Build\\ALL\\$cfg\\$$project_name\\$$project_name".".RES\"";
                    }
                    print FH "/>\n";
                }# if - else size

                    print FH "\t\t\t<Tool\n";
                print FH "\t\t\t\tName=\"VCWebServiceProxyGeneratorTool\"/>\n";

                print FH "\t\t\t<Tool\n";
                print FH "\t\t\t\tName=\"VCXMLDataGeneratorTool\"/>\n";

                print FH "\t\t\t<Tool\n";
                print FH "\t\t\t\tName=\"VCManagedWrapperGeneratorTool\"/>\n";

                print FH "\t\t\t<Tool\n";
                print FH "\t\t\t\tName=\"VCAuxiliaryManagedWrapperGeneratorTool\"/>\n";

# the last step  : cleaning @$options,@cmpl_flags, linker and so
                @$options = ();
                @cmpl_flags = ();
                @link_flags = ();
                @lib_flags  = ();

                print FH "\t\t</Configuration>\n";
}; # foreach

print FH "\t</Configurations>\n";

print FH "\t<References>\n";
print FH "\t</References>\n";

#=========================================================================================

# -------------------------------------------------------------------------------------------------
#
#              Files
#
# -------------------------------------------------------------------------------------------------

# specially for interface files
my @fullpathHfiles=();
print FH "\t<Files>\n";
print FH "\t\t<Filter\n";
print FH "\t\t\tName=\"Source Files\"\n";
print FH "\t\t\tFilter=\"\">\n";

# sorting the basename for customer usability
my @sss=();
@sss = @source_list;
push( @sss, @local_h_file_list );
@source_list = ();
@source_list = sort sort_name @sss;

#  $source_list
foreach my $cpp (@source_list) {
#
# global search throuch CWG/C3D/Source directories
#
#----------------------------------------------------------------------------------------
#
if( $cpp !~ /\.h$/ && $cpp !~ /\.hxx$/ )
{
my $fullname;
my $tmpdir;
my $tmpname;
my $path1;
my $path2;
foreach $fullname(@$global_cpp) {

$tmpname= basename( $fullname );

$tmpdir = dirname($fullname); #D:/perforce/CWG/C3D/Source/RTL/Kernel/Platform/IFXOSLinux

if( $tmpname eq $cpp ) {

$path1=$tmpdir;
$path2=$$source;

$path2=~ s/\/$//;
$path2=~ s/^\s*//;

    if($path1=~ m/$path2\b/ ) {

        if( $tmpdir !~ m/IFXOSLinux$/) { # this separate case while we have the same file names
# in the Linux and OSWin32 directories
            $cpp=$fullname;
        last;

        }# ( $tmpdir !~ m/IFXOSLinux$/ )

    } else {

# treatment all additional files
        my $sz=@$add_source;
        if ($sz > 0 ) {

            foreach(@tmp_add_source){


                my $tdir=dirname($_);

                if($path1=~ m/$tdir\b/ ){
                    $cpp=$fullname;
                }
                last;
            }#foreach

        }#if ($sz > 0 )
    }#
} else {
# TODO we need to add @exclude files processing
}# if - else

               } # foreach (@$global_cpp)

#----------------------------------------------------------------------------------------

#print "cpp - $cpp \n";
               $cpp=~ tr%\/%\\%; # clobal replacement "/" to "\"

               print FH "\t\t\t<File\n";
print FH "\t\t\t\tRelativePath=\"$cpp\">\n";
#debug
print FH "\t\t\t\t<FileConfiguration\n";
print FH "\t\t\t\t\tName=\"Debug|Win32\">\n";
print FH "\t\t\t\t\t\<Tool\n";

#THE FOLLOWING LINES WERE INSERTED BY VC
if( $cpp =~ /\.rc$/ )
{
    print FH "\t\t\t\t\t\tName=\"VCResourceCompilerTool\"\n";
    print FH "\t\t\t\t\t\tAdditionalIncludeDirectories=\"\"\n/>";

} else {

    print FH "\t\t\t\t\t\tName=\"VCCLCompilerTool\"\n";
#    print FH "\t\t\t\t\t\tOptimization=\"0\"\n";
    print FH "\t\t\t\t\t\tAdditionalIncludeDirectories=\"\"\n";
    print FH "\t\t\t\t\t\tPreprocessorDefinitions=\"\"\n";

#
# specially for VC - we need to check BasicRuntimeChecks flags with Optimization flags
#
#    print FH "\t\t\t\t\t\tBasicRuntimeChecks=\"0\"\n";
    print FH "\t\t\t\t\t\tBrowseInformation=\"1\"/>\n";
}

print FH "\t\t\t\t</FileConfiguration>\n";

#Release
print FH "\t\t\t\t<FileConfiguration\n";

print FH "\t\t\t\t\tName=\"Release|Win32\">\n";
print FH "\t\t\t\t\t\<Tool\n";

#THE FOLLOWING LINES WERE INSERTED BY VC
if( $cpp =~ /\.rc$/ )
{
    print FH "\t\t\t\t\t\tName=\"VCResourceCompilerTool\"\n";
    print FH "\t\t\t\t\t\tAdditionalIncludeDirectories=\"\"\n/>";
}
else
{
    print FH "\t\t\t\t\t\tName=\"VCCLCompilerTool\"\n";
#    print FH "\t\t\t\t\t\tOptimization=\"2\"\n";
    print FH "\t\t\t\t\t\tAdditionalIncludeDirectories=\"\"\n";
    print FH "\t\t\t\t\t\tPreprocessorDefinitions=\"\"/>\n";
}
print FH "\t\t\t\t</FileConfiguration>\n";

print FH "\t\t\t</File>\n";

}
else
{
# insert all curr. dir header files
# add path from curr.vcproj to sources path
#               foreach (@local_h_file_list){

    my $curr_h_path=$cpp;
#                  my $curr_h_path=$_;

# set relative path between *.vcproj and sources path
#example -d:\perforce\CWG\C3D\Source\RTL\Component\ImageCodec1\*.h
#print ">>>> $curr_h_path\n";

    $curr_h_path=~ tr%\/%\\%;

    print FH "\t\t\t<File\n";
    print FH "\t\t\t\tRelativePath=\"$curr_h_path\">\n";
    print FH "\t\t\t</File>\n";

    push(@fullpathHfiles,$curr_h_path);
}
            }# foreach my $cpp (@source_list)

                print FH "\t\t</Filter>\n";

#
# TODO insert interface files
            my @InterfaceFiles;

            @InterfaceFiles = vs7_files::GetInterfaceFiles($$source,@source_list,@fullpathHfiles,@$include);

            my $interface_size = @InterfaceFiles;

            if ( $interface_size != 0 ) {

# sorting the basename for customer usability
                my @aaa=();
                @aaa = @InterfaceFiles;
                @InterfaceFiles = ();
                @InterfaceFiles = sort sort_name @aaa;

                print FH "\t\t<Filter\n";
                print FH "\t\t\tName=\"Referenced Files\"\n";
                print FH "\t\t\tFilter=\"\">\n";

                foreach (@InterfaceFiles) {
                    my $interfacefile=$_;
                    print FH "\t\t\t<File\n";
                    print FH "\t\t\t\tRelativePath=\"$interfacefile\">\n";
                    print FH "\t\t\t</File>\n";
                }# InterfaceFiles


                    print FH "\t\t</Filter>\n";

            }# if ( $interface_size != 0)


                print FH "\t</Files>\n";

            print FH "\t<Globals>\n";
            print FH "\t</Globals>\n";

            print FH "</VisualStudioProject>\n";
            close(FH);

            return 0;
}; # create_vcproj(\@\@\$\$\$\$\$\@\@\@\@\@\%\%\@\@\$\$\$\% )

#
#
#
sub get_add_dependencies(\@) {

    my $options=shift(@_);
    my @dep=();

    my $sz=@$options;
    my $str=@$options[$sz-1];
    chomp($str);

    $str=~ s/ADD_LIB//;
        $str=~ s/=//;
        $str=~ s/s\*//;
        $str=~ s/\t//;
        push(@dep,$str);

#print ">>>>> @dep\n";
    return @dep;
}


#
# @new_header_list=relative_path_modification($,$include);
# calculate relative path from "RTL/Build/All/" directory to "Header files location" directoty's
#

sub relative_path_modication(\$\@) {

    my $source=shift(@_);   # example - CWG/C3D/Source/RTL/project/Collision
        my $include=shift(@_);

    my @full_include_path=();

    my $path1 = $ROOTDIR . "RTL/Build/All/";#this is const path. <--- *.vcpoj file path
        my $path2 = $ROOTDIR . $source;

    my $path3; # current include file pathes
        my $path4;
    my $size=@$include;
    my $ind=0;
    foreach my $path0 (@$include){

        $path3 = File::Spec->rel2abs( $path0, $path2 ) ;


        use File::Spec;
        $path4 = File::Spec->abs2rel( $path3, $path1 ) ;
        my $spliter = "\\";
        $path4 = util::SimplifyPath( $path4 , $spliter );
        if($ind<$size-1){ $path4=$path4.","; }
        push(@full_include_path,$path4);
        $ind++;
    }; #foreach


        return @full_include_path;
}#relative_path_modication


#----------------------------------------------------------------------------------------
#  $relpath == ../../ .....#
#  $abspath ==/CWG/C3D/Source/RTL/project/Base/
#
sub __rel2abs(\$\$ ) {

    my $relpath=shift(@_);
    my $abspath=shift(@_);
    my $result;

    $relpath=~ s/^\s*//;
        my $sz1=length($relpath);
    my $sz3=length("../");
    $relpath=~ s/(..\/)*//;     # remove all "../" templates

        my $sz2=length($relpath);
    my $sz4=($sz1-$sz2)/($sz3); # calculate the "../" numbers

#------ remove from abspath the last part of the pathes $sz4 times
#   CWG C3D Source RTL project Base  --> CWG C3D Source RTL
#
        my @arr=split(/\//, $abspath);

        for ( my $i=0; $i<$sz4; $i++ ) {
            pop(@arr);
        }

        $result=join('/',@arr);  # as result we will have /CWG/C3D/Source/RTL/
            $result=$result.'/'.$relpath;
        return $result;
} #__rel2abs(\$,\$)

sub  pars_string(\$) {

    my $string=shift(@_);
    my @newwords=();
    my @words = &Text::ParseWords::quotewords('\/',0,$string );
    foreach (@words) {
        if($_ =~ m/RTL/ ){
            last();
        }
        push(@newwords,$_);
    }
    $string=join("/",@newwords);
    return $string;
} # sub pars_string()


#--------------------------------------------------------------------------------------------------
#
# fill_lists  --> goto $source directory and find all *.cpp and *.h file.
#             --> if these file names don't contain into  @$excl_source list, we save these file names into
#                 @source_list and @header_list arrays
#             --> added into @source_list and @header_list arrayes file from $add_source array
#             --> fill resource files list : rc_list()
#

sub  fill_source_lists(\$\@\@ ) {

    my $source      = shift(@_);
    my $add_source  = shift(@_);
    my $excl_source = shift(@_);

    my @source_list=();
    my $srcpath = $ROOTDIR . $$source;

    my $dir= IO::Dir->new($srcpath) or  die( "can't open $srcpath: $!" );
    my @files=$dir->read();

        foreach my $str ( @files ) {
            if( $str=~ m/\.cpp$/ || $str=~ m/\.c$/ ) {
                $str = "../../../" . $$source . $str;
                push(@source_list,$str);
            }

             if( $str=~ m/.rc$/ ) {
                $str = "../../../" . $$source . $str;
                push(@rc_list,$str);
                push(@source_list,$str);
            }
        };

# added files from @add_source ---------------------------------------------------------
# calculate all additional files full pathes

    foreach my $tmp ( @$add_source ) {
        my $path = "../../../" . $$source . $tmp;
        my $spliter = "/";

        $path = util::SimplifyPath( $path , $spliter );

        if( $path=~ m/.rc$/ ) {
            push(@rc_list,$path);
        }

        push(@source_list,$path);
};

# end added files from @add_source -----------------------------------------------------


# remove from @source_list files which contain into @$excl_source array
my $src=0;
my $dst=0;

EXLUDE:
my $ind=0;
foreach  (@source_list) {
    $src=$_;
    foreach  (@$excl_source) {

        my $dst=$_;
        if($src=~ m/\b$dst\b/) {
            splice(@source_list, $ind,1);
            goto EXLUDE;
        }
    }
    $ind++;
}#

return @source_list;
} #sub  fill_source_lists


#
# get resource files, fill array and return it!
#

sub  fill_header_list(\$ ) {

    my $source = shift(@_);
    my @header_list=();
    my $srcpath = $ROOTDIR . $$source;

    my $dir= IO::Dir->new($srcpath) or  die( "can't open $srcpath: $!" );

    my @files=$dir->read();

    foreach my $str ( @files ) {

        if( $str=~ m/\.h$/ || $str=~ m/\.hxx$/) {
            $str = "../../../" . $$source . $str;
            push(@header_list,$str);
        }
    };

    return @header_list;
} #sub  fill_header_list

#
#  get full path from root to C3D/Source/ directory
#
sub  get_path2source() {

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
#replaced "d" --> "d:"
            $tmp2=":";
        $sz=length($currpath);
        my $tmp=substr($currpath,0, 1 - $sz); #?

            $tmp1=$tmp;
        $tmp=$tmp.$tmp2; # == d:
        my $tmp1=substr($currpath,1);# ==/perforce/CWG/C3D/Source

            $tmp1=$tmp.$tmp1;            # == d:/perforce/CWG/C3D/Source
            $currpath=$tmp1;

    } else {


        $currpath=getcwd; #D:/perforce/CWG/C3D/Source/Tools
            $currpath=~ s/Tools//;
    }

    return $currpath;
}#get_path2source

#
# return full path from root to current sources directory
#
sub get_path2sources_dir(\$\$) {

    my $path1 = shift(@_);
# == relative path from CWG/C3D/Source/ to sources dir : e.g - ../Tests/MOA/Xtras/3DASSET/Source
    my $source = shift(@_);
    my $tmpsource= $$source;

    $tmpsource=~ s{^(\.\./)*}{}; # remove all ../../../

        my $fullpath=$path1.$tmpsource; #D:/perforce/CWG/C3D/Source/Tests/MOA/Xtras/3DAsset/Source/
        return $fullpath;
}


#
# separate COMMONLINKFLAGS options
# return the separate linker flag array
# modified input array
#
sub separate_COMMONLINKFLAGS(\@ ) {
    my $COMMONLINKFLAGS=shift(@_);
    my @arr=();

    my @arr_COMMONLINKFLAGS=split(/ +/,$$COMMONLINKFLAGS );
    foreach (@arr_COMMONLINKFLAGS ) {

        push(@arr,$_);
    }# foreach

        return @arr;
}

#
#  if @AddDependencies contains nonstandart *.Lib we set the full path to this lib
#

sub get_fullDependensPath() {

    my $add_dep = shift(@_);
#
# Source/Tools/Lib/Debug/IFXRenderSW.lib
#

    my $str=$add_dep;
    my @arr=split(/ +/,$str);

    $add_dep="";
    foreach( @arr) {

        my $str=$_;

        if ($_=~ m/Source\//) { #find and set the full path

            $str =~ s/Source\///;
            $str = $ROOTDIR . $str;
    }

    my $rrr=" ";
    $add_dep=$add_dep.$rrr;
    $add_dep=$add_dep.$str;

}

return $add_dep;
} #addlib_modifying


#
#
#
sub sort_name {
    lc basename($a) cmp lc basename($b);
}


#
#    for correspondigly configuration ( debug,release,profile and so),
#    extract  all options, put into @arr and return it
#
sub FillOptionsArray (\$\%\$) {

    my $cfg  = shift(@_);
    my $tree = shift(@_);
    my $proj_name = shift(@_);
    my @arr = ();

    my @arr_0;
    my @arr_1;
    my @arr_2;
    my $kkey;
    my $vvalue;
    my @settings;
    while(($kkey,$vvalue) = each %$tree){

        if($$cfg =~ m/$kkey/ ) {
            @settings = split( "\n", $vvalue );

            foreach my $v ( @settings ) {
                if ( $v !~ m/^([^=]*)=(.*)\z/ ) {
                print $v;
                    die( " XML file syntax requirements error \n" );
                };
                my $name = $1;

                if ( $name eq "ADD_CFLAGS" ) {
                    @arr[0] = $2;
                    @arr[0]=~ s/^\s*//;
                        @arr[0]=~ s/\s+\z//;
                } elsif ( $name eq "ADD_LINKFLAGS" ) {
                    @arr[1] = $2;
                    @arr[1]=~ s/^\s*//;
                        @arr[1]=~ s/\s+\z//;
                } elsif ( $name eq "ADD_LIB" ) {
                    @arr[2] = $2;
                    @arr[2]=~ s/^\s*//;
                        @arr[2]=~ s/\s+\z//;
                }


            };# foreach
                last;
        };#if
    };
    return @arr;
}# sub FillOptionsArray


#
#  1. removed /c flag if there
#  2. remove all "/" and or "/D" prefixes
#
sub _cflag_cleaning(\@\@) {

    my $CFLAGS = shift(@_);
    my $defines = shift(@_);
    @$defines = ();
    my @arr = ();
    for( my $i; $i < @$CFLAGS; $i++ )
    {
        my $str=$$CFLAGS[$i];
        if( $str=~ /^\/D/ ){
          push(@$defines, $$CFLAGS[++$i].";");
        }
        elsif($str ne "" ){
          $str=~ s/\///;
          $str = $str . ";";
          push(@arr,$str);
        }
    }
    return @arr;
} #_cflag_cleaning

#
#
#
sub _separate_cflags(\@) {

    my $ADD_CFLAGS=shift(@_);
    my @arr=();

START:
    my $ind=0;
    foreach (@$ADD_CFLAGS) {

        if($_=~ m/\s*Mt;\s*/i || $_=~ m/\s*MTd;\s*/i || $_=~ m/\s*MD;\s*/i || $_=~ m/\s*MDd;\s*/i || $_=~ m/\s*ML;\s*/i || $_=~ m/\s*MLd;\s*/i){
            push(@arr,$_);
            splice(@$ADD_CFLAGS,$ind,1);
            goto START;

        }elsif( $_=~ m/\s*W0;\s*/i || $_=~ m/\s*W1;\s*/i || $_=~ m/\s*W2;\s*/i || $_=~ m/\s*W3;\s*/i || $_=~ m/\s*W4;\s*/i ){
            push(@arr,$_);
            splice(@$ADD_CFLAGS,$ind,1);
            goto START;
        }elsif($_=~ m/\s*nologo;\s*/i){
            push(@arr,$_);
            splice(@$ADD_CFLAGS,$ind,1);
            goto START;
        }elsif($_=~ m/\s*Gm;\s*/i || $_=~ m/\s*Gx;\s*/i || $_=~ m/\s*Gy;\s*/i ){
            push(@arr,$_);
            splice(@$ADD_CFLAGS,$ind,1);
            goto START;
        }elsif($_=~ m/\s*Z7;\s*/i || $_=~ m/\s*Zd;\s*/i || $_=~ m/\s*Zi;\s*/i || $_=~ m/\s*ZI;\s*/i ){
            push(@arr,$_);
            splice(@$ADD_CFLAGS,$ind,1);
            goto START;
        }elsif ($_=~ m/\s*Od;\s*/i || $_=~ m/\s*O1;\s*/i || $_=~ m/\s*O2;\s*/i || $_=~ m/\s*Ox;\s*/i || $_=~ m/\s*Og;\s*/i ){
            push(@arr,$_);
            splice(@$ADD_CFLAGS,$ind,1);
            goto START;
        }elsif ($_=~ m/\s*Za;\s*/i ){
            push(@arr,$_);
            splice(@$ADD_CFLAGS,$ind,1);
            goto START;
        }elsif ($_=~ m/\s*RTC1;\s*/i || $_=~ m/\s*RTCs;\s*/i || $_=~ m/\s*RTCu;\s*/i || $_=~ m/\s*RTCsu;\s*/i || $_=~ m/\s*RTCu;\s*/i ){
            push(@arr,$_);
            splice(@$ADD_CFLAGS,$ind,1);
            goto START;
        } elsif ($_=~ m/^;!/i) {
            splice(@$ADD_CFLAGS,$ind,1);
            goto START;
        } else {
        }
        $ind++;
    }#foreach (ADD_CFLAGS)

        return @arr;
}# _separate_cflags

#
#  remove all repeated inpur array elements
#
sub _cleanarray(\@) {
    my $arr=shift(@_);
START:
    my $size_ii = @$arr;
    for (my $ii = 0; $ii <= $size_ii; $ii++  ) {
        my $size_jj = @$arr;
        for (my $jj = $ii+1; $jj <= $size_jj - 1; $jj++  ) {
            if ( @$arr[$ii] eq  @$arr[$jj] ) {
                splice(@$arr,$jj,1);
                goto START;
            }
        }#$j
    } #$ii
} #_cleanarray

1;
