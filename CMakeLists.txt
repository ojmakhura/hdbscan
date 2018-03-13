cmake_minimum_required (VERSION 2.8)

project(hdbscan)

# The version number.
set (HDBSCAN_MAJOR_VERSION 3)
set (HDBSCAN_MINOR_VERSION 0)
set (HDBSCAN_PATCH_VERSION 0)
set (HDBSCAN_VERSION ${HDBSCAN_VERSION_MAJOR}.${HDBSCAN_VERSION_MINOR}.${HDBSCAN_PATCH_VERSION})

set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
set(MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules)
set(LISTLIB_DIR ${MODULE_PATH}/listlib)
set(GNULIB_DIR ${MODULE_PATH}/gnulib)

set(HDBSCAN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(GNULIB_INCLUDE_DIR ${GNULIB_DIR}/include)
set(LISTLIB_INCLUDE_DIR ${LISTLIB_DIR}/include)


if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/CMake/hdbscan)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

find_package(GSL REQUIRED) 

#ensure we have pkg-config in the system
find_package(PkgConfig)

# lookup gli-2.0 in pkg-config
pkg_check_modules(GLIB2 REQUIRED glib-2.0>=2.36.0)

OPTION (USE_OpenMP "Use OpenMP" ON)
IF(USE_OpenMP)
  FIND_PACKAGE(OpenMP)
  IF(OPENMP_FOUND)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  ENDIF()
ENDIF()

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3 -Wall -fmessage-length=0 -fPIC -O3")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -Wall -fmessage-length=0 -fPIC -O3")

add_subdirectory(modules)
add_subdirectory(src)
add_subdirectory(sample)

# Add all targets to the build-tree export set
export(TARGETS hdbscan gnulib listlib FILE "${PROJECT_BINARY_DIR}/HdbscanTargets.cmake")
 
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE hdbscan)

# Create the HdbscanConfig.cmake and HdbscanConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}" "${INSTALL_INCLUDE_DIR}")

# ... for the config.h
configure_file(config.h.in "${GNULIB_INCLUDE_DIR}/gnulib/config.h" @ONLY)

# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file(HdbscanConfig.cmake.in "${PROJECT_BINARY_DIR}/HdbscanConfig.cmake" @ONLY)

# ... for the install tree
set(CONF_INCLUDE_DIRS "\${HDBSCAN_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(HdbscanConfig.cmake.in "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/HdbscanConfig.cmake" @ONLY)

# ... for both
configure_file(HdbscanConfigVersion.cmake.in "${PROJECT_BINARY_DIR}/HdbscanConfigVersion.cmake" @ONLY)

set_target_properties(hdbscan PROPERTIES
  PUBLIC_HEADER "include/hdbscan/*.h")
 
# Install the HdbscanConfig.cmake and HdbscanConfigVersion.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/HdbscanConfig.cmake"
  "${PROJECT_BINARY_DIR}/HdbscanConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)
 
# Install the export set for use with the install-tree
install (DIRECTORY ${HDBSCAN_INCLUDE_DIR}/hdbscan DESTINATION ${INSTALL_INCLUDE_DIR})
install (DIRECTORY ${GNULIB_INCLUDE_DIR}/gnulib DESTINATION ${INSTALL_INCLUDE_DIR})
install (DIRECTORY ${LISTLIB_INCLUDE_DIR}/listlib DESTINATION ${INSTALL_INCLUDE_DIR})
INSTALL(FILES ${GNULIB_INCLUDE_DIR}/gnulib/config.h DESTINATION include)

install(EXPORT HdbscanTargets DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)
