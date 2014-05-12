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
#include "canvas.hpp"

#include "cache.hpp"
#include "foreach.hpp"
#include "mark.hpp"
#include "store.hpp"

#include <algorithm>
#include <cmath>
#include <string>

Canvas::Canvas():
  Widget(),
  show_marks(true),
  m_texture(),
  m_sprite(),
  m_marks(),
  m_store(),
  m_background(),
  m_stretch(false),
  m_mark_style(Mark::POINT),
  m_drag_origin(),
  m_drag_tag()
{
  sf::Color c(20, 30, 50);
  m_background.setFillColor(c);
}

Canvas::~Canvas() {
  detach();
  clear_marks();
}

bool Canvas::load(const std::string &p) {
  if (!m_texture.loadFromFile(p))
    return false;
  m_sprite.setTexture(m_texture, true);
  reallocate();
  return true;
}

void Canvas::attach(Store *s) {
  detach();
  clear_marks();
  m_store = s;
  if (!s)
    return;
  typedef Canvas C;
  s->link("note-added", this, &C::on_note_added);
  s->link("note-edited", this, &C::on_note_edited);
  s->link("note-removed", this, &C::on_note_removed);
  s->link("note-moved", this, &C::on_note_moved);
  foreach_pair (int t, const Note &n, *s)
    add_mark(t, n);
}

void Canvas::detach() {
  if (Store *&s = m_store) {
    s->unlink(this);
    s = 0;
  }
}

void Canvas::do_draw(sf::RenderTarget &r) {
  if (!m_stretch)
    r.draw(m_background);
  r.draw(m_sprite);
  if (show_marks) {
    foreach_value (Mark *m, m_marks)
      m->draw(r);
  }
}

sf::Vector2f Canvas::do_size() {
  sf::Vector2u t = m_texture.getSize();
  return sf::Vector2f(t.x, t.y);
}

void Canvas::do_layout() {
  const sf::FloatRect &a = m_allocation;
  sf::RectangleShape &b = m_background;
  float w = a.width, h = a.height;
  b.setSize(sf::Vector2f(w, h));
  float x = a.left, y = a.top;
  b.setPosition(x, y);

  sf::Vector2u t = m_texture.getSize();
  float tw = t.x, th = t.y;
  if (tw < 1.0f || th < 1.0f)
    return;

  float rw = w / tw, rh = h / th;
  float ox = 0.0f, oy = 0.0f;
  if (m_stretch) {
    m_sprite.setScale(rw, rh);
  } else {
    float r = std::min(rw, rh);
    m_sprite.setScale(r, r);
    const sf::Vector2f &l = m_alignment;
    ox = l.x * (w - r * tw);
    oy = l.y * (h - r * th);
  }
  m_sprite.setPosition(x + ox, y + oy);

  foreach_value (Mark *m, m_marks) {
    m->layout(globalize(m->local));
    check_bounds(m);
  }
}

int Canvas::find_mark_tag_at(const sf::Vector2f &p) {
  foreach_pair (int t, Mark *m, m_marks) {
    if (m->contains(p))
      return t;
  }
  return 0;
}

void Canvas::handle_click(const sf::Vector2f &p,
                          sf::Mouse::Button b) {
  if (int t = find_mark_tag_at(p)) {
    Select s(t, b);
    send(s);
    return;
  }
  sf::Vector2f l = p;
  if (localize(l)) {
    Click c(l, b);
    send(c);
  }
}

void Canvas::handle_drag(const sf::Vector2f &l) {
  if (int t = m_drag_tag) {
    Drag d(t, l);
    send(d);
  }
}

static float distance2(const sf::Vector2f &a,
                       const sf::Vector2f &b) {
  float dx = a.x - b.x;
  float dy = a.y - b.y;
  return dx * dx + dy * dy;
}

