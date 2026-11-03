# SafePath

[![CI](https://github.com/YOUR_USERNAME/safepath/actions/workflows/ci.yml/badge.svg)](https://github.com/YOUR_USERNAME/safepath/actions/workflows/ci.yml)
![C++20](https://img.shields.io/badge/C%2B%2B-20-blue)
![License](https://img.shields.io/badge/license-MIT-green)

**A women's-safety-aware routing engine in C++20.** Given a city road network, SafePath returns the
fastest route, the safest route, and balanced alternatives, using a per-segment safety score that can
vary by time of day. Aligned with UN SDG 5 (Gender Equality), SDG 11 (Sustainable Cities) and SDG 16.

> **Important:** the safety score is a *proxy for perceived safety* built from infrastructure and
> activity signals. It is **not** a prediction of crime. See [docs/ethics.md](docs/ethics.md).

## Why this project
Most "women's safety" analytics stop at a heatmap. SafePath turns the data into a decision: a route.
That makes it an algorithms and systems problem (graph search, speed-ups, trade-offs) as well as a
data problem.

## Routing model
Edge cost = `travel_time x (1 + lambda x risk)`.
`lambda = 0` gives the fastest route; larger `lambda` trades extra minutes for lower risk.

Example on the bundled toy graph:

| Route   | Path        | Length  | Time  | Mean risk |
|---------|-------------|---------|-------|-----------|
| Fastest | 0 -> 3 -> 5 | 800 m   | 96 s  | 0.85      |
| Safest (lambda=3) | 0 -> 1 -> 2 -> 5 | 1500 m | 180 s | 0.12 |

## Quick start
```bash
cmake -S . -B build && cmake --build build -j
ctest --test-dir build --output-on-failure
./build/safepath_cli data/sample/nodes.csv data/sample/edges.csv 0 5 3.0
```
Sanitizer build: `cmake -S . -B build-san -DSAFEPATH_SANITIZE=ON -DCMAKE_BUILD_TYPE=Debug`

## Roadmap
- [x] CSR graph with per-edge length / time / risk
- [x] Dijkstra with pluggable cost function
- [x] CLI, unit tests, randomized cross-check vs Floyd-Warshall, CI with sanitizers
- [ ] OSM ingestion for a real city
- [ ] Edge safety features (lighting, POIs, transit, isolation)
- [ ] A* and bidirectional search, benchmarks
- [ ] Customizable Contraction Hierarchies for fast re-weighting
- [ ] Pareto / alternative routes
- [ ] HTTP server and MapLibre web UI
- [ ] Evaluation and ethics write-up

Progress is tracked in GitHub Issues and Milestones.

## Repository layout
```
include/safepath/   public headers (graph, routing)
src/                implementation and CLI
tests/              unit and randomized correctness tests
data/sample/        tiny example graph (raw city data is not committed)
docs/               plan, ethics and design notes
scripts/            repo and GitHub setup helpers
```

## Data and limitations
Street-level incident data is not publicly available at fine resolution in India (NCRB publishes
district-level counts). SafePath therefore uses transparent proxies and documents them. See
[docs/ethics.md](docs/ethics.md).

## License
MIT. See [LICENSE](LICENSE).
