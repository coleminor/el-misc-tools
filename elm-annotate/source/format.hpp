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
#ifndef ELM_ANNOTATE_FORMAT_HPP
#define ELM_ANNOTATE_FORMAT_HPP

#include "preprocessor.hpp"

#include <algorithm>
#include <cstddef>
#include <string>

size_t to_chars(bool, char *, size_t, const char *);
size_t to_chars(char, char *, size_t, const char *);
size_t to_chars(int, char *, size_t, const char *);
size_t to_chars(size_t, char *, size_t, const char *);
size_t to_chars(float, char *, size_t, const char *);
size_t to_chars(double, char *, size_t, const char *);
size_t to_chars(const char *, char *, size_t, const char *);
size_t to_chars(const std::string &, char *, size_t, const char *);
size_t to_chars(void *, char *, size_t, const char *);

#define _FMT_ARGUMENT_MAXIMUM 8
#define _FMT_TEMPLATE_ARGUMENT(N) class T##N
#define _FMT_FUNCTION_ARGUMENT(N) const T##N &t##N
#define _FMT_BIND_CASE(N) \
  case N: u = to_chars(t##N, b, r, extra); break;

#define _FMT_DEFINE_BIND_OF_ARITY(N) \
  template<PP_LIST(N, _FMT_TEMPLATE_ARGUMENT)> \
  Format &bind(PP_LIST(N, _FMT_FUNCTION_ARGUMENT)) { \
    reset(); \
    const char *f = m_format; \
    char *o = m_output; \
    const char *const eo = o + sizeof(m_output) - 1; \
    char *b = m_buffer; \
    const char *const eb = b + sizeof(m_buffer) - 1; \
    Argument *a; \
    size_t r, u; \
    char extra[32]; \
    char *x = extra; \
    const char *const ex = x + sizeof(extra) - 1; \
    size_t n = 0; \
    while (*f && o < eo) { \
      if (*f != '{') { \
        *o++ = *f++; \
        continue; \
      } \
      if (f[1] == '{') { \
        *o++ = '{'; \
        f += 2; \
        continue; \
      } \
      size_t i = 0; \
      x = extra; \
      while (1) { \
        switch (*++f) { \
        case ':': goto start_of_extra; \
        case '}': f++; \
        case '\0': goto end_of_specifier; \
        default: break; \
        } \
        if ('0' <= *f && *f <= '9') { \
          i *= 10; \
          i += *f - '0'; \
        } \
      } \
    start_of_extra: \
      while (x < ex) { \
        switch (*++f) { \
        case '}': f++; \
        case '\0': goto end_of_specifier; \
        default: *x++ = *f; \
        } \
      } \
      while (*f && *f != '}') \
        f++; \
      if (*f == '}') \
        f++; \
    end_of_specifier: \
      *x = '\0'; \
      if (!i) \
        i = ++n; \
      if (i > _FMT_ARGUMENT_MAXIMUM) \
        continue; \
      a = m_arguments + i - 1; \
      if (!a->start) { \
        r = eb - b; \
        switch (i - 1) { \
        PP_REPEAT(N, _FMT_BIND_CASE) \
        default: u = 0; \
        } \
        a->start = b; \
        a->size = u < r ? u : 0; \
        b += a->size; \
      } \
      r = eo - o; \
      u = std::min(r, a->size); \
      std::copy(a->start, a->start + u, o); \
      o += u; \
    } \
    *o = '\0'; \
    return *this; \
  }

class Format {
public:
  Format():m_format("") {
    reset();
  }
  Format(const char *f):m_format(f) {
    reset();
  }
  Format(const Format &o):m_format(o.m_format) {
    reset();
  }
  Format &operator=(const Format &o) {
    if (this != &o) {
      m_format = o.m_format;
      reset();
    }
    return *this;
  }
  Format &operator=(const char *f) {
    m_format = f;
    reset();
    return *this;
  }
  const char *result() const { return m_output; }
  operator const char *() const { return m_output; }
  
  _FMT_DEFINE_BIND_OF_ARITY(1)
  _FMT_DEFINE_BIND_OF_ARITY(2)
  _FMT_DEFINE_BIND_OF_ARITY(3)
  _FMT_DEFINE_BIND_OF_ARITY(4)
  _FMT_DEFINE_BIND_OF_ARITY(5)
  _FMT_DEFINE_BIND_OF_ARITY(6)
  _FMT_DEFINE_BIND_OF_ARITY(7)
  _FMT_DEFINE_BIND_OF_ARITY(8)

private:
  void reset() {
    m_output[0] = '\0';
    for (size_t i = 0; i < _FMT_ARGUMENT_MAXIMUM; ++i)
      m_arguments[i].start = 0;
  }
  struct Argument {
    char *start;
    size_t size;
  };
  const char *m_format;
  char m_output[1024];
  char m_buffer[1024];
  Argument m_arguments[_FMT_ARGUMENT_MAXIMUM];
};

#endif // ELM_ANNOTATE_FORMAT_HPP
