use common::sense;
use HTML::Entities;

# syntax perl histxml2dat.pl < history.xml > history.dat

my @info;

while (<>) {
	if (/<history version="(.+?)" date="(.+?)">/) {
		say "# From history.xml version $1, generated on $2.";
		say "";
	}
	push @info,$1 if (/system name="(.+?)"/);
	if (/<text>/ && @info) {
		say "\$info=",join(",",@info);
		@info = ();
		while (<>) {
			last if (/<\/text>/);
			decode_entities($_);
			print;
		}
		say "\$end";
		say "";
	}
}


