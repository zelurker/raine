#!/usr/bin/perl

use v5.10;
use strict;

# shows games without info in history.dat
# expects a history.dat and raine binary in the current directory

my %game = ();
open(F,"./raine -gl|") || die "can't run ./raine -gl";
while (<F>) {
	if (/^(.+?) +\: /) {
		$game{$1} = 1;
	}
}
close(F);

my %info = ();
open(F,"<history.dat") || die "read history.dat";
while (<F>) {
	chomp;
	if (/^\$info=(.+)/) {
		my @g = split (/\,/,$1);
		foreach (@g) {
			if ($_) {
				$info{$_} = 1;
			}
		}
	}
}
close(F);
foreach (keys %game) {
	if (!$info{$_}) {
		print "pas d'info pour $_";
		my $rom = $_;
		my $parent;
		my $warned = 0;
		open(F,"./raine -li $_|");
		while (<F>) {
			chomp;
			if (/romof (.+)/) {
				$parent = $1;
				if ($info{$parent}) {
					say " mais info pour le parent : $parent";
					$warned = 1;
				}
				last;
			}
		}
		close(F);
		say "." if (!$warned);
	}
}
