project(hdbscan_samples)

add_executable(hdbscan_csample tester.c)
target_link_libraries(hdbscan_csample LINK_PRIVATE ${UTILS_LIBRARY} LINK_PUBLIC ${HDBSCAN_LIBRARY} ${COLLECTIONS_LIBRARY})

add_executable(hdbscan_cppsample tester.cpp)
target_link_libraries(hdbscan_cppsample LINK_PRIVATE ${UTILS_LIBRARY} LINK_PUBLIC ${HDBSCAN_LIBRARY} ${COLLECTIONS_LIBRARY})

include_directories(${HDBSCAN_INCLUDE_DIR} ${COLLECTIONS_INCLUDE_DIR})

install(TARGETS hdbscan_csample
  EXPORT HdbscanTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
)

install(TARGETS hdbscan_cppsample
  EXPORT HdbscanTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
)
