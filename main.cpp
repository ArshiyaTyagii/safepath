// Usage: safepath_cli <nodes.csv> <edges.csv> <src> <dst> [lambda=3.0]
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "safepath/graph.hpp"
#include "safepath/routing.hpp"

using namespace safepath;

static std::vector<std::vector<std::string>> read_csv(const std::string& path) {
  std::ifstream in(path);
  if (!in) throw std::runtime_error("cannot open " + path);
  std::vector<std::vector<std::string>> rows;
  std::string line;
  std::getline(in, line);  // header
  while (std::getline(in, line)) {
    if (line.empty()) continue;
    std::vector<std::string> cells;
    std::stringstream ss(line);
    std::string cell;
    while (std::getline(ss, cell, ',')) cells.push_back(cell);
    rows.push_back(std::move(cells));
  }
  return rows;
}

static void print_route(const char* label, const PathResult& r) {
  std::cout << label << ": ";
  if (!r.found) {
    std::cout << "no route\n";
    return;
  }
  for (std::size_t i = 0; i < r.nodes.size(); ++i) std::cout << (i ? " -> " : "") << r.nodes[i];
  std::cout << std::fixed << std::setprecision(1) << "\n    length " << r.stats.length_m
            << " m, time " << r.stats.time_s << " s, mean risk " << std::setprecision(2)
            << r.stats.mean_risk << "\n";
}

int main(int argc, char** argv) {
  if (argc < 5) {
    std::cerr << "usage: safepath_cli <nodes.csv> <edges.csv> <src> <dst> [lambda]\n";
    return 2;
  }
  try {
    const auto nodes = read_csv(argv[1]);
    const auto rows = read_csv(argv[2]);
    std::vector<EdgeInput> edges;
    for (const auto& c : rows) {
      edges.push_back({static_cast<NodeId>(std::stoul(c.at(0))), static_cast<NodeId>(std::stoul(c.at(1))),
                       std::stof(c.at(2)), std::stof(c.at(3)), std::stof(c.at(4))});
    }
    Graph g = Graph::from_edges(static_cast<std::uint32_t>(nodes.size()), edges, true);
    const NodeId s = static_cast<NodeId>(std::stoul(argv[3]));
    const NodeId t = static_cast<NodeId>(std::stoul(argv[4]));
    const double lambda = argc > 5 ? std::stod(argv[5]) : 3.0;

    print_route("fastest", shortest_path(g, s, t, FastestCost{}));
    print_route("safest ", shortest_path(g, s, t, SafetyWeightedCost{lambda}));
  } catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
