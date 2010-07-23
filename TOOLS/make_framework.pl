#!/usr/bin/perl

my $lib = shift @ARGV || die "pass lib in argument\n";
make_framework($lib);

sub handle_otool($) {
    my $first = shift;
    $first =~ s/^.+name (\/.+) \(.+/$1/;
    chomp $first;
    $first;
}

sub make_framework($) {
    my $lib = shift;
    my ($name) = $lib =~ /lib([_a-zA-Z0-9]+)/;
    print "making $name framework...\n";
    system("rm -rf $name.framework");
    mkdir "$name.framework";
    chdir "$name.framework";
    mkdir "Versions";
    mkdir "Versions/A";
    symlink("A","Versions/Current");
    system("cp $lib Versions/A/$name");
    symlink("Versions/Current/$name","$name");
    chdir "..";
    my $path = "$name.framework/$name";
    my $f;
    open($f,"otool -l $path|grep 'name /'|");
    my $first = handle_otool(<$f>);
    system("install_name_tool -id \@executable_path/../$frameworks/$name.framework/Versions/A/$name $path");
    while (<$f>) {
	$first = handle_otool($_);
	if ($first =~ /local/) {
	    my ($file) = $first =~ /lib\/(.+)/;
	    if (!-e $first) {
		if (-e "/usr/local/lib/$file") {
		    $first = "/usr/local/lib/$file";
		    print "fixing lib path for $first\n";
		} elsif (! -e "/usr/lib/$file") {
		    die "pas trouvé $first ni /usr/local/lib/$file\n";
		}
	    }
	    print "handling $first\n";
	    if (! -e "/usr/lib/$file" && -e "$first") {
		($name) = $first =~ /lib([_a-zA-Z0-9]+)/;
		if (!-d "$name.framework") {
		    make_framework($first);
		}
		system("install_name_tool -change $first \@executable_path/../$frameworks/$name.framework/Versions/A/$name $path");
	    } else {
		print "existe pas $first\n";
	    }
	}
    }
    close($f);
}

