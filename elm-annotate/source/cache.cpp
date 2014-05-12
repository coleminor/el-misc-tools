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
#include "cache.hpp"

#include "defines.hpp"
#include "utility.hpp"

#include <cstdio>
#include <string>

static Cache s_cache;
Cache *g_cache = &s_cache;

Cache::Cache():
  m_textures(),
  m_fonts()
{}

Cache::~Cache() {
  shutdown();
}

static std::string find_resource(const std::string &f) {
  static const char *a[] = {
    "./resources/",
    DATA_DIR "/resources/",
    0
  };
  for (const char **p = a; *p; ++p) {
    std::string r = *p + f;
    if (file_exists(r))
      return r;
  }
  printf("Resource file \"%s\" not found\n", f.c_str());
  return f;
}

bool Cache::initialize() {
  if (!get_font("default"))
    return false;
  return true;
}

void Cache::clear_textures() {
  TextureMap &m = m_textures;
  TextureMap::iterator i, e;
  for (i = m.begin(), e = m.end(); i != e; ++i)
    delete i->second;
  m.clear();
}

void Cache::clear_fonts() {
  FontMap &m = m_fonts;
  FontMap::iterator i, e;
  for (i = m.begin(), e = m.end(); i != e; ++i)
    delete i->second;
  m.clear();
}

void Cache::shutdown() {
  clear_textures();
  clear_fonts();
}

sf::Texture *Cache::get_texture(const std::string &n) {
  TextureMap &m = m_textures;
  TextureMap::iterator i = m.find(n);
  if (i != m.end())
    return i->second;

  sf::Texture *t = new sf::Texture();
  Cleaner<sf::Texture *> c(t);
  std::string p = find_resource(n + ".png");
  if (!t->loadFromFile(p))
    return 0;

  m[n] = t;
  c.cancel();
  return t;
}

sf::Font *Cache::get_font(const std::string &n) {
  FontMap &m = m_fonts;
  FontMap::iterator i = m.find(n);
  if (i != m.end())
    return i->second;

  sf::Font *f = new sf::Font();
  Cleaner<sf::Font *> c(f);
  std::string p = find_resource(n + ".ttf");
  if (!f->loadFromFile(p))
    return 0;

  m[n] = f;
  c.cancel();
  return f;
}
