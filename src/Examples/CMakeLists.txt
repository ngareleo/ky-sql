add_executable(run_example_00_filemetadata 00_filemetadata.c)

target_compile_definitions(run_example_00_filemetadata PRIVATE DEBUG_MODE=1)

target_compile_options(run_example_00_filemetadata PRIVATE -Wall -Wextra)

target_include_directories(run_example_00_filemetadata PRIVATE 
	${CMAKE_SOURCE_DIR}/src/Metadata
	${CMAKE_SOURCE_DIR}/src/Mocks
)

target_link_libraries(run_example_00_filemetadata PRIVATE metadata m mocks)

add_custom_target(run_all_examples
	COMMAND run_example_00_filemetadata
	DEPENDS run_example_00_filemetadata
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Running all example executables"
)
