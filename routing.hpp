#pragma once
// Baseline single-criterion shortest path. The cost function decides what "best" means,
// so the same search gives fastest routes, safety-weighted routes, and later CCH customisation.
#include <algorithm>
#include <functional>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

#include "safepath/graph.hpp"

namespace safepath {

inline constexpr double kInf = std::numeric_limits<double>::infinity();

struct PathStats {
  double length_m = 0.0;
  double time_s = 0.0;
  double mean_risk = 0.0;  // length-weighted
};

struct PathResult {
  bool found = false;
  double cost = kInf;
  std::vector<NodeId> nodes;
  std::vector<EdgeId> edges;
  PathStats stats;
};

struct FastestCost {
  double operator()(const Edge& e) const { return e.time_s; }
};

// cost = time * (1 + lambda * risk). lambda = 0 reproduces the fastest route.
struct SafetyWeightedCost {
  double lambda;
  double operator()(const Edge& e) const { return e.time_s * (1.0 + lambda * e.risk); }
};

// Dijkstra with early exit. cost(edge) must be non-negative.
template <class CostFn>
PathResult shortest_path(const Graph& g, NodeId src, NodeId dst, CostFn&& cost) {
  PathResult r;
  const std::uint32_t n = g.num_nodes();
  if (src >= n || dst >= n) return r;

  std::vector<double> dist(n, kInf);
  std::vector<NodeId> parent_node(n, kInvalidNode);
  std::vector<EdgeId> parent_edge(n, kInvalidEdge);
  using QE = std::pair<double, NodeId>;
  std::priority_queue<QE, std::vector<QE>, std::greater<>> pq;

  dist[src] = 0.0;
  pq.emplace(0.0, src);
  while (!pq.empty()) {
    const auto [d, u] = pq.top();
    pq.pop();
    if (d > dist[u]) continue;  // stale entry
    if (u == dst) break;
    const EdgeId base = g.first_edge(u);
    const auto es = g.out_edges(u);
    for (std::size_t k = 0; k < es.size(); ++k) {
      const double nd = d + cost(es[k]);
      if (nd < dist[es[k].to]) {
        dist[es[k].to] = nd;
        parent_node[es[k].to] = u;
        parent_edge[es[k].to] = base + static_cast<EdgeId>(k);
        pq.emplace(nd, es[k].to);
      }
    }
  }
  if (dist[dst] == kInf) return r;

  r.found = true;
  r.cost = dist[dst];
  for (NodeId v = dst; v != src; v = parent_node[v]) {
    r.nodes.push_back(v);
    r.edges.push_back(parent_edge[v]);
  }
  r.nodes.push_back(src);
  std::reverse(r.nodes.begin(), r.nodes.end());
  std::reverse(r.edges.begin(), r.edges.end());

  double risk_len = 0.0;
  for (EdgeId id : r.edges) {
    const Edge& e = g.edge(id);
    r.stats.length_m += e.length_m;
    r.stats.time_s += e.time_s;
    risk_len += static_cast<double>(e.risk) * e.length_m;
  }
  r.stats.mean_risk = r.stats.length_m > 0.0 ? risk_len / r.stats.length_m : 0.0;
  return r;
}

}  // namespace safepath
