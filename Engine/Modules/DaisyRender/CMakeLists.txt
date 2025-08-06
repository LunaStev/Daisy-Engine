set(DAISY_RENDER_SOURCES
    Source/DaisyRender.cpp
)

set(DAISY_RENDER_HEADERS
    Include/DaisyRender.h
)

add_library(DaisyRender STATIC ${DAISY_RENDER_SOURCES} ${DAISY_RENDER_HEADERS})

target_include_directories(DaisyRender 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
        ${Vulkan_INCLUDE_DIRS}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_SOURCE_DIR}/Engine/Modules/Platform/Include
)

target_link_libraries(DaisyRender 
    PUBLIC 
        DaisyCore
        DaisyPlatform
        ${Vulkan_LIBRARIES}
)

source_group("Header Files" FILES ${DAISY_RENDER_HEADERS})
source_group("Source Files" FILES ${DAISY_RENDER_SOURCES})