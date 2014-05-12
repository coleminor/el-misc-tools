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
#ifndef ELM_ANNOTATE_SENDER_HPP
#define ELM_ANNOTATE_SENDER_HPP

#include "foreach.hpp"
#include "message.hpp"
#include "name.hpp"
#include "utility.hpp"

#include <vector>

class Sender {
public:
  Sender():m_links() {}
  virtual ~Sender() {
    foreach (Link *l, m_links)
      delete l;
  }

  template<class T, class M>
  void link(const Name &n, T *t, void (T::*m)(M &)) {
    m_links.push_back(new Closure<T, M>(n, t, m));
  }

  template<class T>
  void unlink(T *t) {
    void *p = static_cast<void *>(t);
    Links &v = m_links;
    Links::iterator i = v.begin();
    while (i != v.end()) {
      if ((*i)->match(p)) {
        delete *i;
        i = v.erase(i);
      } else {
        ++i;
      }
    }
  }

  void send(Message &m) {
    foreach (Link *l, m_links)
      if (l->name == m.get_name())
        l->send(m);
  }
private:
  struct Link {
    const Name name;
    Link(const Name &n):name(n) {}
    virtual ~Link() {}
    virtual void send(Message &) = 0;
    virtual bool match(void *) = 0;
  };

  template<class T, class M>
  struct Closure : Link {
    DISALLOW_COPY_AND_ASSIGNMENT(Closure);
  public:
    typedef void (T::*Method)(M &);
    T *instance;
    Method method;
    Closure(const Name &n, T *t, Method m):
      Link(n),
      instance(t),
      method(m)
    {}
    virtual void send(Message &m) {
      (instance->*method)(static_cast<M &>(m));
    }
    virtual bool match(void *p) {
      return instance == static_cast<T *>(p);
    }
  };

  typedef std::vector<Link *> Links;
  Links m_links;
};

#endif // ELM_ANNOTATE_SENDER_HPP
