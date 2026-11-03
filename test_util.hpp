#pragma once
#include <cmath>
#include <iostream>

inline int g_failures = 0;
#define CHECK(cond)                                                           \
  do {                                                                        \
    if (!(cond)) {                                                            \
      std::cerr << __FILE__ << ":" << __LINE__ << " CHECK failed: " #cond "\n"; \
      ++g_failures;                                                           \
    }                                                                         \
  } while (0)
#define CHECK_NEAR(a, b, eps) CHECK(std::abs((a) - (b)) <= (eps))
