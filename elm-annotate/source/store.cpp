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
#include "store.hpp"

#include "foreach.hpp"
#include "note.hpp"

Store::Store():
  Sender(),
  m_notes(),
  m_next_tag(1)
{}

int Store::add(const sf::Vector2i &v,
               const sf::Vector2f &l,
               const sf::String &s) {
  int t = m_next_tag++;
  NoteMap::value_type p(t, Note(v, l, s));
  m_notes.insert(p);
  Note::Added m(t, p.second);
  send(m);
  return t;
}

void Store::edit(int t, const sf::String &s) {
  iterator i = find(t);
  if (i == end())
    return;
  Note &n = i->second;
  n.text = s;
  Note::Edited m(t, n);
  send(m);
}

void Store::remove(int t) {
  if (!m_notes.erase(t))
    return;
  Note::Removed m(t);
  send(m);
}

void Store::clear() {
  foreach_key (int t, m_notes) {
    Note::Removed m(t);
    send(m);
  }
  m_notes.clear();
}

int Store::next(int t) {
  iterator i, e = end();
  if (!t) {
    i = begin();
    return i == e ? 0 : i->first;
  }
  i = find(t);
  if (i == e)
    return 0;
  ++i;
  if (i == e)
    i = begin();
  return i->first;
}

void Store::move(int t, const sf::Vector2i &v,
                 const sf::Vector2f &l) {
  iterator i = find(t);
  if (i == end())
    return;
  Note &n = i->second;
  if (n.tile == v)
    return;
  n.tile = v;
  n.local = l;
  Note::Moved m(t, n);
  send(m);
}
