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
#ifndef ELM_ANNOTATE_CACHE_HPP
#define ELM_ANNOTATE_CACHE_HPP

#include <SFML/Graphics.hpp>

#include <map>
#include <string>

class Cache {
public:
  Cache();
  ~Cache();
  bool initialize();
  void shutdown();
  sf::Texture *get_texture(const std::string &n);
  sf::Font *get_font(const std::string &n);
private:
  void clear_textures();
  void clear_fonts();
  typedef std::map<std::string, sf::Texture *> TextureMap;
  TextureMap m_textures;
  typedef std::map<std::string, sf::Font *> FontMap;
  FontMap m_fonts;
};

extern Cache *g_cache;

#endif // ELM_ANNOTATE_CACHE_HPP
