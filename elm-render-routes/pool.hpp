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
#ifndef ELM_RENDER_ROUTES_POOL_HPP
#define ELM_RENDER_ROUTES_POOL_HPP

#include "utility.hpp"

#include <vector>

template<class T>
class Pool {
  DISALLOW_COPY_AND_ASSIGNMENT(Pool);
public:
  Pool(size_t initial_count = 128);
  ~Pool();
  T *obtain();
  void discard(T *);
  T *construct();
  void destroy(T *);
  void *allocate(size_t);
  void deallocate(void *);
  size_t allocated() const;
  size_t trashed() const;
private:
  struct Trash {
    Trash *next;
  };
  STATIC_ASSERT(sizeof(T) >= sizeof(Trash));
  struct Block {
    char *data;
    size_t size;
  };
  typedef std::vector<Block> Blocks;

  char *get(size_t);
  void put(char *);
  void allocate(Block &, size_t);
  void deallocate(Block &);

  Blocks m_blocks;
  Trash *m_trash;
  Block m_block;
  size_t m_offset;
};

template<class T>
Pool<T>::Pool(size_t n):
  m_blocks(),
  m_trash(),
  m_block(),
  m_offset()
{
  allocate(m_block, n * sizeof(T));
}

template<class T>
Pool<T>::~Pool() {
  deallocate(m_block);
  Blocks &v = m_blocks;
  for (size_t i = 0; i < v.size(); ++i)
    deallocate(v[i]);
}

template<class T>
T *Pool<T>::obtain() {
  char *p = get(sizeof(T));
  T *t = new (p) T;
  return t;
}

template<class T>
void Pool<T>::discard(T *t) {
  put(reinterpret_cast<char *>(t));
}

template<class T>
T *Pool<T>::construct() {
  char *p = get(sizeof(T));
  T *t = new (p) T();
  return t;
}

template<class T>
void Pool<T>::destroy(T *t) {
  t->~T();
  put(reinterpret_cast<char *>(t));
}

template<class T>
void *Pool<T>::allocate(size_t s) {
  char *p = get(s);
  return static_cast<void *>(p);
}

template<class T>
void Pool<T>::deallocate(void *t) {
  put(static_cast<char *>(t));
}

template<class T>
char *Pool<T>::get(size_t) {
  if (Trash *t = m_trash) {
    m_trash = t->next;
    return reinterpret_cast<char *>(t);
  }
  if (m_offset >= m_block.size) {
    size_t s = 2 * m_block.size;
    m_blocks.push_back(m_block);
    m_offset = 0;
    allocate(m_block, s);
  }
  char *p = m_block.data + m_offset;
  m_offset += sizeof(T);
  return p;
}

template<class T>
void Pool<T>::put(char *p) {
  Trash *t = reinterpret_cast<Trash *>(p);
  t->next = m_trash;
  m_trash = t;
}

template<class T>
void Pool<T>::allocate(Block &b, size_t s) {
  b.size = s;
  b.data = new char[s];
}

template<class T>
void Pool<T>::deallocate(Block &b) {
  delete[] b.data;
}

template<class T>
size_t Pool<T>::allocated() const {
  size_t n = m_block.size / sizeof(T);
  for (size_t i = 0; i < m_blocks.size(); ++i)
    n += m_blocks[i].size / sizeof(T);
  return n;
}

template<class T>
size_t Pool<T>::trashed() const {
  size_t n = 0;
  for (Trash *t = m_trash; t; t = t->next)
    ++n;
  return n;
}

#endif // ELM_RENDER_ROUTES_POOL_HPP
