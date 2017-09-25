project (hdbscanlib)

# get all *.cpp files recursively
file(GLOB_RECURSE SRC_FILES *.cpp)

add_library(hdbscan SHARED ${SRC_FILES})
target_link_libraries(hdbscan PUBLIC m)
target_include_directories(hdbscan PUBLIC ${HDBSCAN_INCLUDE_DIR})

