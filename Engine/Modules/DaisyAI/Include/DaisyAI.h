#pragma once

#include "Core/Module.h"
#include "Core/Math.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <queue>

namespace Daisy {

enum class AIBehaviorType {
    Economic,    // Trade, production, consumption
    Social,      // Construction, cooperation, rebellion
    Combat,      // Ground and space combat
    Exploration, // Navigation, discovery
    Survival     // Basic needs, resource gathering
};

struct AIAgent {
    uint32_t id = 0;
    std::string name;
    Vector3 position{0, 0, 0};
    Vector3 target{0, 0, 0};
    
    AIBehaviorType primaryBehavior = AIBehaviorType::Survival;
    std::vector<AIBehaviorType> secondaryBehaviors;
    
    float aggression = 0.5f;
    float intelligence = 0.5f;
    float cooperation = 0.5f;
    float greed = 0.5f;
    float curiosity = 0.5f;
    
    std::unordered_map<std::string, float> resources;
    std::vector<uint32_t> relationships; // Other agent IDs
    std::queue<std::string> goals;
    
    bool isActive = true;
    float lastUpdateTime = 0.0f;
};

struct EconomicSystem {
    std::unordered_map<std::string, float> globalPrices;
    std::unordered_map<std::string, float> supply;
    std::unordered_map<std::string, float> demand;
    std::vector<std::string> tradeRoutes;
};

struct SocialStructure {
    std::unordered_map<uint32_t, std::vector<uint32_t>> factions;
    std::unordered_map<uint32_t, std::string> territories;
    std::vector<std::string> laws;
    float overallStability = 1.0f;
};

struct CombatSystem {
    struct CombatGroup {
        std::vector<uint32_t> agentIds;
        Vector3 position;
        std::string target;
        float strength = 1.0f;
    };
    
    std::vector<CombatGroup> activeCombats;
    std::unordered_map<uint32_t, float> threatLevels;
};

class DaisyAI : public Module {
public:
    DaisyAI();
    virtual ~DaisyAI() = default;
    
    bool Initialize() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    
    uint32_t CreateAIAgent(const std::string& name, const Vector3& position);
    void DestroyAIAgent(uint32_t agentId);
    AIAgent* GetAIAgent(uint32_t agentId);
    
    void SetAgentBehavior(uint32_t agentId, AIBehaviorType behavior);
    void AddAgentGoal(uint32_t agentId, const std::string& goal);
    void SetAgentPersonality(uint32_t agentId, float aggression, float intelligence, float cooperation);
    
    void EnableLearning(bool enable) { m_learningEnabled = enable; }
    void SetSimulationSpeed(float speed) { m_simulationSpeed = speed; }
    void SetMaxAgents(uint32_t max) { m_maxAgents = max; }
    
    EconomicSystem& GetEconomicSystem() { return m_economicSystem; }
    SocialStructure& GetSocialStructure() { return m_socialStructure; }
    CombatSystem& GetCombatSystem() { return m_combatSystem; }
    
    void TriggerEvent(const std::string& eventType, const Vector3& position, float severity = 1.0f);
    
private:
    void UpdateEconomicAI(float deltaTime);
    void UpdateSocialAI(float deltaTime);
    void UpdateCombatAI(float deltaTime);
    void UpdateExplorationAI(float deltaTime);
    
    void ProcessAgentBehavior(AIAgent& agent, float deltaTime);
    void ProcessAgentGoals(AIAgent& agent);
    void UpdateAgentRelationships(AIAgent& agent);
    void LearnFromInteractions();
    
    void ManagePopulation();
    void SpawnNewAgents();
    void RemoveInactiveAgents();
    
    std::unordered_map<uint32_t, std::unique_ptr<AIAgent>> m_agents;
    
    EconomicSystem m_economicSystem;
    SocialStructure m_socialStructure;
    CombatSystem m_combatSystem;
    
    uint32_t m_nextAgentId = 1;
    uint32_t m_maxAgents = 10000;
    
    float m_simulationSpeed = 1.0f;
    bool m_learningEnabled = true;
    
    float m_economicUpdateTimer = 0.0f;
    float m_socialUpdateTimer = 0.0f;
    float m_combatUpdateTimer = 0.0f;
    float m_explorationUpdateTimer = 0.0f;
    
    std::vector<std::pair<std::string, Vector3>> m_recentEvents;
};

}