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
#include "mark.hpp"

#include "cache.hpp"
#include "utility.hpp"

Mark::Mark(const sf::Vector2f &l):
  local(l),
  m_selected(false),
  m_style(POINT),
  m_side(RIGHT),
  m_sprite(),
  m_text(),
  m_outline()
{
  setup_sprite();
  sf::Text &t = m_text;
  if (sf::Font *f = font()) {
    t.setFont(*f);
    t.setCharacterSize(16);
  }
  sf::RectangleShape &r = m_outline;
  r.setFillColor(sf::Color::Transparent);
  r.setOutlineColor(sf::Color::Green);
  r.setOutlineThickness(1);
}

sf::Texture *Mark::texture() const {
  const char *n = "mark";
  if (m_style == TARGET)
    n = "target";
  return g_cache->get_texture(n);
}

sf::Font *Mark::font() const {
  return g_cache->get_font("default");
}

void Mark::layout(const sf::Vector2f &p) {
  m_sprite.setPosition(p);
  reposition_label();
}

bool Mark::contains(const sf::Vector2f &p) const {
  sf::FloatRect b = m_sprite.getGlobalBounds();
  return b.contains(p);
}

static void draw_outlined(sf::RenderTarget &r, sf::Text &t) {
  static float d[][2] = {
    {-1, -1}, {1, -1}, {-1, 1}, {1, 1}
  };
  sf::Vector2f o = t.getPosition();
  t.setColor(sf::Color::Black);
  for (size_t i = 0; i < COUNTOF(d); ++i) {
    sf::Vector2f p(o.x + d[i][0], o.y + d[i][1]);
    t.setPosition(p);
    r.draw(t);
  }
  t.setPosition(o);
  t.setColor(sf::Color::White);
  r.draw(t);
}

void Mark::draw(sf::RenderTarget &r) {
  r.draw(m_sprite);
  if (m_style == TARGET)
    return;
  sf::Text &t = m_text;
  draw_outlined(r, t);
  const sf::String &s = t.getString();
  if (m_selected && !s.isEmpty())
    r.draw(m_outline);
}

void Mark::select(bool b) {
  typedef sf::Color C;
  const C &c = b ? C::Green : C::White;
  m_sprite.setColor(c);
  m_selected = b;
}

void Mark::edit(const sf::String &s) {
  m_text.setString(s);
  reposition_label();
}

void Mark::reposition_label() {
  sf::Text &t = m_text;
  const sf::Font *f = t.getFont();
  size_t c = t.getCharacterSize();
  float h = f->getLineSpacing(c);
  sf::FloatRect b, g;
  b = t.getLocalBounds();
  g = m_sprite.getGlobalBounds();
  sf::Vector2f p(g.left, g.top);
  p.x += m_side == LEFT ? -b.width : g.width;
  p.y -= 0.5f * (h - g.height);
  t.setPosition(p);

  sf::RectangleShape &o = m_outline;
  sf::Vector2f s(b.width, h);
  o.setSize(s);
  o.setPosition(p);
}

void Mark::style(Style s) {
  m_style = s;
  setup_sprite();
}

void Mark::setup_sprite() {
  sf::Sprite &s = m_sprite;
  sf::Texture *t = texture();
  if (!t)
    return;
  s.setTexture(*t, true);
  sf::Vector2u e = t->getSize();
  s.setOrigin(0.5f * e.x, 0.5f * e.y);
}

float Mark::width() const {
  sf::FloatRect a, b;
  a = m_sprite.getLocalBounds();
  b = m_outline.getLocalBounds();
  return a.width + b.width;
}

void Mark::side(Side s) {
  m_side = s;
  reposition_label();
}
