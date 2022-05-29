add_subdirectory(${UTILS_DIR})
add_subdirectory(${COLLECTIONS_DIR})

IF(BUILD_JAVA)
	add_subdirectory(${HDBSCAN_JAVA_DIR})
ENDIF()

IF(BUILD_PYTHON3)
	add_subdirectory(${HDBSCAN_PYTHON_DIR})
ENDIF()
