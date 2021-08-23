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

package vs7_sln;
use strict;
use util;

sub create_sln(\@\@\%\%\@ );
#
# This file converts the input XML project format ("project_win.xml") to Visual C++ project file format
#
sub create_sln(\@\@\%\%\@ ) {

    my $arr_file        = shift(@_);
    my $component_list  = shift(@_); # like IFXCollision
    my $guid            = shift(@_); # contains : component name and GUID
    my $vs7_name        = shift(@_); # hash : project name -- vs7.net name
    my $configuration   = shift(@_); # contains configurations list

    my @linker=(); # linker dependencies
    my @arr_postSolution=(); # will contains well ordered project GUID's

    my $relative_path="../../RTL/Build/All/";
    my $sln_name="U3D.sln";
    my $full_sln_name=$relative_path . $sln_name;

    #print "$full_sln_name\n";
    my $path=".\\";
    open(FH, ">$full_sln_name") || die(" cannot create such file : $full_sln_name ! \n ");
    print FH "Microsoft Visual Studio Solution File, Format Version 8.00\n";

    my $currguid;
    my $ind=0;
    my $vcproj;
    my $name;
    my $ext=".vcproj";
    my $val; my $key;

    my $VS7_CPP_GUID="8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942";   # version 7.1

    foreach my $component_name ( @$component_list ) {
     $name=$component_name;
     @linker=();
     @linker = util::pars_linker( @$arr_file,$component_name );

     while ( ($val,$key) = each (%$guid )  ) {
         if ($val=~ m/\b$component_name\b/) {

            $currguid=$key;
            push(@arr_postSolution,$currguid);

         }
     }

     my $new_key;
     my $tmp_val;
     while ( ($val,$key) = each %$vs7_name ) {
        $tmp_val=$val;
        #->$tmp_val=~ s/^IFX//;
         #if ($tmp_val=~ m/$component_name/) {
         if ($tmp_val=~ m/\b$component_name\b/) {
            $new_key=$key;
            chomp($new_key);
            $new_key=~ s/\s*$//;

         }#if
     }# while

     $vcproj=$new_key.$ext;

     print FH "Project(\"{$VS7_CPP_GUID}\") = \"$new_key\", \"$vcproj\", \"{$currguid}\"\n";     # --> new

     print FH "\tProjectSection(ProjectDependencies) = postProject\n";
     foreach (@linker) {
        my $linktmp =$_;
        $linktmp=~ s/^\s*//;
         chop($linktmp);
         #print "link_name=$_,$name,\n";
           while ( ($val,$key) = each (%$guid )  ) {
                #print "val-->$val<--, link====>$_<====\n";
                if ($val=~ m/\b$linktmp\b/) {
                   $currguid=$key;
                   #print ">>>>>>>>>>> $component_name,,,,,,   $_\n";
                   print FH "\t\t{$currguid} = {$currguid}\n";
                }
            }
     }# foreach @linker

     print FH "\tEndProjectSection\n";
     print FH "EndProject\n";

    }# foreach $component_name ( @component_list )


    #
    # source control Settings
    #

    print FH "Global\n";

    #  print FH "\tGlobalSection(SourceCodeControl) = preSolution\n";

    my $size_comp_list=@$component_list;

    #---------------------------------------------------------------------------
    print FH "\tGlobalSection(SolutionConfiguration) = preSolution\n";

    foreach (@$configuration) {

        print FH "\t\t$_ = $_\n";  #print " $_\n"; exit 1;

    }

    print FH "\tEndGlobalSection\n";
    #---------------------------------------------------------------------------


    #---------------------------------------------------------------------------
    print FH "\tGlobalSection(ProjectConfiguration) = postSolution\n";

    my $AAA = "ActiveCfg";
    my $BBB = "Build.0";
    foreach my $currGUID ( @arr_postSolution ) {

         foreach (@$configuration) {
            print FH "\t\t{$currGUID}.$_.$AAA = $_|Win32\n";
            print FH "\t\t{$currGUID}.$_.$BBB = $_|Win32\n";
         }

    } #
    print FH "\tEndGlobalSection\n";
    #---------------------------------------------------------------------------


    print FH "\tGlobalSection(ExtensibilityGlobals) = postSolution\n";
    print FH "\tEndGlobalSection\n";
    print FH "\tGlobalSection(ExtensibilityAddIns) = postSolution\n";
    print FH "\tEndGlobalSection\n";

    print FH "EndGlobal\n";

    close(FH);
    return 0;

}# vs7_sln::create_sln();

1;
