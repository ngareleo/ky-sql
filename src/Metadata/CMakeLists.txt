set(METADATA_SOURCES
	metadata_extraction.c
	metadata_offsets.c
	metadata_schema.c
)

add_library(metadata STATIC ${METADATA_SOURCES})

target_include_directories(metadata PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})