project (gnulib)

file(GLOB_RECURSE GNULIB_SRC_FILES ${GNULIB_SRC_DIR}/*.c)

add_library(gnulib SHARED ${GNULIB_SRC_FILES})
