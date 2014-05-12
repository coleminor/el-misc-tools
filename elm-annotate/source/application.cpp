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
#include "application.hpp"

#include "box.hpp"
#include "cache.hpp"
#include "canvas.hpp"
#include "defines.hpp"
#include "foreach.hpp"
#include "format.hpp"
#include "toplevel.hpp"
#include "label.hpp"
#include "options.hpp"
#include "utility.hpp"
#include "workspace.hpp"

#include <algorithm>
#include <cstdio>

using std::string;

Application::Application():
  m_toplevel(),
  m_workspaces(),
  m_current(),
  m_map_canvas(),
  m_status_label(),
  m_position_label(),
  m_compare(false),
  m_reference_path(),
  m_reference_canvas(),
  m_focus()
{}

bool Application::initialize(Options &o) {
  m_current = 0;
  m_compare = o.compare;
  m_reference_path = o.reference_path;
  foreach (string &s, o.arguments)
    add_workspace(s);
  foreach (Workspace *w, m_workspaces)
    w->set_note_path(o.notes_path);

  if (m_workspaces.empty()) {
    report("No maps found");
    return false;
  }

  if (!g_cache->initialize())
    return false;
  if (!m_toplevel.initialize())
    return false;
  setup_widgets();
  load_workspace();
  return true;
}

void Application::add_workspace(const string &p) {
  Workspaces &v = m_workspaces;
  if (!is_directory(p)) {
    v.push_back(new Workspace(p));
    return;
  }
  Strings a;
  get_files_in_directory(p, a);
  std::sort(a.begin(), a.end());
  foreach (const string &f, a)
    if (Map::match_extension(f))
      v.push_back(new Workspace(f));
}

void Application::setup_widgets() {
  Box *v, *h;
  Canvas *c;
  Label *l;

  v = new Box(VERTICAL);
  h = new Box();

  c = new Canvas();
  c->align(0.5f, 0.5f);
  typedef Application A;
  c->link("canvas-click", this, &A::on_canvas_click);
  c->link("canvas-select", this, &A::on_canvas_select);
  c->link("canvas-motion", this, &A::on_canvas_motion);
  c->link("canvas-drag", this, &A::on_canvas_drag);
  h->add(c, EXPAND);
  m_map_canvas = c;

  c = new Canvas();
  c->align(0.5f, 0.5f);
  c->set_visible(m_compare);
  c->set_mark_style(Mark::TARGET);
  c->link("canvas-click", this, &A::on_canvas_click);
  c->link("canvas-select", this, &A::on_canvas_select);
  c->link("canvas-motion", this, &A::on_canvas_motion);
  c->link("canvas-drag", this, &A::on_canvas_drag);
  h->add(c, EXPAND);
  m_reference_canvas = c;

  v->add(h, EXPAND);
  h = new Box();

  l = new Label();
  h->add(l);
  m_status_label = l;

  l = new Label();
  l->align(1.0f, 0.0f);
  h->add(l, EXPAND);
  m_position_label = l;

  v->add(h);

  Toplevel &t = m_toplevel;
  t.set_child(v);
  t.link("toplevel-key", this, &A::on_toplevel_key);
  t.link("toplevel-input", this, &A::on_toplevel_input);
}

void Application::run() {
  m_toplevel.run();
}

void Application::shutdown() {
  save_workspace();
  m_toplevel.shutdown();
  g_cache->shutdown();
  foreach (Workspace *w, m_workspaces)
    delete w;
  m_workspaces.clear();
}

void Application::clear() {
  m_map_canvas->clear();
  m_reference_canvas->clear();
  clear_focus();
}

bool Application::load_workspace() {
  clear();
  Workspace *w = workspace();

  if (!w->load()) {
    report(w->error);
    return false;
  }

  Map &m = w->map;
  const string &n = m.name();
  const string &i = m.image();
  Format f;
  Canvas *c = m_map_canvas;
  if (!c->load(i)) {
    f = "Failed to load image for {}";
    report(f.bind(n));
    return false;
  }
  c->attach(&w->notes);

  if (m_compare) {
    string r = i;
    strip_directory(r);
    r = m_reference_path + '/' + r;
    c = m_reference_canvas;
    if (!c->load(r)) {
      f = "Failed to load reference for {}";
      report(f.bind(n));
      c->set_visible(false);
      c->attach(0);
    } else {
      c->set_visible(true);
      c->attach(&w->notes);
    }
  }

  report_load();
  update_title();
  return true;
}

void Application::report_load() {
  Workspace *w = workspace();
  Map &m = w->map;
  Format f = "Loaded {} {}x{} {} - ";
  const char *c = w->loaded_map() ? "tiles" : "pixels";
  f.bind(m.name(), m.width(), m.height(), c);
  string r = f.result();
  size_t s = w->notes.size();
  f = s == 1 ? "{} note" : "{} notes";
  f.bind(s);
  r += f.result();
  report(r);
}

void Application::update_title() {
  Workspace *w = workspace();
  const string &n = w->map.name();
  size_t c = m_current + 1, s = m_workspaces.size();
  const char *p = EXECUTABLE_NAME;
  int a = ELM_ANNOTATE_VERSION_MAJOR;
  int i = ELM_ANNOTATE_VERSION_MINOR;
  Format f;
  if (s > 1) {
    f = "{} [{}/{}] - {} v{}.{}";
    f.bind(n, c, s, p, a, i);
  } else {
    f = "{} - {} v{}.{}";
    f.bind(n, p, a, i);
  }
  m_toplevel.set_title(f.result());
}

