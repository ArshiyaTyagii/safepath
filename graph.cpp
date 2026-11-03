#include "safepath/graph.hpp"

#include <stdexcept>

namespace safepath {

Graph Graph::from_edges(std::uint32_t n, const std::vector<EdgeInput>& in, bool bidirectional) {
  for (const auto& e : in) {
    if (e.from >= n || e.to >= n) throw std::out_of_range("edge references unknown node id");
    if (!(e.speed_kmh > 0.0f)) throw std::invalid_argument("speed_kmh must be positive");
    if (e.length_m < 0.0f) throw std::invalid_argument("length_m must be non-negative");
    if (e.risk < 0.0f || e.risk > 1.0f) throw std::invalid_argument("risk must be in [0,1]");
  }

  // Counting sort by source node.
  std::vector<std::uint32_t> offsets(static_cast<std::size_t>(n) + 1, 0);
  for (const auto& e : in) {
    ++offsets[e.from + 1];
    if (bidirectional) ++offsets[e.to + 1];
  }
  for (std::uint32_t i = 0; i < n; ++i) offsets[i + 1] += offsets[i];

  std::vector<Edge> edges(offsets[n]);
  std::vector<std::uint32_t> cursor(offsets.begin(), offsets.end() - 1);
  auto place = [&](NodeId u, NodeId v, const EdgeInput& e) {
    const float time_s = e.length_m / (e.speed_kmh / 3.6f);
    edges[cursor[u]++] = Edge{v, e.length_m, time_s, e.risk};
  };
  for (const auto& e : in) {
    place(e.from, e.to, e);
    if (bidirectional) place(e.to, e.from, e);
  }

  Graph g;
  g.offsets_ = std::move(offsets);
  g.edges_ = std::move(edges);
  return g;
}

}  // namespace safepath
