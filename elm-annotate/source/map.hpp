//
//  Copyright (C) 2014 Cole Minor
//  This file is part of elm-annotate
//
//  elm-annotate is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  elm-annotate is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef ELM_ANNOTATE_MAP_HPP
#define ELM_ANNOTATE_MAP_HPP

#include <string>

class Map {
public:
  Map();
  bool load(const std::string &path);
  const std::string &error() const { return m_error; }
  int width() const { return m_width; }
  int height() const { return m_height; }
  const std::string &name() const { return m_name; }
  const std::string &image() const { return m_image; }
  bool contains(int x, int y) const {
    return 0 <= x && x < m_width && 0 <= y && y < m_height;
  }
  void set_size(int width, int height);
  void set_image(const std::string &);

  static bool match_extension(const std::string &p);
private:
  std::string m_name;
  std::string m_error;
  std::string m_image;
  int m_width;
  int m_height;
};

#endif // ELM_ANNOTATE_MAP_HPP
