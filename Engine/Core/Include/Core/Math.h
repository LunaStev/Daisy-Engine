#pragma once

#include <cmath>
#include <algorithm>

namespace Daisy {

constexpr float PI = 3.14159265358979323846f;
constexpr float TWO_PI = 2.0f * PI;
constexpr float HALF_PI = PI / 2.0f;
constexpr float DEG_TO_RAD = PI / 180.0f;
constexpr float RAD_TO_DEG = 180.0f / PI;

struct Vector3 {
    float x, y, z;
    
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    
    Vector3 operator+(const Vector3& other) const { return {x + other.x, y + other.y, z + other.z}; }
    Vector3 operator-(const Vector3& other) const { return {x - other.x, y - other.y, z - other.z}; }
    Vector3 operator*(float scalar) const { return {x * scalar, y * scalar, z * scalar}; }
    Vector3 operator/(float scalar) const { return {x / scalar, y / scalar, z / scalar}; }
    
    float Dot(const Vector3& other) const { return x * other.x + y * other.y + z * other.z; }
    Vector3 Cross(const Vector3& other) const {
        return {y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x};
    }
    
    float Length() const { return std::sqrt(x * x + y * y + z * z); }
    float LengthSquared() const { return x * x + y * y + z * z; }
    Vector3 Normalized() const { float len = Length(); return len > 0 ? *this / len : Vector3(); }
};

struct Vector4 {
    float x, y, z, w;
    
    Vector4() : x(0), y(0), z(0), w(0) {}
    Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    Vector4(const Vector3& v3, float w) : x(v3.x), y(v3.y), z(v3.z), w(w) {}
};

struct Matrix4 {
    float m[16];
    
    Matrix4() { Identity(); }
    
    void Identity() {
        std::fill(m, m + 16, 0.0f);
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }
    
    static Matrix4 Translation(const Vector3& translation);
    static Matrix4 Rotation(const Vector3& axis, float angle);
    static Matrix4 Scale(const Vector3& scale);
    static Matrix4 Perspective(float fov, float aspect, float near, float far);
    static Matrix4 Orthographic(float left, float right, float bottom, float top, float near, float far);
    
    Matrix4 operator*(const Matrix4& other) const;
    Vector4 operator*(const Vector4& vec) const;
};

struct Quaternion {
    float x, y, z, w;
    
    Quaternion() : x(0), y(0), z(0), w(1) {}
    Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    
    static Quaternion FromAxisAngle(const Vector3& axis, float angle);
    Matrix4 ToMatrix() const;
    Quaternion operator*(const Quaternion& other) const;
    Quaternion Normalized() const;
};

template<typename T>
T Clamp(T value, T min, T max) {
    return std::max(min, std::min(max, value));
}

template<typename T>
T Lerp(T a, T b, float t) {
    return a + (b - a) * t;
}

inline float ToRadians(float degrees) { return degrees * DEG_TO_RAD; }
inline float ToDegrees(float radians) { return radians * RAD_TO_DEG; }

}