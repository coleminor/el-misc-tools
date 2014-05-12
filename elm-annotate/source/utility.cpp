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
#include "utility.hpp"

#include <cstring>
#include <string>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using std::string;

void strip_directory(string &s) {
  size_t p = s.find_last_of("/\\");
  if (p != string::npos)
    s.erase(0, p + 1);
}

void strip_ending(string &s, const char *e) {
  size_t l = s.size(), t = std::strlen(e);
  if (l >= t && 0 == s.compare(l - t, t, e))
    s.erase(l - t);
}

bool ends_with(const string &s, const char *e) {
  size_t l = s.size(), t = std::strlen(e);
  return l >= t && 0 == s.compare(l - t, t, e);
}

bool file_exists(const string &p) {
  return 0 == access(p.c_str(), R_OK);
}

bool is_file(const string &p) {
  struct stat b;
  if (-1 == stat(p.c_str(), &b))
    return false;
  return S_ISREG(b.st_mode);
}

bool is_directory(const string &p) {
  struct stat b;
  if (-1 == stat(p.c_str(), &b))
    return false;
  return S_ISDIR(b.st_mode);
}

void get_files_in_directory(const string &p, Strings &v) {
  DIR *d;
  if (!(d = opendir(p.c_str())))
    return;
  SCOPE_FREE_USING(closedir, DIR *, d);
  struct dirent *e;
  string b = p;
  if (!ends_with(b, "/"))
    b += '/';
  while ((e = readdir(d))) {
    string n = e->d_name;
    if (n == "." || n == "..")
      continue;
    string f = b + n;
    if (is_file(f))
      v.push_back(f);
  }
}
