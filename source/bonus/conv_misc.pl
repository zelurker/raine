#!/usr/bin/perl

# Convert mostly what other scripts don't : WRITE_HANDLER syntax (horrible),
# and GFX_LAYOUT mostly.

use strict;
use LWP::Simple;

$| = 1;
my $cps2_mode;
my $cps1_mode;

sub get_genre($) {
	# get the game genre from maws !
	my $name = shift;

	my $doc = get "http://ungr.emuunlim.org/ngmvsgames.php?action=showimage&image=$name";
	return "GAME_MISC" if (!$doc);
	if ($doc =~ /Genre : (.+?)\</ || $doc =~ /Genre:<\/b\> (.+?)\</) {
			my $genre = $1;
			if ($genre =~ /Shoot/) {
				$genre = "GAME_SHOOT";
			} elsif ($genre =~ /Fight/) {
				$genre = "GAME_BEAT";
			} elsif ($genre =~ /Puzzle/) {
				$genre = "GAME_PUZZLE";
			} elsif ($genre =~ /Quiz/) {
				$genre = "GAME_QUIZZ";
			} elsif ($genre =~ /(Sports|Baseball|Golf|Football|Bowling)/) {
				$genre = "GAME_SPORTS";
			} elsif ($genre =~ /Platform/) {
				$genre = "GAME_PLATFORM";
			} elsif ($genre =~ /Racing/) {
				$genre = "GAME_RACE";
			} elsif ($genre =~ /(Fight|Combat)/) {
				$genre = "GAME_FIGHT";
			} else {
				print STDERR "genre unknown $genre for $name - using GAME_MISC\n";
				$genre = "GAME_MISC";
			}
			return $genre;
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

		my ($year,$name,$parent,$machine,$input, $class, $init, $rot,$company,$long_name,$reste) = split(/\, ?/,$string);
		$long_name =~ s/\£/\,/g;
		$company =~ s/\£/\,/g;
		$name =~ s/ //g;
		$parent =~ s/ //g;
		$input =~ s/ //g;
		$rot =~ s/ //g;
		$machine =~ s/ //g;

		$company =~ s/ \/.+//;
		$company =~ s/"//g;
		$company = uc($company);
		my $genre;
		my $genre = ($parent !~ /^(0|neogeo)$/ ? get_genre($parent) : get_genre($name));
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
			$input = "_$input" if ($input =~ /^\d/);
			if (!$parent) {
				if ($game =~ /cps1_/) {
					print "cps1_game( $name, $long_name, $year, $input\_dsw, $rot, $company, $genre );\n";
				} elsif ($game =~ /forgott/) {
					print "forgottn_game( $name, $long_name, $year, $company, $genre);\n";
				} else {
					print $game."game( $name, $long_name, $year, $input\_dsw, $company, $genre );\n";
				}
			} else {
				if ($game =~ /cps1_/) {
					print "cps1_clone( $name, $long_name, $year, $input\_dsw, $rot, $company, $genre, \"$parent\" );\n";
				} elsif ($game =~ /forgott/) {
					print "forgottn_clone( $name, $long_name, $year, $company, $genre, \"$parent\" );\n";
				} else {
					if ($game =~ /sf2_/ && $input ne "sf2j") {
						# only sf2j so far has its own dipswitches, but there are probably others !!!
						print $game."clone( $name, $long_name, $year, sf2_dsw, $company, $genre, \"$parent\" );\n";
					} else {
						print $game."clone( $name, $long_name, $year, $input\_dsw, $company, $genre, \"$parent\" );\n";
					}
				}
			}
		} elsif ($cps2_mode) {
			if ($input eq "cps2_2p6b") {
				$input = "p2b6";
			} elsif ($input eq "cps2_2p3b") {
				$input = "p2b3";
			} elsif ($input eq "cps2_4p4b") {
				$input = "p4b4";
			} elsif ($input eq "cps2_3p4b") {
				$input = "p4b4";
			} elsif ($input eq "cps2_3p3b") {
				$input = "p3b3";
			} elsif ($input eq "cybots") {
				$input = "p2b4";
			} elsif ($input eq "cps2_2p2b") {
				$input = "p2b2";
			} elsif ($input eq "cps2_2p3b") {
				$input = "p2b3";
			} elsif ($input eq "qndream") {
				$input .= "";
			} elsif ($input eq "cps2_4p2b") {
				$input = "p4b2";
			} elsif ($input eq "cps2_4p3b") {
				$input = "p4b3";
			} elsif ($input eq "cps2_1p2b") {
				$input = "p1b2";
			} elsif ($input eq "cps2_1p3b" || $input eq "choko") {
				$input = "p1b3";
			} elsif ($input eq "cps2_1p4b") {
				$input = "p1b4";
			} elsif ($input eq "cps2_2p1b" || $input eq "pzloop2") {
				$input = "p2b1";
			} else {
				print "controls unknown $input\n";
				exit(1);
			}

			if (!$parent) {
				print "cps2_game( $name, $long_name, $year, $input, $rot, $company, $genre );\n";
			} else {
				print "cps2_clone( $name, $long_name, $year, $input, $genre, \"$parent\", $rot, $company);\n";
			}
		} else {
			if ($parent ne "0") {
				print "CLNEI( $name, $parent, $long_name, $company, $year, $genre);\n";
			}
		}
	}
}
