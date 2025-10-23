set(SOURCES
	metadata_default.c
	metadata_extraction.c
	metadata_offsets.c
	metadata_schema.c
	metadata_storage.c
)

add_library(metadata STATIC ${SOURCES})

target_include_directories(metadata PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)