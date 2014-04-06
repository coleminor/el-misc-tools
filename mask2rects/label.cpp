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
#include "label.hpp"

Label::Label(int v)
  :m_root(this), m_rank(0), m_value(v) {
}

Label *Label::root() {
  if (m_root != this)
    m_root = m_root->root();
  return m_root;
}

void Label::join(Label *other) {
  if (other->m_rank < m_rank) {
    other->m_root = this;
  } else {
    m_root = other;
    if (m_rank == other->m_rank)
      other->m_rank++;
  }
}
