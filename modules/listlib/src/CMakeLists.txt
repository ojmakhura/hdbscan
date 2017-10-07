project (listlib)

file(GLOB_RECURSE LISTLIB_SRC_FILES ${LISTLIB_SRC_DIR}/*.c)
add_library(listlib SHARED ${LISTLIB_SRC_FILES})
include_directories(${GLIB2_INCLUDE_DIRS})
install(TARGETS listlib
  # IMPORTANT: Add the hdbscan library to the "export-set"
  EXPORT HdbscanTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/hdbscan"
    COMPONENT dev)
