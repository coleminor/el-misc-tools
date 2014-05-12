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
#ifndef ELM_ANNOTATE_MESSAGE_HPP
#define ELM_ANNOTATE_MESSAGE_HPP

#include "name.hpp"

struct Message {
  virtual ~Message() {}
  virtual const Name &get_name() const = 0;
};

#define MESSAGE_NAME(s) \
  static const Name &name() { \
    static const Name s_name(s); \
    return s_name; \
  } \
  virtual const Name &get_name() const { \
    return name(); \
  }

#endif // ELM_ANNOTATE_MESSAGE_HPP
