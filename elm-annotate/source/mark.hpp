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
#ifndef ELM_ANNOTATE_MARK_HPP
#define ELM_ANNOTATE_MARK_HPP

#include <SFML/Graphics.hpp>

class Mark {
public:
  enum Style { POINT, TARGET };
  enum Side { LEFT, RIGHT };

  sf::Vector2f local;

  Mark(const sf::Vector2f &l);
  bool contains(const sf::Vector2f &p) const;
  void draw(sf::RenderTarget &);
  void layout(const sf::Vector2f &p);
  void edit(const sf::String &);
  void select(bool);
  void style(Style);
  float width() const;
  void side(Side);
private:
  sf::Texture *texture() const;
  sf::Font *font() const;
  void setup_sprite();
  void reposition_label();

  bool m_selected;
  Style m_style;
  Side m_side;
  sf::Sprite m_sprite;
  sf::Text m_text;
  sf::RectangleShape m_outline;
};

#endif // ELM_ANNOTATE_MARK_HPP
