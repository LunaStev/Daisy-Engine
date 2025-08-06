set(SCRIPT_SYSTEM_SOURCES
    Source/ScriptSystem.cpp
)

set(SCRIPT_SYSTEM_HEADERS
    Include/ScriptSystem.h
)

add_library(ScriptSystem STATIC ${SCRIPT_SYSTEM_SOURCES} ${SCRIPT_SYSTEM_HEADERS})

target_include_directories(ScriptSystem 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(ScriptSystem 
    PUBLIC 
        DaisyCore
)

source_group("Header Files" FILES ${SCRIPT_SYSTEM_HEADERS})
source_group("Source Files" FILES ${SCRIPT_SYSTEM_SOURCES})