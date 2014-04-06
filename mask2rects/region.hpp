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
#ifndef MASK2RECTS_REGION_HPP
#define MASK2RECTS_REGION_HPP

#include <list>
#include <string>
#include <vector>

#include "box.hpp"
#include "point.hpp"

typedef std::list<Box> Cover;

class Region;
typedef std::list<Region *> RegionList;

class Region {
private:
  int m_width, m_height, m_size;
  std::vector<int> m_data;
  Point m_origin;
  RegionList m_regions;
  Cover m_cover;

  void get_near_values(int x, int y, const std::vector<int> &values,
                       std::vector<int> &results) const; 
  void get_uncovered(const std::vector<int> &values,
                     std::vector<Point> &results);
  int grow_box(Box &b, const std::vector<int> &values, int d);
  void clear_regions();

public:
  Region() {}
  Region(int w, int h);
  ~Region();

  int width() const { return m_width; }
  int height() const { return m_height; }
  Point origin() const { return m_origin; }
  int get(int x, int y) const { return m_data[x + y * m_width]; }
  void clear();
  bool read(const std::string &filename);
  bool write(const std::string &filename);

  void calculate_connected_regions();
  RegionList& get_regions() { return m_regions; }

  void calculate_cover();
  Cover& get_cover() { return m_cover; }
};

#endif // MASK2RECTS_REGION_HPP
