#!/usr/bin/perl

use strict;
use warnings;

open (IN, "sgb-words.txt") or die "$!";
my @dict = sort <IN>;
chomp @dict;
close IN;

my $total = 0;
my %uniq = ();

print "<ul>\n";
while (<>) {
	my @cols = split /\t/, $_;
	my $regex = ($cols[0] =~ s/\*/./r);
	my @words = grep { /$regex/ } @dict;
	print "  <li>($cols[0])  ", join(', ', @words), "</li>\n";
	$total += scalar(@words);
	foreach (@words) {
		$uniq{$_} = 1;
	}
}
print "</ul>\n";
print scalar(%uniq)."\n";
print $total."\n";
