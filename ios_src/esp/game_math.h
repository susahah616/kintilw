#pragma once

// ============================================================
//  MATH UTILITIES
//  Vector2, Vector3, Matrix4x4, WorldToScreen
// ============================================================

struct Vec2 {
    float x, y;
    Vec2() : x(0), y(0) {}
    Vec2(float x, float y) : x(x), y(y) {}
    Vec2 operator+(const Vec2& o) const { return {x+o.x, y+o.y}; }
    Vec2 operator-(const Vec2& o) const { return {x-o.x, y-o.y}; }
    Vec2 operator*(float f) const { return {x*f, y*f}; }
    float Length() const { return __builtin_sqrtf(x*x + y*y); }
};

struct Vec3 {
    float x, y, z;
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    Vec3 operator-(const Vec3& o) const { return {x-o.x, y-o.y, z-o.z}; }
    float Length() const { return __builtin_sqrtf(x*x + y*y + z*z); }
    float Dist(const Vec3& o) const { return (*this - o).Length(); }
};

// Pure C struct agar Clang ARM64 compiler memperlakukannya sebagai HFA (pass/return via s0-s2 registers)
struct UnityVector3 {
    float x;
    float y;
    float z;
};

struct Matrix4x4 {
    float m[4][4];
};

// ============================================================
//  WorldToScreen
//  Mengkonversi posisi 3D world ke koordinat 2D screen
//  matrix: View Projection matrix dari kamera Unity
// ============================================================
inline bool WorldToScreen(const Vec3& world, Vec2& screen,
                           const Matrix4x4& vp,
                           float screenW, float screenH)
{
    // Clip space
    float clipX = world.x * vp.m[0][0] + world.y * vp.m[1][0] + world.z * vp.m[2][0] + vp.m[3][0];
    float clipY = world.x * vp.m[0][1] + world.y * vp.m[1][1] + world.z * vp.m[2][1] + vp.m[3][1];
    float clipW = world.x * vp.m[0][3] + world.y * vp.m[1][3] + world.z * vp.m[2][3] + vp.m[3][3];

    if (clipW < 0.001f) return false;

    float ndcX =  clipX / clipW;
    float ndcY = -clipY / clipW;

    screen.x = (ndcX + 1.0f) * 0.5f * screenW;
    screen.y = (ndcY + 1.0f) * 0.5f * screenH;

    return true;
}

// ---- Distance 2D ----
inline float Dist2D(const Vec2& a, const Vec2& b) {
    float dx = a.x - b.x, dy = a.y - b.y;
    return __builtin_sqrtf(dx*dx + dy*dy);
}

// ---- Distance 3D ----
inline float Dist3D(const Vec3& a, const Vec3& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return __builtin_sqrtf(dx*dx + dy*dy + dz*dz);
}

// ---- Clamp ----
inline float Clamp(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

// ---- Lerp ----
inline float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

// ---- HP bar color (hijau → kuning → merah) ----
inline void HPBarColor(float pct, float& r, float& g, float& b) {
    // pct: 0.0 = mati, 1.0 = full
    if (pct > 0.6f) {
        r = 0.2f; g = 0.85f; b = 0.3f; // hijau
    } else if (pct > 0.3f) {
        r = 1.0f; g = 0.75f; b = 0.0f; // kuning
    } else {
        r = 0.9f; g = 0.2f; b = 0.2f;  // merah
    }
}

// ============================================================
//  WorldToMinimap
//  Mengkonversi posisi 3D world ke koordinat UI Minimap
// ============================================================
inline Vec2 WorldToMinimap(int campType, const Vec3& heroPos, float mapSize, float startX, float startY) {
    float angleDeg = (campType == 2) ? 314.60f : 134.76f;
    float angle = angleDeg * 0.017453292519943295f; // (PI / 180)
    float angleCos = __builtin_cosf(angle);
    float angleSin = __builtin_sinf(angle);
    
    // Original CodeBreaker Magic Scale Constant
    float mapScale = 74.11f;
    
    float rx = (angleCos * heroPos.x - angleSin * (heroPos.z * -1.0f)) / mapScale;
    float ry = (angleSin * heroPos.x + angleCos * (heroPos.z * -1.0f)) / mapScale;
    
    Vec2 res;
    // Mirror both axes (CodeBreaker + OverlayService logic)
    res.x = (-rx * mapSize) + startX + mapSize / 2.0f;
    res.y = (-ry * mapSize) + startY + mapSize / 2.0f;
    
    return res;
}
