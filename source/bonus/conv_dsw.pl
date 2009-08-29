#!/usr/bin/perl

# This script is now quite unreadable because it does a quite crazy conversion
# it even knows how to convert macros included in the DSW definition from mame.
# and keep the default values even from the macros...

use strict;

my %raine_dsw =
    (
     "Unknown" => "MSG_UNKNOWN",
     "None" => "MSG_UNKNOWN",
     "Normal" => "MSG_NORMAL",
     "Hard" => "MSG_HARD",
     "Alternate" => "MSG_ALTERNATE",
     "Easy" => "MSG_EASY",
     "Very_Easy" => "MSG_VERY_EASY",
     "Very_Hard" => "MSG_VERY_HARD",
     "Hardest" => "MSG_HARDEST",
     "Medium" => "MSG_MEDIUM",

     "Off" => "MSG_OFF",
     "On" => "MSG_ON",
     "Coin_A" => "MSG_COIN1",
     "9C_1C" => "MSG_9COIN_1PLAY",
     "8C_1C" => "MSG_8COIN_1PLAY",
     "8C_3C" => "MSG_8COIN_3PLAY",
     "7C_1C" => "MSG_7COIN_1PLAY",

     "6C_1C" => "MSG_6COIN_1PLAY",
     "6C_2C" => "MSG_6COIN_2PLAY",
     "6C_3C" => "MSG_6COIN_3PLAY",
     "6C_4C" => "MSG_6COIN_4PLAY",

     "5C_1C" => "MSG_5COIN_1PLAY",
     "5C_2C" => "MSG_5COIN_2PLAY",
     "5C_3C" => "MSG_5COIN_3PLAY",
     "5C_4C" => "MSG_5COIN_4PLAY",

     "4C_1C" => "MSG_4COIN_1PLAY",
     "4C_2C" => "MSG_4COIN_2PLAY",
     "4C_3C" => "MSG_4COIN_3PLAY",
     "4C_4C" => "MSG_4COIN_4PLAY",
     "4C_5C" => "MSG_4COIN_5PLAY",
     "4C_6C" => "MSG_4COIN_6PLAY",
     "4C_7C" => "MSG_4COIN_7PLAY",
     "4C_8C" => "MSG_4COIN_8PLAY",

     "3C_1C" => "MSG_3COIN_1PLAY",
     "3C_2C" => "MSG_3COIN_2PLAY",
     "3C_3C" => "MSG_3COIN_3PLAY",
     "3C_4C" => "MSG_3COIN_4PLAY",
     "3C_5C" => "MSG_3COIN_5PLAY",
     "3C_6C" => "MSG_3COIN_6PLAY",
     "3C_7C" => "MSG_3COIN_7PLAY",
     "3C_8C" => "MSG_3COIN_8PLAY",

     "2C_1C" => "MSG_2COIN_1PLAY",
     "2C_2C" => "MSG_2COIN_2PLAY",
     "2C_3C" => "MSG_2COIN_3PLAY",
     "2C_4C" => "MSG_2COIN_4PLAY",
     "2C_5C" => "MSG_2COIN_5PLAY",
     "2C_6C" => "MSG_2COIN_6PLAY",
     "2C_7C" => "MSG_2COIN_7PLAY",
     "2C_8C" => "MSG_2COIN_8PLAY",

     "1C_1C" => "MSG_1COIN_1PLAY",
     "1C_2C" => "MSG_1COIN_2PLAY",
     "1C_3C" => "MSG_1COIN_3PLAY",
     "1C_4C" => "MSG_1COIN_4PLAY",
     "1C_5C" => "MSG_1COIN_5PLAY",
     "1C_6C" => "MSG_1COIN_6PLAY",
     "1C_7C" => "MSG_1COIN_7PLAY",
     "1C_8C" => "MSG_1COIN_8PLAY",
     "1C_9C" => "MSG_1COIN_9PLAY",

     "Coin_B" => "MSG_COIN2",
     "Demo_Sounds" => "MSG_DEMO_SOUND",
     "Flip_Screen" => "MSG_SCREEN",
     "Cabinet" => "MSG_CABINET",
     "Upright" => "MSG_UPRIGHT",
     "Cocktail" => "MSG_TABLE",
     "Difficulty" => "MSG_DIFFICULTY",
     "Bonus_Life" => "MSG_EXTRA_LIFE",
     "Lives" => "MSG_LIVES",
     "Yes" => "MSG_YES",
     "No" => "MSG_NO",
     "Free_Play" => "MSG_FREE_PLAY",
     "Unused" => "MSG_UNUSED",
     "Coinage" => "MSG_COINAGE",
     "Service_Mode" => "MSG_SERVICE",
     );

use vars qw(@dsw $name $nb @tab_data @tab_name);

