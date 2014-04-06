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
#include <vector>
#include "gd.h"

#include "image.hpp"
#include "log.hpp"
#include "region.hpp"

using namespace std;

bool read_image(const string &filename,
                vector<int> &bitmap,
                int &width, int &height) {

  FILE *f;
  const char *fn = filename.c_str();
  logver("going to read image '%s'", fn);
  if (!(f = fopen(fn, "rb"))) {
    logerr("failed to open '%s' for reading: %s", fn, strerror(errno));
    return false;
  }

  gdImagePtr im;
  im = gdImageCreateFromPng(f);
  fclose(f);
  
  width = gdImageSX(im);
  height = gdImageSY(im);
  bitmap.resize(width * height, 0);

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      int c = gdImageGetPixel(im, x, y);
      int p = x + y * width;
      if (gdImageRed(im, c) > 125
          || gdImageGreen(im, c > 125)
          || gdImageBlue(im, c > 125)) {
        bitmap[p] = 1;
      }
    }
  }

  gdImageDestroy(im);
  return true;
}


bool write_image(const string &filename,
                 const vector<int> &bitmap,
                 int width, int height) {

  FILE *f;
  const char *fn = filename.c_str();
  logver("going to write image '%s'", fn);
  if (!(f = fopen(fn, "wb"))) {
    logerr("failed to open '%s' for writing: %s", fn, strerror(errno));
    return 0;
  }

  gdImagePtr im = gdImageCreate(width, height);
  gdImageColorAllocate(im, 0, 0, 0); /* background */
  int white = gdImageColorAllocate(im, 255, 255, 255);

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      int p = x + y * width;
      if (!bitmap[p])
        continue;
      gdImageSetPixel(im, x, y, white);
    }
  }

  gdImagePng(im, f);
  fclose(f);
  gdImageDestroy(im);
  logver("wrote %dx%d PNG image to '%s'", width, height, fn);
  return true;
}

#define N_COLORS (sizeof(color_list)/sizeof(color_list[0]))
static int color_list[][3] = {
  {120, 63, 193},
  {31, 150, 152},
  {255, 172, 253},
  {177, 204, 113},
  {241, 8, 92},
  {255, 255, 203},
  {0, 0, 254},
  {253, 0, 0},
  {0, 253, 0},
  {0, 0, 151},
  {255, 0, 182},
  {0, 183, 0},
  {255, 211, 0},
  {0, 159, 255},
  {154, 77, 66},
};

bool render_image(const string &filename, Region &r) {
  const char *fn = filename.c_str();
  logver("going to render %dx%d region to image '%s'", r.width(), r.height(), fn);

  gdImagePtr im;
  im = gdImageCreate(r.width(), r.height());
  gdImageColorAllocate(im, 0, 0, 0); // background
  int white = gdImageColorAllocate(im, 255, 255, 255);
  int green = gdImageColorAllocate(im, 100, 200, 100);

  vector<int> ca;
  int ci = 0;
  for (unsigned i = 0; i < N_COLORS; i++) {
    int r = color_list[i][0];
    int g = color_list[i][1];
    int b = color_list[i][2];
    ca.push_back(gdImageColorAllocate(im, r, g, b));
  }

  RegionList &regions = r.get_regions();

  for (RegionList::iterator i = regions.begin(); i != regions.end(); ++i) {
    Region *s = *i;
    Point o = s->origin();
    for (int y = 0; y < s->height(); ++y) {
      int v = o.y + y;
      for (int x = 0; x < s->width(); ++x) {
        int u = o.x + x;
        if (s->get(x, y)) 
          gdImageSetPixel(im, u, v, white);
      }
    }
    int u0 = o.x;
    int v0 = o.y;
    int u1 = o.x + s->width() - 1;
    int v1 = o.y + s->height() - 1;
    gdImageRectangle(im, u0, v0, u1, v1, green);

    Cover &cover = s->get_cover();

    for (Cover::iterator b = cover.begin(); b != cover.end(); ++b) {
      gdImageFilledRectangle(im, o.x + b->x0, o.y + b->y0,
                             o.x + b->x1, o.y + b->y1, ca[ci]);
      ci = (ci + 1) % N_COLORS;
    }
  }

  FILE *f;
  if (!(f = fopen(fn, "wb"))) {
    logerr("failed to open '%s' for writing: %s", fn, strerror(errno));
    return false;
  }

  gdImagePng(im, f);
  fclose(f);
  gdImageDestroy(im);

  loginfo("wrote PNG image to '%s'", fn);

  return true;
}
