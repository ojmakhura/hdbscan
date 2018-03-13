project (gnulib)

file(GLOB_RECURSE GNULIB_SRC_FILES ${GNULIB_SRC_DIR}/*.c)
add_library(gnulib SHARED ${GNULIB_SRC_FILES})
include_directories(${GNULIB_INCLUDE_DIR})

set_target_properties(gnulib PROPERTIES
  PUBLIC_HEADER "include/gnulib/*.h;${CMAKE_CURRENT_BINARY_DIR}/config.h")

install(TARGETS gnulib
  # IMPORTANT: Add the hdbscan library to the "export-set"
  EXPORT HdbscanTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/hdbscan"
    COMPONENT dev)
