use strict;

# conversion to LOAD8_16 and LOAD16_64 macros

sub get_list {
	my $list = undef;
	do {
		$_ = <>;
		($list) = $_ =~ /\{ *?(.*) *?\}/;
	} while (!$list);
	$list;
}

while (<>) {
	if (/\{ *?(.*) *?\}/) {
		my $list = $1;
		my ($rom1, $size,$crc1, $region, $offset, $load) = split(/\,/,$list);
		if ($load =~ /LOAD_8_16/) {
			$list = get_list();
			my ($rom2, $size,$crc2, $region, $offset2, $load) = split(/\,/,$list);
			if (hex($offset2) < hex($offset)) {
				print STDERR "invert $rom1 & $rom2\n";
				my @tab = ($rom1, $size,$crc1, $region, $offset, $load);
				($rom1, $size,$crc1, $region, $offset, $load) =
				($rom2, $size,$crc2, $region, $offset2, $load);
				($rom2, $size,$crc2, $region, $offset2, $load) = @tab;
			} else {
				print STDERR "$offset2 < $offset for $rom2 & $rom1\n";
			}

			print "  LOAD8_16( $region, $offset, $size,\n";
			print "            $rom1, $crc1, $rom2, $crc2),\n";
			next;
		} elsif ($load =~ /LOAD_16_64/) {
			$list = get_list();
			my ($rom2, $size,$crc2, $region, $offset2, $load) = split(/\,/,$list);
			$list = get_list();
			my ($rom3, $size,$crc3, $region, $offset3, $load) = split(/\,/,$list);
			$list = get_list();
			my ($rom4, $size,$crc4, $region, $offset4, $load) = split(/\,/,$list);
			print "  LOAD16_64( $region, $offset, $size,\n";
			print "            $rom1, $crc1, $rom2, $crc2,\n";
			print "            $rom3, $crc3, $rom4, $crc4),\n";
			next;
		}
	}
	print;
}




