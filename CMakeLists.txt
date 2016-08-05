# set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_CURRENT_SOURCE_DIR}/c_flag_overrides.cmake)
# set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX ${CMAKE_CURRENT_SOURCE_DIR}/cxx_flag_overrides.cmake)

cmake_minimum_required(VERSION 3.5)
project(matchmaking-server)

# find_package(Protobuf REQUIRED)

set(protobuf_BUILD_TESTS OFF CACHE BOOL "Build tests")
add_subdirectory(third-party/protobuf/cmake)

add_subdirectory(protos)
add_subdirectory(src)