#!/usr/bin/perl

use common::sense;

while (<>) {
	chomp;
	s/^script/luascript/;
	s/\$/0x/g;
	s/poke ([^ ]+?) "(.+)"/poke($1,"$2")/;
	s/poke ([^ ]+?) '(.+)'/poke($1,"$2")/;
	s/poke (.+?) (.+?) /poke($1,$2) /;
	s/poke (.+?) (.+)/poke($1,$2)/;
	s/if ([^#]+)/if $1 then/;
	s/endif/end/;
	s/elsif/elseif/;
	s/\)(and|or)\(/\) $1 \(/g;
	s/#([^ ])/-- $1/;
	s/#$/ --/;
	s/#/--/;
	s/!=/\~=/g;
	s/then--/then --/;
	s/  then/ then/g;
	s/rnd\(\)/math.random\(\)\*255/;

	say;
}

