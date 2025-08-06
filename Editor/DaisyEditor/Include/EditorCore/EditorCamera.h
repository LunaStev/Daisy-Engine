#pragma once

#include "Core/Math.h"

namespace DaisyEditor {

class EditorCamera {
public:
    EditorCamera();
    
    void Update(float deltaTime);
    void HandleInput(float deltaTime);
    
    const Daisy::Vector3& GetPosition() const { return m_position; }
    void SetPosition(const Daisy::Vector3& position) { m_position = position; }
    
    const Daisy::Vector3& GetTarget() const { return m_target; }
    void SetTarget(const Daisy::Vector3& target) { m_target = target; }
    
    float GetFOV() const { return m_fov; }
    void SetFOV(float fov) { m_fov = fov; }
    
    float GetNearPlane() const { return m_nearPlane; }
    float GetFarPlane() const { return m_farPlane; }
    void SetClipPlanes(float near, float far) { m_nearPlane = near; m_farPlane = far; }
    
    float GetAspectRatio() const { return m_aspectRatio; }
    void SetAspectRatio(float aspect) { m_aspectRatio = aspect; }
    
    Daisy::Matrix4 GetViewMatrix() const;
    Daisy::Matrix4 GetProjectionMatrix() const;
    
    void LookAt(const Daisy::Vector3& target);
    void Orbit(const Daisy::Vector3& center, float deltaYaw, float deltaPitch);
    void Pan(float deltaX, float deltaY);
    void Zoom(float delta);
    
    void SetOrbitMode(bool orbit) { m_orbitMode = orbit; }
    bool IsOrbitMode() const { return m_orbitMode; }
    
    void SetMovementSpeed(float speed) { m_movementSpeed = speed; }
    void SetRotationSpeed(float speed) { m_rotationSpeed = speed; }
    
private:
    void UpdateVectors();
    
    Daisy::Vector3 m_position{0, 0, 10};
    Daisy::Vector3 m_target{0, 0, 0};
    Daisy::Vector3 m_up{0, 1, 0};
    Daisy::Vector3 m_right{1, 0, 0};
    Daisy::Vector3 m_forward{0, 0, -1};
    
    float m_yaw = -90.0f;
    float m_pitch = 0.0f;
    
    float m_fov = 45.0f;
    float m_nearPlane = 0.1f;
    float m_farPlane = 10000000.0f; // For space scale
    float m_aspectRatio = 16.0f / 9.0f;
    
    float m_movementSpeed = 10.0f;
    float m_rotationSpeed = 0.1f;
    float m_orbitDistance = 10.0f;
    
    bool m_orbitMode = false;
    bool m_firstMouse = true;
    
    float m_lastMouseX = 0.0f;
    float m_lastMouseY = 0.0f;
};

}