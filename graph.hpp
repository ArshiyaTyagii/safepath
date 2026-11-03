#pragma once
// Compact directed road graph in CSR (compressed sparse row) layout.
// Each edge carries the attributes SafePath needs: length, travel time, risk.
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace safepath {

using NodeId = std::uint32_t;
using EdgeId = std::uint32_t;
inline constexpr NodeId kInvalidNode = 0xFFFFFFFFu;
inline constexpr EdgeId kInvalidEdge = 0xFFFFFFFFu;

// Raw edge as read from data files.
struct EdgeInput {
  NodeId from;
  NodeId to;
  float length_m;
  float speed_kmh;
  float risk;  // 0 (safest) .. 1 (riskiest); a proxy score, not a crime prediction
};

// Stored edge (target node plus precomputed attributes).
struct Edge {
  NodeId to;
  float length_m;
  float time_s;
  float risk;
};

struct Coord {
  double lat = 0.0;
  double lon = 0.0;
};

class Graph {
 public:
  Graph() : offsets_{0} {}

  // Builds the CSR graph. Throws std::out_of_range / std::invalid_argument on bad input.
  // If bidirectional is true, each input edge is also added in reverse.
  static Graph from_edges(std::uint32_t num_nodes, const std::vector<EdgeInput>& edges,
                          bool bidirectional);

  std::uint32_t num_nodes() const { return static_cast<std::uint32_t>(offsets_.size() - 1); }
  std::size_t num_edges() const { return edges_.size(); }

  std::span<const Edge> out_edges(NodeId u) const {
    return {edges_.data() + offsets_[u], edges_.data() + offsets_[u + 1]};
  }
  EdgeId first_edge(NodeId u) const { return offsets_[u]; }
  const Edge& edge(EdgeId id) const { return edges_[id]; }

  void set_coords(std::vector<Coord> coords) { coords_ = std::move(coords); }
  const std::vector<Coord>& coords() const { return coords_; }

 private:
  std::vector<std::uint32_t> offsets_;  // size = num_nodes + 1
  std::vector<Edge> edges_;
  std::vector<Coord> coords_;
};

}  // namespace safepath
