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
#include "timer.hpp"

#include <sys/time.h>

Timer::Timer():
  m_seconds()
{}

void Timer::start() {
  struct timeval t;
  gettimeofday(&t, 0);
  m_seconds = t.tv_sec;
  m_seconds += 0.000001 * t.tv_usec;
}

void Timer::stop() {
  struct timeval t;
  gettimeofday(&t, 0);
  double s = 0.000001 * t.tv_usec + t.tv_sec;
  m_seconds = s - m_seconds;
}
