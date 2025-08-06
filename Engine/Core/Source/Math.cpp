#include "Core/Math.h"
#include <cstring>

namespace Daisy {

Matrix4 Matrix4::Translation(const Vector3& translation) {
    Matrix4 result;
    result.m[12] = translation.x;
    result.m[13] = translation.y;
    result.m[14] = translation.z;
    return result;
}

Matrix4 Matrix4::Rotation(const Vector3& axis, float angle) {
    return Quaternion::FromAxisAngle(axis, angle).ToMatrix();
}

Matrix4 Matrix4::Scale(const Vector3& scale) {
    Matrix4 result;
    result.m[0] = scale.x;
    result.m[5] = scale.y;
    result.m[10] = scale.z;
    return result;
}

Matrix4 Matrix4::Perspective(float fov, float aspect, float near, float far) {
    Matrix4 result;
    std::memset(result.m, 0, sizeof(result.m));
    
    float tanHalfFov = std::tan(fov * 0.5f);
    
    result.m[0] = 1.0f / (aspect * tanHalfFov);
    result.m[5] = 1.0f / tanHalfFov;
    result.m[10] = -(far + near) / (far - near);
    result.m[11] = -1.0f;
    result.m[14] = -(2.0f * far * near) / (far - near);
    
    return result;
}

Matrix4 Matrix4::Orthographic(float left, float right, float bottom, float top, float near, float far) {
    Matrix4 result;
    std::memset(result.m, 0, sizeof(result.m));
    
    result.m[0] = 2.0f / (right - left);
    result.m[5] = 2.0f / (top - bottom);
    result.m[10] = -2.0f / (far - near);
    result.m[12] = -(right + left) / (right - left);
    result.m[13] = -(top + bottom) / (top - bottom);
    result.m[14] = -(far + near) / (far - near);
    result.m[15] = 1.0f;
    
    return result;
}

Matrix4 Matrix4::operator*(const Matrix4& other) const {
    Matrix4 result;
    std::memset(result.m, 0, sizeof(result.m));
    
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 4; ++k) {
                result.m[i * 4 + j] += m[i * 4 + k] * other.m[k * 4 + j];
            }
        }
    }
    
    return result;
}

Vector4 Matrix4::operator*(const Vector4& vec) const {
    return Vector4(
        m[0] * vec.x + m[4] * vec.y + m[8]  * vec.z + m[12] * vec.w,
        m[1] * vec.x + m[5] * vec.y + m[9]  * vec.z + m[13] * vec.w,
        m[2] * vec.x + m[6] * vec.y + m[10] * vec.z + m[14] * vec.w,
        m[3] * vec.x + m[7] * vec.y + m[11] * vec.z + m[15] * vec.w
    );
}

Quaternion Quaternion::FromAxisAngle(const Vector3& axis, float angle) {
    float halfAngle = angle * 0.5f;
    float sin = std::sin(halfAngle);
    Vector3 normalizedAxis = axis.Normalized();
    
    return Quaternion(
        normalizedAxis.x * sin,
        normalizedAxis.y * sin,
        normalizedAxis.z * sin,
        std::cos(halfAngle)
    );
}

Matrix4 Quaternion::ToMatrix() const {
    Matrix4 result;
    
    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    float xy = x * y;
    float xz = x * z;
    float yz = y * z;
    float wx = w * x;
    float wy = w * y;
    float wz = w * z;
    
    result.m[0] = 1.0f - 2.0f * (yy + zz);
    result.m[1] = 2.0f * (xy + wz);
    result.m[2] = 2.0f * (xz - wy);
    result.m[3] = 0.0f;
    
    result.m[4] = 2.0f * (xy - wz);
    result.m[5] = 1.0f - 2.0f * (xx + zz);
    result.m[6] = 2.0f * (yz + wx);
    result.m[7] = 0.0f;
    
    result.m[8] = 2.0f * (xz + wy);
    result.m[9] = 2.0f * (yz - wx);
    result.m[10] = 1.0f - 2.0f * (xx + yy);
    result.m[11] = 0.0f;
    
    result.m[12] = 0.0f;
    result.m[13] = 0.0f;
    result.m[14] = 0.0f;
    result.m[15] = 1.0f;
    
    return result;
}

Quaternion Quaternion::operator*(const Quaternion& other) const {
    return Quaternion(
        w * other.x + x * other.w + y * other.z - z * other.y,
        w * other.y - x * other.z + y * other.w + z * other.x,
        w * other.z + x * other.y - y * other.x + z * other.w,
        w * other.w - x * other.x - y * other.y - z * other.z
    );
}

Quaternion Quaternion::Normalized() const {
    float length = std::sqrt(x * x + y * y + z * z + w * w);
    if (length > 0.0f) {
        return Quaternion(x / length, y / length, z / length, w / length);
    }
    return Quaternion();
}

}