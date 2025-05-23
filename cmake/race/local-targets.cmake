# Enable non-unique custom local targets (e.g., format)

# Duplicate targets are only supported for makefiles
if("${CMAKE_GENERATOR}" MATCHES "Make")
    set_property(GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS 1)
    set(ALLOW_LOCAL_TARGETS 1)
endif()

function(add_local_target target)
    if(ALLOW_LOCAL_TARGETS)
        cmake_parse_arguments(LOCAL_TARGET "" "" "DEPENDS" ${ARGN})
        add_custom_target(${target} DEPENDS ${LOCAL_TARGET_DEPENDS})
    endif()
endfunction()
