
file(GLOB HDBSCAN_CSRC "*.c")
file(GLOB HDBSCAN_CPPSRC "*.cpp")

add_library(hdbscan SHARED ${HDBSCAN_CSRC} ${HDBSCAN_CPPSRC})
target_link_libraries(hdbscan ${GLIB2_LIBRARIES} m gnulib listlib)
include_directories(${HDBSCAN_INCLUDE_DIR} ${GLIB2_INCLUDE_DIRS} ${LISTLIB_INCLUDE_DIR} ${GNULIB_INCLUDE_DIR})


install(TARGETS hdbscan
  # IMPORTANT: Add the hdbscan library to the "export-set"
  EXPORT HdbscanTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/hdbscan"
    COMPONENT dev)