my $port = $ARGV[1] || 0;
$port = value($port);
$port-=2;
my $port0 = $port;
my $default = 0;
my $output;
my $started = undef;
my $length;
my ($bit,$def,$function);
my $main_out="";
my $dsw_name;
my $roll = undef;

sub update_start() {
  if ($dsw_name) {
    if ($length) {
      if ($roll) {
	$port++;
#	$default >>= 8;
      }
      my $found = undef;
      for (my $n=0; $n<=$#tab_data; $n++) {
	if ($tab_data[$n] eq $main_out) {
	  print STDERR "Found $dsw_name = $tab_name[$n]\n";
	  push @dsw,[$tab_name[$n],$default,$port];
	  $found = 1;
	}
      }
      if ($dsw_name =~ /mspacman_4/ && !$found) {
	print "not found : $main_out\n";
	exit(1);
      }
      if (!$found) {
	push @dsw,[$dsw_name,$default,$port];
	print "static struct DSW_DATA $dsw_name\[\] =\n{\n";
# 	if ($started) {
# 	  $main_out .= "  added $length { $function, $bit, $length },\n" if ($length >= 1);
# 	  $main_out .= " output : $output";
# 	}
	print "$main_out  { NULL, 0, 0}\n};\n\n";
	push @tab_name,$dsw_name;
	push @tab_data,$main_out;
	$main_out = "";
      }
    }
  }
  $nb++;
  $length = 0;
  if ($roll) {
    $port++;
  } else {
    $port+=2;
  }
  $roll = undef;
  $default = 0;
  $main_out = "";
  $dsw_name = "dsw_data_$name\_$nb";
#   print STDERR "starting $dsw_name\n";
}

sub get_raine_dsw($) {
  my $dsw = shift;
  my $function;
  if ($raine_dsw{$dsw}) {
    $function = $raine_dsw{$dsw};
  } else {
    print STDERR "warning : Unknown DSW $dsw from $_\n";
    $function = "\"$dsw\"";
  }
  $function;
}

sub value($) {
  my $a = shift;
  $a =~ s/^[ \t]*//;
  if ($a =~ /^0x/) {
    return hex($a);
  }
  return $a;
}

