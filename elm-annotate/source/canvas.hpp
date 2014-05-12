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
#ifndef ELM_ANNOTATE_CANVAS_HPP
#define ELM_ANNOTATE_CANVAS_HPP

#include "mark.hpp"
#include "note.hpp"
#include "widget.hpp"
#include "utility.hpp"

#include <map>
#include <string>

class Store;

class Canvas : public Widget {
  DISALLOW_COPY_AND_ASSIGNMENT(Canvas);
public:
  bool show_marks;

  Canvas();
  ~Canvas();
  bool load(const std::string &image);
  void clear();
  void attach(Store *);
  void select(int tag, bool b = true);

  bool get_stretch() const;
  void set_stretch(bool);
  Mark::Style get_mark_style() const;
  void set_mark_style(Mark::Style);

  struct Click : Message {
    MESSAGE_NAME("canvas-click");
    sf::Vector2f local;
    sf::Mouse::Button button;
    Click(const sf::Vector2f &l, sf::Mouse::Button b):
      local(l), button(b)
    {}
  };

  struct Select : Message {
    MESSAGE_NAME("canvas-select");
    int tag;
    sf::Mouse::Button button;
    Select(int t, sf::Mouse::Button b):
      tag(t), button(b)
    {}
  };

  struct Motion : Message {
    MESSAGE_NAME("canvas-motion");
    sf::Vector2f local;
    Motion(const sf::Vector2f &l):local(l) {}
  };

  struct Drag : Message {
    MESSAGE_NAME("canvas-drag");
    int tag;
    sf::Vector2f local;
    Drag(int t, const sf::Vector2f &l):
      tag(t), local(l)
    {}
  };

  virtual void do_draw(sf::RenderTarget &);
  virtual sf::Vector2f do_size();
  virtual bool do_handle(const sf::Event &);

protected:
  typedef std::map<int, Mark *> MarkMap;

  void detach();
  void clear_marks();
  void add_mark(int t, const Note &);
  Mark *find(int tag);
  bool localize(sf::Vector2f &p) const;
  sf::Vector2f globalize(const sf::Vector2f &l) const;
  int find_mark_tag_at(const sf::Vector2f &p);
  void handle_click(const sf::Vector2f &p, sf::Mouse::Button);
  void handle_drag(const sf::Vector2f &p);
  void check_bounds(Mark *m);

  void on_note_added(Note::Added &);
  void on_note_edited(Note::Edited &);
  void on_note_removed(Note::Removed &);
  void on_note_moved(Note::Moved &);

  virtual void do_layout();

  sf::Texture m_texture;
  sf::Sprite m_sprite;
  MarkMap m_marks;
  Store *m_store;
  sf::RectangleShape m_background;
  bool m_stretch;
  Mark::Style m_mark_style;
  sf::Vector2f m_drag_origin;
  int m_drag_tag;
};

#endif // ELM_ANNOTATE_CANVAS_HPP
