# Contributing

1. Fork and create a branch from `main`.
2. Build and test locally, including the sanitizer build:
   `cmake -S . -B build-san -DSAFEPATH_SANITIZE=ON -DCMAKE_BUILD_TYPE=Debug && cmake --build build-san && ctest --test-dir build-san`
3. Format with `clang-format` (config in `.clang-format`).
4. Add or update tests for any behaviour change. Algorithms must be cross-checked against a simple reference.
5. Open a pull request using the template.

Be careful with data: do not commit raw OSM extracts or any personal data.
