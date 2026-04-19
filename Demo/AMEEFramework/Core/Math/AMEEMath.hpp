#ifndef __AMEE_MATH_H__
#define __AMEE_MATH_H__
#pragma once
#include <cmath>
#include <cstring>

namespace AMEE {

// Constants
constexpr float PI = 3.14159265358979323846f;
constexpr float TWO_PI = PI * 2.0f;
constexpr float HALF_PI = PI * 0.5f;
constexpr float DEG2RAD = PI / 180.0f;
constexpr float RAD2DEG = 180.0f / PI;

// =========================================================================
// Vec2
// =========================================================================
struct Vec2 {
    float x, y;

    Vec2() : x(0), y(0) {}
    Vec2(float v) : x(v), y(v) {}
    Vec2(float x, float y) : x(x), y(y) {}

    Vec2 operator+(const Vec2& r) const { return {x + r.x, y + r.y}; }
    Vec2 operator-(const Vec2& r) const { return {x - r.x, y - r.y}; }
    Vec2 operator*(float s) const { return {x * s, y * s}; }
    Vec2 operator/(float s) const { float inv = 1.0f / s; return {x * inv, y * inv}; }
    Vec2 operator-() const { return {-x, -y}; }

    Vec2& operator+=(const Vec2& r) { x += r.x; y += r.y; return *this; }
    Vec2& operator-=(const Vec2& r) { x -= r.x; y -= r.y; return *this; }
    Vec2& operator*=(float s) { x *= s; y *= s; return *this; }

    float Length() const { return std::sqrt(x * x + y * y); }
    float LengthSq() const { return x * x + y * y; }

    Vec2 Normalized() const {
        float len = Length();
        return len > 0 ? *this / len : Vec2(0);
    }

    static float Dot(const Vec2& a, const Vec2& b) {
        return a.x * b.x + a.y * b.y;
    }
};

inline Vec2 operator*(float s, const Vec2& v) { return v * s; }

// =========================================================================
// Vec3
// =========================================================================
struct Vec3 {
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float v) : x(v), y(v), z(v) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    Vec3(const Vec2& v, float z = 0) : x(v.x), y(v.y), z(z) {}

    Vec3 operator+(const Vec3& r) const { return {x + r.x, y + r.y, z + r.z}; }
    Vec3 operator-(const Vec3& r) const { return {x - r.x, y - r.y, z - r.z}; }
    Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
    Vec3 operator/(float s) const { float inv = 1.0f / s; return {x * inv, y * inv, z * inv}; }
    Vec3 operator-() const { return {-x, -y, -z}; }

    Vec3& operator+=(const Vec3& r) { x += r.x; y += r.y; z += r.z; return *this; }
    Vec3& operator-=(const Vec3& r) { x -= r.x; y -= r.y; z -= r.z; return *this; }
    Vec3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }

    float Length() const { return std::sqrt(x * x + y * y + z * z); }
    float LengthSq() const { return x * x + y * y + z * z; }

    Vec3 Normalized() const {
        float len = Length();
        return len > 0 ? *this / len : Vec3(0);
    }

    static float Dot(const Vec3& a, const Vec3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    static Vec3 Cross(const Vec3& a, const Vec3& b) {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }
};

inline Vec3 operator*(float s, const Vec3& v) { return v * s; }

// =========================================================================
// Vec4
// =========================================================================
struct Vec4 {
    float x, y, z, w;

    Vec4() : x(0), y(0), z(0), w(0) {}
    Vec4(float v) : x(v), y(v), z(v), w(v) {}
    Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    Vec4(const Vec3& v, float w = 0) : x(v.x), y(v.y), z(v.z), w(w) {}

    Vec4 operator+(const Vec4& r) const { return {x + r.x, y + r.y, z + r.z, w + r.w}; }
    Vec4 operator-(const Vec4& r) const { return {x - r.x, y - r.y, z - r.z, w - r.w}; }
    Vec4 operator*(float s) const { return {x * s, y * s, z * s, w * s}; }
    Vec4 operator/(float s) const { float inv = 1.0f / s; return {x * inv, y * inv, z * inv, w * inv}; }

    float Dot(const Vec4& r) const { return x * r.x + y * r.y + z * r.z + w * r.w; }

    Vec3 xyz() const { return {x, y, z}; }
    Vec2 xy() const { return {x, y}; }
};

inline Vec4 operator*(float s, const Vec4& v) { return v * s; }

// =========================================================================
// Mat4 (column-major, OpenGL convention)
// =========================================================================
struct Mat4 {
    float m[16];

