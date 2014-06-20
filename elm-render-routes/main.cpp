//
//  Copyright (C) 2014 Cole Minor
//  This file is part of elm-render-routes
//
//  elm-render-routes is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  elm-render-routes is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "brush.hpp"
#include "color.hpp"
#include "file.hpp"
#include "foreach.hpp"
#include "format.hpp"
#include "grid.hpp"
#include "image.hpp"
#include "options.hpp"
#include "path.hpp"
#include "report.hpp"
#include "route.hpp"

#include <map>
#include <stdexcept>
#include <vector>

typedef std::vector<Route> Routes;
typedef std::vector<Path> Paths;

static void read_routes(Routes &v) {
  char s[1024];
  Route r;
  const std::string &p = g_options->routes_path;
  File i(p);
  for (int l = 1; i.readline(s, sizeof s); ++l) {
    try {
      r.parse(s);
    } catch (Route::ParseError &e) {
      Format f = "'{}' line {}: {}";
      warn(f.bind(p, l, e.what()));
      continue;
    }
    v.push_back(r);
  }
}

static void fill_cells(Grid &g, const Image &i) {
  const int d = g_options->cell_size;
  const int w = i.width() / d;
  const int h = i.height() / d;
  const float l = g_options->land_cost;
  Color c;
  g.resize(w, h);
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      int s = 0, m = 0;
      for (int v = y * d; v < (y + 1) * d; ++v) {
        for (int u = x * d; u < (x + 1) * d; ++u) {
          i.get(u, v, c);
          s += c.rgba[0];
          m += 255;
        }
      }
      float r = 1.0f + l * s / m;
      g.set(x, y, r);
    }
  }
}

static void occupy_path_cells(Grid &g, Path &p) {
  const float s = g_options->cross_cost;
  foreach (const Tile &t, p.tiles) {
    float c = g.get(t);
    g.set(t, std::max(c, s));
  }
}

static Tile tile_to_cell(const Tile &t) {
  const int &d = g_options->cell_size;
  return Tile(t.x / d, t.y / d);
}

static Tile cell_to_tile(const Tile &t) {
  const int &d = g_options->cell_size;
  return Tile(t.x * d + d / 2, t.y * d + d / 2);
}

static void find_paths(const Routes &l, Grid &g, Paths &v) {
  Format f;
  for (size_t i = 0; i < l.size(); ++i) {
    const Route &r = l[i];
    Tile s = tile_to_cell(r.start);
    Tile e = tile_to_cell(r.end);
    Path &p = v[i];
    if (!p.find(g, s, e)) {
      f = "No path found for route {}: {}";
      warn(f.bind(i, r));
      continue;
    }
    f = "Found path of length {} cells for route {}: {}";
    info(f.bind(p.length(), i, r));
    occupy_path_cells(g, p);
  }
}

static void draw_routes(Brush &b, const Routes &l, const Paths &v) {
  b.width(g_options->line_width);
  const Numbers &d = g_options->dashes;
  if (!d.empty())
    b.dashes(d);

  for (size_t i = 0; i < l.size(); ++i) {
    const Route &r = l[i];
    const Path &p = v[i];
    const Tiles &c = p.tiles;
    if (c.empty())
      continue;

    Tiles t;
    t.push_back(r.start);
    for (size_t j = 1; j < c.size() - 1; ++j)
      t.push_back(cell_to_tile(c[j]));
    t.push_back(r.end);

    b.color(Color::palette(i));
    if (g_options->use_lines)
      b.lines(t);
    else
      b.curve(t);
  }

  if (g_options->draw_anchors) {
    for (size_t i = 0; i < l.size(); ++i) {
      b.color(Color::palette(i));
      const Route &r = l[i];
      b.anchor(r.start);
      b.anchor(r.end);
    }
  }
}

static void save_output(Image &i) {
  const std::string &p = g_options->output_path;
  i.save(p);
  Format f = "Wrote PNG image '{}'";
  report(f.bind(p));
}

static void process_routes() {
  const Strings &a = g_options->arguments;
  if (a.size() < 1) {
    const char *e = "Missing image argument";
    throw std::runtime_error(e);
  }

  Image m(a[0]);
  Grid g;
  fill_cells(g, m);

  Routes l;
  read_routes(l);

  Paths v(l.size());
  find_paths(l, g, v);

  Image o(m.width(), m.height());
  Image &i = g_options->overlay ? m : o;
  Brush b(i);
  draw_routes(b, l, v);

  save_output(i);
}

int main(int argc, char **argv) {
  try {
    g_options->parse(argc, argv);
    process_routes();
  } catch (std::exception &e) {
    error(e.what());
    return 1;
  }
  return 0;
}
