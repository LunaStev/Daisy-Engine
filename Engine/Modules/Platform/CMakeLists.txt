cmake_minimum_required(VERSION 3.20)
project(DaisyPlatform)

set(CMAKE_CXX_STANDARD 20)

# Source files
set(SOURCES
    Source/DaisyPlatform.cpp
    Source/WindowsWindow.cpp
)

# Header files
set(HEADERS
    Include/DaisyPlatform.h
    Include/WindowsWindow.h
    Include/Platform.h
)

# Create library
add_library(DaisyPlatform ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(DaisyPlatform PUBLIC Include)
target_include_directories(DaisyPlatform PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Include
)

# Windows-specific libraries
if(WIN32)
    target_link_libraries(DaisyPlatform PRIVATE
        user32
        gdi32
        winmm
    )
endif()

# Link with core engine
target_link_libraries(DaisyPlatform PRIVATE DaisyCore)