cmake_minimum_required(VERSION 3.0)
project(matchmaking-server)

set(protobuf_BUILD_TESTS OFF CACHE BOOL "Build tests")
add_subdirectory(third-party/protobuf/cmake)

add_subdirectory(protos)
add_subdirectory(src)
