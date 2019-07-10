cmake_minimum_required (VERSION 2.8)

project(hdbscan)
set(HDBSCAN_DESCRIPTION "Implementation of HDBSCAN algorithm in C, C++, Java and python(2, 3)")
# The version number.
set (HDBSCAN_MAJOR_VERSION 3)
set (HDBSCAN_MINOR_VERSION 1)
set (HDBSCAN_PATCH_VERSION 6)
set (HDBSCAN_VERSION ${HDBSCAN_MAJOR_VERSION}.${HDBSCAN_MINOR_VERSION}.${HDBSCAN_PATCH_VERSION})

# Set modules to build
OPTION (BUILD_SAMPLES "Build Samples" ON)
OPTION (BUILD_PYTHON2 "Build python 2 bindings" ON)
OPTION (BUILD_PYTHON3 "Build python 3 bindings" ON)
OPTION (BUILD_JAVA "Build java bindings" ON)

OPTION (USE_CUDA "Use CUDA" OFF)
IF(USE_CUDA)
  #complete
ENDIF()

#detect architecture
if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")	
	message("Detected 64 bit architecture.")
	set(LIBDIR "lib64")
else()
	message("Detected 32 bit architecture.")
	set(LIBDIR "lib")
endif()

set(INSTALL_LIB_DIR "${LIBDIR}" CACHE PATH "Installation directory for libraries")
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
set(INSTALL_SHARE_DIR share/hdbscan CACHE PATH "Installation directory for extension files")

set(MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules)
set(LISTLIB_DIR ${MODULE_PATH}/listlib)
set(UTILS_DIR ${MODULE_PATH}/utils)
set(GNULIB_DIR ${MODULE_PATH}/gnulib)
set(HDBSCAN_JAVA_DIR ${MODULE_PATH}/java)
set(HDBSCAN_PYTHON_DIR ${MODULE_PATH}/python)

set(HDBSCAN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(UTILS_INCLUDE_DIR ${UTILS_DIR}/include)
set(GNULIB_INCLUDE_DIR ${GNULIB_DIR}/include)
set(LISTLIB_INCLUDE_DIR ${LISTLIB_DIR}/include)

set(GNULIB_LIBRARY gnulib)
set(UTILS_LIBRARY utils)
set(LISTLIB_LIBRARY listlib)
set(HDBSCAN_LIBRARY hdbscan)

set(HDBSCAN_LIB_DIR ${PROJECT_BINARY_DIR}/src)
set(UTILS_LIB_DIR ${PROJECT_BINARY_DIR}/modules/utils/src)
set(GNULIB_LIB_DIR ${PROJECT_BINARY_DIR}/modules/gnulib/src)
set(LISTLIB_LIB_DIR ${PROJECT_BINARY_DIR}/modules/listlib/src)
set(JAVA_LIB_DIR ${PROJECT_BINARY_DIR}/modules/java)

set(PRIVATE_LIBS "-l${HDBSCAN_LIBRARY} -l${UTILS_LIBRARY} -l${LISTLIB_LIBRARY} -l${GNULIB_LIBRARY}")

if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR ${INSTALL_LIB_DIR}/CMake/hdbscan)
endif()

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE SHARE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

#find_package(GSL REQUIRED) 

#ensure we have pkg-config in the system
find_package(PkgConfig)

# lookup gli-2.0 in pkg-config
pkg_check_modules(GLIB2 REQUIRED glib-2.0>=2.36.0)

# A little hack for the python bindings
list(GET GLIB2_INCLUDE_DIRS 0 GLIB2_FIRST_INDCLUDE_DIR)
list(GET GLIB2_INCLUDE_DIRS 1 GLIB2_SECOND_INDCLUDE_DIR)
message("GLIB2_FIRST_INDCLUDE_DIR is ${GLIB2_FIRST_INDCLUDE_DIR}")
message("GLIB2_SECOND_INDCLUDE_DIR is ${GLIB2_SECOND_INDCLUDE_DIR}")

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

# ... config files
configure_file(config.h.in "${PROJECT_BINARY_DIR}/config.h" @ONLY)# set up include-directories
configure_file(hdbscan.pc.in "${PROJECT_BINARY_DIR}/hdbscan.pc" @ONLY)# set up include-directories

include_directories("${PROJECT_BINARY_DIR}" ${UTILS_INCLUDE_DIR})  # to find config.h

add_subdirectory(src)
add_subdirectory(modules)

IF(BUILD_SAMPLES)
	add_subdirectory(sample)
ENDIF()
# Add all targets to the build-tree export set
export(TARGETS ${HDBSCAN_LIBRARY}-${HDBSCAN_VERSION} gnulib listlib FILE "${PROJECT_BINARY_DIR}/HdbscanTargets.cmake")
 
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE hdbscan)

# Create the HdbscanConfig.cmake and HdbscanConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}" "${INSTALL_INCLUDE_DIR}")

# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file(HdbscanConfig.cmake.in "${PROJECT_BINARY_DIR}/HdbscanConfig.cmake" @ONLY)

# ... for the install tree
set(CONF_INCLUDE_DIRS "${INSTALL_INCLUDE_DIR}")
set(CONF_LIB_DIRS "${INSTALL_LIB_DIR}")
set(CONF_HDBSCAN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")
message("HDBSCAN to be installed in ${CMAKE_INSTALL_PREFIX}")
configure_file(HdbscanConfig.cmake.in "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/HdbscanConfig.cmake" @ONLY)

# ... for both
configure_file(HdbscanConfigVersion.cmake.in "${PROJECT_BINARY_DIR}/HdbscanConfigVersion.cmake" @ONLY)

set_target_properties(${HDBSCAN_LIBRARY}-${HDBSCAN_VERSION} PROPERTIES
  PUBLIC_HEADER "include/hdbscan/*.h")

# Install the HdbscanConfig.cmake and HdbscanConfigVersion.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/HdbscanConfig.cmake"
  "${PROJECT_BINARY_DIR}/HdbscanConfigVersion.cmake"
  DESTINATION "${INSTALL_LIB_DIR}/cmake" COMPONENT dev)
 
# Install the export set for use with the install-tree
INSTALL (DIRECTORY ${HDBSCAN_INCLUDE_DIR}/hdbscan DESTINATION ${INSTALL_INCLUDE_DIR})
INSTALL (DIRECTORY ${UTILS_INCLUDE_DIR}/hdbscan DESTINATION ${INSTALL_INCLUDE_DIR})
INSTALL (DIRECTORY ${GNULIB_INCLUDE_DIR}/gnulib DESTINATION ${INSTALL_INCLUDE_DIR})
INSTALL (DIRECTORY ${LISTLIB_INCLUDE_DIR}/listlib DESTINATION ${INSTALL_INCLUDE_DIR})
INSTALL (FILES ${PROJECT_BINARY_DIR}/config.h DESTINATION ${INSTALL_INCLUDE_DIR}/gnulib)
INSTALL (FILES ${PROJECT_BINARY_DIR}/hdbscan.pc DESTINATION ${INSTALL_LIB_DIR}/pkgconfig)

INSTALL (EXPORT HdbscanTargets DESTINATION "${INSTALL_LIB_DIR}/cmake" COMPONENT dev)