sub read_next_line() {
  while (<F>) {
    next if (/^[ \t]*\/\//); # skip // comments
    if (/^[ \t]*\/\*/) { # /* comments
      while (!/\*\//) {
	$_ = <F>;
      }
      s/^.+\*\///; # remove everything until */
    }
    last;
  }
  return $_;
}

my $macro = undef;
my %default_macro = ();

open(F,"<$ARGV[0]") || die "Impossible to open $ARGV[0]";
my $last_line = "";
while (read_next_line()) {
  if (/INPUT_PORTS_START\( (.+?) \)/ || $macro) {
    if (!$macro) {
      $name = $1;
      $name = "_".$name if ($name =~ /^\d/);
    } else {
      $macro = 2;
    }
    @dsw = ();
    $nb = -1;
    $dsw_name = undef;
    while ($macro == 2 || read_next_line()) {
      # Normalize spaces to be sure to find same dipswitches
      s/ ([\,\"\)])/$1/g;
      s/ +$//;
      s/ +/ /g;
      $macro = 1 if ($macro == 2);
      next if (/^[ \t]*\/\//);
      s/PORT_CODE.+?\)//; # PORT_CODE not supported (yet) in raine
      # mame has really crazy syntaxes sometimes !
      s/PORT_BIT\((.+)IPT_DIPSWITCH_NAME *\) *PORT_NAME\( *(\".+?\")/PORT_DIPNAME\($1$2/;
      if (/PORT_START/) {
	update_start();
	$started = undef;
      } elsif (/PORT_BITX\([ \t]?(.+)[ \t]?\)/) {
	# Not sure wether it will be enough...
	my $args = $1;
	my ($bit,$def,$function,$name,$rest) = split(/\, ?/,$args);
	if (value($bit) > 0) { # Be sure it looks like a dipname...
	  $_ = "PORT_DIPNAME( $bit, $def, $name )";
	  redo;
	}
      } elsif (/PORT_DIPNAME\( ?(.+) ?\)/) {
	my $args = $1;
	if ($started) {
	  $main_out .= "  { $function, $bit, $length },\n" if ($length >= 1);
	  $main_out .= $output;
	}
	$started = 1;
	my $rest;
	if ((($bit,$def,$function,$rest) = split(/\, ?/,$args))) {
	  my $bit2 = value($bit);
	  if ($bit2 > 0xff && !$roll) {
	    update_start();
	    $started = 1;
	    $roll = 1;
	  }
	  $bit = "0x".sprintf("%x",$bit2 >> 8) if ($roll);
	  $function .= ",$rest" if ($rest);
	  $def = hex($def) if ($def =~ /^0x/);
	  $def >>=8 if ($roll);
          $default |= $def;
	  $output = "";
	  $length = 0;
	  if ($function =~ /DEF_STR\( ?(.+?) ?\)/) {
	    $function = get_raine_dsw($1);
          }
	} else {
	  print STDERR "Parsing error : $args\n";
	  exit(1);
	}
      } elsif (/PORT_SERVICE\( *(.+) *\)/) {
	my $args = $1;
	my ($mask,$bit) = split(/\, */,$args);
	if (!$started) {
	  $output = "";
	  $started = 1;
	}
	if ($roll) {
	  $mask = "0x".sprintf("%x",value($mask) >> 8);
	}
	$output .= "  { MSG_SERVICE, $mask,2 },\n";
	if ($bit =~ /IP_ACTIVE_LOW/) {
	  $output .= "  { MSG_ON, 0,0 },\n";
	  $output .= "  { MSG_OFF, $mask,0 },\n";
	  $mask = hex($mask);
	  $default |= $mask;
	} elsif ($bit =~ /IP_ACTIVE_HIGH/) {
	  $output .= "  { MSG_OFF, 0,0 },\n";
	  $output .= "  { MSG_ON, $mask,0 },\n";
	} else {
	  print STDERR "Unknown service port $args ($bit)\n";
	  exit(1);
	}
      } elsif (/PORT_DIPSETTING\( *(.+) *\)/) {
	my $args = $1;
	$args =~ s/\)[ \t]*\/[\/\*].+//; # remove comments from the args
	my ($mask,$f2,$rest) = split(/\, */,$args);
	my $bit2 = value($mask);
	$mask = "0x".sprintf("%x",$bit2 >> 8) if ($roll);
	$f2 .= $rest if ($rest);
	if ($f2 =~ /DEF_STR\( ?(.+?) ?\)/) {
	  $f2 = get_raine_dsw($1);
	}
	$output .= "  { $f2, $mask, 0x00 },";
	$output .= "\t// $1" if (/\/\/ (.+)/);
	$output .= "\n";
	$length++;
      } elsif (!/^[ \t]*(\/\*.+)?$/) {
	# macro
	my $macro_name = $_;
	chomp $macro_name;
	if (!/IPT_UNKNOWN/ && !/PORT/) {
	  $macro_name =~ s/[ \t]+//g;
	  if (!defined($default_macro{$macro_name})) {
	    print STDERR "macro non reconnue $macro_name - je continue\n";
	  }
	  $main_out .= $_;
	  $default |= $default_macro{$macro_name};
	}
      } elsif ($started) { # end of the dsw section if started
	$main_out .= "  { $function, $bit, $length },\n$output";
	$started = undef;
      }
      last if (/PORTS_END/);
      if ($macro && !(/\\$/)) {
	last;
      }
    } # while (<F>)
    if ($length) {
      if ($started) {
	$main_out .= "  { $function, $bit, $length },\n" if ($length >= 1);
	$main_out .= $output;
	$output = "";
	$started = undef;
      }

      my $found = undef;
      if (!$macro) {
	for (my $n=0; $n<=$#tab_data; $n++) {
	  if ($tab_data[$n] eq $main_out) {
	    print STDERR "Found $dsw_name = $tab_name[$n]\n";
	    push @dsw,[$tab_name[$n],$default,$port];
	    $found = 1;
	  }
	}
      }
      if ($macro) {
	print $last_line;
	$main_out =~ s/\n/\\\n/mg;
	print "$main_out\n";
	my $macro_name = $last_line;
	$macro_name =~ s/^\#define //;
	$macro_name =~ s/[ \t]+//g;
	$macro_name =~ s/\\\n//;
	$default_macro{$macro_name} = $default;
	$macro = undef;
	$main_out = "";
      } elsif (!$found) {
	push @dsw,[$dsw_name,$default,$port];
	print "static struct DSW_DATA $dsw_name\[\] =\n{\n";
	if ($started) {
	  $main_out .= "  { $function, $bit, $length },\n" if ($length >= 1);
	  $main_out .= $output;
	}
	print "$main_out  { NULL, 0, 0}\n};\n\n";
	push @tab_name,$dsw_name;
	push @tab_data,$main_out;
	$main_out = "";
      }
    }
    $started = undef;
    $port = $port0;

    # dsw_info
    if ($#dsw>=0) {
      print "static struct DSW_INFO $name\_dsw\[\] =\n{\n";
      for (my $n=0; $n<=$#dsw; $n++) {
	my ($dname, $def, $port) = @{$dsw[$n]};
	print "  { ".sprintf("0x%x",$port).", 0x",sprintf("%02x",$def),", $dname },\n";
      }
      print "  { 0, 0, NULL }\n";
      print "};\n\n";
    }
  } elsif (/PORT_DIPNAME\(/) {
    $macro = 1;
    redo;
  } else {
    $last_line = $_;
  }
}
close(F);

