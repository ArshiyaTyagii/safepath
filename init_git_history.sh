#!/usr/bin/env bash
# Creates a clean, logical commit history using YOUR git identity.
# Run once from the repo root:  bash scripts/init_git_history.sh
set -euo pipefail

if ! git config user.name >/dev/null || ! git config user.email >/dev/null; then
  echo "Set your identity first:"
  echo '  git config --global user.name "Your Name"'
  echo '  git config --global user.email "you@example.com"'
  exit 1
fi
if [ -d .git ]; then echo ".git already exists; aborting."; exit 1; fi

git init -b main
git add LICENSE .gitignore .clang-format
git commit -m "chore: add license, gitignore and clang-format config"

git add CMakeLists.txt include src tests data
git commit -m "feat: CSR road graph, Dijkstra routing, CLI and tests

- CSR graph with per-edge length, travel time and risk
- Pluggable cost function (fastest, safety-weighted)
- Unit tests and randomized cross-check against Floyd-Warshall"

git add .github/workflows
git commit -m "ci: build and test with gcc/clang, with and without sanitizers"

git add README.md docs CHANGELOG.md CONTRIBUTING.md
git commit -m "docs: README, ethics note, plan, changelog and contributing guide"

git add .github/ISSUE_TEMPLATE .github/PULL_REQUEST_TEMPLATE.md scripts
git commit -m "chore: issue and PR templates, repo setup scripts"

git tag -a v0.1.0 -m "v0.1.0: foundation (graph, Dijkstra, CLI, tests, CI)"
git log --oneline
