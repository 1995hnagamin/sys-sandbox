#!/usr/bin/perl

use strict;
use warnings;

open (IN, "sgb-words.txt") or die "$!";
my @dict = sort <IN>;
chomp @dict;
close IN;

print "<ul>\n";
while (<>) {
	my @cols = split /\t/, $_;
	my $regex = ($cols[0] =~ s/\*/./r);
	print "  <li>($cols[0])  ", join(', ', grep { /$regex/ } @dict), "</li>\n";
}
print "</ul>\n";
