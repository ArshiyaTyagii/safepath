#include <stdexcept>

#include "safepath/graph.hpp"
#include "test_util.hpp"

using namespace safepath;

int main() {
  // CSR degrees and bidirectional expansion.
  Graph g = Graph::from_edges(3, {{0, 1, 100, 36, 0.2f}, {1, 2, 200, 36, 0.4f}}, true);
  CHECK(g.num_nodes() == 3);
  CHECK(g.num_edges() == 4);
  CHECK(g.out_edges(0).size() == 1);
  CHECK(g.out_edges(1).size() == 2);
  CHECK(g.out_edges(2).size() == 1);
  CHECK_NEAR(g.out_edges(0)[0].time_s, 10.0, 1e-4);  // 100 m at 36 km/h = 10 m/s

  // Directed build.
  Graph d = Graph::from_edges(2, {{0, 1, 50, 18, 0.0f}}, false);
  CHECK(d.out_edges(0).size() == 1);
  CHECK(d.out_edges(1).empty());

  // Bad input is rejected.
  bool threw = false;
  try { Graph::from_edges(2, {{0, 5, 10, 30, 0.1f}}, false); } catch (const std::out_of_range&) { threw = true; }
  CHECK(threw);
  threw = false;
  try { Graph::from_edges(2, {{0, 1, 10, 30, 1.5f}}, false); } catch (const std::invalid_argument&) { threw = true; }
  CHECK(threw);

  // Empty graph is valid.
  Graph e;
  CHECK(e.num_nodes() == 0);

  return g_failures == 0 ? 0 : 1;
}
