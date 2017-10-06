set (LISTLIB_INCLUDE_DIR ${LISTLIB_DIR}/include)
set (LISTLIB_SRC_DIR ${LISTLIB_DIR}/src)

include_directories (${LISTLIB_INCLUDE_DIR})

add_subdirectory (${LISTLIB_SRC_DIR})
