project (gnulib)

file(GLOB_RECURSE GNULIB_SRC_FILES ${GNULIB_SRC_DIR}/*.c)
add_library(gnulib SHARED ${GNULIB_SRC_FILES})
include_directories(${GNULIB_INCLUDE_DIR} ${PROJECT_BINARY_DIR})

install(TARGETS gnulib
  # IMPORTANT: Add the hdbscan library to the "export-set"
  EXPORT HdbscanTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib)
