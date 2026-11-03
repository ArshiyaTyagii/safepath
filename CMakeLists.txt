cmake_minimum_required(VERSION 3.20)
project(safepath VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

option(SAFEPATH_SANITIZE "Build with AddressSanitizer + UBSan" OFF)

add_library(safepath_core STATIC src/graph.cpp)
target_include_directories(safepath_core PUBLIC include)
target_compile_options(safepath_core PUBLIC -Wall -Wextra -Wpedantic)
if(SAFEPATH_SANITIZE)
  target_compile_options(safepath_core PUBLIC -fsanitize=address,undefined -fno-omit-frame-pointer)
  target_link_options(safepath_core PUBLIC -fsanitize=address,undefined)
endif()

add_executable(safepath_cli src/cli/main.cpp)
target_link_libraries(safepath_cli PRIVATE safepath_core)

enable_testing()
foreach(t test_graph test_routing)
  add_executable(${t} tests/${t}.cpp)
  target_link_libraries(${t} PRIVATE safepath_core)
  add_test(NAME ${t} COMMAND ${t})
endforeach()
