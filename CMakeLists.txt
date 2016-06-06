cmake_minimum_required (VERSION 2.6)
project (hdbscan CXX)
set(CMAKE_BUILD_TYPE Release)
# ---[ Includes
set(HDBSCAN_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
include_directories(${HDBSCAN_INCLUDE_DIR})

set(CADT_INCLUDE_DIR /usr/local/cadt-1.0/include)
include_directories(${HDBSCAN_INCLUDE_DIR})

set(CADT_LIB_DIR /usr/local/hdbscan-1.0/lib)
set(HDBSCAN_SRC_DIR ${CADT_INCLUDE_DIR}/src)

# get all *.cpp files recursively
file(GLOB_RECURSE SRC_FILES ${HDBSCAN_SRC_DIR}/*.cpp)
file(GLOB_RECURSE INCLUDE_FILES ${HDBSCAN_INCLUDE_DIR}/*.hpp)

# ---[ HDBSCAN version
set(HDBSCAN_TARGET_VERSION "1.0.0")
set(HDBSCAN_TARGET_SOVERSION "1.0.0")
add_definitions(-DHDBSCAN_VERSION=${HDBSCAN_TARGET_VERSION})

# -- project options
set( CMAKE_CXX_FLAGS "-D__cplusplus=201103L -O3 -march=native -flto -fwhole-program -g3 -Wall -fmessage-length=0 -std=c++11")
set ( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -O3 -march=native -flto -fwhole-program -lhdbscan++" )
add_library(hdbscan++ SHARED ${SRC_FILES})
 
install (TARGETS hdbscan++ DESTINATION lib)
install (FILES ${INCLUDE_FILES} DESTINATION include)
