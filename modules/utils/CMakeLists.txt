set (UTILS_INCLUDE_DIR ${UTILS_DIR}/include)
set (UTILS_SRC_DIR ${UTILS_DIR}/src)

include_directories (${UTILS_INCLUDE_DIR})
add_subdirectory (${UTILS_SRC_DIR})

set_target_properties(${UTILS_LIBRARY} PROPERTIES
  PUBLIC_HEADER "include/hdbscan/*.h"
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
)