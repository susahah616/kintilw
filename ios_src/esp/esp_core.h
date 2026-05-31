#pragma once
#include "imgui.h"
#include "config.h"
#include "game_math.h"
#include "icon_loader.h"   // Sistem icon: base64 -> stb_image -> MTLTexture

// GetHeroIcon: void* = (__bridge void*)id<MTLTexture> untuk Metal ImGui backend
inline void* GetHeroIcon(int heroId) {
    if (HeroIcon.empty()) return nullptr;
    auto ic = ICTexture(heroId);
    return ic.IsValid ? ic.texture : nullptr;
}

// GetMonsterIcon: void* = (__bridge void*)id<MTLTexture>
inline void* GetMonsterIcon(int monsterId) {
    if (MonsterIcon.empty()) return nullptr;
    auto ic = MonsterTexture(monsterId);
    return ic.IsValid ? ic.texture : nullptr;
}

// ============================================================
// Unity Native Camera API
// ============================================================
#include "../Il2CppResolver.h"

inline void* GetCameraMain() {
    static void* get_main_method = nullptr;
    if (!get_main_method) {
        get_main_method = Il2CppGetMethodOffset("UnityEngine.CoreModule.dll", "UnityEngine", "Camera", "get_main", 0);
        if (!get_main_method) {
            get_main_method = Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Camera", "get_main", 0);
        }
    }
    if (get_main_method) {
        return reinterpret_cast<void*(*)()>(get_main_method)();
    }
    return nullptr;
}



inline bool UnityWorldToScreen(void* camera, const Vec3& world, Vec2& screen, float screenW, float screenH) {
    if (!camera) return false;
    
    static void* w2s_method = nullptr;
    if (!w2s_method) {
        w2s_method = Il2CppGetMethodOffset("UnityEngine.CoreModule.dll", "UnityEngine", "Camera", "WorldToScreenPoint", 1);
        if (!w2s_method) {
            w2s_method = Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Camera", "WorldToScreenPoint", 1);
        }
    }
    if (w2s_method) {
        UnityVector3 inWorld = { world.x, world.y, world.z };
        UnityVector3 result = reinterpret_cast<UnityVector3(*)(void*, UnityVector3)>(w2s_method)(camera, inWorld);
        
        // Z < 0 berarti objek ada di belakang kamera
        if (result.z < 0.01f) return false;
        
        // Unity Camera.WorldToScreenPoint mengembalikan koordinat dalam Unity Screen Pixels.
        // Unity Screen.width/height = resolusi render game (bisa 720p/1080p, BUKAN resolusi device).
        // ImGui memakai Point coords (io.DisplaySize) = device points.
        // Konversi: screen_pt = result_px * (displayPt / unityScreenPx)
        
        int unityW = GetUnityScreenWidth();
        int unityH = GetUnityScreenHeight();
        
        float espScale = g_ESPCfg.ScreenScale > 0.1f ? g_ESPCfg.ScreenScale : 1.0f;
        
        float scaleX, scaleY;
        if (unityW > 0 && unityH > 0) {
            // Pakai rasio Unity screen → ImGui display (paling akurat)
            scaleX = (float)unityW / (screenW * espScale);
            scaleY = (float)unityH / (screenH * espScale);
        } else {
            // Fallback: pakai contentScaleFactor
            float cs = g_ContentScaleFactor * espScale;
            scaleX = scaleY = cs;
        }
        
        // Unity origin: bottom-left. ImGui origin: top-left.
        screen.x = (result.x / scaleX) + g_ESPCfg.ScreenOffsetX;
        screen.y = (screenH - (result.y / scaleY)) + g_ESPCfg.ScreenOffsetY;
        return true;
    }
    return false;
}

// Forward declaration untuk menghindari circular dependency
void DrawPlayerESP(ImDrawList* draw, void* camera, float screenW, float screenH, bool hasSelf, const ImVec2& selfPosVec2);
void DrawMonsterESP(ImDrawList* draw, void* camera, float screenW, float screenH);

inline void RenderESPCore() {
    // Load icons sekali saat frame pertama (background thread GL)
    InitAllIcons();
    
    // Dapatkan instance Camera.main dari Unity
    void* cameraMain = GetCameraMain();
    
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    ImGuiIO& io = ImGui::GetIO();
    
    // Memanggil fungsi ESP dari esp_player.h
    DrawPlayerESP(drawList, cameraMain, io.DisplaySize.x, io.DisplaySize.y, false, ImVec2(0,0));
    DrawMonsterESP(drawList, cameraMain, io.DisplaySize.x, io.DisplaySize.y);
}

inline float GetDynamicOffset(float screenHeight, float fovScale) {
    float baseOffset = screenHeight / 6.5f;
    return baseOffset / fovScale;
}

inline ImU32 GetHealthColor(int health, int max_health) {
    float healthPercent = (float)health / (float)max_health;
    if (healthPercent > 0.75f) {
        return IM_COL32(50, 255, 50, 255);
    } else if (healthPercent > 0.5f) {
        int green = 255;
        int red = (int)(255 * (1.0f - (healthPercent - 0.5f) * 2.0f));
        return IM_COL32(red, green, 50, 255);
    } else if (healthPercent > 0.25f) {
        int red = 255;
        int green = (int)(255 * (healthPercent * 2.0f));
        return IM_COL32(red, green, 50, 255);
    } else {
        return IM_COL32(255, 50, 50, 255);
    }
}
