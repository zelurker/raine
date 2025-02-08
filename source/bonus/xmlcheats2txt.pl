#!/usr/bin/perl

# converts an xml cheat file from mame to a script txt
# some parts are awkward mainly because they don't have any prefix for
# their hex numbers ! I wonder how they avoid collisions with variable
# names, I guess they avoid variables like ab then !

use common::sense;
use XML::LibXML;

our $cpu = 0;

sub handle_subarg {
	# traite les arguments entre ()
	# l'idée est de réussir à extraire les hexa sans préfixe, et ne pas les
	# confondre avec des variables, un vrai bordel !
	my $arg = shift;
	$arg =~ s/^([0-9a-f]+)([\+\-\*\%]|$)/\$$1$2/i if (!/^0x/);
	while ($arg =~ s/(\+|\-|\/|\*|\%|\&|\~|>|<|>=|<=|==|!=)([0-9a-f]+)([\+\-\*\%\)]|$)/$1\$$2$3/i) {}
	$arg;
}

sub handle_arg {
	my $cond = shift;
	if ($cond =~ s/audiocpu\./maincpu\./g) {
		$cpu = 1;
		say " "x4,"cpu audio";
	}
	if ($cond =~ /maincpu.[mopr]p?q@([0-9a-f]+)==([0-9a-f]+)/i) {
		# specific to hsf2 !
		my $adr = $1;
		my $adr2 = sprintf("%x",hex($adr)+4);
		my $val = $2;
		my $vall = substr($val,0,8);
		my $valh = substr($val,8);
		$cond =~ s/maincpu.[mopr]p?q@([0-9a-f]+) ?== ?([0-9a-f]+)/\(lpeek(\$$1)==$vall&&lpeek(\$$adr2)==$valh)/gi;
	}
	$cond =~ s/= *([0-9a-f])/=\$$1/gi;
	$cond =~ s/maincpu.[mopr]p?b@([0-9a-f]+)/peek(\$$1)/gi;
	$cond =~ s/maincpu.[mopr]p?w@([0-9a-f]+)/dpeek(\$$1)/gi;
	$cond =~ s/maincpu.[mopr]p?d@([0-9a-f]+)/lpeek(\$$1)/gi;
	# variante, () mais avec juste un hexa dedans !
	$cond =~ s/maincpu.[mopr]p?b@\(([0-9a-f]+)\)/peek(\$$1)/gi;
	$cond =~ s/maincpu.[mopr]p?w@\(([0-9a-f]+)\)/dpeek(\$$1)/gi;
	$cond =~ s/maincpu.[mopr]p?d@\(([0-9a-f]+)\)/lpeek(\$$1)/gi;
	$cond =~ s/ LT /</gi;
	$cond =~ s/ GT />/gi;
	$cond =~ s/ GE />=/gi;
	$cond =~ s/ LE /<=/gi;
	if ($cond =~ / (AND|OR) /i) {
		my @arg = split(/ (and|or) /i,$cond); # adding () around and and or becomes mandatory !
		for (my $n=0; $n<=$#arg; $n++) {
			if ($n & 1) {
				$arg[$n] = lc($arg[$n]);
			} else {
				$arg[$n] = "($arg[$n])";
			}
		}
		$cond = join("",@arg);
	}
	$cond =~ s/ BAND /\&/gi;
	$cond =~ s/ BOR /\|/gi;
	$cond =~ s/ //g;
	# 2ème passe, ()
	$cond =~ s/maincpu.[mopr]p?b@\((.+?)\)/peek($1)/gi;
	$cond =~ s/maincpu.[mopr]p?w@\((.+?)\)/dpeek($1)/gi;
	$cond =~ s/maincpu.[mopr]p?d@\((.+?)\)/lpeek($1)/gi;
	# si il en reste encore, ça doit être une variable alors... !
	$cond =~ s/maincpu.[mopr]p?b@([0-9a-z]+)/peek($1)/gi;
	$cond =~ s/maincpu.[mopr]p?w@([0-9a-z]+)/dpeek($1)/gi;
	$cond =~ s/maincpu.[mopr]p?d@([0-9a-z]+)/lpeek($1)/gi;

	$cond = handle_subarg($cond);
	# I might have overthought this, stuck in too many regexes... !
	# noramly handle_subarg should be able to handle the whole expression, now I hope I didn't forget any case... !
	# without this an expression like ((xxxx)*1c won't convert the *1c in the end (truxton2 case)
	# $cond =~ s/\((.+)\)/"(".handle_subarg($1).")"/gie;
	# $cond = handle_subarg($cond) if ($cond !~ /\(/ && $cond =~ /[\+\-\/\%]/);

	$cond;
}

