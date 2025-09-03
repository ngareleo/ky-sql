add_library(mocks STATIC mocks_metadata_defintions.c)

target_include_directories(mocks PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
