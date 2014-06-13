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
#include "path.hpp"

#include "foreach.hpp"
#include "format.hpp"
#include "grid.hpp"
#include "heuristic.hpp"
#include "options.hpp"
#include "pool.hpp"
#include "report.hpp"
#include "timer.hpp"

#include <cmath>
#include <queue>
#include <tr1/functional>
#include <tr1/unordered_set>

using std::tr1::hash;
using std::tr1::unordered_set;

Path::Path():
  tiles(),
  m_heuristic(g_options->heuristic)
{}

static inline bool fequal(float a, float b) {
  return fabsf(a - b) < 0.00001f;
}

struct Node {
  const Tile tile;
  float total;
  float cost;
  float remaining;
  Node *next;
  Node(const Tile &t, float c):
    tile(t),
    total(),
    cost(c),
    remaining(),
    next()
  {}
  void *operator new(size_t s, Pool<Node> &p) {
    return p.allocate(s);
  }
  void operator delete(void *d, Pool<Node> &p) {
    p.deallocate(d);
  }
  void mark_as_replaced() {
    total = -1.0f;
  }
  bool was_replaced() const {
    return fequal(total, -1.0f);
  }
};

struct node_hash {
  hash<int> h;
  size_t operator()(const Node *n) const {
    return h(n->tile.x) ^ h(n->tile.y);
  }
};

struct node_equal {
  bool operator()(const Node *a, const Node *b) const {
    return a->tile == b->tile;
  }
};

struct node_compare {
  bool operator()(const Node *a, const Node *b) const {
    return a->total > b->total;
  }
};

typedef unordered_set<Node *, node_hash, node_equal> NodeSet;
typedef std::vector<Node *> Nodes;
typedef std::priority_queue<Node *, Nodes, node_compare> NodeQueue;
typedef Pool<Node> NodePool;

static void neighbors(Node *n, Nodes &r, NodePool &p, const Grid &g) {
  Tile v[16];
  size_t s = g.adjacent(n->tile, v, COUNTOF(v));
  r.clear();
  for (Tile *t = v; t < v + s; ++t) {
    float c = g.get(*t);
    if (c > 0.1f) {
      Node *a = new (p) Node(*t, c);
      r.push_back(a);
    }
  }
}

static Node *lookup(Node *n, NodeSet &s) {
  NodeSet::iterator i = s.find(n);
  return i != s.end() ? *i : 0;
}

static void trace(Node *n, Tiles &v) {
  v.clear();
  while (n) {
    v.push_back(n->tile);
    n = n->next;
  }
  std::reverse(v.begin(), v.end());
}

static void report_stats(Timer &t, const NodePool &p) {
  t.stop();
  Format f = "Path found in {} seconds";
  info(f.bind(t.value()));
  f = "Pool allocated {} nodes ({} in trash)";
  info(f.bind(p.allocated(), p.trashed()));
}

bool Path::find(const Grid &g, const Tile &start, const Tile &goal) {
  Format f = "Finding path from cell {} to {}";
  info(f.bind(start, goal));

  Timer t;
  t.start();

  NodePool p;
  Node *n = new (p) Node(start, 0.0f);
  const Heuristic &h = m_heuristic;
  n->remaining = h.estimate(n->tile, goal);
  n->total = n->cost + n->remaining;
  NodeQueue q;
  q.push(n);
  NodeSet frontier;
  frontier.insert(n);

  NodeSet interior;
  Node *b;
  Nodes v;
  while (!q.empty()) {
    n = q.top();
    q.pop();
    if (n->was_replaced()) {
      p.discard(n);
      continue;
    }
    if (n->tile == goal) {
      trace(n, tiles);
      report_stats(t, p);
      return true;
    }
    frontier.erase(n);
    interior.insert(n);

    neighbors(n, v, p, g);
    foreach (Node *a, v) {
      a->cost += n->cost;
      if ((b = lookup(a, frontier))) {
        if (b->cost <= a->cost) {
          p.discard(a);
          continue;
        }
        frontier.erase(b);
        b->mark_as_replaced();
      }
      if ((b = lookup(a, interior))) {
        if (b->cost <= a->cost) {
          p.discard(a);
          continue;
        }
        interior.erase(b);
        p.discard(b);
      }
      a->remaining = h.estimate(a->tile, goal);
      a->total = a->cost + a->remaining;
      a->next = n;
      q.push(a);
      frontier.insert(a);
    }
  }

  report_stats(t, p);
  return false;
}
