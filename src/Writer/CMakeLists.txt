set(SOURCES
    writer_col.c
    writer_file.c
    writer_size.c
)

add_library(writer STATIC ${SOURCES})

target_include_directories(writer PUBLIC include/${CMAKE_CURRENT_SOURCE_DIR})