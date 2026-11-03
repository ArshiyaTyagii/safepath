#!/usr/bin/env bash
# Creates the GitHub repo, topics, labels, milestones and roadmap issues.
# Requires the GitHub CLI (https://cli.github.com) and `gh auth login`.
# Run after init_git_history.sh:  bash scripts/setup_github.sh [repo-name]
set -euo pipefail
REPO="${1:-safepath}"

gh auth status >/dev/null || { echo "Run: gh auth login"; exit 1; }
OWNER="$(gh api user --jq .login)"

# Fill in the README badge links with your username and commit that change.
if grep -q YOUR_USERNAME README.md; then
  sed -i.bak "s|YOUR_USERNAME|$OWNER|g" README.md && rm -f README.md.bak
  git add README.md && git commit -m "docs: set repository links in README"
fi

gh repo create "$REPO" --public --source=. --remote=origin --push \
  --description "Women's-safety-aware routing engine in C++20: fastest vs safest routes on city road networks"
gh repo edit "$OWNER/$REPO" \
  --add-topic cpp --add-topic cpp20 --add-topic routing --add-topic graph-algorithms \
  --add-topic openstreetmap --add-topic womens-safety --add-topic smart-cities --add-topic sdg
git push origin v0.1.0

gh label create roadmap --color 1f6feb --description "Planned project work" 2>/dev/null || true
gh label create performance --color d93f0b --description "Speed and benchmarks" 2>/dev/null || true
gh label create data --color 0e8a16 --description "Datasets and pipelines" 2>/dev/null || true
gh label create ethics --color 5319e7 --description "Ethics, privacy and limitations" 2>/dev/null || true

ms() { gh api "repos/$OWNER/$REPO/milestones" -f title="$1" -f description="$2" >/dev/null; }
ms "Week 1: Foundation" "Understand and extend the core; benchmark baseline"
ms "Week 2: OSM ingestion" "Real city graph from OpenStreetMap"
ms "Week 3: Safety features" "Per-edge features and interpretable risk score"
ms "Week 4-5: Faster search" "A*, bidirectional search, benchmark suite"
ms "Week 6-7: Contraction hierarchies" "CCH with fast re-weighting"
ms "Week 8-9: Multi-criteria routes" "Pareto and alternative routes"
ms "Week 10-12: Product and evaluation" "Server, web UI, evaluation, write-up"

issue() { gh issue create --title "$1" --body "$2" --milestone "$3" --label "$4" >/dev/null; }

issue "CLI: add a lambda sweep table" "Print time vs mean risk for lambda in {0,0.5,1,2,3,5}. Becomes the first trade-off plot." "Week 1: Foundation" "roadmap"
issue "Add Dijkstra benchmark on a random 100k-node graph" "Record baseline latency (p50/p99). Use Google Benchmark or a simple timer." "Week 1: Foundation" "performance"
issue "Extract roads, nodes and tags from an OSM extract" "highway, lit, maxspeed, oneway, surface. Keep raw data out of git." "Week 2: OSM ingestion" "data"
issue "Build graph from OSM: split ways, haversine lengths, default speeds, SCC" "Keep the largest strongly connected component. Handle oneway." "Week 2: OSM ingestion" "data"
issue "Validate routes against known landmark pairs" "Compare distance and time with a map service within tolerance." "Week 2: OSM ingestion" "roadmap"
issue "Spatial index to snap POIs to edges" "Grid or R-tree. Shops, bus stops, police stations, hospitals." "Week 3: Safety features" "data"
issue "Compute per-edge safety features" "Lit flag, road class, open-POI density, transit proximity, isolation." "Week 3: Safety features" "data"
issue "Interpretable risk score with tunable weights and CSV export" "Score in [0,1]. Document each weight." "Week 3: Safety features" "ethics"
issue "A* and bidirectional Dijkstra" "Compare search-space size and latency against baseline." "Week 4-5: Faster search" "performance"
issue "Benchmark suite and regression thresholds in CI" "Fail CI if latency regresses badly." "Week 4-5: Faster search" "performance"
issue "Contraction hierarchy: node ordering and preprocessing" "Start with a simple edge-difference ordering." "Week 6-7: Contraction hierarchies" "performance"
issue "Customizable CH: re-weight per lambda and time band" "Metric-independent order, fast customization." "Week 6-7: Contraction hierarchies" "performance"
issue "Correctness: CH queries match Dijkstra on random queries" "Randomized cross-check in tests." "Week 6-7: Contraction hierarchies" "roadmap"
issue "Pareto-optimal and alternative routes" "Bi-criteria label-setting and penalty/plateau alternatives." "Week 8-9: Multi-criteria routes" "roadmap"
issue "HTTP route server" "Endpoints for fastest, safest, balanced; JSON with geometry and stats." "Week 10-12: Product and evaluation" "roadmap"
issue "MapLibre web UI with lambda slider and day/night toggle" "Show route alternatives and top risk contributors." "Week 10-12: Product and evaluation" "roadmap"
issue "Evaluation: detour vs risk reduction, ablation, small user study" "Report honestly, including failures." "Week 10-12: Product and evaluation" "ethics"
issue "Write-up: method, limitations and ethics" "Finalize docs/ethics.md and a short design doc." "Week 10-12: Product and evaluation" "ethics"

echo "Done: https://github.com/$OWNER/$REPO"
