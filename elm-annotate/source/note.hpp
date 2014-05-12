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
#ifndef ELM_ANNOTATE_NOTE_HPP
#define ELM_ANNOTATE_NOTE_HPP

#include "message.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/System/String.hpp>

struct Note {
  sf::Vector2i tile;
  sf::Vector2f local;
  sf::String text;
  Note(const sf::Vector2i &v,
       const sf::Vector2f &l,
       const sf::String &s):
    tile(v), local(l), text(s)
  {}

  struct Added : Message {
    MESSAGE_NAME("note-added");
    const int tag;
    const Note &note;
    Added(int t, const Note &n):
      tag(t), note(n)
    {}
  };

  struct Edited : Message {
    MESSAGE_NAME("note-edited");
    const int tag;
    const Note &note;
    Edited(int t, const Note &n):
      tag(t), note(n)
    {}
  };

  struct Removed : Message {
    MESSAGE_NAME("note-removed");
    const int tag;
    Removed(int t):tag(t) {}
  };

  struct Moved : Message {
    MESSAGE_NAME("note-moved");
    const int tag;
    const Note &note;
    Moved(int t, const Note &n):
      tag(t), note(n)
    {}
  };
};

#endif // ELM_ANNOTATE_NOTE_HPP
