set(SOURCES
    reader.c
)

add_library(reader STATIC ${SOURCES})

target_include_directories(reader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})