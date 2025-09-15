set(SOURCES
    translation.c
)

add_library(translation STATIC ${SOURCES})

target_include_directories(translation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})