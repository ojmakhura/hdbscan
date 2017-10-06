project (listlib)

file(GLOB_RECURSE LISTLIB_SRC_FILES ${LISTLIB_SRC_DIR}/*.c)
add_library(listlib SHARED ${LISTLIB_SRC_FILES})
