cmake_minimum_required(VERSION 2.8.6)
project (hdbscan_jni)

find_package(JNI REQUIRED)
find_package(Java REQUIRED)
include(UseJava)
set(CMAKE_JNI_TARGET TRUE)

#Create_javah(
#	TARGET target_headers
#	CLASSES hdbscan.Hdbscan
#)
set(JAVA_SOURCE_FILES src/hdbscan/Hdbscan.java)
add_jar(hdbscan_java ${JAVA_SOURCE_FILES})
#get_target_property(_jarFile hdbscan_java JAR_FILE)
#get_target_property(_classDir hdbscan_java CLASSDIR)

set(HDBSCAN_JNI_SRC src/Hdbscan.cpp )

add_library(hdbscan_jni SHARED ${HDBSCAN_JNI_SRC} )
target_link_libraries(hdbscan_jni LINK_PUBLIC hdbscan gnulib listlib)
include_directories(${HDBSCAN_INCLUDE_DIR} ${GLIB2_INCLUDE_DIRS} ${LISTLIB_INCLUDE_DIR} ${GNULIB_INCLUDE_DIR} ${JNI_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -Wall -fmessage-length=0 -fPIC -O3 -std=c++11 -lc -shared")
SET_TARGET_PROPERTIES(hdbscan_jni PROPERTIES LINKER_LANGUAGE CXX)

