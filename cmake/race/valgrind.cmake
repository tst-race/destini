include(race/local-targets)

find_program(VALGRIND_TOOL valgrind REQUIRED)

function(setup_valgrind_for_target TARGET_NAME)
    set(OPTIONS EXCLUDE_FROM_ALL)
    set(ONE_VALUE_ARGS "")
    set(MULTI_VALUE_ARGS DEPENDS EXECUTABLE_ARGS)
    cmake_parse_arguments(TEST "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

    set(VALGRIND_OUT_FILE ${TARGET_NAME}.valgrind.txt)
    set(VALGRIND_CMD
        ${VALGRIND_TOOL}
        --leak-check=full
        --show-leak-kinds=all
        --track-origins=yes
        --verbose
        --log-file=${VALGRIND_OUT_FILE}
        --error-exitcode=1
        ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME} ${TEST_EXECUTABLE_ARGS}
    )

    add_custom_target(valgrind_${TARGET_NAME}
        COMMAND echo "=================== valgrind ===================="
        COMMAND echo ${VALGRIND_CMD}
        COMMAND ${VALGRIND_CMD}

        BYPRODUCTS ${VALGRIND_OUT_FILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${TARGET_NAME} ${TEST_DEPENDS}
        VERBATIM
        COMMENT "Running test through valgrind"
    )
    if(NOT ${TEST_EXCLUDE_FROM_ALL})
        add_local_target(valgrind DEPENDS valgrind_${TARGET_NAME})
    endif()
endfunction(setup_valgrind_for_target)