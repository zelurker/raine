#!/usr/bin/perl

use common::sense;

# update po files based on a patch passed on the command line
# the idea to use it : apply the patch to git, commit it, then direct it to a file
# with git log -p -1 > patch
# and finally call this script perl source/bonus/update_po.pl patch
# it will update the po files with all the strings which changed, adding the fuzzy attribute for an eventual later check...

my ($file,$line,@old,@new);

while (<>) {
	chomp;
	if (/^--- a\/(.+)/) {
		$file = $1;
		if ($file !~ /^source/) {
			die "patch should be on the source files only";
		}
		say "file $file";
	} elsif (/^@@ \-(\d+)/ || /^diff/) {
		$line = $1;
		say "line $line";
		for (my $n=0; $n<=$#old; $n++) {
			if ($old[$n] eq $new[$n]) {
				last if ($n > $#old);
				say "removing $old[$n]";
				splice @old,$n,1;
				splice @new,$n,1;
				redo if (@old);
				last if ($n > $#old);
			}
		}
		if (@old) {
			say "still have ",$#old+1," strings to update";
			my $rep;
			while (my $po = glob("locale/*.po")) {
				say "$po...";
				my ($f,$g);
				open($f,"<$po") || die "can't open $po";
				open($g,">temp") || die "can't create temp file";
				$rep = 0;
				while (my $l = <$f>) {
					chomp $l;
					my $done = 0;
					for (my $n=0; $n<=$#old; $n++) {
						if (index($l,"msgid \"$old[$n]\"") == 0) {
							say "found $old[$n], replacing by $new[$n]";
							say $g "#, fuzzy";
							say $g "msgid \"$new[$n]\"";
							$rep++;
							$done = 1;
						}
					}
					if (!$done) {
						# unmodified line in this case
						say $g $l;
					}
				}
				close($f);
				close($g);
				# say "update done, temp file ready, press enter to continue";
				# <STDIN>;
				rename("temp", "$po") if ($rep);
				say "not found ? assuming already applied..." if (!$rep);
			}
			if (!$rep) {
				@old = @new = ();
			} elsif ($rep >= $#old+1) {
				@old = @new = ();
			} else {
				say "got rep = $rep old = $#old";
				die;
			}
		}
	} elsif (/^\-/) {
		say "watching -";
		while (s/(gettext|_)\("(.+?)\"//) {
			push @old,$2;
			say "old = $2";
		}
	} elsif (/^\+/ && $_ !~ /^\+\+/) {
		say "watching + $_";
		while (s/(gettext|_)\("(.+?)\"//) {
			push @new,$2;
			say "new = $2";
		}
	} elsif (/^ /) {
		$line++;
	}
}





