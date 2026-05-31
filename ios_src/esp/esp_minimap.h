#pragma once
#include "imgui.h"
#include "config.h"
#include "game_math.h"
#include "esp_core.h"
#include <math.h>

inline void DrawMinimapBorder(ImDrawList* draw) {
    if (!g_ESPCfg.MinimapESP || g_ESPCfg.MinimapHideBackground) return;

    ImVec2 minPos = ImVec2(g_ESPCfg.MinimapPosX, g_ESPCfg.MinimapPosY);
    ImVec2 maxPos = ImVec2(g_ESPCfg.MinimapPosX + g_ESPCfg.MinimapSize, g_ESPCfg.MinimapPosY + g_ESPCfg.MinimapSize);
    
    // Garis batas redup sebagai jalur
    draw->AddRect(minPos, maxPos, IM_COL32(255, 255, 255, 30), 0.0f, 0, 1.0f);
    
    auto GetPerimeterPos = [](ImVec2 m, float sz, float d) -> ImVec2 {
        d = fmod(d, 4.0f * sz);
        if (d < 0) d += 4.0f * sz;
        if (d < sz) return ImVec2(m.x + d, m.y);
        d -= sz;
        if (d < sz) return ImVec2(m.x + sz, m.y + d);
        d -= sz;
        if (d < sz) return ImVec2(m.x + sz - d, m.y + sz);
        d -= sz;
        return ImVec2(m.x, m.y + sz - d);
    };
    
    // Animasi cahaya berjalan (glowing trail)
    float time = (float)ImGui::GetTime();
    float perimeter = g_ESPCfg.MinimapSize * 4.0f;
    float speed = perimeter * 0.5f; 
    float headDist = fmod(time * speed, perimeter);
    float trailLength = g_ESPCfg.MinimapSize * 1.2f; 
    
    int segments = 30; 
    for (int i = 0; i < segments; i++) {
        float d1 = headDist - (trailLength * i / segments);
        float d2 = headDist - (trailLength * (i + 1) / segments);
        ImVec2 p1 = GetPerimeterPos(minPos, g_ESPCfg.MinimapSize, d1);
        ImVec2 p2 = GetPerimeterPos(minPos, g_ESPCfg.MinimapSize, d2);
        int alpha = 255 - (255 * i / segments); 
        draw->AddLine(p1, p2, IM_COL32(255, 255, 255, alpha), 2.5f);
    }
}

inline void DrawMinimapIcon(ImDrawList* draw, int entityId, int hp, int hpMax, const Vec3& pos, int localCamp) {
    if (!g_ESPCfg.MinimapESP) return;

    Vec2 minimapPos = WorldToMinimap(localCamp, pos, g_ESPCfg.MinimapSize, g_ESPCfg.MinimapPosX, g_ESPCfg.MinimapPosY);
    void* tex = GetHeroIcon(entityId);
    
    float iconSize = g_ESPCfg.MinimapIconSize;
    float iconHalfSize = iconSize / 2.0f;
    
    ImVec2 center = ImVec2(minimapPos.x, minimapPos.y);
    ImVec2 mMin = ImVec2(center.x - iconHalfSize, center.y - iconHalfSize);
    ImVec2 mMax = ImVec2(center.x + iconHalfSize, center.y + iconHalfSize);
    
    if (tex != nullptr) {
        draw->AddImageRounded(tex, mMin, mMax, ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255,255,255,255), iconSize);
        
        float a_max = 3.14159265359f * 2.0f;
        float radius = iconHalfSize;
        
        float healthPercent = (float)hp / (float)hpMax;
        ImU32 healthColor = IM_COL32(50, 255, 50, 255);
        if (healthPercent <= 0.3f) healthColor = IM_COL32(255, 50, 50, 255);
        else if (healthPercent <= 0.5f) healthColor = IM_COL32(255, 200, 50, 255);
        
        float arcThickness = fmaxf(1.5f, iconSize * 0.08f); // proporsional, min 1.5pt
        draw->PathArcTo(center, radius, -(a_max / 4.0f) + (a_max / hpMax) * (hpMax - hp), a_max - (a_max / 4.0f));
        draw->PathStroke(healthColor, ImDrawFlags_None, arcThickness);
    } else {
        draw->AddCircleFilled(center, 8.0f, IM_COL32(255, 0, 0, 255));
        draw->AddCircle(center, 8.0f, IM_COL32(0, 0, 0, 255), 0, 1.5f);
    }
}
