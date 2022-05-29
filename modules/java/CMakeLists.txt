project (${HDBSCAN_JAVA_LIBRARY} VERSION ${HDBSCAN_VERSION})

OPTION (MVN_INSTALL "Install jar to mvn repository" OFF)

find_package(JNI REQUIRED)
find_package(Java REQUIRED)
include(UseJava)
set(CMAKE_JNI_TARGET TRUE)

set(HDBSCAN_JNI_SRC src/Hdbscan.cpp )
set(LIB_VERSION ${HDBSCAN_MAJOR_VERSION}.${HDBSCAN_MINOR_VERSION}.${HDBSCAN_PATCH_VERSION})
set(JAVA_SOURCE_FILES src/bw/co/roguesystems/Hdbscan.java)

add_library(${PROJECT_NAME} SHARED ${HDBSCAN_JNI_SRC} )
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${HDBSCAN_LIBRARY} ${JAVA_JVM_LIBRARY})
include_directories(${HDBSCAN_INCLUDE_DIR} ${COLLECTIONS_INCLUDE_DIR} ${JNI_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -Wall -fmessage-length=0 -fPIC -O3 -std=c++11 -lc -shared")
SET_TARGET_PROPERTIES(
    ${PROJECT_NAME} PROPERTIES 
    LINKER_LANGUAGE CXX
    VERSION ${PROJECT_VERSION}
    SOVERSION ${HDBSCAN_MAJOR_VERSION}
)

add_jar(
    ${HDBSCAN_LIBRARY}.jar ${JAVA_SOURCE_FILES}
    OUTPUT_NAME ${HDBSCAN_LIBRARY}
)
install_jar(${HDBSCAN_LIBRARY}.jar DESTINATION ${INSTALL_SHARE_DIR}/java COMPONENT jar)

# INSTALL (FILES ${JAVA_LIB_DIR}/lib${PROJECT_NAME}-${HDBSCAN_VERSION}.so DESTINATION ${INSTALL_JNI_LIB_DIR})

install(TARGETS ${PROJECT_NAME}
  EXPORT HdbscanTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT java
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT java
)

# if(${MVN_INSTALL})
# INSTALL(CODE "execute_process( \
#     COMMAND mvn install:install-file \
#         -Dfile=modules/java/hdbscan.jar -DgroupId=bw.co.roguesystems \
#         -DartifactId=hdbscan \
#         -Dversion=${LIB_VERSION} -Dpackaging=jar  \
#     )"
# )
# endif()
