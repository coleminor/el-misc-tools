//
// Copyright 2013 Cole Minor <c.minor@inbox.com>
//
//    This file is part of mask2rects.
//
//    mask2rects is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    mask2rects is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with mask2rects.  If not, see <http://www.gnu.org/licenses/>.
//
#include <cstdio>

#include "log.hpp"
#include "options.hpp"

using namespace std;

void log_message(int type, const char *file, int line,
                 const char *func, const char *fmt, ...) {
  if (type == LOG_VERBOSE && !opts.verbose && !opts.debug)
    return;

  va_list ap;
  char b[1024];
  FILE *f;

  va_start(ap, fmt);
  vsnprintf(b, sizeof(b), fmt, ap);
  va_end(ap);

  f = type == LOG_ERROR ? stderr : stdout;

  if (type == LOG_ERROR)
    fprintf(f, "ERROR: ");
  if (opts.debug)
    fprintf(f, "[%s/%s +%d] ", func, file, line);
  fprintf(f, "%s\n", b);
}
