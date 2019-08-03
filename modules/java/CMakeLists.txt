cmake_minimum_required(VERSION 2.8.6)
project (hdbscan_java)

find_package(JNI REQUIRED)
find_package(Java REQUIRED)
include(UseJava)
set(CMAKE_JNI_TARGET TRUE)

#Create_javah(
#	TARGET target_headers
#	CLASSES hdbscan.Hdbscan
#)
set(JAVA_SOURCE_FILES src/hdbscan/Hdbscan.java)
set(LIB_VERSION ${HDBSCAN_MAJOR_VERSION}${HDBSCAN_MINOR_VERSION}${HDBSCAN_PATCH_VERSION})
add_jar(hdbscan-${LIB_VERSION} ${JAVA_SOURCE_FILES})

set(HDBSCAN_JNI_SRC src/Hdbscan.cpp )

add_library(${PROJECT_NAME}-${LIB_VERSION} SHARED ${HDBSCAN_JNI_SRC} )
target_link_libraries(${PROJECT_NAME}-${LIB_VERSION} LINK_PUBLIC ${HDBSCAN_LIBRARY}_static)
include_directories(${HDBSCAN_INCLUDE_DIR} ${LISTLIB_INCLUDE_DIR} ${GNULIB_INCLUDE_DIR} ${JNI_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -Wall -fmessage-length=0 -fPIC -O3 -std=c++11 -lc -shared")
SET_TARGET_PROPERTIES(${PROJECT_NAME}-${LIB_VERSION} PROPERTIES LINKER_LANGUAGE CXX)

INSTALL (FILES ${JAVA_LIB_DIR}/hdbscan-${LIB_VERSION}.jar DESTINATION ${INSTALL_SHARE_DIR}/java)
INSTALL (FILES ${JAVA_LIB_DIR}/lib${PROJECT_NAME}-${LIB_VERSION}.so DESTINATION ${INSTALL_SHARE_DIR}/java)
INSTALL(CODE "execute_process( \
    COMMAND ${CMAKE_COMMAND} -E create_symlink \
    ${INSTALL_SHARE_DIR}/java/lib${PROJECT_NAME}-${LIB_VERSION}.so \
    ${INSTALL_SHARE_DIR}/java/lib${PROJECT_NAME}.so   \
    )"
)