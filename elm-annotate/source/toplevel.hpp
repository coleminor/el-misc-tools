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
#ifndef ELM_ANNOTATE_TOPLEVEL_HPP
#define ELM_ANNOTATE_TOPLEVEL_HPP

#include "sender.hpp"
#include "utility.hpp"

#include <SFML/Graphics.hpp>

class Widget;

class Toplevel : public Sender {
  DISALLOW_COPY_AND_ASSIGNMENT(Toplevel);
public:
  Toplevel();
  ~Toplevel();
  bool initialize();
  void run();
  void shutdown();
  void set_child(Widget *);
  void set_title(const sf::String &);
  void quit() { m_window.close(); }

  struct Input : Message {
    MESSAGE_NAME("toplevel-input");
    sf::Uint32 unicode;
    Input(sf::Uint32 u):unicode(u) {}
  };

  struct Key : Message {
    MESSAGE_NAME("toplevel-key");
    sf::Keyboard::Key code;
    Key(sf::Keyboard::Key c):code(c) {}
  };

  struct Wheel : Message {
    MESSAGE_NAME("toplevel-wheel");
    int delta;
    Wheel(int d):delta(d) {}
  };

private:
  void handle(const sf::Event &);
  void resize(size_t w, size_t h);
  sf::RenderWindow m_window;
  Widget *m_child;
};

#endif // ELM_ANNOTATE_TOPLEVEL_HPP
