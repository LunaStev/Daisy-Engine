#include "DaisyPhysics.h"
#include "Core/Logger.h"
#include <algorithm>
#include <chrono>

namespace Daisy {

DaisyPhysics::DaisyPhysics() : Module("DaisyPhysics") {
}

bool DaisyPhysics::Initialize() {
    DAISY_INFO("Initializing Daisy Physics Engine");
    
    m_rigidBodies.reserve(10000);
    m_gravityWells.reserve(1000);
    
    m_initialized = true;
    DAISY_INFO("Daisy Physics Engine initialized successfully");
    return true;
}

void DaisyPhysics::Update(float deltaTime) {
    if (!m_initialized) return;
    
    ApplyGravity(deltaTime);
    IntegrateRigidBodies(deltaTime);
    CheckCollisions();
    UpdateLOD();
}

void DaisyPhysics::Shutdown() {
    if (!m_initialized) return;
    
    DAISY_INFO("Shutting down Daisy Physics Engine");
    
    m_rigidBodies.clear();
    m_collisionShapes.clear();
    m_gravityWells.clear();
    m_atmosphericDensity.clear();
    
    m_initialized = false;
    DAISY_INFO("Daisy Physics Engine shut down successfully");
}

uint32_t DaisyPhysics::CreateRigidBody(const Vector3& position, float mass) {
    auto body = std::make_unique<RigidBody>();
    body->id = m_nextBodyId++;
    body->position = position;
    body->mass = mass;
    body->invMass = mass > 0.0f ? 1.0f / mass : 0.0f;
    
    uint32_t id = body->id;
    m_rigidBodies.push_back(std::move(body));
    
    return id;
}

void DaisyPhysics::DestroyRigidBody(uint32_t id) {
    auto it = std::remove_if(m_rigidBodies.begin(), m_rigidBodies.end(),
        [id](const std::unique_ptr<RigidBody>& body) {
            return body->id == id;
        });
    
    if (it != m_rigidBodies.end()) {
        m_rigidBodies.erase(it, m_rigidBodies.end());
        m_collisionShapes.erase(id);
        m_atmosphericDensity.erase(id);
    }
}

RigidBody* DaisyPhysics::GetRigidBody(uint32_t id) {
    auto it = std::find_if(m_rigidBodies.begin(), m_rigidBodies.end(),
        [id](const std::unique_ptr<RigidBody>& body) {
            return body->id == id;
        });
    
    return it != m_rigidBodies.end() ? it->get() : nullptr;
}

void DaisyPhysics::SetCollisionShape(uint32_t bodyId, std::unique_ptr<CollisionShape> shape) {
    m_collisionShapes[bodyId] = std::move(shape);
}

void DaisyPhysics::AddGravityWell(const Vector3& position, float mass, float radius, bool isPlanet) {
    GravityWell well;
    well.position = position;
    well.mass = mass;
    well.radius = radius;
    well.isPlanet = isPlanet;
    well.isStar = mass > 1e30f;
    
    m_gravityWells.push_back(well);
}

void DaisyPhysics::ApplyForce(uint32_t bodyId, const Vector3& force) {
    auto* body = GetRigidBody(bodyId);
    if (body && !body->isStatic) {
        body->force = body->force + force;
    }
}

void DaisyPhysics::ApplyImpulse(uint32_t bodyId, const Vector3& impulse) {
    auto* body = GetRigidBody(bodyId);
    if (body && !body->isStatic) {
        body->velocity = body->velocity + impulse * body->invMass;
    }
}

void DaisyPhysics::ApplyTorque(uint32_t bodyId, const Vector3& torque) {
    auto* body = GetRigidBody(bodyId);
    if (body && !body->isStatic) {
        body->torque = body->torque + torque;
    }
}

void DaisyPhysics::SetAtmosphere(uint32_t bodyId, float density) {
    m_atmosphericDensity[bodyId] = density;
}

void DaisyPhysics::IntegrateRigidBodies(float deltaTime) {
    for (auto& body : m_rigidBodies) {
        if (body->isStatic) continue;
        
        body->acceleration = body->force * body->invMass;
        body->velocity = body->velocity + body->acceleration * deltaTime;
        body->position = body->position + body->velocity * deltaTime;
        
        if (m_atmosphericDensity.find(body->id) != m_atmosphericDensity.end()) {
            ApplyAtmosphericDrag(*body, deltaTime);
        }
        
        Vector3 angularAcceleration = body->torque * body->invMass;
        body->angularVelocity = body->angularVelocity + angularAcceleration * deltaTime;
        
        if (body->angularVelocity.LengthSquared() > 0) {
            float angle = body->angularVelocity.Length() * deltaTime;
            Vector3 axis = body->angularVelocity.Normalized();
            Quaternion deltaRotation = Quaternion::FromAxisAngle(axis, angle);
            body->rotation = (deltaRotation * body->rotation).Normalized();
        }
        
        body->force = Vector3(0, 0, 0);
        body->torque = Vector3(0, 0, 0);
    }
}

void DaisyPhysics::ApplyGravity(float deltaTime) {
    for (auto& body : m_rigidBodies) {
        if (body->isStatic || !body->useGravity) continue;
        
        if (m_globalGravity.LengthSquared() > 0) {
            Vector3 globalForce = m_globalGravity * body->mass;
            body->force = body->force + globalForce;
        }
        
        for (const auto& well : m_gravityWells) {
            Vector3 direction = well.position - body->position;
            float distance = direction.Length();
            
            if (distance > 0 && distance < well.radius) {
                direction = direction.Normalized();
                
                float gravitationalForce = (6.674e-11f * well.mass * body->mass) / (distance * distance);
                
                if (well.isPlanet && distance < well.radius * 0.1f) {
                    gravitationalForce *= (distance / (well.radius * 0.1f));
                }
                
                Vector3 force = direction * gravitationalForce;
                body->force = body->force + force;
            }
        }
    }
}

void DaisyPhysics::CheckCollisions() {
    for (size_t i = 0; i < m_rigidBodies.size(); ++i) {
        for (size_t j = i + 1; j < m_rigidBodies.size(); ++j) {
            auto& bodyA = m_rigidBodies[i];
            auto& bodyB = m_rigidBodies[j];
            
            if (bodyA->isStatic && bodyB->isStatic) continue;
            
            Vector3 direction = bodyB->position - bodyA->position;
            float distance = direction.Length();
            
            float radiusA = 1.0f;
            float radiusB = 1.0f;
            
            auto shapeA = m_collisionShapes.find(bodyA->id);
            auto shapeB = m_collisionShapes.find(bodyB->id);
            
            if (shapeA != m_collisionShapes.end() && shapeA->second->type == CollisionShape::Sphere) {
                radiusA = shapeA->second->dimensions.x;
            }
            if (shapeB != m_collisionShapes.end() && shapeB->second->type == CollisionShape::Sphere) {
                radiusB = shapeB->second->dimensions.x;
            }
            
            if (distance < radiusA + radiusB && distance > 0) {
                Vector3 normal = direction.Normalized();
                float overlap = (radiusA + radiusB) - distance;
                
                Vector3 separation = normal * (overlap * 0.5f);
                if (!bodyA->isStatic) bodyA->position = bodyA->position - separation;
                if (!bodyB->isStatic) bodyB->position = bodyB->position + separation;
                
                Vector3 relativeVelocity = bodyB->velocity - bodyA->velocity;
                float velocityAlongNormal = relativeVelocity.Dot(normal);
                
                if (velocityAlongNormal > 0) continue;
                
                float e = std::min(bodyA->restitution, bodyB->restitution);
                float j = -(1 + e) * velocityAlongNormal;
                j /= bodyA->invMass + bodyB->invMass;
                
                Vector3 impulse = normal * j;
                if (!bodyA->isStatic) bodyA->velocity = bodyA->velocity - impulse * bodyA->invMass;
                if (!bodyB->isStatic) bodyB->velocity = bodyB->velocity + impulse * bodyB->invMass;
            }
        }
    }
}

void DaisyPhysics::ApplyAtmosphericDrag(RigidBody& body, float deltaTime) {
    float density = m_atmosphericDensity[body.id];
    float dragCoefficient = 0.47f;
    float area = 1.0f;
    
    Vector3 velocity = body.velocity;
    float speed = velocity.Length();
    
    if (speed > 0) {
        Vector3 dragDirection = velocity.Normalized() * -1.0f;
        float dragMagnitude = 0.5f * density * speed * speed * dragCoefficient * area;
        
        Vector3 dragForce = dragDirection * dragMagnitude;
        body.force = body.force + dragForce;
    }
}

void DaisyPhysics::UpdateLOD() {
    // Placeholder for LOD system
    // Would implement distance-based collision detail reduction
}

}