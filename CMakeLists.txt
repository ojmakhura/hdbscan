cmake_minimum_required (VERSION 2.6)
project (hdbscan)
set(CMAKE_BUILD_TYPE Release)

set(HDBSCAN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# ---[ HDBSCAN version
set(HDBSCAN_TARGET_VERSION "2.0.0")
set(HDBSCAN_TARGET_SOVERSION "2.0.0")
add_definitions(-DHDBSCAN_VERSION=${HDBSCAN_TARGET_VERSION})

add_subdirectory(src)
add_subdirectory(sample)
