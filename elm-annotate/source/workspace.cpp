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
#include "workspace.hpp"

#include "format.hpp"
#include "note.hpp"
#include "utility.hpp"

#include <SFML/Graphics/Image.hpp>
#include <SFML/System/Utf.hpp>

#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstring>

typedef std::basic_string<sf::Uint32> string32;

static bool parse_tile(const char *s, sf::Vector2i &t) {
  return 2 == sscanf(s, "%d,%d", &t.x, &t.y);
}

static void chomp(string32 &s) {
  static const sf::Uint32 w[] = {
    '\n', '\r', 0
  };
  size_t p = s.find_first_of(w);
  if (p != string32::npos)
    s.erase(p);
}

static sf::String parse_label(const char *s) {
  while (*s && !isblank(*s))
    s++;
  while (*s && isblank(*s))
    s++;
  const char *e = s + strlen(s);
  string32 t;
  sf::Utf8::toUtf32(s, e, std::back_inserter(t));
  chomp(t);
  sf::String r(t);
  return r;
}

static std::string to_utf8(const sf::String &s) {
  std::string r;
  sf::Utf32::toUtf8(s.begin(), s.end(),
                    std::back_inserter(r));
  return r;
}

Workspace::Workspace(const std::string &p):
  error(),
  map(),
  notes(),
  m_filename(p),
  m_note_path(),
  m_saved(true),
  m_have_map(false)
{
  Store &s = notes;
  typedef Workspace W;
  s.link("note-added", this, &W::on_note_changed);
  s.link("note-removed", this, &W::on_note_changed);
  s.link("note-edited", this, &W::on_note_changed);
  s.link("note-moved", this, &W::on_note_changed);
}

void Workspace::set_note_path(const std::string &p) {
  m_note_path = p;
}

bool Workspace::load() {
  const std::string &p = m_filename;
  Format f;
  if (Map::match_extension(p)) {
    if (!map.load(p)) {
      f = "Failed to load map \"{}\": {}";
      error = f.bind(p, map.error());
      return false;
    }
    m_have_map = true;
  } else {
    sf::Image i;
    if (!i.loadFromFile(p)) {
      f = "Could not load \"{}\" as an image";
      error = f.bind(p);
      return false;
    }
    sf::Vector2u s = i.getSize();
    map.set_size(s.x, s.y);
    map.set_image(p);
    m_have_map = false;
  }

  if (!load_notes()) {
    f = "Failed to load notes: {}";
    error = f.bind(error);
    return false;
  }
  m_saved = true;
  return true;
}

bool Workspace::save() {
  if (!save_notes()) {
    Format f = "Failed to save notes: {}";
    error = f.bind(error);
    return false;
  }
  m_saved = true;
  return true;
}

bool Workspace::changed() const {
  return !m_saved;
}

std::string Workspace::notes_file() const {
  std::string p = map.name() + "-notes.txt";
  if (!m_note_path.empty())
    p = m_note_path + '/' + p;
  return p;
}

bool Workspace::load_notes() {
  std::string p = notes_file();
  notes.clear();
  if (!file_exists(p))
    return true;

  FILE *f;
  if (!(f = fopen(p.c_str(), "r"))) {
    const char *e = strerror(errno);
    Format m = "Failed to open \"{}\" for reading: {}";
    error = m.bind(p, e);
    return false;
  }
  SCOPE_FREE_USING(fclose, FILE *, f);

  char b[256];
  while (fgets(b, sizeof b, f)) {
    sf::Vector2i t;
    if (!parse_tile(b, t))
      continue;
    if (!map.contains(t.x, t.y))
      continue;
    sf::Vector2f l = tile_to_local(t);
    sf::String s = parse_label(b);
    notes.add(t, l, s);
  }
  return true;
}

bool Workspace::save_notes() {
  std::string p = notes_file();

  FILE *f;
  if (!(f = fopen(p.c_str(), "w"))) {
    const char *e = strerror(errno);
    Format m = "Failed to open \"{}\": {}";
    error = m.bind(p, e);
    return false;
  }
  SCOPE_FREE_USING(fclose, FILE *, f);

  foreach_value (const Note &n, notes) {
    int x = n.tile.x, y = n.tile.y;
    std::string u = to_utf8(n.text);
    fprintf(f, "%d,%d %s\n", x, y, u.c_str());
  }
  return true;
}

sf::Vector2i Workspace::local_to_tile(const sf::Vector2f &l) {
  sf::Vector2i t;
  t.x = l.x * map.width();
  t.y = (1.0f - l.y) * (map.height() - 1);
  return t;
}

sf::Vector2f Workspace::tile_to_local(const sf::Vector2i &t) {
  float x = t.x, y = t.y;
  sf::Vector2f l;
  l.x = x / map.width();
  l.y = 1.0f - y / (map.height() - 1);
  return l;
}

void Workspace::on_note_changed(Message &) {
  m_saved = false;
}
