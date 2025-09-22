set(SOURCES
    translation_context.c
    translation_insert.c
    translation_query.c

)

add_library(translation STATIC ${SOURCES})

target_include_directories(translation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)