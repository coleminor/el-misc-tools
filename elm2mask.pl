#!/usr/bin/perl
#
# elm2mask.pl
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

our $VERSION = 0.1;

use Carp qw(confess);
use File::Basename;
use File::chdir;
use File::Copy;
use File::Spec::Functions qw(catfile);
use File::PathConvert qw(realpath rel2abs);
use File::Path::Tiny;
use File::Temp qw(tempfile);
use Games::EternalLands::Loader;
use Getopt::Compact;
use Image::Size;
use IO::File;
use IPC::Run3;
use POSIX qw(strftime);

my $ctx = {
  progname => 'elm2mask',
  verbose => 0,
  debug => 0,
  contpath => {
    terr => [qw(3dobjects)],
    quad => [qw(2dobjects ground)],
    mesh => [qw(3dobjects)],
    maps => [qw(maps)],
  },
  mapext => [qw(elm elm.gz elm.zip elm.bz2 elm.Z elm.xz)],
  initdir => $CWD,
  contdir => undef,
  loader => undef,
  povfile => undef,
  povout => undef,
  cmd_povray => 'povray',
  render_sizes => undef,
  render_file => undef,
  no_render => 0,
};

my $map = {};

sub vprint { print @_ if $ctx->{verbose}; }
sub dprint { print @_ if $ctx->{debug}; }

sub run {
  my ($c) = @_;
  my $i = \undef;
  my $o = $ctx->{verbose} ? undef : \undef;
  vprint "Running: @$c\n";
  my $r = run3 $c, $i, $o, $o, { return_if_system_error => 1 };
  my $e = $? >> 8;
  if ($? == -1) {
    confess "Failed to run external command: $!\nCommand was: @$c\n";
  }
  return $e == 0;
}

sub make_dir {
  my ($d) = @_;
  return if -d $d;
  File::Path::Tiny::mk $d or die "Failed to create directory '$d': $!\n";
}

sub has_content {
  my ($d) = @_;
  vprint "Checking for content paths in '$d'\n";
  my %v;
  for my $s (values %{$ctx->{contpath}}) {
    my $p = catfile $d, @$s;
    next if $v{$p}++;
    if (not -d $p) {
      vprint "Did not find expected directory: $p\n";
      return 0;
    } else {
      vprint "Found: $p\n";
    }
  }
  vprint "All needed content paths found in '$d'\n";
  return 1;
}

sub find_contdir {
  my ($c) = @_;
  my $r;
  my %v;
  for my $p (@$c) {
    $p = realpath $p;
    next if $v{$p}++;
    next unless -d $p;
    if (has_content $p) {
      $r = $p;
      last;
    }
    local $CWD = $p;
    my $f = 0;
    while (1) {
      $f = has_content $CWD;
      last if $f or not pop @CWD;
    }
    if ($f) {
      $r = $CWD;
      last;
    }
  }
  if ($r) {
    vprint "Found contdir '$r'\n";
    $ctx->{contdir} = $r;
  }
}

sub find_map {
  my ($f) = @_;
  $f = basename $f if $f =~ /[\/\\]/;
  my $n = catfile @{$ctx->{contpath}{maps}}, $f;
  my $p = $ctx->{contdir};
  my @m;
  my $b = catfile($p, $n);
  push @m, $n if -e $b;
  for my $e (@{$ctx->{mapext}}) {
    my $t = "$b.$e";
    vprint "Checking for map file with extension '$e': $t\n";
    if (-e $t) {
      push @m, "$n.$e";
      vprint "  Found: $n.$e\n";
    }
  }
  if (@m > 1) {
    vprint "Multiple maps found for '$f' (first will be used):\n";
    vprint "  $_\n" for @m;
  }
  vprint "Using map: $m[0]\n" if $m[0];
  return $m[0];
}

sub setup_loader {
  $ctx->{loader} = new Games::EternalLands::Loader;
  $ctx->{loader}->content_path($ctx->{contdir});
}

sub map_basename {
  my ($n) = @_;
  $n = basename $n;
  while (1) {
    last unless $n =~ s/\.[^.]+$//;
  }
  return $n;
}

sub load_entities {
  my ($k) = @_;
  my %d;
  my $l = $ctx->{loader};
  my $c = 0;
  for my $o (@{$map->{$k . '_objects'}}) {
    my $n = $o->{entity_name};
    next if exists $d{$n};
    if (exists $o->{blending_level} and $o->{blending_level} == 20) {
      # io/map_io.c +320 do_load_map(): seems to ignore if blended == 20
      vprint "Skipping object $o->{id} $n, marked as \"invisible\"\n";
      next;
    }
    vprint "Loading $k entity '$n' ...\n";
    my $e = $l->load($n);
    if (!$e) {
      warn "WARNING: failed to load $k entity '$n': ".$l->errstr."\n";
      $d{$n} = 0;
    } else {
      $d{$n} = $e;
      $c++;
    }
  }
  $map->{$k . '_entities'} = \%d;
  vprint "Loaded $c $k entities\n";
}