bool Application::save_workspace() {
  Workspace *w = workspace();
  if (!w->changed())
    return true;
  if (!w->save()) {
    report(w->error);
    return false;
  }
  Format f = "Saved {} - {} notes";
  report(f.bind(w->map.name(), w->notes.size()));
  return true;
}

void Application::on_canvas_click(Canvas::Click &m) {
  if (m.button != sf::Mouse::Left)
    return;
  Workspace *w = workspace();
  const sf::Vector2f &l = m.local;
  sf::Vector2i t = w->local_to_tile(l);
  w->notes.add(t, l, "");
}

void Application::on_canvas_select(Canvas::Select &m) {
  typedef sf::Mouse M;
  M::Button b = m.button;
  int t = m.tag;
  if (b == M::Left) {
    if (m_focus == t)
      clear_focus();
    else
      focus(t);
  } else if (b == M::Right) {
    Workspace *w = workspace();
    w->notes.remove(t);
    clear_focus();
  }
}

void Application::on_canvas_motion(Canvas::Motion &m) {
  Workspace *w = workspace();
  sf::Vector2i t = w->local_to_tile(m.local);
  Format f = "({}, {})";
  f.bind(t.x, t.y);
  m_position_label->set_text(f.result());
}

void Application::on_canvas_drag(Canvas::Drag &m) {
  Workspace *w = workspace();
  const sf::Vector2f &l = m.local;
  sf::Vector2i t = w->local_to_tile(l);
  w->notes.move(m.tag, t, l);
}

Workspace *Application::workspace() {
  return m_workspaces[m_current];
}

void Application::change_workspace(int offset) {
  int s = m_workspaces.size();
  if (s < 2)
    return;
  int c = m_current;
  c += offset;
  while (c >= s)
    c -= s;
  while (c < 0)
    c += s;
  size_t n = c;
  if (n == m_current)
    return;
  if (!save_workspace())
    return;
  m_current = n;
  load_workspace();
}

void Application::on_toplevel_key(Toplevel::Key &m) {
  typedef sf::Keyboard K;
  K::Key k = m.code;

  if (k == K::Escape) {
    clear_focus();
  } else if (k == K::Tab) {
    next_note();
  } else if (k == K::Return) {
    next_empty();
  } else if (k == K::Delete) {
    Workspace *w = workspace();
    w->notes.remove(m_focus);
    clear_focus();
  }

  if (m_focus)
    return;

  if (k == K::A) {
    toggle_stretch();
  } else if (k == K::B || k == K::BackSpace) {
    change_workspace(-1);
  } else if (k == K::L) {
    load_workspace();
  } else if (k == K::M) {
    toggle_marks();
  } else if (k == K::N || k == K::Space) {
    change_workspace(1);
  } else if (k == K::Q) {
    m_toplevel.quit();
  } else if (k == K::R) {
    toggle_reference();
  } else if (k == K::S) {
    save_workspace();
  } else if (k == K::PageDown) {
    change_workspace(10);
  } else if (k == K::PageUp) {
    change_workspace(-10);
  }
}

void Application::toggle_stretch() {
  Canvas *c = m_map_canvas;
  bool v = !c->get_stretch();
  c->set_stretch(v);
  m_reference_canvas->set_stretch(v);
}

void Application::toggle_reference() {
  Canvas *c = m_reference_canvas;
  c->set_visible(!c->get_visible());
}

void Application::toggle_marks() {
  m_map_canvas->show_marks ^= 1;
  m_reference_canvas->show_marks ^= 1;
}

void Application::report(const char *s) {
  printf("%s\n", s);
  if (Label *l = m_status_label)
    l->set_text(s);
}

void Application::report(const string &s) {
  report(s.c_str());
}

void Application::focus(int t) {
  if (t == m_focus)
    return;
  clear_focus();
  m_focus = t;
  m_map_canvas->select(t);
  m_reference_canvas->select(t);
}

void Application::clear_focus() {
  if (int &t = m_focus) {
    m_map_canvas->select(t, false);
    m_reference_canvas->select(t, false);
    t = 0;
  }
}

void Application::next_note() {
  Workspace *w = workspace();
  focus(w->notes.next(m_focus));
}

void Application::next_empty() {
  size_t c = m_workspaces.size();
  for (size_t k = 0; k < c; ++k) {
    Workspace *w = workspace();
    Store &s = w->notes;
    int t = s.next(m_focus);
    for (size_t j = 0; j < s.size(); ++j) {
      Store::iterator i = s.find(t);
      if (i == s.end())
        continue;
      const Note &n = i->second;
      if (n.text.isEmpty()) {
        focus(t);
        return;
      }
      t = s.next(t);
    }
    clear_focus();
    if (k)
      break;
    if (c > 1)
      change_workspace(1);
  }
}

void Application::on_toplevel_input(Toplevel::Input &m) {
  const int &f = m_focus;
  if (!f)
    return;
  Workspace *w = workspace();
  Store &s = w->notes;
  Store::iterator i = s.find(f);
  if (i == s.end())
    return;
  sf::String t = i->second.text;
  sf::Uint32 u = m.unicode;
  if (u == '\b') {
    if (t.isEmpty())
      return;
    t.erase(t.getSize() - 1);
  } else if (u < 32) {
    return;
  } else {
    t += sf::String(u);
  }
  s.edit(f, t);
}
