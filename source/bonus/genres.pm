package genres;

use LWP::Simple;
use strict;

our %genre = ();
dbmopen %genre, "genres",0666;

sub get_genre($$) {
	# get the game genre from maws !
	my ($name,$long) = @_;
	die "pass name,long name" if (!$long);
	return $genre{$name} if ($genre{$name} && $genre{$name} ne "GAME_MISC");
	print STDERR "no genre for $name...\n";

	$genre{$name} = "GAME_MISC"; # default genre
	# my $doc = get "http://ungr.emuunlim.org/ngmvsgames.php?action=showimage&image=$name";
	$long =~ s/ /_/g;
	$long =~ s/_\/.+//;
	$long =~ s/_\(.+//;
	say STDERR "url https://en.wikipedia.org/wiki/$long";
	my $doc = get "https://en.wikipedia.org/wiki/$long";
	if ($doc =~ /may refer to/) {
		say STDERR "trying with _(video_game)";
		$long .= "_(video_game)";
		$doc = get "https://en.wikipedia.org/wiki/$long";
	}
	if (!$doc) {
		print STDERR "no info for $name, sleep 3 and retry...\n";
		sleep 3;
		$doc = get "http://www.arcadehits.net/index.php?p=roms&jeu=$name";
		print STDERR "still no info\n" if (!$doc);
	}
	return "GAME_MISC" if (!$doc);
	if ($doc =~ /Genre : (.+?)\</ || $doc =~ /Genre:<\/b\> (.+?)\</ || $doc =~ /genre=(.+?)>/ || $doc =~ /title="Video game genre".+?title="(.+?)"/s) {
			my $genre = $1;
			if ($genre =~ /Shoot/) {
				$genre = "GAME_SHOOT";
			} elsif ($genre =~ /Fight/) {
				$genre = "GAME_BEAT";
			} elsif ($genre =~ /(Puzzle|Mahjong)/) {
				$genre = "GAME_PUZZLE";
			} elsif ($genre =~ /Quiz/) {
				$genre = "GAME_QUIZZ";
			} elsif ($genre =~ /(Sports|Baseball|Golf|Football|Bowling)/) {
				$genre = "GAME_SPORTS";
			} elsif ($genre =~ /Platform/) {
				$genre = "GAME_PLATFORM";
			} elsif ($genre =~ /(Racing|Race)/) {
				$genre = "GAME_RACE";
			} elsif ($genre =~ /(Fight|Combat)/) {
				$genre = "GAME_FIGHT";
			} elsif ($genre =~ /Breakout/) {
				$genre = "GAME_BREAKOUT";
			} elsif ($genre =~ /Maze/) {
				$genre = "GAME_MAZE";
			} else {
				print STDERR "genre unknown $genre for $name - using GAME_MISC\n";
				$genre = "GAME_MISC";
			}
			$genre{$name} = $genre;
			return $genre;
	}
	open(F,">doc.html");
	print F $doc;
	close(F);
	die "genre not found in page for $name - using GAME_MISC\n";
	return "GAME_MISC";
}

1;

