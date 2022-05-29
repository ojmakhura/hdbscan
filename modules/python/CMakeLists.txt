project (${HDBSCAN_PYTHON_LIBRARY} VERSION ${HDBSCAN_VERSION})
set (PYTHON_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Python3 REQUIRED)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

include_directories("${HDBSCAN_INCLUDE_DIR}")
include_directories("${UTILS_INCLUDE_DIR}")
include_directories("${COLLECTIONS_INCLUDE_DIR}")
include_directories("${CMAKE_BINARY_DIR}")

file (GLOB SOURCE_FILES "${PYTHON_MODULE_DIR}/src/PyHdbscan.c")

# Set up such that XCode organizes the files
# source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES} ${PYTHON_FILES} m)

find_package(pybind11 REQUIRED)
pybind11_add_module(${PROJECT_NAME} 
	${SOURCE_FILES}
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${HDBSCAN_LIBRARY} ${UTILS_LIBRARY} ${COLLECTIONS_LIBRARY} )

install(TARGETS ${PROJECT_NAME}
	EXPORT HdbscanTargets
	LIBRARY DESTINATION "${Python3_SITELIB}" COMPONENT python
)
