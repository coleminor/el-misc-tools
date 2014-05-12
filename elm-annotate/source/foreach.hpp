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
#ifndef ELM_ANNOTATE_FOREACH_HPP
#define ELM_ANNOTATE_FOREACH_HPP

#include "preprocessor.hpp"

//
// Adapted from boost::foreach.
// Only supports non-cv lvalue containers.
//
// See:
//   http://www.artima.com/cppsource/foreach.html
//   http://cplusplus.bordoon.com/boost_foreach_techniques.html
//
// Example:
//   using namespace std;
//   vector<int> v;
//   ...
//   foreach (int i, v) {
//     some_function(i);
//   }
//   foreach (int &i, v)
//     i *= 2;
//
//   map<int, string> m;
//   ...
//   foreach_pair(int i, const string &s, m) {
//     cout << i << ' ' << s << endl;
//   }
//

namespace _foreach {

struct Any {
  virtual ~Any() {}
  operator bool() const { return false; }
};

template<class T>
struct Holder : Any {
  Holder(const T &t):item(t) {}
  mutable T item;
};

template<class T>
T &any_cast(const Any &a) {
  return static_cast<const Holder<T> &>(a).item;
}

template<class T>
struct Type {};

template<class T>
Type<T> reify_type(const T &) {
  return Type<T>();
}

struct Probe {
  template<class T>
  operator Type<T>() const {
    return Type<T>();
  }
};

template<class C>
Holder<typename C::iterator> begin(C &c) {
  return c.begin();
}

template<class C>
Holder<typename C::iterator> end(C &c) {
  return c.end();
}

template<class C>
void increment(const Any &a, Type<C>) {
  ++any_cast<typename C::iterator>(a);
}

template<class C>
typename C::reference dereference(const Any &a, Type<C>) {
  return *any_cast<typename C::iterator>(a);
}

template<class C>
bool equal(const Any &a, const Any &b, Type<C>) {
  typedef typename C::iterator i;
  return any_cast<i>(a) == any_cast<i>(b);
}

template<class C>
const typename C::key_type &first(const Any &a, Type<C>) {
  return any_cast<typename C::iterator>(a)->first;
}

template<class C>
typename C::mapped_type &second(const Any &a, Type<C>) {
  return any_cast<typename C::iterator>(a)->second;
}

} // namespace _foreach

#define _FE_TYPE(x) \
  (1 ? _foreach::Probe() : _foreach::reify_type(x))
#define _FE_ANY const _foreach::Any &
#define _FE_BEGIN(c) _foreach::begin(c)
#define _FE_END(c) _foreach::end(c)
#define _FE_EQ(a, b, c) _foreach::equal(a, b, c)
#define _FE_INC(a, c) _foreach::increment(a, c)
#define _FE_DEREF(a, c) _foreach::dereference(a, c)
#define _FE_FIRST(a, c) _foreach::first(a, c)
#define _FE_SECOND(a, c) _foreach::second(a, c)

#define foreach(decl, c) \
  if (_FE_ANY PP_ID(i) = _FE_BEGIN(c)) {} else \
  if (_FE_ANY PP_ID(e) = _FE_END(c)) {} else \
  for (bool PP_ID(m) = true; \
       PP_ID(m) && !_FE_EQ(PP_ID(i), PP_ID(e), _FE_TYPE(c)); \
       PP_ID(m) ? _FE_INC(PP_ID(i), _FE_TYPE(c)) : (void) 0) \
  if ((PP_ID(m) = false)) {} else \
  for (decl = _FE_DEREF(PP_ID(i), _FE_TYPE(c)); \
       !PP_ID(m); PP_ID(m) = true)

#define foreach_key(decl, c) \
  if (_FE_ANY PP_ID(i) = _FE_BEGIN(c)) {} else \
  if (_FE_ANY PP_ID(e) = _FE_END(c)) {} else \
  for (bool PP_ID(m) = true; \
       PP_ID(m) && !_FE_EQ(PP_ID(i), PP_ID(e), _FE_TYPE(c)); \
       PP_ID(m) ? _FE_INC(PP_ID(i), _FE_TYPE(c)) : (void) 0) \
  if ((PP_ID(m) = false)) {} else \
  for (decl = _FE_FIRST(PP_ID(i), _FE_TYPE(c)); \
       !PP_ID(m); PP_ID(m) = true)

#define foreach_value(decl, c) \
  if (_FE_ANY PP_ID(i) = _FE_BEGIN(c)) {} else \
  if (_FE_ANY PP_ID(e) = _FE_END(c)) {} else \
  for (bool PP_ID(m) = true; \
       PP_ID(m) && !_FE_EQ(PP_ID(i), PP_ID(e), _FE_TYPE(c)); \
       PP_ID(m) ? _FE_INC(PP_ID(i), _FE_TYPE(c)) : (void) 0) \
  if ((PP_ID(m) = false)) {} else \
  for (decl = _FE_SECOND(PP_ID(i), _FE_TYPE(c)); \
       !PP_ID(m); PP_ID(m) = true)

#define foreach_pair(decl1, decl2, c) \
  if (_FE_ANY PP_ID(i) = _FE_BEGIN(c)) {} else \
  if (_FE_ANY PP_ID(e) = _FE_END(c)) {} else \
  for (bool PP_ID(m) = true; \
       PP_ID(m) && !_FE_EQ(PP_ID(i), PP_ID(e), _FE_TYPE(c)); \
       PP_ID(m) ? _FE_INC(PP_ID(i), _FE_TYPE(c)) : (void) 0) \
  if ((PP_ID(m) = false)) {} else \
  for (decl1 = _FE_FIRST(PP_ID(i), _FE_TYPE(c)); !PP_ID(m);) \
  for (decl2 = _FE_SECOND(PP_ID(i), _FE_TYPE(c)); \
       !PP_ID(m); PP_ID(m) = true)

#endif // ELM_ANNOTATE_FOREACH_HPP