sub prune_objects {
  my ($k, $pats) = @_;
  my @m;
  my $c = 0;
  for my $o (@{$map->{$k.'_objects'}}) {
    my $n = $o->{entity_name};
    dprint "checking '$n' against patterns...\n";
    for my $p (@$pats) {
      if ($o->{entity_name} =~ /$p/) {
        dprint "  MATCHED /$p/\n";
        push @m, $o;
        $c++;
        last;
      } else {
        dprint "  did not match /$p/\n";
      }
    }
  }
  vprint "Matched $c $k objects to patterns\n";
  $map->{$k.'_objects'} = \@m;
}

sub setup_map {
  my ($n, $p) = @_;
  print "Reading $n from $ctx->{contdir}\n";
  my $l = $ctx->{loader};
  $map = $l->load($n)
    or die "Failed to load map '$n': ".$l->errstr."\n";
  prune_objects 'quad', $p;
  prune_objects 'mesh', $p;
  load_entities 'quad';
  load_entities 'mesh';
}

sub get_output {
  unless ($ctx->{povout}) {
    my $f = $ctx->{povfile};
    $ctx->{povout} = new IO::File "> $f"
      or die "Failed to open '$f' for writing: $!\n";
  }
  return $ctx->{povout};
}

sub em {
  my $o = get_output;
  print $o @_;
}

sub vec_pov {
  my $v = ref $_[0] eq 'ARRAY' ? $_[0] : \@_;
  return '<'.join(',', @$v).'>';
}

sub emit_intro {
  my $n = $map->{name};
  my $cl = $ctx->{cmdline};
  my $sx = $map->{tile_length} * 0.5;
  my $sy = $map->{tile_breadth} * 0.5;
  my $c = vec_pov $sx / 2, $sy / 2, 100;
  my $l = vec_pov $sx / 2, $sy / 2, 0;
  my $a = "<1,1,1>";
  my $d = strftime '%Y/%m/%d %H:%M:%S GMT', gmtime;
  em<<"EOS";
/*
 * This is an automatically generated POV-Ray scene file
 * used to render an Eternal Lands tab map for '$n'.
 *
 * Command used to create this file:
 *   $cl
 *
 * Date of creation: $d
 */
camera {
  orthographic
  location $c
  right - x * $sx
  up y * $sy
  look_at $l
}
global_settings {
  ambient_light $a
  max_trace_level 8
  assumed_gamma 2.2
}
#declare Texture_White = texture {
  pigment { color rgb <1,1,1> }
  finish { ambient 1 }
}
EOS
  vprint "Emitted intro\n";
}

sub setup_contdir {
  my ($m) = @_;
  if (not $ctx->{contdir}) {
    my @p;
    push @p, $ctx->{initdir};
    push @p, realpath dirname $m if $m =~ m!/!;
    push @p, catfile qw(usr share games eternal-lands);
    find_contdir \@p;
    die "Game content files not found"
      . " (do you need to set the --contdir option?)\n"
      unless $ctx->{contdir};
  }
  vprint "Using contdir: $ctx->{contdir}\n";
}

sub setup_pov {
  my ($m) = @_;
  return if $ctx->{povfile};
  my $n = map_basename($m) . "-mask.pov";
  $ctx->{povfile} = $n;
  $ctx->{render_sizes} = [$map->{tile_length}, $map->{tile_breadth}];
}

sub entity_pov {
  my ($f) = @_;
  my $n = $f;
  $n =~ s/\W/_/g;
  return "Entity_$n";
}

