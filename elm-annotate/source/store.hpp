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
#ifndef ELM_ANNOTATE_STORE_HPP
#define ELM_ANNOTATE_STORE_HPP

#include "sender.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/System/String.hpp>

#include <map>

class Note;

class Store : public Sender {
public:
  Store();
  ~Store() {}
  bool empty() const { return m_notes.empty(); }
  size_t size() const { return m_notes.size(); }
  void clear();
  int add(const sf::Vector2i &tile,
          const sf::Vector2f &local,
          const sf::String &text);
  void edit(int tag, const sf::String &);
  void remove(int tag);
  void move(int tag, const sf::Vector2i &tile,
            const sf::Vector2f &local);
  int next(int tag);

  typedef std::map<int, Note> NoteMap;
  typedef NoteMap::iterator iterator;
  typedef NoteMap::key_type key_type;
  typedef NoteMap::mapped_type mapped_type;

  iterator begin() { return m_notes.begin(); }
  iterator end() { return m_notes.end(); }
  iterator find(int tag) { return m_notes.find(tag); }

private:
  NoteMap m_notes;
  int m_next_tag;
};

#endif // ELM_ANNOTATE_STORE_HPP
