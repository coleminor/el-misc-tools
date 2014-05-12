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
#include "toplevel.hpp"

#include "defines.hpp"
#include "widget.hpp"

Toplevel::Toplevel():
  m_window(),
  m_child()
{}

Toplevel::~Toplevel() {
  delete m_child;
}

bool Toplevel::initialize() {
  sf::VideoMode m(800, 600);
  m_window.create(m, EXECUTABLE_NAME);

  return true;
}

void Toplevel::shutdown() {
  delete m_child;
  m_child = 0;
}

void Toplevel::run() {
  sf::RenderWindow &w = m_window;
  while (w.isOpen()) {
    sf::Event e;
    w.waitEvent(e);
    handle(e);
    while (w.pollEvent(e))
      handle(e);
    w.clear();
    m_child->draw(w);
    w.display();
  }
}

void Toplevel::handle(const sf::Event &e) {
  typedef sf::Event E;
  E::EventType t = e.type;
  if (t == E::Closed) {
    quit();
  } else if (t == E::Resized) {
    resize(e.size.width, e.size.height);
  } else if (m_child->handle(e)) {
    return;
  }
  if (t == E::KeyPressed) {
    Key m(e.key.code);
    send(m);
  } else if (t == E::TextEntered) {
    Input m(e.text.unicode);
    send(m);
  } else if (t == E::MouseWheelMoved) {
    Wheel m(e.mouseWheel.delta);
    send(m);
  }
}

void Toplevel::resize(size_t w, size_t h) {
  sf::FloatRect r(0, 0, w, h);
  sf::View v(r);
  m_window.setView(v);
  m_child->allocate(r);
}

void Toplevel::set_child(Widget *w) {
  delete m_child;
  m_child = w;
}

void Toplevel::set_title(const sf::String &s) {
  m_window.setTitle(s);
}
