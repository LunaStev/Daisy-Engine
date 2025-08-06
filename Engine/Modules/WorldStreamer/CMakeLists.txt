set(WORLD_STREAMER_SOURCES
    Source/WorldStreamer.cpp
)

set(WORLD_STREAMER_HEADERS
    Include/WorldStreamer.h
)

add_library(WorldStreamer STATIC ${WORLD_STREAMER_SOURCES} ${WORLD_STREAMER_HEADERS})

target_include_directories(WorldStreamer 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(WorldStreamer 
    PUBLIC 
        DaisyCore
)

source_group("Header Files" FILES ${WORLD_STREAMER_HEADERS})
source_group("Source Files" FILES ${WORLD_STREAMER_SOURCES})