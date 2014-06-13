//
//  Copyright (C) 2014 Cole Minor
//  This file is part of elm-render-routes
//
//  elm-render-routes is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  elm-render-routes is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "file.hpp"

#include "format.hpp"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <stdexcept>

File::File(const std::string &p):
  m_handle()
{
  const char *t = p.c_str();
  FILE *h = fopen(t, "r");
  if (!h) {
    const char *e = strerror(errno);
    Format f = "Failed to open '{}': {}";
    f.bind(t, e);
    throw std::runtime_error(f.result());
  }
  m_handle = h;
}

File::~File() {
  close();
}

void File::close() {
  if (FILE *&f = m_handle) {
    fclose(f);
    f = 0;
  }
}

static void chomp(char *s) {
  size_t i = strcspn(s, "\r\n");
  s[i] = '\0';
}

bool File::readline(char *b, size_t l) {
  if (!fgets(b, l, m_handle))
    return false;
  chomp(b);
  return true;
}
