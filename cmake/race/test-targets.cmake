include(race/local-targets)

enable_testing()

function(setup_project_test_targets project_name)
    cmake_parse_arguments(TEST "UNIT;INTEGRATION" "" "" ${ARGN})

    # Create custom build-tests target for the project
    add_custom_target(build_${project_name}_tests)
    add_local_target(build_tests DEPENDS build_${project_name}_tests)

    # Create custom run-tests target for the project
    add_custom_target(run_${project_name}_tests
        COMMAND ${CMAKE_CTEST_COMMAND} -L ${project_name} --output-on-failure \$\(ARGS\)
        DEPENDS build_${project_name}_tests
    )
    add_local_target(run_tests DEPENDS run_${project_name}_tests)

    # Create custom run-unit-tests target for the project, if enabled
    if (TEST_UNIT)
        add_custom_target(run_${project_name}_unit_tests
            COMMAND ${CMAKE_CTEST_COMMAND} -L unit -L ${project_name} --output-on-failure \$\(ARGS\)
            DEPENDS build_${project_name}_tests
        )
        add_local_target(run_unit_tests DEPENDS run_${project_name}_unit_tests)
    endif()

    # Create custom run-integration-tests target for the project, if enabled
    if (TEST_INTEGRATION)
        add_custom_target(run_${project_name}_integ_tests
            COMMAND ${CMAKE_CTEST_COMMAND} -L integration -L ${project_name} --output-on-failure \$\(ARGS\)
            DEPENDS build_${project_name}_tests
        )
        add_local_target(run_integ_tests DEPENDS run_${project_name}_integ_tests)
    endif()
endfunction()
