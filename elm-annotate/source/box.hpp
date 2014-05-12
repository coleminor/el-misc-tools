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
#ifndef ELM_ANNOTATE_BOX_HPP
#define ELM_ANNOTATE_BOX_HPP

#include "widget.hpp"

enum Orientation {
  VERTICAL,
  HORIZONTAL
};

enum Packing {
  SHRINK,
  EXPAND
};

class Box : public Widget {
public:
  Box(Orientation o = HORIZONTAL);
  virtual ~Box();
  void add(Widget *w, Packing p = SHRINK);

  virtual void do_draw(sf::RenderTarget &);
  virtual bool do_handle(const sf::Event &);
  virtual sf::Vector2f do_size();

protected:
  virtual void do_layout();

  struct Child;
  typedef std::vector<Child> Children;
  Children m_children;
  bool m_horizontal;
};

#endif // ELM_ANNOTATE_BOX_HPP
