add_subdirectory(${UTILS_DIR})
add_subdirectory(${LISTLIB_DIR})

IF(BUILD_JAVA)
	add_subdirectory(${HDBSCAN_JAVA_DIR})
ENDIF()

add_subdirectory(${HDBSCAN_PYTHON_DIR})
