
add_subdirectory(${LISTLIB_DIR})
add_subdirectory(${GNULIB_DIR})

IF(BUILD_JAVA)
	add_subdirectory(${HDBSCAN_JAVA_DIR})
ENDIF()

add_subdirectory(${HDBSCAN_PYTHON_DIR})
