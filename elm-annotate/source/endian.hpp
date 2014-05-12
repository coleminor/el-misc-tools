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
#ifndef ELM_ANNOTATE_ENDIAN_HPP
#define ELM_ANNOTATE_ENDIAN_HPP

#include "defines.hpp"

#include <stdint.h>

namespace Endian {

#if IS_BIG_ENDIAN
inline void convert(uint32_t &v) {
  v = __builtin_bswap32(v);
}
#else
inline void convert(uint32_t &) {}
#endif

} // namespace Endian

#endif // ELM_ANNOTATE_ENDIAN_HPP
