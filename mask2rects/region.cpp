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
#include <algorithm>
#include <map>

#include "box.hpp"
#include "image.hpp"
#include "label.hpp"
#include "log.hpp"
#include "options.hpp"
#include "random.hpp"
#include "region.hpp"

using namespace std;

typedef map<int, Box> BoxMap;
typedef map<int, Label *> LabelMap;

Region::Region(int w, int h)
  :m_width(w), m_height(h), m_size(w * h), m_data(m_size, 0) {
}

void Region::clear() {
  m_data.clear();
  m_width = 0;
  m_height = 0;
  m_size = 0;
  m_origin = Point();
}

void Region::get_near_values(int x, int y, const vector<int> &values,
                             vector<int> &results) const {
  vector<int> positions;
  if (x > 0)
    positions.push_back(x - 1 + y * m_width);
  if (y > 0)
    positions.push_back(x + (y - 1) * m_width);

  for (unsigned i = 0; i < positions.size(); ++i) {
    int p = positions[i];
    if (m_data[p]) {
      int v = values[p];
      if (v)
        results.push_back(v);
    }
  }
}

bool Region::read(const string &filename) {
  clear();
  if (!read_image(filename, m_data, m_width, m_height))
    return false;

  m_size = m_width * m_height;
  logver("region is %dx%d (%d total)", m_width, m_height, m_size);
  return true;
}

bool Region::write(const string &filename) {
  if (!write_image(filename, m_data, m_width, m_height))
    return false;
  return true;
}

// a.k.a connected-component labeling
void Region::calculate_connected_regions() {
  logver("calculating connected regions");
  clear_regions();

  vector<int> values(m_size, 0);
  LabelMap labels;
  int next_label_value = 1;

  logver("labeling foreground points");
  for (int y = 0; y < m_height; ++y) {
    for (int x = 0; x < m_width; ++x) {
      int p = x + y * m_width;
      if (!m_data[p])
        continue;

      vector<int> near_values;
      get_near_values(x, y, values, near_values);

      if (near_values.empty()) {
        int v = next_label_value++;
        labels[v] = new Label(v);
        values[p] = v;
        continue;
      }

      int v = *min_element(near_values.begin(), near_values.end());
      Label *l = labels[v];
      Label *r = l->root();

      for (unsigned i = 0; i < near_values.size(); i++) {
        Label *n = labels[near_values[i]];
        if (r->value() != n->root()->value()) {
          n->join(l);
        }
      }
      values[p] = v;
    }
  }

  logver("reducing labels");
  for (int p = 0; p < m_size; ++p) {
    int v = values[p];
    if (!v)
      continue;
    int m = labels[v]->root()->value();
    if (m < v)
      values[p] = m;
  }

  for (LabelMap::iterator i = labels.begin(); i != labels.end(); ++i) {
    delete i->second;
  }

  BoxMap boxes;
  
  logver("finding bounding boxes");
  for (int y = 0; y < m_height; ++y) {
    for (int x = 0; x < m_width; ++x) {
      int p = x + y * m_width;
      int v = values[p];
      if (!v)
        continue;

      BoxMap::iterator i = boxes.find(v);
      if (i == boxes.end()) {
        boxes[v] = Box(x, y);
      } else {
        Box &b = i->second;
        b.add(x, y);
      }
    }
  }

  logver("copying connected sub-regions");
  for (BoxMap::iterator i = boxes.begin(); i != boxes.end(); ++i) {
    int v = i->first;
    Box &b = i->second;
    Region *r = new Region(b.width(), b.height());
    r->m_origin = b.origin();
    for (int y = 0; y < b.height(); ++y) {
      for (int x = 0; x < b.width(); ++x) {
        int p = x + b.x0 + (y + b.y0) * m_width;
        int o = x + y * b.width();
        if (values[p] == v)
          r->m_data[o] = m_data[p];
      }
    }
    m_regions.push_back(r);
  }

  logver("found %u connected regions", m_regions.size());
}

void Region::get_uncovered(const vector<int> &values, vector<Point> &results) {
  for (int y = 0; y < m_height; ++y) {
    for (int x = 0; x < m_width; ++x) {
      int p = x + y * m_width;
      if (values[p] == 1)
        results.push_back(Point(x, y));
    }
  }
}

int Region::grow_box(Box &b, const vector<int> &values, int d) {
  int x0 = b.x0, y0 = b.y0, x1 = b.x1, y1 = b.y1;
  int s, e, a;
  switch (d) {
  case D_SOUTH:
    if (y0 < 1)
      return 0;
    s = x0 + (y0 - 1) * m_width;
    e = x1 + (y0 - 1) * m_width;
    a = 1;
    break;
  case D_WEST:
    if (x0 < 1)
      return 0;
    s = (x0 - 1) + y0 * m_width;
    e = (x0 - 1) + y1 * m_width;
    a = m_width;
    break;
  case D_NORTH:
    if (y1 >= m_height - 1)
      return 0;
    s = x0 + (y1 + 1) * m_width;
    e = x1 + (y1 + 1) * m_width;
    a = 1;
    break;
  case D_EAST:
    if (x1 >= m_width - 1)
      return 0;
    s = (x1 + 1) + y0 * m_width;
    e = (x1 + 1) + y1 * m_width;
    a = m_width;
    break;
  default:
    return 0;
  }

  for (int i = s; i <= e; i += a) {
    if (values[i] != 1)
      return 0;
  }
  b.expand(d);
  return 1;
}

void Region::calculate_cover() {
  logver("calculating cover for %dx%d region", m_width, m_height);
  m_cover.clear();
  size_t uncovered_count = m_size;

  for (size_t i = 0; i < opts.maximum_iterations; ++i) {
    Cover c;
    size_t r = m_size;
    vector<int> values(m_data);

    while (c.size() < opts.maximum_box_count) {
      vector<Point> u;
      get_uncovered(values, u);
      r = u.size();
      if (r <= opts.maximum_uncovered_points)
        break;

      Point p = u[random_int(r)];
      Box b(p);
      int can_grow[4] = { 1, 1, 1, 1 }, grew = 0;
      do {
        grew = 0;
        for (int d = 0; d < 4; d++) {
          if (can_grow[d]) {
            if (grow_box(b, values, d))
              grew++;
            else
              can_grow[d] = 0;
          }
        }
      } while (grew);
        
      for (int y = b.y0; y <= b.y1; ++y) {
        for (int x = b.x0; x <= b.x1; ++x) {
          int p = x + y * m_width;
          values[p] = 2;
          r--;
        }
      }

      if (b.area() >= opts.minimum_box_area)
        c.push_back(b);
    }
    logver("iteration %u: cover found has %u rectangles"
           " (%u uncovered points)", i, c.size(), r);
    if (m_cover.empty() || c.size() < m_cover.size()
        || (c.size() == m_cover.size() && r < uncovered_count)) {
      m_cover = c;
      uncovered_count = r;
    }
  }
  logver("final cover has %u rectangles and %u uncovered points",
         m_cover.size(), uncovered_count);
}

void Region::clear_regions() {
  for (RegionList::iterator i = m_regions.begin(); i != m_regions.end(); i++) {
    delete *i;
  }
  m_regions.clear();
}

Region::~Region() {
  clear_regions();
}
