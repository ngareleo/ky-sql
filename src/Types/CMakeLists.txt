set(SOURCES
    data_block_type.c
)

add_library(types STATIC ${SOURCES})

target_include_directories(types PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})