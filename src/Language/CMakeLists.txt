set(SOURCES
	language_insert.c
	language_query.c
)

add_library(lang STATIC ${SOURCES})

target_include_directories(lang PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})