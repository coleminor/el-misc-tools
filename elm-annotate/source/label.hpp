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
#ifndef ELM_ANNOTATE_LABEL_HPP
#define ELM_ANNOTATE_LABEL_HPP

#include "widget.hpp"

class Label : public Widget {
public:
  Label();
  Label(const sf::String &);
  void set_text(const sf::String &);

  virtual void do_draw(sf::RenderTarget &);
  virtual sf::Vector2f do_size();
protected:
  virtual void do_layout();
  void setup();

  sf::Text m_text;
};

#endif // ELM_ANNOTATE_LABEL_HPP
