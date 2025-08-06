set(DAISY_EDITOR_SOURCES
    main.cpp
    Source/DaisyEditor.cpp
    Source/EditorCore/Scene.cpp
    Source/EditorCore/EditorCamera.cpp
    Source/EditorCore/AssetManager.cpp
    Source/EditorUI/EditorWindow.cpp
)

set(DAISY_EDITOR_HEADERS
    Include/DaisyEditor.h
    Include/EditorCore/Scene.h
    Include/EditorCore/EditorCamera.h
    Include/EditorCore/AssetManager.h
    Include/EditorUI/EditorWindow.h
)

add_executable(DaisyEditor ${DAISY_EDITOR_SOURCES} ${DAISY_EDITOR_HEADERS})

target_include_directories(DaisyEditor 
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
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

target_link_libraries(DaisyEditor
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

# Create Assets directory structure
add_custom_command(TARGET DaisyEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:DaisyEditor>/Assets
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:DaisyEditor>/Assets/Scenes
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:DaisyEditor>/Assets/Meshes
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:DaisyEditor>/Assets/Textures
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:DaisyEditor>/Assets/Materials
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:DaisyEditor>/Assets/Sounds
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:DaisyEditor>/Assets/Scripts
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:DaisyEditor>/Assets/Shaders
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:DaisyEditor>/Assets/Fonts
    COMMENT "Creating Assets directory structure"
)

# Set the working directory for debugging
set_target_properties(DaisyEditor PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Group source files for IDEs
source_group("Header Files" FILES ${DAISY_EDITOR_HEADERS})
source_group("Source Files" FILES ${DAISY_EDITOR_SOURCES})
source_group("Header Files\\EditorCore" FILES 
    Include/EditorCore/Scene.h
    Include/EditorCore/EditorCamera.h
    Include/EditorCore/AssetManager.h
)
source_group("Source Files\\EditorCore" FILES 
    Source/EditorCore/Scene.cpp
    Source/EditorCore/EditorCamera.cpp
    Source/EditorCore/AssetManager.cpp
)
source_group("Header Files\\EditorUI" FILES Include/EditorUI/EditorWindow.h)
source_group("Source Files\\EditorUI" FILES Source/EditorUI/EditorWindow.cpp)