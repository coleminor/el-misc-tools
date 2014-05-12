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
#ifndef ELM_ANNOTATE_WIDGET_HPP
#define ELM_ANNOTATE_WIDGET_HPP

#include "sender.hpp"
#include "utility.hpp"

#include <SFML/Graphics.hpp>

class Widget : public Sender {
  DISALLOW_COPY_AND_ASSIGNMENT(Widget);
public:
  Widget();
  virtual ~Widget() {}
  void allocate(const sf::FloatRect &r);
  void set_parent(Widget *w);
  void align(float x, float y);
  bool get_visible() const;
  void set_visible(bool);
  void draw(sf::RenderTarget &);
  sf::Vector2f size();
  bool handle(const sf::Event &);

  virtual void do_draw(sf::RenderTarget &) = 0;
  virtual sf::Vector2f do_size() = 0;
  virtual bool do_handle(const sf::Event &);

protected:
  virtual void do_layout() = 0;

  void reallocate();
  bool contains(const sf::Event &);

  Widget *m_parent;
  sf::FloatRect m_allocation;
  sf::Vector2f m_alignment;
  bool m_visible;
};

#endif // ELM_ANNOTATE_WIDGET_HPP
