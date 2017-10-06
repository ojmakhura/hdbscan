project(hdbscan_samples)

add_executable(hdbscan_csample tester.c)
target_link_libraries(hdbscan_csample hdbscan)

add_executable(hdbscan_cppsample tester.cpp)
target_link_libraries(hdbscan_cppsample hdbscan gnulib listlib)

include_directories(${HDBSCAN_INCLUDE_DIR} ${GLIB2_INCLUDE_DIRS} ${LISTLIB_INCLUDE_DIR} ${GNULIB_INCLUDE_DIR})

install(TARGETS hdbscan_csample
  # IMPORTANT: Add the bar executable to the "export-set"
  EXPORT HdbscanTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin)
