#!/usr/bin/perl
#
# rects2def
#
#    Copyright 2013 Cole Minor <c.minor@inbox.com>
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#

use warnings;
use strict;

sub usage {
  print<<EOS;
Usage: rects2def.pl rects-file section-name [key1=value1] [key2=value2] ...

Example:
\$ $0 rects.txt walk_damage value=1 > output.def
EOS
  exit 1;
}

my $f = shift or usage;
my $d = shift or usage;
my @v;
for my $s (@ARGV) {
  next unless $s =~ /^([^=]+)=(.+)$/;
  push @v, [$1, $2];
}

open IN, $f or die "failed to open '$f' for reading: $!\n";

while (<IN>) {
  chomp;
  next unless /^\s*(\d+),(\d+),(\d+),(\d+)\s*$/;
  my ($x0, $y0, $x1, $y1) = ($1, $2, $3, $4);
  print<<EOS;
[$d]
min_x: $x0
min_y: $y0 
max_x: $x1
max_y: $y1
EOS
  print "$_->[0]: $_->[1]\n" for @v;
  print "[/$d]\n";
}
