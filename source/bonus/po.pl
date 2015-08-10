#!/usr/bin/env perl

use strict;

my $file;
$| = 1;
while ($file = shift @ARGV) {
	open(F,"<$file") || die "can't open $file\n";
	print "$file ... ";
	open(G,">truc.cpp");
	my $rep = 0;
	my $active = 0;
	while (<F>) {
		if (!$active) {
			if ((/(menu_item_t|DEF_INPUT_EMU|DSW_DATA) .*\=/ && !/;/) || # Declaration of a struct
				(/\= new /) || # Declaration of an object...
				(/(print_ingame|print_menu_string|MessageBox|print_tf_state|add_layer_info)/) || # specific function
				(/ \= "/)) { # Direct string affectation
				$active = 1;
			}
		}
		# remove eventual _("...") already inserted...
		$rep -= s/_\("([\w\d ,\(\)\=\<\>\+\'\:\.\-]+)"\)/"$1"/g if ($active);
		$rep += s/"([\w\d ,\(\)\=\<\>\+\'\:\.\-]+)"/_("$1")/g if ($active);
		$rep -= s/_\("(\d+)"\)/"$1"/g; # remove numbers only
		print G;
		$active = 0 if (/;/);
	}
	close(F);
	if ($rep) {
		print "replacements $rep\n";
		unlink $file;
		rename "truc.cpp",$file;
	}
}

