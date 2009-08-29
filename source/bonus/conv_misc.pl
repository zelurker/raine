#!/usr/bin/perl

# Convert mostly what other scripts don't : WRITE_HANDLER syntax (horrible),
# and GFX_LAYOUT mostly.

use strict;
use LWP::Simple;

my $cps2_mode;
my $cps1_mode;

sub get_genre($) {
  # get the game genre from maws !
  my $name = shift;

  my $doc = get "http://www.mameworld.net/maws/romset/$name";
  return "GAME_MISC" if (!$doc);
  my @lines = split(/\n/,$doc);
  for (my $n=0; $n<=$#lines; $n++) {
    if ($lines[$n] =~ />genre</) {
      $lines[$n+1] =~ />(\w+?)<\/a/;
      my $genre = $1;
      if ($genre =~ /Shoot/) {
	$genre = "GAME_SHOOT";
      } elsif ($genre =~ /Fight/) {
	$genre = "GAME_BEAT";
      } elsif ($genre =~ /Puzzle/) {
	$genre = "GAME_PUZZLE";
      } elsif ($genre =~ /Quiz/) {
	$genre = "GAME_QUIZZ";
      } elsif ($genre =~ /Sports/) {
	$genre = "GAME_SPORTS";
      } elsif ($genre =~ /Platform/) {
	$genre = "GAME_PLATFORM";
      } else {
	print STDERR "genre unknown $genre for $name - using GAME_MISC\n";
	$genre = "GAME_MISC";
      }
      return $genre;
    }
  }
  print STDERR "genre not found in page for $name - using GAME_MISC\n";
  return "GAME_MISC";
}

