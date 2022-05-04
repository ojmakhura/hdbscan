set (PYTHON_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# if (BUILD_PYTHON2)
# 	add_subdirectory(python2)
# endif()

IF (BUILD_PYTHON3)
# 	add_subdirectory(python3)
	find_program(PYTHON3 "python3")

	set(PYTHON ${PYTHON3})
	SET(TARGET_NAME "python3")
	set(OUTPUT      "${CMAKE_CURRENT_BINARY_DIR}/build/py3_timestamp")
	include(common.cmake)
	unset(OUTPUT)
	unset(PYTHON)
	unset(TARGET_NAME)
ENDIF()
