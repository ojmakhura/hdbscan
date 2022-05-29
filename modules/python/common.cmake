set(SETUP_PY_IN "${PYTHON_MODULE_DIR}/setup.py.in")
set(SETUP_PY    "${CMAKE_CURRENT_BINARY_DIR}/setup.py")
set(DEPS        "${PYTHON_MODULE_DIR}/__init__.py")
    
configure_file(${SETUP_PY_IN} ${SETUP_PY})
add_custom_command(
    OUTPUT ${OUTPUT}
    COMMAND ${PYTHON} ${SETUP_PY} build
    COMMAND ${PYTHON} ${SETUP_PY} bdist
    COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT}
    DEPENDS ${DEPS}
)

add_custom_target(${TARGET_NAME} ALL DEPENDS ${OUTPUT})
# message(${TARGET_NAME} ${CMAKE_CURRENT_BINARY_DIR})
# message(${OUTPUT}})

# install(CODE "execute_process(COMMAND ${PYTHON} ${SETUP_PY} install --prefix=${CMAKE_INSTALL_PREFIX})")

#INSTALL (FILES ${TARGET_NAME}/hdbscan.pc DESTINATION ${INSTALL_LIB_DIR}/pkgconfig COMPONENT dev)
