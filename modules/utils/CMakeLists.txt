set (UTILS_INCLUDE_DIR ${UTILS_DIR}/include)
set (UTILS_SRC_DIR ${UTILS_DIR}/src)
#set (UTILS_TST_DIR ${UTILS_DIR}/tests)

include_directories (${UTILS_INCLUDE_DIR})
add_subdirectory (${UTILS_SRC_DIR})
#add_subdirectory (${LISTLIB_TST_DIR})

set_target_properties(${UTILS_LIBRARY} PROPERTIES
  PUBLIC_HEADER "include/hdbscan/*.h")