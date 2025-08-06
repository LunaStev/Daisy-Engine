#include "DaisyEditor.h"
#include "Core/Logger.h"
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "===========================================================" << std::endl;
    std::cout << "                    DAISY EDITOR                          " << std::endl;
    std::cout << "         THE END Series Game Engine Editor                " << std::endl;
    std::cout << "===========================================================" << std::endl;
    
    DAISY_INFO("Starting Daisy Editor...");
    
    // Initialize editor
    if (!DAISY_EDITOR.Initialize()) {
        DAISY_ERROR("Failed to initialize Daisy Editor");
        return -1;
    }
    
    // Create some default content for demonstration
    auto* scene = DAISY_EDITOR.GetCurrentScene();
    if (scene) {
        // Create some sample entities
        auto* earth = scene->CreateEntity("Earth");
        earth->GetTransform().position = Daisy::Vector3(0, 0, 0);
        earth->GetTransform().scale = Daisy::Vector3(6371000, 6371000, 6371000); // Earth radius
        
        auto* moon = scene->CreateEntity("Moon");
        moon->GetTransform().position = Daisy::Vector3(384400000, 0, 0); // Moon distance
        moon->GetTransform().scale = Daisy::Vector3(1737400, 1737400, 1737400); // Moon radius
        
        auto* spaceStation = scene->CreateEntity("International Space Station");
        spaceStation->GetTransform().position = Daisy::Vector3(400000, 0, 0); // ISS orbit
        spaceStation->GetTransform().scale = Daisy::Vector3(100, 100, 100);
        
        // Create a hierarchy
        auto* earthSystem = scene->CreateEntity("Earth System");
        earth->SetParent(earthSystem);
        
        auto* solarSystem = scene->CreateEntity("Solar System");
        earthSystem->SetParent(solarSystem);
        
        DAISY_INFO("Created sample scene with {} entities", scene->GetEntities().size());
    }
    
    // Set editor camera to a good starting position
    auto* camera = DAISY_EDITOR.GetEditorCamera();
    if (camera) {
        camera->SetPosition(Daisy::Vector3(10000000, 5000000, 10000000)); // High orbit view
        camera->LookAt(Daisy::Vector3(0, 0, 0)); // Look at Earth
        camera->SetOrbitMode(true);
    }
    
    std::cout << std::endl;
    std::cout << "Editor Controls (Simulation):" << std::endl;
    std::cout << "- The editor will run for a demonstration" << std::endl;
    std::cout << "- In a real implementation, this would show:" << std::endl;
    std::cout << "  * 3D Viewport with scene rendering" << std::endl;
    std::cout << "  * Scene Hierarchy with drag & drop" << std::endl;
    std::cout << "  * Inspector with component editing" << std::endl;
    std::cout << "  * Asset Browser with thumbnails" << std::endl;
    std::cout << "  * Console with real-time logs" << std::endl;
    std::cout << "  * Menu bar with all editor functions" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Current Editor State:" << std::endl;
    std::cout << "- Scene: " << scene->GetName() << " (" << scene->GetEntities().size() << " entities)" << std::endl;
    std::cout << "- Camera: Position(" << camera->GetPosition().x << ", " 
              << camera->GetPosition().y << ", " << camera->GetPosition().z << ")" << std::endl;
    std::cout << "- Assets Directory: " << DAISY_EDITOR.GetAssetManager()->GetAssetsDirectory() << std::endl;
    std::cout << std::endl;
    
    // Run the editor
    DAISY_EDITOR.Run();
    
    // Shutdown
    DAISY_EDITOR.Shutdown();
    
    DAISY_INFO("Daisy Editor shut down successfully");
    
    std::cout << std::endl;
    std::cout << "===========================================================" << std::endl;
    std::cout << "             Thank you for using Daisy Editor!            " << std::endl;
    std::cout << "===========================================================" << std::endl;
    
    return 0;
}