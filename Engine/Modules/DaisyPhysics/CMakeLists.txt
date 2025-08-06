set(DAISY_PHYSICS_SOURCES
    Source/DaisyPhysics.cpp
)

set(DAISY_PHYSICS_HEADERS
    Include/DaisyPhysics.h
)

add_library(DaisyPhysics STATIC ${DAISY_PHYSICS_SOURCES} ${DAISY_PHYSICS_HEADERS})

target_include_directories(DaisyPhysics 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(DaisyPhysics 
    PUBLIC 
        DaisyCore
)

source_group("Header Files" FILES ${DAISY_PHYSICS_HEADERS})
source_group("Source Files" FILES ${DAISY_PHYSICS_SOURCES})