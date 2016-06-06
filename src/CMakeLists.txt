project (hdbscanlib CXX)

set( CMAKE_CXX_FLAGS "-D__cplusplus=201103L -O3 -march=native -flto -fwhole-program -g3 -Wall -fmessage-length=0 -std=c++11")
# get all *.cpp files recursively
file(GLOB_RECURSE SRC_FILES *.cpp)
set ( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -O3 -march=native -flto -fwhole-program -lhdbscan++" )
add_library(hdbscan++ SHARED ${SRC_FILES})
