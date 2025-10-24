set(SOURCES
    reader_file.c
    reader_data_block.c
)

add_library(reader STATIC ${SOURCES})

target_include_directories(reader PUBLIC include/${CMAKE_CURRENT_SOURCE_DIR})