#include "EditorCore/EditorCamera.h"
#include "Core/Logger.h"
#include <algorithm>

namespace DaisyEditor {

EditorCamera::EditorCamera() {
    UpdateVectors();
}

void EditorCamera::Update(float deltaTime) {
    // Camera update logic
    UpdateVectors();
}

void EditorCamera::HandleInput(float deltaTime) {
    // Input handling would be implemented here with actual input system
    // For now, this is a placeholder
    
    // Example movement (would use actual input):
    // W/A/S/D for movement
    // Mouse for looking around
    // Scroll wheel for zoom
    
    static float time = 0.0f;
    time += deltaTime;
    
    // Simple orbit animation for demonstration
    if (m_orbitMode) {
        float radius = m_orbitDistance;
        m_position.x = std::cos(time * 0.1f) * radius;
        m_position.z = std::sin(time * 0.1f) * radius;
        m_position.y = 5.0f;
        
        LookAt(m_target);
    }
}

Daisy::Matrix4 EditorCamera::GetViewMatrix() const {
    Daisy::Vector3 forward = (m_target - m_position).Normalized();
    Daisy::Vector3 right = forward.Cross(Daisy::Vector3(0, 1, 0)).Normalized();
    Daisy::Vector3 up = right.Cross(forward);
    
    Daisy::Matrix4 view;
    // Implement look-at matrix manually (simplified)
    view.m[0] = right.x;
    view.m[1] = up.x;
    view.m[2] = -forward.x;
    view.m[3] = 0.0f;
    
    view.m[4] = right.y;
    view.m[5] = up.y;
    view.m[6] = -forward.y;
    view.m[7] = 0.0f;
    
    view.m[8] = right.z;
    view.m[9] = up.z;
    view.m[10] = -forward.z;
    view.m[11] = 0.0f;
    
    view.m[12] = -right.Dot(m_position);
    view.m[13] = -up.Dot(m_position);
    view.m[14] = forward.Dot(m_position);
    view.m[15] = 1.0f;
    
    return view;
}

Daisy::Matrix4 EditorCamera::GetProjectionMatrix() const {
    return Daisy::Matrix4::Perspective(
        Daisy::ToRadians(m_fov),
        m_aspectRatio,
        m_nearPlane,
        m_farPlane
    );
}

void EditorCamera::LookAt(const Daisy::Vector3& target) {
    m_target = target;
    
    Daisy::Vector3 direction = (m_position - target).Normalized();
    
    // Calculate pitch and yaw from direction
    m_pitch = Daisy::ToDegrees(std::asin(-direction.y));
    m_yaw = Daisy::ToDegrees(std::atan2(-direction.x, -direction.z));
    
    UpdateVectors();
}

void EditorCamera::Orbit(const Daisy::Vector3& center, float deltaYaw, float deltaPitch) {
    m_yaw += deltaYaw;
    m_pitch += deltaPitch;
    
    // Clamp pitch
    m_pitch = Daisy::Clamp(m_pitch, -89.0f, 89.0f);
    
    // Calculate new position
    float yawRad = Daisy::ToRadians(m_yaw);
    float pitchRad = Daisy::ToRadians(m_pitch);
    
    m_position.x = center.x + m_orbitDistance * std::cos(pitchRad) * std::cos(yawRad);
    m_position.y = center.y + m_orbitDistance * std::sin(pitchRad);
    m_position.z = center.z + m_orbitDistance * std::cos(pitchRad) * std::sin(yawRad);
    
    m_target = center;
    UpdateVectors();
}

void EditorCamera::Pan(float deltaX, float deltaY) {
    Daisy::Vector3 rightMovement = m_right * (-deltaX * m_movementSpeed);
    Daisy::Vector3 upMovement = m_up * (deltaY * m_movementSpeed);
    
    m_position = m_position + rightMovement + upMovement;
    m_target = m_target + rightMovement + upMovement;
    
    UpdateVectors();
}

void EditorCamera::Zoom(float delta) {
    if (m_orbitMode) {
        m_orbitDistance -= delta * m_movementSpeed;
        m_orbitDistance = std::max(0.1f, m_orbitDistance);
        
        // Update position based on new orbit distance
        float yawRad = Daisy::ToRadians(m_yaw);
        float pitchRad = Daisy::ToRadians(m_pitch);
        
        m_position.x = m_target.x + m_orbitDistance * std::cos(pitchRad) * std::cos(yawRad);
        m_position.y = m_target.y + m_orbitDistance * std::sin(pitchRad);
        m_position.z = m_target.z + m_orbitDistance * std::cos(pitchRad) * std::sin(yawRad);
    } else {
        Daisy::Vector3 forward = m_forward * (delta * m_movementSpeed);
        m_position = m_position + forward;
    }
    
    UpdateVectors();
}

void EditorCamera::UpdateVectors() {
    // Calculate forward vector
    Daisy::Vector3 direction;
    direction.x = std::cos(Daisy::ToRadians(m_yaw)) * std::cos(Daisy::ToRadians(m_pitch));
    direction.y = std::sin(Daisy::ToRadians(m_pitch));
    direction.z = std::sin(Daisy::ToRadians(m_yaw)) * std::cos(Daisy::ToRadians(m_pitch));
    
    m_forward = direction.Normalized();
    
    // Calculate right and up vectors
    m_right = m_forward.Cross(Daisy::Vector3(0, 1, 0)).Normalized();
    m_up = m_right.Cross(m_forward).Normalized();
    
    if (!m_orbitMode) {
        m_target = m_position + m_forward;
    }
}

}