bool Canvas::do_handle(const sf::Event &e) {
  typedef sf::Event E;
  E::EventType t = e.type;
  if (t == E::MouseButtonPressed) {
    const E::MouseButtonEvent &u = e.mouseButton;
    sf::Vector2f p(u.x, u.y);
    m_drag_origin = p;
    if (u.button == sf::Mouse::Left)
      m_drag_tag = find_mark_tag_at(p);
  } else if (t == E::MouseButtonReleased) {
    const E::MouseButtonEvent &u = e.mouseButton;
    sf::Vector2f p(u.x, u.y);
    if (distance2(p, m_drag_origin) < 9.0f)
      handle_click(p, u.button);
    m_drag_tag = 0;
  } else if (t == E::MouseMoved) {
    const E::MouseMoveEvent &u = e.mouseMove;
    sf::Vector2f p(u.x, u.y);
    if (localize(p)) {
      handle_drag(p);
      Motion m(p);
      send(m);
    }
  } else {
    return false;
  }
  return true;
}

bool Canvas::localize(sf::Vector2f &p) const {
  sf::FloatRect b = m_sprite.getGlobalBounds();
  float w = b.width, h = b.height;
  if (w < 1.0f || h < 1.0f || !b.contains(p))
    return false;
  p.x = (p.x - b.left) / w;
  p.y = (p.y - b.top) / h;
  return true;
}

sf::Vector2f Canvas::globalize(const sf::Vector2f &l) const {
  sf::FloatRect b = m_sprite.getGlobalBounds();
  sf::Vector2f g;
  g.x = b.width * l.x + b.left;
  g.y = b.height * l.y + b.top;
  return g;
}

bool Canvas::get_stretch() const  {
  return m_stretch;
}

void Canvas::set_stretch(bool b) {
  m_stretch = b;
  do_layout();
}

Mark::Style Canvas::get_mark_style() const {
  return m_mark_style;
}

void Canvas::set_mark_style(Mark::Style s) {
  m_mark_style = s;
  foreach_value (Mark *m, m_marks)
    m->style(s);
}

void Canvas::clear() {
  m_sprite = sf::Sprite();
  m_texture = sf::Texture();
  clear_marks();
}

void Canvas::clear_marks() {
  foreach_value (Mark *m, m_marks)
    delete m;
  m_marks.clear();
}

void Canvas::add_mark(int t, const Note &n) {
  const sf::Vector2f &l = n.local;
  Mark *m = new Mark(l);
  m->edit(n.text);
  m->layout(globalize(l));
  m->style(m_mark_style);
  check_bounds(m);
  m_marks[t] = m;
}

Mark *Canvas::find(int t) {
  MarkMap::iterator i = m_marks.find(t);
  return i == m_marks.end() ? 0 : i->second;
}

void Canvas::on_note_added(Note::Added &a) {
  add_mark(a.tag, a.note);
}

void Canvas::on_note_edited(Note::Edited &e) {
  const Note &n = e.note;
  if (Mark *m = find(e.tag)) {
    m->edit(n.text);
    check_bounds(m);
  }
}

void Canvas::on_note_removed(Note::Removed &r) {
  MarkMap &m = m_marks;
  MarkMap::iterator i = m.find(r.tag);
  if (i != m.end()) {
    delete i->second;
    m.erase(i);
  }
}

void Canvas::select(int t, bool b) {
  if (Mark *m = find(t))
    m->select(b);
}

void Canvas::check_bounds(Mark *m) {
  sf::Vector2f g = globalize(m->local);
  sf::FloatRect b = m_sprite.getGlobalBounds();
  Mark::Side s = Mark::RIGHT;
  if (g.x + m->width() > b.left + b.width)
    s = Mark::LEFT;
  m->side(s);
}

void Canvas::on_note_moved(Note::Moved &e) {
  const Note &n = e.note;
  if (Mark *m = find(e.tag)) {
    const sf::Vector2f &l = n.local;
    m->local = l;
    m->layout(globalize(l));
  }
}
