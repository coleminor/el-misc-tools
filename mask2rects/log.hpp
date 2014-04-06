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
#ifndef MASK2RECTS_LOG_HPP
#define MASK2RECTS_LOG_HPP

#include <cstdarg>

enum log_type { LOG_INFO, LOG_VERBOSE, LOG_ERROR };

void log_message(int type, const char *file, int line,
                 const char *func, const char *fmt, ...);

#define logerr(fmt, ...) \
  log_message(LOG_ERROR, __FILE__, __LINE__, __func__, \
              fmt, ## __VA_ARGS__)

#define loginfo(fmt, ...) \
  log_message(LOG_INFO, __FILE__, __LINE__, __func__, \
              fmt, ## __VA_ARGS__)

#define logver(fmt, ...) \
  log_message(LOG_VERBOSE, __FILE__, __LINE__, __func__, \
              fmt, ## __VA_ARGS__)

#endif
