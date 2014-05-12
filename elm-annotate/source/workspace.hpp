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
#ifndef ELM_ANNOTATE_WORKSPACE_HPP
#define ELM_ANNOTATE_WORKSPACE_HPP

#include "map.hpp"
#include "store.hpp"

#include <SFML/System/Vector2.hpp>

#include <string>

struct Workspace {
  std::string error;
  Map map;
  Store notes;

  Workspace(const std::string &filename);
  void set_note_path(const std::string &);
  bool load();
  bool save();
  bool changed() const;
  bool loaded_map() const { return m_have_map; }
  sf::Vector2i local_to_tile(const sf::Vector2f &l);
  sf::Vector2f tile_to_local(const sf::Vector2i &t);
private:
  std::string notes_file() const;
  void on_note_changed(Message &);
  bool load_notes();
  bool save_notes();

  std::string m_filename;
  std::string m_note_path;
  bool m_saved;
  bool m_have_map;
};

#endif // ELM_ANNOTATE_WORKSPACE_HPP
