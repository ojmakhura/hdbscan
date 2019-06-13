project (listlib)

file(GLOB_RECURSE LISTLIB_SRC_FILES ${LISTLIB_SRC_DIR}/*.c)
add_library(${LISTLIB_LIBRARY} SHARED ${LISTLIB_SRC_FILES})
target_link_libraries(${LISTLIB_LIBRARY} LINK_PUBLIC m ${GNULIB_LIBRARY})

add_library(${LISTLIB_LIBRARY}_static STATIC ${LISTLIB_SRC_FILES})
target_link_libraries(${LISTLIB_LIBRARY}_static LINK_PUBLIC m ${GNULIB_LIBRARY}_static)
include_directories(${GLIB2_INCLUDE_DIRS} ${GNULIB_INCLUDE_DIR})

install(TARGETS ${LISTLIB_LIBRARY}
  # IMPORTANT: Add the listlib library to the "export-set"
  EXPORT HdbscanTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib)
