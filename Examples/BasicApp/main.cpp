#include "DaisyEngine.h"
#include "DaisyPhysics.h"
#include "DaisyRender.h"
#include "DaisySound.h"
#include "DaisyAI.h"
#include "DaisyNet.h"
#include "WorldStreamer.h"
#include "ScriptSystem.h"
#include <thread>
#include <chrono>

using namespace Daisy;

int main() {
    DAISY_INFO("Starting Daisy Engine Example Application");
    
    if (!DAISY_ENGINE.Initialize()) {
        DAISY_ERROR("Failed to initialize Daisy Engine");
        return -1;
    }
    
    auto* engine = DAISY_ENGINE.GetEngine();
    
    // Register all engine modules
    auto* physics = engine->RegisterModule<DaisyPhysics>();
    auto* renderer = engine->RegisterModule<DaisyRender>();
    auto* sound = engine->RegisterModule<DaisySound>();
    auto* ai = engine->RegisterModule<DaisyAI>();
    auto* network = engine->RegisterModule<DaisyNet>();
    auto* worldStreamer = engine->RegisterModule<WorldStreamer>();
    auto* scriptSystem = engine->RegisterModule<ScriptSystem>();
    
    DAISY_INFO("All modules registered successfully");
    
    // Create some procedural content positions
    Daisy::Vector3 earthPos(0, 0, 0);
    Daisy::Vector3 moonPos(384400000, 0, 0); // Moon distance from Earth
    Daisy::Vector3 stationPos(400000, 0, 0); // ISS-like orbit
    
    // Configure renderer for space simulation
    if (renderer) {
        renderer->SetViewport(0, 0, 1920, 1080);
        renderer->SetClearColor(0.02f, 0.02f, 0.1f, 1.0f);
        renderer->EnableInfiniteWorld(true);
        renderer->SetWorldScale(1e12); // Observable universe scale
        
        renderer->GenerateProceduralPlanet(earthPos, 6371000); // Earth radius
        renderer->GenerateProceduralPlanet(moonPos, 1737400);  // Moon radius
        renderer->GenerateProceduralShip(stationPos, 100);     // Space station
        
        // Add some cities on Earth
        renderer->GenerateProceduralCity(Daisy::Vector3(0, 6371000, 0), 1000000); // City on Earth surface
    }
    
    // Configure physics for space simulation
    if (physics) {
        physics->SetGlobalGravity(Daisy::Vector3(0, 0, 0)); // No global gravity in space
        physics->EnableFluidDynamics(true);
        
        // Add gravitational bodies
        physics->AddGravityWell(earthPos, 5.972e24f, 1e8f, true); // Earth
        physics->AddGravityWell(moonPos, 7.342e22f, 1e7f, true);  // Moon
        
        // Create some physics objects
        uint32_t ship = physics->CreateRigidBody(Daisy::Vector3(7000000, 0, 0), 10000); // Spacecraft
        physics->ApplyForce(ship, Daisy::Vector3(0, 1000, 0)); // Thrust
    }
    
    // Configure AI system
    if (ai) {
        ai->SetMaxAgents(1000);
        ai->EnableLearning(true);
        ai->SetSimulationSpeed(1.0f);
        
        // Create some AI agents
        for (int i = 0; i < 100; ++i) {
            Daisy::Vector3 pos(
                (rand() % 20000) - 10000,
                6371000 + 1000, // Above Earth surface
                (rand() % 20000) - 10000
            );
            
            uint32_t agentId = ai->CreateAIAgent("Citizen_" + std::to_string(i), pos);
            
            // Set random behaviors
            AIBehaviorType behaviors[] = {
                AIBehaviorType::Economic,
                AIBehaviorType::Social,
                AIBehaviorType::Exploration,
                AIBehaviorType::Survival
            };
            
            ai->SetAgentBehavior(agentId, behaviors[rand() % 4]);
            ai->SetAgentPersonality(agentId, 
                (rand() % 100) / 100.0f, // aggression
                (rand() % 100) / 100.0f, // intelligence
                (rand() % 100) / 100.0f  // cooperation
            );
        }
    }
    
    // Configure world streaming
    if (worldStreamer) {
        worldStreamer->EnableInfiniteWorld(true);
        worldStreamer->SetWorldScale(1e12);
        
        StreamingSettings settings;
        settings.loadRadius = 1000000.0f;    // 1000 km
        settings.unloadRadius = 2000000.0f;  // 2000 km
        settings.predictionRadius = 500000.0f; // 500 km
        settings.enablePredictiveStreaming = true;
        settings.enableServerSideStreaming = true;
        
        worldStreamer->SetStreamingSettings(settings);
    }
    
    // Configure sound system
    if (sound) {
        sound->SetMasterVolume(0.8f);
        sound->EnableDopplerEffect(true);
        sound->EnableVoiceChat(true);
        
        EnvironmentSettings env;
        env.spaceEnvironment = true; // No sound propagation in vacuum
        env.reverbLevel = 0.0f;
        sound->SetEnvironment(env);
    }
    
    // Configure networking (optional)
    if (network) {
        // Start as server by default
        network->SetMaxClients(1000);
        network->EnableModSupport(true);
        
        // Set up message handler
        network->SetMessageHandler([](const NetworkMessage& msg) {
            DAISY_INFO("Received network message type: {}", msg.type);
        });
        
        // Uncomment to start server
        // network->StartServer(7777);
    }
    
    // Configure script system
    if (scriptSystem) {
        scriptSystem->EnableModSupport(true);
        scriptSystem->SetScriptDirectory("Scripts/");
        
        // Load a basic script
        std::string basicScript = R"(
            # Basic Daisy Engine Script
            set player_energy 100
            set game_time 0
            
            # This script would handle basic game logic
        )";
        
        scriptSystem->LoadScriptFromString("basic_logic", basicScript);
        
        // Register event handlers
        scriptSystem->RegisterEventHandler("player_spawn", "basic_logic");
    }
    
    DAISY_INFO("Starting main engine loop...");
    
    // Simulate a camera moving through space
    Daisy::Vector3 cameraPos(7000000, 0, 0); // Start in low Earth orbit
    Daisy::Vector3 cameraVelocity(0, 7800, 0); // Orbital velocity
    
    // Main game loop
    int frameCount = 0;
    while (engine->IsRunning() && frameCount < 1000) { // Limit frames for example
        // Update camera position (simulate orbital motion)
        cameraPos = cameraPos + cameraVelocity * engine->GetDeltaTime();
        
        // Update observer position for world streaming
        if (worldStreamer) {
            worldStreamer->SetObserverPosition(cameraPos);
        }
        
        // Update renderer camera
        if (renderer) {
            Daisy::Camera camera = renderer->GetCamera();
            camera.position = cameraPos;
            camera.target = earthPos; // Always look at Earth
            renderer->SetCamera(camera);
            renderer->SetObserverPosition(cameraPos);
        }
        
        // Update sound listener
        if (sound) {
            Daisy::AudioListener listener = sound->GetListener();
            listener.position = cameraPos;
            listener.velocity = cameraVelocity;
            sound->SetListener(listener);
        }
        
        // Trigger some AI events periodically
        if (ai && frameCount % 100 == 0) {
            ai->TriggerEvent("economic_update", cameraPos, 1.0f);
        }
        
        // Execute main engine update
        engine->Update();
        
        frameCount++;
        
        // Log progress periodically
        if (frameCount % 100 == 0) {
            DAISY_INFO("Frame {}: Camera at ({:.0f}, {:.0f}, {:.0f})", 
                frameCount, cameraPos.x, cameraPos.y, cameraPos.z);
        }
        
        // Simulate frame rate (remove in real application)
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }
    
    DAISY_INFO("Shutting down Daisy Engine Example Application");
    DAISY_ENGINE.Shutdown();
    
    return 0;
}