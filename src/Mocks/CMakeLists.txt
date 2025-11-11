set(SOURCES
    mocks_metadata_defintions.c
    mocks_insert_statements.c
)

add_library(mocks STATIC ${SOURCES})

target_include_directories(mocks PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
