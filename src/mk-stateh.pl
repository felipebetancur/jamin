#!/usr/bin/perl -w

open(SRC, "state-vars.txt") || die "Can't open 'state-vars.txt'";
open(OUT, "| indent -kr > state.h") || die "Can't invoke indent and write";

my @symbols;
my %card;
my %desc;
my $id;

$id = 0;

while(<SRC>) {
	next if (/^#/);
	if (/([a-z-]+)\t+(.+)/) {
		push @symbols, $1;
		$card{$1} = 1;
		$desc{$1} = $2;
	} elsif (/([a-z-]+)\((\d+)\)\t+(.+)/) {
		push @symbols, $1;
		$card{$1} = $2;
		$desc{$1} = $3;
	} else {
		die "Syntax error: $_";
	}
}

print OUT <<EOB;
#ifndef STATE_H
#define STATE_H

#include <gtk/gtk.h>

typedef void(* s_callback_func)(int id, float value);

void state_init();
void s_set_value_ui(int id, float value);
void s_set_value(int id, float value, int time);
void s_set_value_no_history(int id, float value);
void s_clear_history();
void s_set_callback(int id, s_callback_func callback);
void s_set_adjustment(int id, GtkAdjustment *adjustment);
void s_undo();

#define S_NONE -1
EOB

for $sym (@symbols) {
	$macro = $sym;
	$macro =~ s/-/_/g;
	$macro = "\U$macro";
	if ($card{$sym} == 1) {
		print OUT "#define S_$macro ".$id++."\n";
	} else {
		print OUT "#define S_$macro(n) ($id + n)\n";
		$id += $card{$sym};
	}
}

print OUT "\n#define S_SIZE $id\n\n";

$first = 1;
print OUT "static const char * const s_description[S_SIZE] = {\n";
for $sym (@symbols) {
	if ($card{$sym} > 1) {
		for ($i = 1; $i <= $card{$sym}; $i++) {
			if (!$first) {
				print OUT ",";
			} else {
				$first = 0;
			}
			print OUT "\"$desc{$sym} $i\"";
		}
	} else {
		if (!$first) {
			print OUT ",";
		} else {
			$first = 0;
		}
		print OUT "\"$desc{$sym}\"";
	}
}
print OUT "};\n\n";

$first = 1;
print OUT "static const char * const s_symbol[S_SIZE] = {\n";
for $sym (@symbols) {
	if ($card{$sym} > 1) {
		for ($i = 0; $i < $card{$sym}; $i++) {
			if (!$first) {
				print OUT ",";
			} else {
				$first = 0;
			}
			print OUT "\"$sym$i\"";
		}
	} else {
		if (!$first) {
			print OUT ",";
		} else {
			$first = 0;
		}
		print OUT "\"$sym\"";
	}
}
print OUT "};\n\n";
print OUT "#endif\n";

close OUT;