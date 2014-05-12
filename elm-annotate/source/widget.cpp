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
#include "widget.hpp"

Widget::Widget():
  Sender(),
  m_parent(),
  m_allocation(),
  m_alignment(),
  m_visible(true)
{}

void Widget::reallocate() {
  if (m_parent)
    m_parent->reallocate();
  else
    do_layout();
}

bool Widget::contains(const sf::Event &e) {
  typedef sf::Event E;
  E::EventType t = e.type;
  const sf::FloatRect &a = m_allocation;
  if (t == E::MouseMoved) {
    sf::Vector2f p(e.mouseMove.x, e.mouseMove.y);
    return a.contains(p);
  } else if (t == E::MouseButtonPressed
             || t == E::MouseButtonReleased) {
    sf::Vector2f p(e.mouseButton.x, e.mouseButton.y);
    return a.contains(p);
  }
  return true;
}

void Widget::align(float x, float y) {
  sf::Vector2f &l = m_alignment;
  l.x = x;
  l.y = y;
  do_layout();
}

void Widget::allocate(const sf::FloatRect &r) {
  m_allocation = r;
  do_layout();
}

void Widget::set_parent(Widget *w) {
  m_parent = w;
}

bool Widget::handle(const sf::Event &e) {
  if (!m_visible || !contains(e))
    return false;
  return do_handle(e);
}

bool Widget::do_handle(const sf::Event &) {
  return false;
}

bool Widget::get_visible() const {
  return m_visible;
}

void Widget::set_visible(bool b) {
  if (m_visible == b)
    return;
  m_visible = b;
  reallocate();
}

void Widget::draw(sf::RenderTarget &r) {
  if (m_visible)
    do_draw(r);
}

sf::Vector2f Widget::size() {
  return m_visible ? do_size() : sf::Vector2f();
}
