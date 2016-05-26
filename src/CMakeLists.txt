project (hdbscanlib CXX)

set( CMAKE_CXX_FLAGS "-D__cplusplus=201103L -O0 -g3 -Wall -fmessage-length=0 -std=c++11")
# get all *.cpp files recursively
file(GLOB_RECURSE SRC_FILES *.cpp)

add_library(hdbscan++ SHARED ${SRC_FILES})
