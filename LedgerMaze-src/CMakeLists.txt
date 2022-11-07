
# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.2)

# Project's name 
project(libLedgerMaze_openssl)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -O3")

find_package(OpenSSL REQUIRED)

include_directories(${OPENSSL_INCLUDE_DIR})
  
add_executable(test_ledgermaze test/test_ledgermaze.cpp)

add_executable(bench_all bench/bench_all.cpp)

target_link_libraries(test_ledgermaze ${OPENSSL_LIBRARIES})

target_link_libraries(bench_all ${OPENSSL_LIBRARIES})