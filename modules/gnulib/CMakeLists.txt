set (GNULIB_INCLUDE_DIR ${GNULIB_DIR}/include)
set (GNULIB_SRC_DIR ${GNULIB_DIR}/src)

include_directories (${GNULIB_INCLUDE_DIR})
add_subdirectory (${GNULIB_SRC_DIR})

set_target_properties(gnulib PROPERTIES
  PUBLIC_HEADER "include/gnulib/*.h;${CMAKE_CURRENT_BINARY_DIR}/config.h")
