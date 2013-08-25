#!/usr/bin/perl
#
# elm2pov.pl
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

our $VERSION = 0.7;

use Carp qw(confess);
use Data::Dumper; $Data::Dumper::Indent = 1;
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
  progname => 'elm2pov',
  verbose => 0,
  debug => 0,
  regenerate => 0,
  generated => {},
  contpath => {
    terr => [qw(3dobjects)],
    quad => [qw(2dobjects ground)],
    mesh => [qw(3dobjects)],
    maps => [qw(maps)],
  },
  mapext => [qw(elm elm.gz elm.zip elm.bz2 elm.Z elm.xz)],
  imgext => [qw(png bmp dds)],
  initdir => $CWD,
  contdir => undef,
  workdir => undef,
  loader => undef,
  povfile => undef,
  povout => undef,
  cmd_povray => 'povray',
  cmd_convert => 'convert',
  render_size => 512,
  render_file => undef,
  render_lights => 0,
  mipmaps => 0,
  bitmap => 0,
  blur => 0,
  cull => 0.0,
  antialias => 0,
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

sub dumpit {
  return unless $ctx->{debug};
  my ($n, $d) = @_;
  if (not defined $d) {
    $d = $n;
    $n = 'data';
  }
  print Data::Dumper->Dump([$d], [$n]);
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

sub setup_map {
  my ($n) = @_;
  print "Reading $n from $ctx->{contdir}\n";
  my $l = $ctx->{loader};
  $map = $l->load($n)
    or die "Failed to load map '$n': ".$l->errstr."\n";
  load_entities 'quad';
  load_entities 'mesh';
}

sub find_texture {
  my ($n, $d) = @_;
  vprint "Searching for texture '$n'\n";
  my $b = catfile $ctx->{contdir}, @$d;
  my $k = catfile $b, "$n.png";
  if (-e $k) {
    vprint "  Found $k\n";
    return $k;
  }
  if (exists $ctx->{generated}{$k}) {
    my $i = $ctx->{generated}{$k};
    vprint "  Using already generated texture: $i\n";
    return $i;
  }
  my $i = catfile $ctx->{workdir}, 'images', @$d, "$n.png";
  if (-e $i and not $ctx->{regenerate}) {
    vprint "  Found $i\n";
    return $i;
  }
  my $s;
  for my $e (@{$ctx->{imgext}}) {
    my $p = catfile $b, "$n.$e";
    vprint "  Checking $p\n";
    if (-e $p) {
      vprint "  Found $p\n";
      $s = $p;
      last;
    }
  }
  if (!$s) {
    warn "WARNING: No texture image found for '$n' in '$b'\n";
    return '';
  }
  make_dir dirname $i;
  my @c = ($ctx->{cmd_convert}, $s, $i);
  print "Creating povray compatible texture images\n" unless $ctx->{convmsg}++;
  run \@c or die "Conversion failed: $!\n";
  $ctx->{generated}{$k} = $i;
  return $i;
}

sub find_terrain_texture {
  my ($t) = @_;
  return '' if $t == 255;
  $t = 231 if $t == 0 and $map->{indoors};
  my $n = "tile$t";
  my $i = find_texture $n, $ctx->{contpath}{terr};
  return $i if not $i or not $ctx->{mipmaps};
  my $s = $ctx->{render_size} / $map->{terrain_length};
  my $p = 1024;
  while ($p > 1 and $p / 2 > $s) {
    $p /= 2;
  }
  my ($ix, $iy, $ie) = imgsize($i);
  if (not $ix) {
    warn "WARNING: Failed to determine image size for '$i': $ie\n";
    return $i;
  }
  my $m = $ix;
  return $i if $m <= $p;
  my $e = 'png';
  my $r = $i;
  $e = $1 if $r =~ s/\.([^.]+)$//;
  $r = "$r-$p.$e";
  return $r if exists $ctx->{generated}{$r};
  return $r if -e $r and not $ctx->{regenerate};
  print "Generating terrain texture mipmaps\n" unless $ctx->{mipmapmsg}++;
  vprint "Creating ${p}x$p terrain texture mipmap from $i\n";
  my @c = ($ctx->{cmd_convert}, $i);
  push @c, '-resize', "${p}x$p";
  push @c, '-blur', '0x'.($p / 2) if $ctx->{blur};
  push @c, $r;
  run \@c or die "Mipmap generation failed: $!\n";
  $ctx->{generated}{$r}++;
  return $r;
}

sub find_quad_texture {
  my ($t) = @_;
  my $n = $t;
  $n =~ s/\.[^.]+$//;
  return find_texture $n, $ctx->{contpath}{quad};
}

sub find_mesh_texture {
  my ($t) = @_;
  my $n = $t;
  $n =~ s/\.[^.]+$//;
  return find_texture $n, $ctx->{contpath}{mesh};
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

sub vec_dot {
  my ($a, $b) = @_;
  return $a->[0] * $b->[0] + $a->[1] * $b->[1] + $a->[2] * $b->[2];
}

sub vec_len {
  my $v = ref $_[0] eq 'ARRAY' ? $_[0] : \@_;
  return sqrt vec_dot($v, $v);
}

sub emit_intro {
  my $n = $map->{name};
  my $cl = $ctx->{cmdline};
  my $sx = $map->{tile_length} * 0.5;
  my $sy = $map->{tile_breadth} * 0.5;
  my $c = vec_pov $sx / 2, $sy / 2, 100;
  my $l = vec_pov $sx / 2, $sy / 2, 0;
  my $a = "<1,1,1>";
  if ($ctx->{render_lights}) {
    my $v = $map->{ambient_light};
    $v = [0.3, 0.3, 0.3] if not $v or vec_len($v) < 0.3;
    $a = vec_pov $v;
  }
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
EOS
  vprint "Emitted intro\n";
}

sub setup_workdir {
  my $d = $ctx->{workdir};
  if (not defined $d) {
    $d = catfile $ENV{HOME}, '.' . $ctx->{progname};
  }
  make_dir $d;
  $ctx->{workdir} = $d;
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
  my $n = map_basename($m) . ".pov";
  #$ctx->{povfile} = catfile $ctx->{workdir}, $n;
  $ctx->{povfile} = $n;
}

sub terrain_pov {
  return "Terrain$_[0]";
}

sub is_water_terrain {
  my ($t) = @_;
  return ($t == 0 or (230 < $t and $t < 255));
}

sub emit_terrains {
  my %ti;
  my $c = 0;
  my $mii = $map->{indoors};
  my $td = $ctx->{render_lights} ? 1 : 0;
  for my $t (@{$map->{terrain_map}}) {
    next if exists $ti{$t};
    my $i = find_terrain_texture $t;
    if ($i) {
      my $n = terrain_pov $t;
      my $w = (not $mii and is_water_terrain $t);
      em "#declare $n = polygon {\n";
      em "  5, <0,0>, <0,1>, <1,1>, <1,0>, <0,0>\n";
      em "  scale 3\n";
      if ($w) {
        em "  texture {\n";
        em "    finish { ambient 1 diffuse $td }\n";
        em "    pigment { color rgb <0.2, 0.4, 0.8> }\n";
        em "  }\n";
      }
      em "  texture {\n";
      # NB all images are in screen coordinates
      # and need to be vertically flipped
      em "    scale <3, -3, 1>\n";
      em "    finish { ambient 1 diffuse $td }\n";
      em "    pigment {\n";
      em "      image_map {\n";
      em "        png \"$i\"\n";
      em "        interpolate 2\n";
      em "        transmit all 0.9\n" if $w;
      em "      }\n";
      em "    }\n";
      em "  }\n";
      em "}\n";
      $c++;
    }
    $ti{$t} = $i;
  }
  vprint "Emitted $c terrain declarations\n";
  $c = 0;
  my ($tl, $tb) = @{$map}{qw(terrain_length terrain_breadth)};
  my ($px, $py, $pz) = (0, 0, 0);
  for my $y (0 .. $tb - 1) {
    $py = $y * 3;
    for my $x (0 .. $tl - 1) {
      my $t = $map->{terrain_map}[$y * $tl + $x];
      next unless $ti{$t};
      $px = $x * 3;
      $pz = is_water_terrain($t) ? -0.25 : 0;
      my $n = terrain_pov $t;
      em "object { $n translate <$px, $py, $pz> }\n";
      $c++;
    }
  }
  vprint "Emitted $c terrain squares\n";
}

sub entity_pov {
  my ($f) = @_;
  my $n = $f;
  $n =~ s/\W/_/g;
  return "Entity_$n";
}

sub texture_pov {
  my ($f) = @_;
  my $n = $f;
  $n =~ s/\W/_/g;
  return "Texture_$n";
}

sub emit_placeholder_texture {
  return if exists $ctx->{placeholder_texture};
  my $n = 'NOT_FOUND';
  my $t = texture_pov $n;
  em<<"EOS";
#declare $t = texture { pigment { color rgb <1,1,1> } }
EOS
  $ctx->{placeholder_texture} = $n;
}

sub emit_quads {
  my $c = 0;
  my %tv;
  my $td = $ctx->{render_lights} ? 1 : 0;
  while (my ($f, $e) = each %{$map->{quad_entities}}) {
    next unless $e;
    my $t = $e->{texture_name};
    next if $tv{$t}++;
    my $i = find_quad_texture $t;
    unless ($i) {
      emit_placeholder_texture;
      next;
    }
    my $n = texture_pov $i;
    em<<"EOS";
#declare $n = texture {
  uv_mapping scale <1, -1, 1>
  finish { ambient 1 diffuse $td }
  pigment { image_map { png "$i" } }
}
EOS
    $c++;
  }
  vprint "Emitted $c quad textures\n";
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
    if (1) {
      my $i = find_quad_texture $e->{texture_name};
      unless ($i) {
        warn "WARNING: texture '$e->{texture_name}' not found for"
          . " quad '$f', using placeholder\n";
        $i = $ctx->{placeholder_texture};
      }
      my $t = texture_pov $i;
      em "  texture_list {\n    1, texture { $t }\n  }\n";
    }
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
  my %tv;
  my $td = $ctx->{render_lights} ? 1 : 0;
  while (my ($f, $e) = each %{$map->{mesh_entities}}) {
    next unless $e;
    my $sa = $e->{submeshes};
    for my $s (@$sa) {
      my $t = $s->{texture_name};
      next if $tv{$t}++;
      my $i = find_mesh_texture $t;
      unless ($i) {
        emit_placeholder_texture;
        next;
      }
      my $n = texture_pov $i;
      em<<"EOS";
#declare $n = texture {
  uv_mapping scale <1, -1, 1>
  finish { ambient 1 diffuse $td }
  pigment { image_map { png "$i" } }
}
EOS
      $c++;
    }
  }
  vprint "Emitted $c mesh textures\n";
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
      my $i = find_mesh_texture $s->{texture_name};
      unless ($i) {
        warn "WARNING: texture '$s->{texture_name}' not found for"
          . " mesh '$f', using placeholder\n";
        $i = $ctx->{placeholder_texture};
      }
      my $t = texture_pov $i;
      my $tn = texture_pov $i;
      em ", texture { $tn }";
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

sub emit_lights {
  my $c = 0;
  for my $l (@{$map->{light_objects}}) {
    my $p = vec_pov $l->{position};
    my $a = vec_pov $l->{color};
    em "light_source {\n";
    em "  $p, rgb $a\n";
    em "  shadowless\n";
    em "  media_interaction off\n";
    em "  fade_power 1\n";
    em "  fade_distance 0.71\n";
    em "}\n";
    $c++;
  }
  vprint "Emitted $c light objects\n";
}

sub emit_end {
  $ctx->{povout}->close;
}

sub render {
  my $s = $ctx->{render_size};
  my $p = $ctx->{povfile};
  my $n = map_basename($map->{name});
  my $r = $ctx->{render_file};
  my $t = $r || "$n-render_$$.png";
  my $e = $ctx->{bitmap} ? 'bmp' : 'png';
  my $f = $r || "$n.$e";
  my @c = ($ctx->{cmd_povray}, '-D');
  push @c, "+I$p", "+O$t";
  push @c, "+W$s", "+H$s";
  push @c, "+A" if $ctx->{antialias};
  print "Rendering $f\n";
  run \@c or die "Render failed: $!\n";
  if (not $r) {
    if ($ctx->{bitmap}) {
      run [$ctx->{cmd_convert}, $t, '-colors', 255, $f]
        or die "Failed to convert '$t' to '$f': $!\n";
      unlink $t;
    } else {
      move $t, $f or die "Failed to move '$t' to '$f': $!\n";
    }
  }
}

sub cull_meshes {
  my $m = $ctx->{cull};
  return if $m <= 0.001;
  vprint "Culling mesh entities smaller than $m pixels squared in the image...\n";
  my $rs = $ctx->{render_size};
  my $tl = $map->{tile_length};
  my $l = 2.0 * $rs / $tl;
  my $b = $l * $l;
  dprint "cull params: m=$m rs=$rs tl=$tl l=$l b=$b\n";
  my %r;
  my $INF = 1000000.0;
  while (my ($f, $e) = each %{$map->{mesh_entities}}) {
    my ($x, $y, $X, $Y) = ($INF, $INF, -$INF, -$INF);
    for my $v (@{$e->{vertices}}) {
      my ($vx, $vy) = @{$v->{position}}[0, 1];
      $x = $vx if $vx < $x;
      $X = $vx if $vx > $X;
      $y = $vy if $vy < $y;
      $Y = $vy if $vy > $Y;
    }
    my $a = $b * ($X - $x) * ($Y - $y);
    $r{$f} = $e if $a > $m;
    if ($ctx->{debug}) {
      dprint "mesh \"$f\" x=$x y=$y X=$X Y=$Y | a=$a "
        .($a > $m ? 'kept' : 'CULLED')."\n";
    }
  }
  my $c = keys %{$map->{mesh_entities}};
  my $n = keys %r;
  my $d = $c - $n;
  $map->{mesh_entities} = \%r;
  print "Culled $d of $c mesh entities\n";
}

sub setup_opts {
  $ctx->{cmdline} = join ' ', $0, @ARGV;
  my $g = new Getopt::Compact(
    name => $ctx->{progname},
    args => 'map',
    struct => [
      [[qw(v verbose)], 'print more messages about what is happening',
        '+', \$ctx->{verbose}],
      [[qw(d debug)], 'print LOTS of low level debugging information',
        '+', \$ctx->{debug}],
      [[qw(c contdir)], 'read game content files from this directory',
        '=s', \$ctx->{contdir}],
      [[qw(w workdir)], 'store intermediate files in this directory',
        '=s', \$ctx->{workdir}],
      [[qw(p povfile)], 'write the POV-Ray scene to this file',
        '=s', \$ctx->{povfile}],
      [[qw(o output)], 'write the rendered PNG image to this file',
        '=s', \$ctx->{render_file}],
      [[qw(g generate)], 'generate POV-Ray textures even if they already exist',
        '+', \$ctx->{regenerate}],
      [[qw(n no-render)], 'do not run POV-Ray after creating the scene file',
        '+', \$ctx->{no_render}],
      [[qw(s size)], 'side length of the generated square image (default 512)',
        '=i', \$ctx->{render_size}],
      [[qw(m mipmaps)], 'create and use terrain texture mipmaps',
        '+', \$ctx->{mipmaps}],
      [[qw(B blur)], 'apply an extra blur filter to terrain texture mipmaps',
        '+', \$ctx->{blur}],
      [[qw(b bitmap)], 'convert the rendered image to a 255-color 8-bit BMP file',
        '+', \$ctx->{bitmap}],
      [[qw(l lights)], 'render ambient and point light sources',
        '+', \$ctx->{render_lights}],
      [[qw(C cull)], 'skip meshes whose area will be less than this amount of pixels',
        '=f', \$ctx->{cull}],
      [[qw(a antialias)], 'render with antialiasing enabled (slow!)',
        '+', \$ctx->{antialias}],
    ],
  );
  $g->opts;
  $ctx->{getopt} = $g;
}

sub main {
  setup_opts;
  my $m = shift @ARGV or die $ctx->{getopt}->usage;
  setup_contdir $m;
  setup_workdir;
  setup_loader;
  my $n = find_map $m or die "Map not found: $m\n";
  setup_map $n;
  setup_pov $n;
  cull_meshes;
  print "Writing $ctx->{povfile}\n";
  emit_intro;
  emit_terrains;
  emit_quads;
  emit_meshes;
  emit_lights if $ctx->{render_lights};
  emit_end;
  render unless $ctx->{no_render};
  print "All done.\n";
  exit 0;
}

main;
