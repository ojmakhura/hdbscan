project (hdbscan_java)

OPTION (MVN_INSTALL "Install jar to mvn repository" OFF)

find_package(JNI REQUIRED)
find_package(Java REQUIRED)
include(UseJava)
set(CMAKE_JNI_TARGET TRUE)

set(HDBSCAN_JNI_SRC src/Hdbscan.cpp )
set(LIB_VERSION ${HDBSCAN_MAJOR_VERSION}.${HDBSCAN_MINOR_VERSION}.${HDBSCAN_PATCH_VERSION})
set(JAVA_SOURCE_FILES src/bw/co/roguesystems/hdbscan/Hdbscan.java)

add_library(${PROJECT_NAME}-${LIB_VERSION} SHARED ${HDBSCAN_JNI_SRC} )
target_link_libraries(${PROJECT_NAME}-${LIB_VERSION} LINK_PUBLIC ${HDBSCAN_LIBRARY}_static ${JAVA_JVM_LIBRARY})
include_directories(${HDBSCAN_INCLUDE_DIR} ${LISTLIB_INCLUDE_DIR} ${JNI_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -Wall -fmessage-length=0 -fPIC -O3 -std=c++11 -lc -shared")
SET_TARGET_PROPERTIES(${PROJECT_NAME}-${LIB_VERSION} PROPERTIES LINKER_LANGUAGE CXX)

add_jar(
    hdbscan ${JAVA_SOURCE_FILES}
)
install_jar(hdbscan DESTINATION ${INSTALL_SHARE_DIR}/java)

INSTALL (FILES ${JAVA_LIB_DIR}/lib${PROJECT_NAME}-${LIB_VERSION}.so DESTINATION ${INSTALL_JNI_LIB_DIR})
INSTALL(CODE "execute_process( \
    COMMAND ${CMAKE_COMMAND} -E create_symlink \
    ${INSTALL_JNI_LIB_DIR}/lib${PROJECT_NAME}-${LIB_VERSION}.so \
    ${INSTALL_JNI_LIB_DIR}/lib${PROJECT_NAME}.so   \
    )"
)

if(${MVN_INSTALL})
INSTALL(CODE "execute_process( \
    COMMAND mvn install:install-file \
        -Dfile=modules/java/hdbscan.jar -DgroupId=bw.co.roguesystems.hdbscan
        -DartifactId=hdbscan \
        -Dversion=${LIB_VERSION} -Dpackaging=jar  \
    )"
)
endif()
