use common::sense;

while (<roms2.c>) {
	my $f = $_;
	open(F,"<$f") || die "can't read $f";
	@_ = <F>;
	close(F);
	my ($prefix,$reg,$off,$len,$double);
	my $changed = grep s/{ *(".+?"), *(.+?), *(.+?), *REGION_(.+?), *(.+?), *LOAD_NORMAL,? *}/LOAD( $4, $1, $5, $2, $3)/ ||
	s/{ *(".+?"), *(.+?), *(.+?), *REGION_(.+?), *(.+?), *0,? *}/LOAD( $4, $1, $5, $2, $3)/ || # 0 au lieu de LOAD_NORMAL
	s/{ *(".+?"), *(.+?), *(.+?), *REGION_([A-Z0-9]+?), *([\dxa-f]+?),? *}/LOAD( $4, $1,yyy $5, $2, $3)/i ||  # implicit LOAD_NORMAL
	s/{ *(".+?"), *(.+?), *(.+?), *REGION_([A-Z0-9]+?),? *}/LOAD( $4, $1, 0, $2, $3)/ || # implicit offset (0)
	s/{ *(".+?"), *(.+?), *(.+?), *REGION_(.+?), *(.+?), *LOAD_SWAP_16,? *}/LOAD_SW16( $4, $1, $5, $2, $3)/ ||
	s/{ *(".+?"), *(.+?), *(.+?), *REGION_(.+?), *(.+?), *LOAD_8_16,? *}/LOAD_16_8( $4, $1, $5, $2, $3)/ ||
	s/{ *(".+?"), *(.+?), *(.+?), *REGION_(.+?), *(.+?), *LOAD32_SWAP_16,? *}/LOAD_32_SWAP_16( $4, $1, $5, $2, $3)/ ||
	s/{ *(".+?"), *(.+?), *(.+?), *REGION_(.+?), *(.+?), *LOAD_16_32,? *}/LOAD16_32( $4, $1, $5, $2, $3)/ ||
	s/^(.+)LOAD8_16\( *(.+), *(.+?), *(.+?), *(".+?"), *(.+?), *(".+?"), *(.+?)\)/$1LOAD8_16( $2, $5, $3, $4, $6),\n$1LOAD8_16( $2, $7, $3+1, $4, $8)/ ||
	s/FILL\( *([0-9a-fx]+?), *(.+?), *(.+?), *(.+?)\)/FILL( $4, $1, $2, $3)/,@_; # region de FILL au début !
	$changed += grep {
	    if (!$double && s/^([ \t]+)LOAD8_16\( *([A-Z0-9]+?),[ \t]*([0-9a-fx]+?),[ \t]*([0-9a-fx]+?),//) {
		($prefix,$reg,$off,$len,$double) = ($1,$2,$3,$4,1);
	    } elsif ($double && s/^.+?(".+?") ?, *(.+?), *(".+?") ?, *(.+?)\)/$prefix."LOAD8_16( $reg, $1, $off, $len, $2),\n$prefix"."LOAD8_16( $reg, $3, $off+1, $len, $4)"/e) {
		$double = 0;
	    }
	} @_;
	if ($changed) {
		say "$f $changed changes";
		open(G,">$f") || die "can't write to $f";
		print G @_;
		close(G);
	}
}