while (<>) {
  if (!$cps1_mode && !$cps2_mode) {
    if (/\( cps1_/) {
      $cps1_mode = 1;
    } elsif (/\( cps2_/) {
      $cps2_mode = 1;
    }
  }
  if (s/READ8?_HANDLER\((.+)\)/UINT8 $1\(UINT32 offset\)/ ||
      s/READ16_HANDLER\((.+)\)/UINT16 $1\(UINT32 offset\)/ ||
      s/WRITE8?_HANDLER\((.+)\)/void $1\(UINT32 offset, UINT8 data\)/ ||
      s/WRITE16_HANDLER\((.+)\)/void $1\(UINT32 offset, UINT16 data\)/) {
    chomp;
    my $line = <>;
    if ($line =~ /^\{/) {
      print "$_ {\n";
      while (<>) {
	print;
	last if ($_ =~ /^\}/);
      }
      print "\n";
    } else {
      print "$_\n";
      $_ = $line;
      redo;
    }
  }

  # copy the gfxlayouts blocks
  if (/GfxLayout.+\=/) {
    print;
    while (<>) {
      print;
      chomp;
      last if ($_ eq "};");
    }
    print "\n";
  }

  # convert gfxdecodeinfo stuff
  if (s/GfxDecodeInfo(.+\=)/GFX_LIST$1/) {
    print;
    while (<>) {
      my ($region, $start,$layout,$start_col,$banks) = split(/\, ?/);
      if ($banks) {
	$banks =~ s/ ?\}//;
	print "$region, $layout }, // $banks color banks\n";
	if ($start > 0) {
	  print STDERR "WARNING : GfxDecodeInfo has $region starting at $start\nYou will have to manually adjust this.\n";
	}
      } else {
	s/\-1/0\, NULL/;
	print;
      }
      chomp;
      last if ($_ eq "};");
    }
    print "\n";
  }
  if (/^GAME\( (.+) ?\)/) {
    my $string = $1;
    while ($string =~ s/"(.+?),(.+?)", *"(.+)" *,/"$1\£$2", "$3",/g) {}
    while ($string =~ s/"(.+?)", *"(.+?),(.+?)" *,/"$1", "$2\£$3",/g) {}

    my ($year,$name,$parent,$machine,$drv1, $drv, $rot,$company,$long_name,$reste) = split(/\,/,$string);
    $long_name =~ s/\£/\,/g;
    $company =~ s/\£/\,/g;
    $name =~ s/ //g;
    $parent =~ s/ //g;
    $drv1 =~ s/ //g;
    $rot =~ s/ //g;
    $machine =~ s/ //g;

    if ($company =~ /Raizing/i) {
      $company = "COMPANY_ID_RAIZING";
    } elsif ($company =~ /mitchell/i) {
      $company = "COMPANY_ID_MITCHELL";
    } elsif ($company =~ /Cave/) {
      $company = "COMPANY_ID_CAVE";
    } elsif ($company =~ /bootleg/i) {
      $company = "COMPANY_ID_BOOTLEG";
    } elsif ($company =~ /capcom/i) {
      $company = "COMPANY_ID_CAPCOM";
    } else {
      print STDERR "company unknown yet $company\n";
    }
    my $genre = "GAME_MISC"; # get_genre($name);
#    my $genre = get_genre($name);
    if ($cps1_mode || $cps2_mode) {
      if ($rot eq "ROT0") {
	$rot = "&cps1_video";
      } elsif ($rot eq "ROT270") {
	$rot = "&cps1_video_270";
      } else {
	die "angle $rot not handled yet, sorry this is a case of error\n";
      }
    }
    if ($cps1_mode) {
      my $game;
      if ($long_name =~ /(Capcom Word|Quiz )/i) {
	$game = "cps1b4_";
      } elsif ($machine =~ /qsound/) {
	$game = "qsound_";
      } elsif ($long_name =~ /Forgotten worlds|Lost Worlds/i) {
	$game = "forgottn_";
      } elsif ($name =~ /sf2/) {
	$game = "sf2_";
      } elsif ($name =~ /sfzch/) {
	$game = "sfzch_";
      } else {
	$game = "cps1_";
      }
      $drv1 = "_$drv1" if ($drv1 =~ /^\d/);
      if (!$parent) {
	if ($game =~ /cps1_/) {
	  print "cps1_game( $name, $long_name, $year, $drv1\_dsw, $rot, $company, $genre );\n";
	} elsif ($game =~ /forgott/) {
	  print "forgottn_game( $name, $long_name, $year, $company, $genre);\n";
	} else {
	  print $game."game( $name, $long_name, $year, $drv1\_dsw, $company, $genre );\n";
	}
      } else {
	if ($game =~ /cps1_/) {
	  print "cps1_clone( $name, $long_name, $year, $drv1\_dsw, $rot, $company, $genre, \"$parent\" );\n";
	} elsif ($game =~ /forgott/) {
	  print "forgottn_clone( $name, $long_name, $year, $company, $genre, \"$parent\" );\n";
	} else {
	  if ($game =~ /sf2_/ && $drv1 ne "sf2j") {
	    # only sf2j so far has its own dipswitches, but there are probably others !!!
	    print $game."clone( $name, $long_name, $year, sf2_dsw, $company, $genre, \"$parent\" );\n";
	  } else {
	    print $game."clone( $name, $long_name, $year, $drv1\_dsw, $company, $genre, \"$parent\" );\n";
	  }
	}
      }
    } elsif ($cps2_mode) {
      if ($drv1 eq "cps2_2p6b") {
	$drv1 = "p2b6";
      } elsif ($drv1 eq "cps2_2p3b") {
	$drv1 = "p2b3";
      } elsif ($drv1 eq "cps2_4p4b") {
	$drv1 = "p4b4";
      } elsif ($drv1 eq "cps2_3p4b") {
	$drv1 = "p4b4";
      } elsif ($drv1 eq "cps2_3p3b") {
	$drv1 = "p3b3";
      } elsif ($drv1 eq "cybots") {
	$drv1 = "p2b4";
      } elsif ($drv1 eq "cps2_2p2b") {
	$drv1 = "p2b2";
      } elsif ($drv1 eq "cps2_2p3b") {
	$drv1 = "p2b3";
      } elsif ($drv1 eq "qndream") {
	$drv1 .= "";
      } elsif ($drv1 eq "cps2_4p2b") {
	$drv1 = "p4b2";
      } elsif ($drv1 eq "cps2_4p3b") {
	$drv1 = "p4b3";
      } elsif ($drv1 eq "cps2_1p2b") {
	$drv1 = "p1b2";
      } elsif ($drv1 eq "cps2_1p3b" || $drv1 eq "choko") {
	$drv1 = "p1b3";
      } elsif ($drv1 eq "cps2_1p4b") {
	$drv1 = "p1b4";
      } elsif ($drv1 eq "cps2_2p1b" || $drv1 eq "pzloop2") {
	$drv1 = "p2b1";
      } else {
	print "controls unknown $drv1\n";
	exit(1);
      }

      if (!$parent) {
	print "cps2_game( $name, $long_name, $year, $drv1, $rot, $company, $genre );\n";
     } else {
	print "cps2_clone( $name, $long_name, $year, $drv1, $genre, \"$parent\", $rot, $company);\n";
     }
   } else {
     $name = "_$name" if ($name =~ /^\d/); # stupid digit in front of the name
     $parent = "_$parent" if ($parent ne "0" && $parent =~ /^\d/); # stupid digit in front of the name
     print "static struct DIR_INFO $name\_dirs[] =\n";
     print "{\n";
     print "  { \"$name\" },\n";
     if ($parent ne "0") {
       print "  { ROMOF(\"$parent\") },\n  { CLONEOF(\"$parent\"), },\n";
     }
     print "  { NULL }\n";
     print "};\n\n";
   }
 }
}
