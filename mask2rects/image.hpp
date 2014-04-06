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
#ifndef MASK2RECTS_IMAGE_HPP
#define MASK2RECTS_IMAGE_HPP

#include <string>
#include <vector>

bool read_image(const std::string &file,
                std::vector<int> &bitmap,
                int &width, int &height);

bool write_image(const std::string &file,
                 const std::vector<int> &bitmap,
                 int width, int height);

class Region;

bool render_image(const std::string &file, Region &r);


#endif // MASK2RECTS_IMAGE_HPP
