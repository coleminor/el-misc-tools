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
#ifndef ELM_ANNOTATE_APPLICATION_HPP
#define ELM_ANNOTATE_APPLICATION_HPP

#include "canvas.hpp"
#include "toplevel.hpp"
#include "utility.hpp"

#include <string>

class Label;
class Options;
class Workspace;

class Application {
  DISALLOW_COPY_AND_ASSIGNMENT(Application);
public:
  Application();
  bool initialize(Options &);
  void run();
  void shutdown();
private:
  void setup_widgets();
  void clear();
  Workspace *workspace();
  void add_workspace(const std::string &);
  void change_workspace(int offset);
  bool load_workspace();
  bool save_workspace();
  void toggle_stretch();
  void toggle_reference();
  void toggle_marks();
  void report(const char *);
  void report(const std::string &);
  void report_load();
  void focus(int tag);
  void clear_focus();
  void next_note();
  void next_empty();
  void update_position(const sf::Vector2f &);
  void update_title();

  void on_canvas_click(Canvas::Click &);
  void on_canvas_select(Canvas::Select &);
  void on_canvas_motion(Canvas::Motion &);
  void on_canvas_drag(Canvas::Drag &);
  void on_toplevel_key(Toplevel::Key &);
  void on_toplevel_input(Toplevel::Input &);

  typedef std::vector<Workspace *> Workspaces;

  Toplevel m_toplevel;
  Workspaces m_workspaces;
  size_t m_current;
  Canvas *m_map_canvas;
  Label *m_status_label;
  Label *m_position_label;
  bool m_compare;
  std::string m_reference_path;
  Canvas *m_reference_canvas;
  int m_focus;
};

#endif // ELM_ANNOTATE_APPLICATION_HPP
