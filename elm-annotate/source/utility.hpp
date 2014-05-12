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
#ifndef ELM_ANNOTATE_UTILITY_HPP
#define ELM_ANNOTATE_UTILITY_HPP

#include "preprocessor.hpp"

#include <string>
#include <vector>

typedef std::vector<std::string> Strings;

void strip_directory(std::string &path);
void strip_ending(std::string &s, const char *e);
bool ends_with(const std::string &s, const char *e);

bool file_exists(const std::string &path);
bool is_file(const std::string &path);
bool is_directory(const std::string &path);
void get_files_in_directory(const std::string &path, Strings &v);

#define DISALLOW_COPY_AND_ASSIGNMENT(C) \
  private: C(const C &); void operator=(const C &)

template<typename T, size_t N>
char (&_countof(T (&a)[N]))[N];
#define COUNTOF(a) (sizeof(_countof(a)))

template<typename P>
class Cleaner {
  DISALLOW_COPY_AND_ASSIGNMENT(Cleaner);
private:
  typedef void (*F)(P);
  P pointer;
  F function;
  static void operator_delete(P p) {
    delete p;
  }
public:
  Cleaner(P p, F f = operator_delete):
    pointer(p),
    function(f)
  {}
  ~Cleaner() {
    if (pointer)
      function(pointer);
  }
  void cancel() {
    pointer = 0;
  }
};

#define SCOPE_DELETE(P, p) \
  Cleaner<P> PP_ID(c)(p)

#define SCOPE_FREE_USING(f, P, p) \
  struct PP_ID(Freer) { \
    static void free(P p) { f(p); } \
  }; \
  Cleaner<P> PP_ID(c)(p, PP_ID(Freer)::free)

#endif // ELM_ANNOTATE_UTILITY_HPP
