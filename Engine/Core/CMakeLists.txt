set(DAISY_CORE_SOURCES
    Source/Engine.cpp
    Source/DaisyEngine.cpp
    Source/Logger.cpp
    Source/Math.cpp
    Source/Memory.cpp
)

set(DAISY_CORE_HEADERS
    Include/DaisyEngine.h
    Include/Core/Engine.h
    Include/Core/Module.h
    Include/Core/Logger.h
    Include/Core/Math.h
    Include/Core/Memory.h
)

add_library(DaisyCore STATIC ${DAISY_CORE_SOURCES} ${DAISY_CORE_HEADERS})

target_include_directories(DaisyCore 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(DaisyCore PRIVATE)

if(WIN32)
    target_link_libraries(DaisyCore PRIVATE winmm.lib)
endif()

# Group source files for IDEs
source_group("Header Files" FILES ${DAISY_CORE_HEADERS})
source_group("Source Files" FILES ${DAISY_CORE_SOURCES})