set (LISTLIB_INCLUDE_DIR ${LISTLIB_DIR}/include)
set (LISTLIB_SRC_DIR ${LISTLIB_DIR}/src)
set (LISTLIB_TST_DIR ${LISTLIB_DIR}/tests)

include_directories (${LISTLIB_INCLUDE_DIR})
add_subdirectory (${LISTLIB_SRC_DIR})
add_subdirectory (${LISTLIB_TST_DIR})

set_target_properties(listlib PROPERTIES
  PUBLIC_HEADER "include/listlib/*.h")
  
