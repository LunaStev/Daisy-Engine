cmake_minimum_required(VERSION 3.20)
project(DaisyEngine VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build configuration
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)

# Platform detection - Windows only (temporarily disabled for WSL development)
# if(NOT WIN32)
#     message(FATAL_ERROR "Daisy Engine currently supports Windows only")
# endif()
add_compile_definitions(DAISY_PLATFORM_WINDOWS)

# Compiler flags
if(MSVC)
    add_compile_options(/W4 /permissive-)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Debug/Release flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DAISY_DEBUG)
    if(MSVC)
        add_compile_options(/Od /Zi)
    else()
        add_compile_options(-O0 -g)
    endif()
else()
    add_compile_definitions(DAISY_RELEASE)
    if(MSVC)
        add_compile_options(/O2)
    else()
        add_compile_options(-O3 -DNDEBUG)
    endif()
endif()

# Find packages - Windows specific
find_package(Vulkan REQUIRED)

# Windows-specific libraries
if(MSVC)
    # Link with Windows libraries for graphics and input
    link_libraries(
        user32.lib
        gdi32.lib
        winmm.lib
        imm32.lib
        ole32.lib
        oleaut32.lib
        version.lib
        uuid.lib
        advapi32.lib
        dinput8.lib
        dxguid.lib
    )
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Engine/Core/Include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Engine/Modules)
include_directories(${Vulkan_INCLUDE_DIRS})

# Engine modules
add_subdirectory(Engine/Core)
add_subdirectory(Engine/Modules/Platform)
add_subdirectory(Engine/Modules/DaisyPhysics)
add_subdirectory(Engine/Modules/DaisyRender)
add_subdirectory(Engine/Modules/DaisySound)
add_subdirectory(Engine/Modules/DaisyAI)
add_subdirectory(Engine/Modules/DaisyNet)
add_subdirectory(Engine/Modules/WorldStreamer)
add_subdirectory(Engine/Modules/ScriptSystem)

# Example application
add_subdirectory(Examples/BasicApp)

# Daisy Editor
add_subdirectory(Editor/DaisyEditor)

# Tests (optional)
option(DAISY_BUILD_TESTS "Build tests" OFF)
if(DAISY_BUILD_TESTS)
    enable_testing()
    add_subdirectory(Tests)
endif()

# Installation
install(DIRECTORY Engine/Core/Include/ DESTINATION include/DaisyEngine)
install(DIRECTORY Engine/Modules/ DESTINATION include/DaisyEngine/Modules 
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")