
if (PYTHON)
    set(SETUP_PY_IN "${PYTHON_MODULE_DIR}/setup.py.in")
    set(SETUP_PY    "${CMAKE_CURRENT_BINARY_DIR}/setup.py")
    set(DEPS        "${PYTHON_MODULE_DIR}/__init__.py")
    
    configure_file(${SETUP_PY_IN} ${SETUP_PY})
    add_custom_command(OUTPUT ${OUTPUT}
                       COMMAND ${PYTHON} ${SETUP_PY} build
                       COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT}
                       DEPENDS ${DEPS})

    add_custom_target(${TARGET_NAME} ALL DEPENDS ${OUTPUT})

    install(CODE "execute_process(COMMAND ${PYTHON} ${SETUP_PY} install)")
endif()
