project (utils)

file(GLOB_RECURSE UTILS_SRC_FILES ${UTILS_SRC_DIR}/*.c)

add_library(${UTILS_LIBRARY} STATIC ${UTILS_SRC_FILES})
target_link_libraries(${UTILS_LIBRARY} LINK_PUBLIC m )
include_directories(${UTILS_INCLUDE_DIR})

install(TARGETS utils
  # IMPORTANT: Add the listlib library to the "export-set"
  EXPORT HdbscanTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib  
)
