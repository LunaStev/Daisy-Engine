#pragma once

#include "Core/Module.h"
#include "Core/Math.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace Daisy {

struct RigidBody {
    Vector3 position{0, 0, 0};
    Vector3 velocity{0, 0, 0};
    Vector3 acceleration{0, 0, 0};
    Vector3 force{0, 0, 0};
    
    Quaternion rotation;
    Vector3 angularVelocity{0, 0, 0};
    Vector3 torque{0, 0, 0};
    
    float mass = 1.0f;
    float invMass = 1.0f;
    float restitution = 0.5f;
    float friction = 0.5f;
    
    bool isStatic = false;
    bool useGravity = true;
    
    uint32_t id = 0;
};

struct CollisionShape {
    enum Type { Sphere, Box, Capsule, Mesh } type;
    Vector3 dimensions{1, 1, 1};
    
    CollisionShape(Type t) : type(t) {}
};

struct GravityWell {
    Vector3 position{0, 0, 0};
    float mass = 1.0f;
    float radius = 100.0f;
    bool isPlanet = false;
    bool isStar = false;
};

class DaisyPhysics : public Module {
public:
    DaisyPhysics();
    virtual ~DaisyPhysics() = default;
    
    bool Initialize() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    
    uint32_t CreateRigidBody(const Vector3& position, float mass = 1.0f);
    void DestroyRigidBody(uint32_t id);
    RigidBody* GetRigidBody(uint32_t id);
    
    void SetCollisionShape(uint32_t bodyId, std::unique_ptr<CollisionShape> shape);
    
    void AddGravityWell(const Vector3& position, float mass, float radius, bool isPlanet = false);
    void SetGlobalGravity(const Vector3& gravity) { m_globalGravity = gravity; }
    
    void ApplyForce(uint32_t bodyId, const Vector3& force);
    void ApplyImpulse(uint32_t bodyId, const Vector3& impulse);
    void ApplyTorque(uint32_t bodyId, const Vector3& torque);
    
    void SetAtmosphere(uint32_t bodyId, float density);
    void EnableFluidDynamics(bool enable) { m_fluidDynamicsEnabled = enable; }
    
    void SetLODDistance(float distance) { m_lodDistance = distance; }
    
private:
    void IntegrateRigidBodies(float deltaTime);
    void ApplyGravity(float deltaTime);
    void CheckCollisions();
    void ApplyAtmosphericDrag(RigidBody& body, float deltaTime);
    void UpdateLOD();
    
    std::vector<std::unique_ptr<RigidBody>> m_rigidBodies;
    std::unordered_map<uint32_t, std::unique_ptr<CollisionShape>> m_collisionShapes;
    std::vector<GravityWell> m_gravityWells;
    
    Vector3 m_globalGravity{0, -9.81f, 0};
    uint32_t m_nextBodyId = 1;
    
    float m_lodDistance = 1000.0f;
    bool m_fluidDynamicsEnabled = false;
    
    std::unordered_map<uint32_t, float> m_atmosphericDensity;
};

}