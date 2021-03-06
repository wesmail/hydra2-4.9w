#!/usr/bin/perl 
use strict;
use warnings;
use IO::File;
use Getopt::Std;
use Data::Dumper;


my $help = "usage : replaceIniDat -t template -d iniDat -i input1 -o output -n trigger [-s input2 ] [-u input3 ] [ -p geompath ]
    \t This script replaces the key words  TRIGGER_NUM, INPUT_FILE1,
    \t INPUT_FILE2, OUTPUT_FILE iniside a HGEANT ini.dat file by the
    \t parameters provide as command line
    \t template  == template iniDat file for HGEANT containng keywords,
    \t iniDat    == iniDat file will be created from template iniDat
    \t input1    == .evt file for input 1 of HGEANT,
    \t input2    == .evt file for input 2 of HGEANT (optional),
    \t input3    == .evt file for input 3 of HGEANT (optional),
    \t geompath  ==  path to geomdir (optional),
    \t output    == .root file generated by HGEANT,
    \t trigger   == number of triggers to run with HGEANT
    \n";

my $template        = "";
my $iniDat          = "";
my $trigger         = -1;
my $inputFile1      = "";
my $inputFile2      = "";
my $inputFile3      = "";
my $geomPath        = "";
my $outputFile      = "";

my $NARGS = $#ARGV ; #  $#ARGV will be rest by getopt()!!!

my %args;
getopt('tdisuonp', \%args);

$template   = $args{"t"} ? $args{"t"} : "" ;
$iniDat     = $args{"d"} ? $args{"d"} : "" ;
$trigger    = $args{"n"} ? $args{"n"} : -1 ;
$inputFile1 = $args{"i"} ? $args{"i"} : "" ;
$inputFile2 = $args{"s"} ? $args{"s"} : "" ;
$inputFile3 = $args{"u"} ? $args{"u"} : "" ;
$geomPath   = $args{"p"} ? $args{"p"} : "" ;
$outputFile = $args{"o"} ? $args{"o"} : "" ;

my %parameters = ("template"    => $template,
                  "iniDat"      => $iniDat,
                  "trigger"     => $trigger,
                  "inputFile1"  => $inputFile1,
                  "inputFile2"  => $inputFile2,
                  "inputFile3"  => $inputFile3,
                  "geomPath"    => $geomPath,
                  "outputFile"  => $outputFile
                 );

print Dumper \%parameters;

if($NARGS < 6 || 
   $template   eq "" ||
   $iniDat     eq "" ||
   $inputFile1 eq "" ||
   $outputFile eq ""  
  ){
    print $help;
    exit(1);
}


 
open (rFile,"$template")
    or die "Cannot open initial ini file: $template!";

open (wFile,">","$iniDat")
    or die "Cannot open modified ini file: $iniDat!";

while(<rFile>)
{
    s/TRIGGER_NUM/$trigger/;
    s/INPUT_FILE1/$inputFile1/;
    s/OUTPUT_FILE/$outputFile/;
    if( $inputFile2 ne ""){
        s/INPUT_FILE2/$inputFile2/;
    }
    if( $inputFile3 ne ""){
        s/INPUT_FILE3/$inputFile3/;
    }
    if( $geomPath ne ""){
        s/GEOMPATH/$geomPath/;
    }
    print wFile "$_";
}

close rFile;
close wFile;
