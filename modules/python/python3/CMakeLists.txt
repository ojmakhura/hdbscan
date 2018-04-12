find_program(PYTHON3 "python3")

set(PYTHON ${PYTHON3})
SET(TARGET_NAME "python3")
set(OUTPUT      "${CMAKE_CURRENT_BINARY_DIR}/build/py3_timestamp")
include(../common.cmake)
unset(OUTPUT)
unset(PYTHON)
unset(TARGET_NAME)