    Mat4() { std::memset(m, 0, sizeof(m)); }

    // Identity matrix
    static Mat4 Identity() {
        Mat4 result;
        result.m[0] = result.m[5] = result.m[10] = result.m[15] = 1.0f;
        return result;
    }

    // Column access (0-3)
    float* operator[](int col) { return &m[col * 4]; }
    const float* operator[](int col) const { return &m[col * 4]; }

    // Element access (col, row)
    float& at(int col, int row) { return m[col * 4 + row]; }
    const float& at(int col, int row) const { return m[col * 4 + row]; }

    // Get raw pointer for OpenGL
    const float* Data() const { return m; }

    // Matrix multiplication
    Mat4 operator*(const Mat4& r) const {
        Mat4 result;
        for (int col = 0; col < 4; col++) {
            for (int row = 0; row < 4; row++) {
                float sum = 0;
                for (int k = 0; k < 4; k++) {
                    sum += at(k, row) * r.at(col, k);
                }
                result.at(col, row) = sum;
            }
        }
        return result;
    }

    // Transform Vec4
    Vec4 operator*(const Vec4& v) const {
        Vec4 result;
        result.x = at(0,0)*v.x + at(1,0)*v.y + at(2,0)*v.z + at(3,0)*v.w;
        result.y = at(0,1)*v.x + at(1,1)*v.y + at(2,1)*v.z + at(3,1)*v.w;
        result.z = at(0,2)*v.x + at(1,2)*v.y + at(2,2)*v.z + at(3,2)*v.w;
        result.w = at(0,3)*v.x + at(1,3)*v.y + at(2,3)*v.z + at(3,3)*v.w;
        return result;
    }

    // Transform Vec3 (w=1)
    Vec3 TransformPoint(const Vec3& v) const {
        Vec4 r = *this * Vec4(v, 1.0f);
        return {r.x / r.w, r.y / r.w, r.z / r.w};
    }

    // Transform Vec3 direction (w=0)
    Vec3 TransformDir(const Vec3& v) const {
        Vec4 r = *this * Vec4(v, 0.0f);
        return {r.x, r.y, r.z};
    }

    // =========================================================================
    // Factory methods
    // =========================================================================

    // Translation matrix
    static Mat4 Translate(const Vec3& t) {
        Mat4 result = Identity();
        result.at(3, 0) = t.x;
        result.at(3, 1) = t.y;
        result.at(3, 2) = t.z;
        return result;
    }

    // Scale matrix
    static Mat4 Scale(const Vec3& s) {
        Mat4 result = Identity();
        result.at(0, 0) = s.x;
        result.at(1, 1) = s.y;
        result.at(2, 2) = s.z;
        return result;
    }

    // Rotation around arbitrary axis
    static Mat4 rotate(float AngleDeg, const Vec3& axis) {
        float rad = AngleDeg * DEG2RAD;
        float c = std::cos(rad);
        float s = std::sin(rad);
        float t = 1.0f - c;

        Vec3 a = axis.Normalized();
        float x = a.x, y = a.y, z = a.z;

        Mat4 result = Identity();
        result.at(0, 0) = t*x*x + c;
        result.at(0, 1) = t*x*y + s*z;
        result.at(0, 2) = t*x*z - s*y;

        result.at(1, 0) = t*x*y - s*z;
        result.at(1, 1) = t*y*y + c;
        result.at(1, 2) = t*y*z + s*x;

        result.at(2, 0) = t*x*z + s*y;
        result.at(2, 1) = t*y*z - s*x;
        result.at(2, 2) = t*z*z + c;

        return result;
    }

    // Rotation around X axis
    static Mat4 RotateX(float AngleDeg) {
        float rad = AngleDeg * DEG2RAD;
        float c = std::cos(rad);
        float s = std::sin(rad);
        Mat4 result = Identity();
        result.at(1, 1) = c;
        result.at(1, 2) = s;
        result.at(2, 1) = -s;
        result.at(2, 2) = c;
        return result;
    }

    // Rotation around Y axis
    static Mat4 RotateY(float AngleDeg) {
        float rad = AngleDeg * DEG2RAD;
        float c = std::cos(rad);
        float s = std::sin(rad);
        Mat4 result = Identity();
        result.at(0, 0) = c;
        result.at(0, 2) = -s;
        result.at(2, 0) = s;
        result.at(2, 2) = c;
        return result;
    }

