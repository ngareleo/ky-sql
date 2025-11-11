set(SOURCES
    shell_tables.c
)

add_library(shell STATIC ${SOURCES})

target_include_directories(shell PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})