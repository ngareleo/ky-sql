set(SOURCES
	count.c
	now_default_value.c
)

add_library(utilities STATIC ${SOURCES})

target_include_directories(utilities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})