    // Rotation around Z axis
    static Mat4 RotateZ(float AngleDeg) {
        float rad = AngleDeg * DEG2RAD;
        float c = std::cos(rad);
        float s = std::sin(rad);
        Mat4 result = Identity();
        result.at(0, 0) = c;
        result.at(0, 1) = s;
        result.at(1, 0) = -s;
        result.at(1, 1) = c;
        return result;
    }

    // Perspective projection (FOV in Degrees)
    static Mat4 Perspective(float FovDeg, float aspect, float near, float far) {
        float TanHalf = std::tan(FovDeg * DEG2RAD * 0.5f);
        Mat4 result;
        result.at(0, 0) = 1.0f / (aspect * TanHalf);
        result.at(1, 1) = 1.0f / TanHalf;
        result.at(2, 2) = -(far + near) / (far - near);
        result.at(2, 3) = -1.0f;
        result.at(3, 2) = -(2.0f * far * near) / (far - near);
        return result;
    }

    // Orthographic projection
    static Mat4 Ortho(float left, float right, float bottom, float top, float near, float far) {
        Mat4 result = Identity();
        result.at(0, 0) = 2.0f / (right - left);
        result.at(1, 1) = 2.0f / (top - bottom);
        result.at(2, 2) = -2.0f / (far - near);
        result.at(3, 0) = -(right + left) / (right - left);
        result.at(3, 1) = -(top + bottom) / (top - bottom);
        result.at(3, 2) = -(far + near) / (far - near);
        return result;
    }

    // View matrix (look at)
    static Mat4 LookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
        Vec3 f = (center - eye).Normalized();
        Vec3 s = Vec3::Cross(f, up).Normalized();
        Vec3 u = Vec3::Cross(s, f);

        Mat4 result = Identity();
        result.at(0, 0) = s.x;
        result.at(0, 1) = u.x;
        result.at(0, 2) = -f.x;

        result.at(1, 0) = s.y;
        result.at(1, 1) = u.y;
        result.at(1, 2) = -f.y;

        result.at(2, 0) = s.z;
        result.at(2, 1) = u.z;
        result.at(2, 2) = -f.z;

        result.at(3, 0) = -Vec3::Dot(s, eye);
        result.at(3, 1) = -Vec3::Dot(u, eye);
        result.at(3, 2) = Vec3::Dot(f, eye);

        return result;
    }

    // Transpose
    Mat4 Transposed() const {
        Mat4 result;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                result.at(j, i) = at(i, j);
        return result;
    }

    // Simple inverse for transformation matrices (rotation + translation)
    // Not a general matrix inverse
    Mat4 InverseTransform() const {
        Mat4 result;

        // Transpose of rotation part
        result.at(0, 0) = at(0, 0);
        result.at(0, 1) = at(1, 0);
        result.at(0, 2) = at(2, 0);

        result.at(1, 0) = at(0, 1);
        result.at(1, 1) = at(1, 1);
        result.at(1, 2) = at(2, 1);

        result.at(2, 0) = at(0, 2);
        result.at(2, 1) = at(1, 2);
        result.at(2, 2) = at(2, 2);

        // -R^T * t
        Vec3 t(at(3, 0), at(3, 1), at(3, 2));
        result.at(3, 0) = -(result.at(0,0)*t.x + result.at(1,0)*t.y + result.at(2,0)*t.z);
        result.at(3, 1) = -(result.at(0,1)*t.x + result.at(1,1)*t.y + result.at(2,1)*t.z);
        result.at(3, 2) = -(result.at(0,2)*t.x + result.at(1,2)*t.y + result.at(2,2)*t.z);

        result.at(3, 3) = 1.0f;
        return result;
    }

    // Translate * Rotate * Scale (TRS)
    static Mat4 Trs(const Vec3& position, const Vec3& EulerDeg, const Vec3& Scale) {
        return Translate(position)
             * RotateZ(EulerDeg.z)
             * RotateY(EulerDeg.y)
             * RotateX(EulerDeg.x)
             * Mat4::Scale(Scale);
    }
};

// =========================================================================
// Utility
// =========================================================================
inline float Radians(float deg) { return deg * DEG2RAD; }
inline float Degrees(float rad) { return rad * RAD2DEG; }

inline float Clamp(float v, float min, float max) {
    return v < min ? min : (v > max ? max : v);
}

inline float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

inline Vec3 Lerp(const Vec3& a, const Vec3& b, float t) {
    return a + (b - a) * t;
}

} // namespace AMEE

#endif // __AMEE_MATH_H__