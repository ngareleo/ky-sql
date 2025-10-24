set(SOURCES
	count.c
	now_default_value.c
	null_guard_str.c
	allocation.c
)

add_library(utilities STATIC ${SOURCES})

target_include_directories(utilities PUBLIC include/${CMAKE_CURRENT_SOURCE_DIR})