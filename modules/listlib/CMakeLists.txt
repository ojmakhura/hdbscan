set (LISTLIB_INCLUDE_DIR ${LISTLIB_DIR}/include)
set (LISTLIB_SRC_DIR ${LISTLIB_DIR}/src)

if(BUILD_MODULE_TESTS)
  set (LISTLIB_TST_DIR ${LISTLIB_DIR}/tests)
endif()

include_directories (${LISTLIB_INCLUDE_DIR} ${UTILS_INCLUDE_DIR})
add_subdirectory (${LISTLIB_SRC_DIR})

if(BUILD_MODULE_TESTS)
  add_subdirectory (${LISTLIB_TST_DIR})
endif()

set_target_properties(${LISTLIB_LIBRARY} PROPERTIES
  PUBLIC_HEADER "include/listlib/*.h")
  
