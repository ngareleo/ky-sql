set (INNER_MODULES
    utilities
    types
    shell
    mocks
    metadata
    lang
    translation
    writer
    reader
)

function(add_example name src)

    set(target "run_example_${name}")
    add_executable(${target} ${src})
    target_compile_definitions(${target} PRIVATE DEBUG_MODE=1)
    target_compile_options(${target} PRIVATE 
        -Wall 
        -Wextra   
        -Werror=implicit-function-declaration
        -Werror=incompatible-pointer-types
    )
    
    target_include_directories(
        ${target} 
        PRIVATE
        ${CMAKE_SOURCE_DIR}/src/Metadata
        ${CMAKE_SOURCE_DIR}/src/Mocks
    )
    
    target_link_libraries(${target} PRIVATE m ${INNER_MODULES})
    
    set(run_target "run_${target}")
        add_custom_target(${run_target}
        DEPENDS ${target}
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${target}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running ${target}"
    )

    if(NOT DEFINED EXAMPLE_RUN_TARGETS)
        set(EXAMPLE_RUN_TARGETS ${run_target} PARENT_SCOPE)
    else()
        set(EXAMPLE_RUN_TARGETS "${EXAMPLE_RUN_TARGETS};${run_target}" PARENT_SCOPE)
    endif()
endfunction()

add_example(00_filemetadata 00_filemetadata.c )
add_example(01_datasection 	01_datasection.c  )

add_custom_target(run_all_examples
    DEPENDS ${EXAMPLE_RUN_TARGETS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running all examples"
)
