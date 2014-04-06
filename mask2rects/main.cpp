//
// Copyright 2013 Cole Minor <c.minor@inbox.com>
//
//    This file is part of mask2rects.
//
//    mask2rects is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    mask2rects is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with mask2rects.  If not, see <http://www.gnu.org/licenses/>.
//
#include <cerrno>
#include <cstdio>
#include <cstring>

#include "image.hpp"
#include "log.hpp"
#include "options.hpp"
#include "random.hpp"
#include "region.hpp"

using namespace std;

int main(int argc, char **argv) {
  if (!parse_command_line(argc, argv))
    return 1;

  if (opts.arguments.size() < 1) {
    logerr("missing input image argument");
    return 1;
  }

  initialize_rng(opts.random_seed);

  Region r;
  if (!r.read(opts.arguments[0]))
    return 1;
  
  r.calculate_connected_regions();
  RegionList &regions = r.get_regions();

  FILE *f;
  const char *fn = opts.output_rects_file.c_str();
  if (!(f = fopen(fn, "w"))) {
    logerr("failed to open '%s' for writing: %s", fn, strerror(errno));
    return 1;
  }

  if (opts.flip_rectangle_y_coordinates)
    loginfo("Flipping y coordinates in output to '%s'", fn);

  int h = r.height();
  for (RegionList::iterator i = regions.begin(); i != regions.end(); ++i) {
    Region *s = *i;
    Point o = s->origin();

    s->calculate_cover();
    Cover& cover = s->get_cover();

    for (Cover::iterator b = cover.begin(); b != cover.end(); ++b) {
      int x0 = o.x + b->x0;
      int y0 = o.y + b->y0;
      int x1 = o.x + b->x1;
      int y1 = o.y + b->y1;
      if (opts.flip_rectangle_y_coordinates) {
        int t = y0;
        y0 = h - 1 - y1;
        y1 = h - 1 - t;
      }
      fprintf(f, "%d,%d,%d,%d\n", x0, y0, x1, y1);
    }
  }
  fclose(f);
  loginfo("wrote rectangle bounds to '%s'", fn);

  if (opts.output_image_file.size())
    render_image(opts.output_image_file, r);

  logver("all done");
  return 0;
}
