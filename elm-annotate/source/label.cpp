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
#include "label.hpp"

#include "cache.hpp"

Label::Label():
  Widget(),
  m_text()
{
  setup();
}

Label::Label(const sf::String &s):
  Widget(),
  m_text()
{
  setup();
  m_text.setString(s);
}

void Label::setup() {
  sf::Font *f = g_cache->get_font("default");
  if (f)
    m_text.setFont(*f);
  m_text.setCharacterSize(24);
}

void Label::do_draw(sf::RenderTarget &r) {
  r.draw(m_text);
}

sf::Vector2f Label::do_size() {
  const sf::Font *f = m_text.getFont();
  size_t c = m_text.getCharacterSize();
  int h = f->getLineSpacing(c);
  sf::FloatRect r(m_text.getLocalBounds());
  return sf::Vector2f(r.width, h);
}

void Label::do_layout() {
  const sf::FloatRect &a = m_allocation;
  sf::FloatRect b = m_text.getLocalBounds();
  const sf::Font *f = m_text.getFont();
  size_t c = m_text.getCharacterSize();
  float h = f->getLineSpacing(c);
  const sf::Vector2f &l = m_alignment;
  float x = l.x * (a.width - b.width);
  float y = l.y * (a.height - h);
  m_text.setPosition(a.left + x, a.top + y);
}

void Label::set_text(const sf::String &s) {
  m_text.setString(s);
  do_layout();
}
