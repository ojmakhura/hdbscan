find_program(PYTHON "python3")

SET(TARGET_NAME "python3")
set(OUTPUT      "${CMAKE_CURRENT_BINARY_DIR}/build/py3_timestamp")
include(../common.cmake)
unset(OUTPUT)
unset(TARGET_NAME)
