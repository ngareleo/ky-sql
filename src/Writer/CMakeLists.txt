set(SOURCES
    writer.c
)

add_library(writer STATIC ${SOURCES})

target_include_directories(writer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})