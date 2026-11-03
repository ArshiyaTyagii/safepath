#include <random>
#include <vector>

#include "safepath/graph.hpp"
#include "safepath/routing.hpp"
#include "test_util.hpp"

using namespace safepath;

static Graph toy() {
  // Matches data/sample: main road 0-1-2-5 (long, low risk) vs alley 0-3-5 (short, high risk).
  std::vector<EdgeInput> e = {{0, 1, 500, 30, 0.10f}, {1, 2, 500, 30, 0.10f}, {2, 5, 500, 30, 0.15f},
                              {0, 3, 400, 30, 0.80f}, {3, 5, 400, 30, 0.90f}, {3, 4, 300, 30, 0.50f},
                              {4, 5, 350, 30, 0.40f}};
  return Graph::from_edges(6, e, true);
}

static void test_fastest_vs_safest() {
  Graph g = toy();
  auto fast = shortest_path(g, 0, 5, FastestCost{});
  CHECK(fast.found);
  CHECK((fast.nodes == std::vector<NodeId>{0, 3, 5}));

  auto safe = shortest_path(g, 0, 5, SafetyWeightedCost{3.0});
  CHECK(safe.found);
  CHECK((safe.nodes == std::vector<NodeId>{0, 1, 2, 5}));
  CHECK(safe.stats.mean_risk < fast.stats.mean_risk);
  CHECK(safe.stats.time_s > fast.stats.time_s);  // the safer route costs extra time

  auto zero = shortest_path(g, 0, 5, SafetyWeightedCost{0.0});
  CHECK(zero.nodes == fast.nodes);  // lambda = 0 equals fastest
}

static void test_edge_cases() {
  Graph g = Graph::from_edges(3, {{0, 1, 100, 36, 0.1f}}, false);
  CHECK(!shortest_path(g, 0, 2, FastestCost{}).found);   // unreachable
  CHECK(!shortest_path(g, 1, 0, FastestCost{}).found);   // directed
  CHECK(!shortest_path(g, 0, 99, FastestCost{}).found);  // invalid id
  auto self = shortest_path(g, 1, 1, FastestCost{});
  CHECK(self.found && self.nodes.size() == 1 && self.cost == 0.0);
}

// Cross-check Dijkstra against Floyd-Warshall on random directed graphs.
static void test_random_vs_floyd_warshall() {
  std::mt19937 rng(12345);
  for (int round = 0; round < 20; ++round) {
    const std::uint32_t n = 30;
    std::uniform_int_distribution<std::uint32_t> node(0, n - 1);
    std::uniform_real_distribution<float> len(50.f, 500.f), spd(20.f, 60.f), rsk(0.f, 1.f);
    std::vector<EdgeInput> in;
    for (int i = 0; i < 90; ++i) in.push_back({node(rng), node(rng), len(rng), spd(rng), rsk(rng)});
    Graph g = Graph::from_edges(n, in, false);
    const SafetyWeightedCost cost{1.5};

    std::vector<std::vector<double>> fw(n, std::vector<double>(n, kInf));
    for (NodeId u = 0; u < n; ++u) {
      fw[u][u] = 0.0;
      for (const Edge& e : g.out_edges(u)) fw[u][e.to] = std::min(fw[u][e.to], cost(e));
    }
    for (NodeId k = 0; k < n; ++k)
      for (NodeId i = 0; i < n; ++i)
        for (NodeId j = 0; j < n; ++j)
          if (fw[i][k] + fw[k][j] < fw[i][j]) fw[i][j] = fw[i][k] + fw[k][j];

    for (NodeId s = 0; s < n; ++s)
      for (NodeId t = 0; t < n; ++t) {
        auto r = shortest_path(g, s, t, cost);
        CHECK(r.found == (fw[s][t] != kInf));
        if (r.found) CHECK_NEAR(r.cost, fw[s][t], 1e-6);
      }
  }
}

int main() {
  test_fastest_vs_safest();
  test_edge_cases();
  test_random_vs_floyd_warshall();
  return g_failures == 0 ? 0 : 1;
}
