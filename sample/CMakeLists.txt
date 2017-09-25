option(BUILD_EXAMPLES "Build samples" ON)

add_executable(hdbscan-test tester.cpp)
target_link_libraries(hdbscan-test hdbscan)
target_include_directories(hdbscan-test PUBLIC ${HDBSCAN_INCLUDE_DIR})