sub emit_quads {
  my $c = 0;
  $c = 0;
  while (my ($f, $e) = each %{$map->{quad_entities}}) {
    next unless $e;
    my $n = entity_pov $f;
    my $vn = @{$e->{vertices}};
    my $v = $e->{vertices};
    em "#declare $n = mesh2 {\n";
    em "  vertex_vectors {\n    $vn";
    em ", ".vec_pov($_->{position}) for @$v;
    em "\n  }\n";
    em "  normal_vectors {\n    $vn";
    em ", ".vec_pov($_->{normal}) for @$v;
    em "\n  }\n";
    em "  uv_vectors {\n    $vn";
    em ", ".vec_pov($_->{uv}) for @$v;
    em "\n  }\n";
    em "  texture_list {\n    1, texture { Texture_White }\n  }\n";
    my $ia = $e->{indices};
    my $fn = @$ia / 3;
    em "  face_indices {\n    $fn";
    my $is = 0;
    my $ie = @$ia;
    my $si = 0;
    for (my $i = $is; $i < $ie; $i += 3) {
      em ", ".vec_pov(@{$ia}[$i..$i+2]).", $si";
    }
    em "\n  }\n";
    em "  hierarchy off\n";
    em "}\n";
    $c++;
  }
  vprint "Emitted $c quad entity declarations\n";
  $c = 0;
  for my $o (@{$map->{quad_objects}}) {
    my $e = $o->{entity_name};
    next unless $map->{quad_entities}{$e};
    my $n = entity_pov $e;
    my $p = vec_pov $o->{position};
    my $r = vec_pov $o->{rotation};
    em "object { $n rotate $r translate $p }\n";
    $c++;
  }
  vprint "Emitted $c quad objects\n";
}

sub emit_meshes {
  my $c = 0;
  $c = 0;
  while (my ($f, $e) = each %{$map->{mesh_entities}}) {
    next unless $e;
    my $n = entity_pov $f;
    my $vn = @{$e->{vertices}};
    my $v = $e->{vertices};
    em "#declare $n = mesh2 {\n";
    em "  vertex_vectors {\n    $vn";
    em ", ".vec_pov($_->{position}) for @$v;
    em "\n  }\n";
    em "  normal_vectors {\n    $vn";
    em ", ".vec_pov($_->{normal}) for @$v;
    em "\n  }\n";
    em "  uv_vectors {\n    $vn";
    em ", ".vec_pov($_->{uv}) for @$v;
    em "\n  }\n";
    my $sa = $e->{submeshes};
    my $sn = @$sa;
    em "  texture_list {\n    $sn";
    for my $s (@$sa) {
      em ", texture { Texture_White }";
    }
    em "\n  }\n";
    my $ia = $e->{indices};
    my $fn = @$ia / 3;
    em "  face_indices {\n    $fn";
    for (my $si = 0; $si < $sn; $si++) {
      my $s = $sa->[$si];
      my $is = $s->{index_element_offset};
      my $ie = $is + $s->{index_element_count};
      for (my $i = $is; $i < $ie; $i += 3) {
        em ", ".vec_pov(@{$ia}[$i..$i+2]).", $si";
      }
    }
    em "\n  }\n";
    em "}\n";
    $c++;
  }
  vprint "Emitted $c mesh entity declarations\n";
  $c = 0;
  for my $o (@{$map->{mesh_objects}}) {
    my $e = $o->{entity_name};
    next unless $map->{mesh_entities}{$e};
    my $n = entity_pov $e;
    my $p = vec_pov $o->{position};
    my $r = vec_pov $o->{rotation};
    em "object { $n rotate $r translate $p }\n";
    $c++;
  }
  vprint "Emitted $c mesh objects\n";
}

sub emit_end {
  $ctx->{povout}->close;
}

sub render {
  my $s = $ctx->{render_sizes};
  my $p = $ctx->{povfile};
  my $n = map_basename($map->{name});
  my $f = "$n-mask.png";
  my @c = ($ctx->{cmd_povray}, '-D');
  push @c, "+I$p", "+O$f";
  push @c, "+W$s->[0]", "+H$s->[1]";
  print "Rendering $f\n";
  run \@c or die "Render failed: $!\n";
}

sub setup_opts {
  $ctx->{cmdline} = join ' ', $0, @ARGV;
  my $g = new Getopt::Compact(
    name => $ctx->{progname},
    args => 'map object-pattern1 [object-pattern2 ...]',
    struct => [
      [[qw(v verbose)], 'print more messages about what is happening',
        '+', \$ctx->{verbose}],
      [[qw(d debug)], 'print LOTS of low level debugging information',
        '+', \$ctx->{debug}],
      [[qw(c contdir)], 'read game content files from this directory',
        '=s', \$ctx->{contdir}],
      [[qw(n no-render)], 'do not run POV-Ray after creating the scene file',
        '+', \$ctx->{no_render}],
    ],
  );
  $g->opts;
  $ctx->{getopt} = $g;
}

sub main {
  setup_opts;
  my $m = shift @ARGV or die $ctx->{getopt}->usage;
  my @p = @ARGV;
  die $ctx->{getopt}->usage unless @p;
  setup_contdir $m;
  setup_loader;
  my $n = find_map $m or die "Map not found: $m\n";
  setup_map $n, \@p;
  setup_pov $n;
  print "Writing $ctx->{povfile}\n";
  emit_intro;
  emit_quads;
  emit_meshes;
  emit_end;
  render unless $ctx->{no_render};
  print "All done.\n";
  exit 0;
}

main;
