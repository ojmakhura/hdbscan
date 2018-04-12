set (PYTHON_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if (BUILD_PYTHON2)
	add_subdirectory(python2)
endif()

if (BUILD_PYTHON3)
	add_subdirectory(python3)
ENDIF()