sub handle_output {
	my $indent = pop @_;
	foreach (@_) {
		my $format = $_->getAttribute("format");
		my $output = $_->to_literal();
		my $count = $_->findnodes('./argument/@count');
		$output =~ s/\n//;
		$output =~ s/^[ \t]+//;
		$output = handle_arg($output);
		say " "x$indent,"print_ingame 1 \"$format\" \\";
		if ($count) {
			$count = sprintf("%d",$count);
			for (my $n=0; $n<$count; $n++) {
				my $res = $output;
				chomp $res;
				$res =~ s/argindex/$n/;
				say " $res \\";
			}
			say " ";
		} else {
			print " $output";
		}
	}
}

sub handle_action {
	my $indent = pop @_;
	my $last_cond = "";
	foreach (@_) {
		my $cond = $_->getAttribute("condition");
		# ils mélangent allègrement de l'hexa avec et sans préfixe !
		$cond = handle_arg($cond);
		if ($cond && $cond ne $last_cond) {
			if ($last_cond) {
				$indent -= 2;
				say " "x$indent,"endif"
			}
			say " "x$indent,"if $cond";
			$indent += 2;
			$last_cond = $cond;
		}
		my $action = $_->to_literal();
		# Apparemment ils ont utilisé , comme séparateur d'instructions !!!
		# Faut croire qu'ils n'utilisent pas de fonctions alors... !
		# en tous cas on en veut pas, donc :
		my @cmds = split(/,/,$action);
		while ($action = shift @cmds) {
			$action =~ s/^[ \t]//;
			$action =~ s/\n//g;
			next if ($action =~ /^[ \t]*$/);

			print " "x$indent;
			if ($action =~ /(maincpu|audiocpu).[mopr]p?(.)@(.+)=(.+)/) {
				my ($_cpu,$mode,$adr,$val) = ($1,$2,$3,$4);
				if ($_cpu eq "audiocpu" && !$cpu) {
					say "cpu audio";
					print " "x$indent;
					$cpu = 1;
				}
				if ($mode eq "d") { print "lpoke "; }
				elsif ($mode eq "b") { print "poke "; }
				elsif ($mode eq "w") { print "dpoke "; }
				elsif ($mode eq "q") {
					# gros bazar quad word, 64 bits quoi...
					# pour le z80 l'ordre est 0,8 puis 8,0
					# mais c'est l'inverse pour le z80, je laisse par défaut le 68000, seul cas connu pour le z80 jusqu'ici : dkong, kill screen fix, lpoke en rom
					if ($val =~ /^[ 0-9a-fA-F]+$/) {
						say "lpoke \$$adr \$",substr($val,0,8);
						$adr = sprintf("%x",hex($adr)+4);
						$val = substr($val,8);
					} else {
						say "lpoke \$$adr $val"."l";
						$adr = sprintf("%x",hex($adr)+4);
						$val .= "h";
					}
					print " "x$indent,"lpoke ";
				} else {
					die "action : mode unknown $mode for $action";
				}
				$adr = handle_arg($adr);
				$val = handle_arg($val);
				$adr = "\$$adr" if ($adr =~ /^[0-9a-f]/i);
				$val = "\$$val" if ($val =~ /^[0-9a-f]/i);
				say "$adr $val";
			} elsif ($action =~ /(.+)=maincpu.[mopr]p?(.)@(.+)/) {
				$action = handle_arg($action);
				if ($action =~ /(.+)=maincpu.[mopr]p?q@(.+)/) {
					say "$1l=lpeek(\$$2)";
					say " "x$indent,"$1h=lpeek(\$",sprintf("%x",hex($2)+4),")";
				} else {
					say $action;
				}
			} else {
				# on suppose qu'on a une action genre variable = valeur qui
				# peut être copiée telle quelle...
				$action = handle_arg($action);
				say $action;
			}
		} # while
	}
	if ($last_cond) {
		$indent -= 2;
		say " "x$indent,"endif"
	}
	if ($cpu) {
		say " "x$indent,"cpu main";
		$cpu = 0;
	}
}

sub handle_script {
	my $script = shift;
	return if (!$script);
	say $script->getAttribute("state"),":";
	handle_action($script->findnodes("./action"),4);
	handle_output($script->findnodes("./output"),4);
}

my $ref = XML::LibXML->load_xml(location => $ARGV[0]);
foreach ($ref->findnodes("//cheat")) {
	next if ($_->findvalue('./@desc') =~ /Sound Test/i);
	print 'script "',$_->findvalue('./@desc'),'" ';
	my $com = $_->findnodes('./comment');
	$com =~ s/"/'/g;
	print "\\\n \"comm:$com\" " if ($com);
	foreach my $param ($_->findnodes("./parameter")) {
		if ($param) {
			my ($min,$max,$inter) = ($param->getAttribute("min"),$param->getAttribute("max"),$param->getAttribute("step"));
			if ($max) {
				print "\\\n inter=$min,$max,$inter";
			} else {
				my @item = $param->findnodes("./item");
				foreach (@item) {
					my $s = $_->to_literal();
					$s =~ s/\"//g;
					print "\\\n \"",$_->getAttribute("value"),"/",$s,'"';
				}
			}
		}
	}
	say " ";
	my @script = $_->findnodes("./script");
	foreach (@script) {
		handle_script($_);
	}
}

