find_program(PYTHON2 "python2")

set(PYTHON ${PYTHON2})
SET(TARGET_NAME "python2")
set(OUTPUT      "${CMAKE_CURRENT_BINARY_DIR}/build/py2_timestamp")

include(../common.cmake)
unset(OUTPUT)
unset(PYTHON)
unset(TARGET_NAME)
