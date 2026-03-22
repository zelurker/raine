#!/usr/bin/perl

use common::sense;

open(F,"./raine -gl-avail|") || die "can't run raine -gl-avail";
my $list = 0;
while (<F>) {
	chomp;
	if (/^list/) {
		$list = 1;
		next;
	}
	if ($list) {
		system("./raine -test $_");
		if ($? == -1) {
			die "can't execute raine -nogui -test $_";
		} elsif ($? >> 8) {
			say STDERR "$_ returned ",($? >> 8);
		}
	}
}
close(F);

