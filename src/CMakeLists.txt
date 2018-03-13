
file(GLOB HDBSCAN_CSRC "*.c")
file(GLOB HDBSCAN_CPPSRC "*.cpp")

add_library(hdbscan SHARED ${HDBSCAN_CSRC} ${HDBSCAN_CPPSRC})
target_link_libraries(hdbscan LINK_PUBLIC ${GLIB2_LIBRARIES} ${GSL_LIBRARIES} m LINK_PRIVATE gnulib listlib)
include_directories(${HDBSCAN_INCLUDE_DIR} ${GLIB2_INCLUDE_DIRS} ${LISTLIB_INCLUDE_DIR} ${GNULIB_INCLUDE_DIR})
  
install(TARGETS hdbscan
  # IMPORTANT: Add the hdbscan library to the "export-set"
  EXPORT HdbscanTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib)
