set(DAISY_SOUND_SOURCES
    Source/DaisySound.cpp
)

set(DAISY_SOUND_HEADERS
    Include/DaisySound.h
)

add_library(DaisySound STATIC ${DAISY_SOUND_SOURCES} ${DAISY_SOUND_HEADERS})

target_include_directories(DaisySound 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(DaisySound 
    PUBLIC 
        DaisyCore
)

source_group("Header Files" FILES ${DAISY_SOUND_HEADERS})
source_group("Source Files" FILES ${DAISY_SOUND_SOURCES})