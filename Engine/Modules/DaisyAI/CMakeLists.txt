set(DAISY_AI_SOURCES
    Source/DaisyAI.cpp
)

set(DAISY_AI_HEADERS
    Include/DaisyAI.h
)

add_library(DaisyAI STATIC ${DAISY_AI_SOURCES} ${DAISY_AI_HEADERS})

target_include_directories(DaisyAI 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(DaisyAI 
    PUBLIC 
        DaisyCore
)

source_group("Header Files" FILES ${DAISY_AI_HEADERS})
source_group("Source Files" FILES ${DAISY_AI_SOURCES})