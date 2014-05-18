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
#include "box.hpp"

#include "foreach.hpp"

struct Box::Child {
  Widget *widget;
  Packing packing;
  size_t space;
  Child(Widget *w, Packing p):
    widget(w),
    packing(p),
    space(0)
  {}
};

Box::Box(Orientation o):
  Widget(),
  m_children(),
  m_horizontal(o == HORIZONTAL)
{}

Box::~Box() {
  foreach (Child &c, m_children)
    delete c.widget;
}

void Box::add(Widget *w, Packing p) {
  w->set_parent(this);
  m_children.push_back(Child(w, p));
}

void Box::do_draw(sf::RenderTarget &r) {
  foreach (Child &c, m_children)
    c.widget->draw(r);
}

bool Box::do_handle(const sf::Event &e) {
  foreach (Child &c, m_children)
    if (c.widget->handle(e))
      return true;
  return false;
}

sf::Vector2f Box::do_size() {
  sf::Vector2f s, r;
  foreach (Child &c, m_children) {
    r = c.widget->size();
    if (m_horizontal) {
      s.x += r.x;
      s.y = std::max(s.y, r.y);
    } else {
      s.x = std::max(s.x, r.x);
      s.y += r.y;
    }
  }
  return s;
}
 
struct Space {
  size_t position;
  float amount;
  bool visible;
  Space(size_t p, float a, bool v):
    position(p), amount(a), visible(v)
  {}
};

inline bool operator<(const Space &a, const Space &b) {
  return a.amount < b.amount;
}

void Box::do_layout() {
  const bool &h = m_horizontal;
  std::vector<Space> v;
  sf::Vector2f r;
  for (size_t i = 0; i < m_children.size(); ++i) {
    Child &c = m_children[i];
    float a = 0.0f;
    Widget *w = c.widget;
    bool u = w->get_visible();
    if (u) {
      r = w->size();
      a = h ? r.x : r.y;
    }
    v.push_back(Space(i, a, u));
  }
  std::sort(v.begin(), v.end());

  const sf::FloatRect &a = m_allocation;
  float u = h ? a.width : a.height;
  size_t e = 0;
  foreach (Space &s, v) {
    if (!s.visible)
      continue;
    Child &c = m_children[s.position];
    if (c.packing == SHRINK) {
      c.space = std::min(u, s.amount);
      u = std::max(0.0f, u - c.space);
    } else {
      e++;
    }
  }

  float d = e ? u / e : 0.0f;
  foreach (Space &s, v) {
    Child &c = m_children[s.position];
    if (c.packing == EXPAND && s.visible)
      c.space = d;
  }

  sf::FloatRect t(a);
  float &s = h ? t.width : t.height;
  float &o = h ? t.left : t.top;
  foreach (Child &c, m_children) {
    s = c.space;
    c.widget->allocate(t);
    o += s;
  }
}
