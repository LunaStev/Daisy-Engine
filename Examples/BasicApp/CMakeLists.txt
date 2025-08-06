set(BASIC_APP_SOURCES
    main.cpp
)

add_executable(DaisyEngineExample ${BASIC_APP_SOURCES})

target_include_directories(DaisyEngineExample 
    PRIVATE
        ${CMAKE_SOURCE_DIR}/Engine/Core/Include
        ${CMAKE_SOURCE_DIR}/Engine/Modules/Platform/Include
        ${CMAKE_SOURCE_DIR}/Engine/Modules/DaisyPhysics/Include
        ${CMAKE_SOURCE_DIR}/Engine/Modules/DaisyRender/Include
        ${CMAKE_SOURCE_DIR}/Engine/Modules/DaisySound/Include
        ${CMAKE_SOURCE_DIR}/Engine/Modules/DaisyAI/Include
        ${CMAKE_SOURCE_DIR}/Engine/Modules/DaisyNet/Include
        ${CMAKE_SOURCE_DIR}/Engine/Modules/WorldStreamer/Include
        ${CMAKE_SOURCE_DIR}/Engine/Modules/ScriptSystem/Include
)

target_link_libraries(DaisyEngineExample
    DaisyCore
    DaisyPlatform
    DaisyPhysics
    DaisyRender
    DaisySound
    DaisyAI
    DaisyNet
    WorldStreamer
    ScriptSystem
)

# Copy any required assets to build directory
add_custom_command(TARGET DaisyEngineExample POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:DaisyEngineExample>/Scripts
    COMMENT "Creating Scripts directory"
)

# Set the working directory for debugging (Visual Studio)
set_target_properties(DaisyEngineExample PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)