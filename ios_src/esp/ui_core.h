#pragma once
#include "imgui.h"

inline float clampf(float value, float min, float max) {
    return value < min ? min : (value > max ? max : value);
}

inline int clampi(int value, int min, int max) {
    return value < min ? min : (value > max ? max : value);
}

static const ImVec4 AMOLED_BG          = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
static const ImVec4 AMOLED_BG_CHILD    = ImVec4(0.02f, 0.02f, 0.02f, 0.98f);
static const ImVec4 ACCENT_RED         = ImVec4(0.95f, 0.20f, 0.20f, 1.00f);
static const ImVec4 ACCENT_RED_HOVER   = ImVec4(1.00f, 0.30f, 0.30f, 1.00f);
static const ImVec4 ACCENT_RED_ACTIVE  = ImVec4(0.85f, 0.15f, 0.15f, 1.00f);
static const ImVec4 TEXT_PRIMARY       = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
static const ImVec4 TEXT_SECONDARY     = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
static const ImVec4 FRAME_BG           = ImVec4(0.08f, 0.08f, 0.08f, 0.95f);
static const ImVec4 FRAME_BG_HOVER     = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
static const ImVec4 FRAME_BG_ACTIVE    = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);

inline void ApplyDarkAMOLEDTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    style.WindowPadding = ImVec2(12, 12);
    style.FramePadding = ImVec2(8, 6);
    style.ItemSpacing = ImVec2(8, 6);
    style.ItemInnerSpacing = ImVec2(6, 4);
    style.IndentSpacing = 20.0f;
    style.ScrollbarSize = 12.0f;
    style.GrabMinSize = 12.0f;
    
    style.WindowRounding = 12.0f;
    style.ChildRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.PopupRounding = 8.0f;
    style.ScrollbarRounding = 8.0f;
    style.GrabRounding = 6.0f;
    style.TabRounding = 6.0f;
    
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_None;
    
    style.WindowBorderSize = 1.0f;
    style.ChildBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text]                   = TEXT_PRIMARY;
    colors[ImGuiCol_TextDisabled]           = TEXT_SECONDARY;
    colors[ImGuiCol_WindowBg]               = AMOLED_BG;
    colors[ImGuiCol_ChildBg]                = AMOLED_BG_CHILD;
    colors[ImGuiCol_PopupBg]                = ImVec4(0.02f, 0.02f, 0.02f, 0.98f);
    colors[ImGuiCol_Border]                 = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.80f);
    colors[ImGuiCol_FrameBg]                = FRAME_BG;
    colors[ImGuiCol_FrameBgHovered]         = FRAME_BG_HOVER;
    colors[ImGuiCol_FrameBgActive]          = FRAME_BG_ACTIVE;
    colors[ImGuiCol_TitleBg]                = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.75f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.90f);
    colors[ImGuiCol_ScrollbarGrab]          = ACCENT_RED;
    colors[ImGuiCol_ScrollbarGrabHovered]   = ACCENT_RED_HOVER;
    colors[ImGuiCol_ScrollbarGrabActive]    = ACCENT_RED_ACTIVE;
    colors[ImGuiCol_CheckMark]              = ACCENT_RED;
    colors[ImGuiCol_SliderGrab]             = ACCENT_RED;
    colors[ImGuiCol_SliderGrabActive]       = ACCENT_RED_HOVER;
    colors[ImGuiCol_Button]                 = ACCENT_RED;
    colors[ImGuiCol_ButtonHovered]          = ACCENT_RED_HOVER;
    colors[ImGuiCol_ButtonActive]           = ACCENT_RED_ACTIVE;
    colors[ImGuiCol_Header]                 = ImVec4(0.85f, 0.20f, 0.20f, 0.55f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.90f, 0.25f, 0.25f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.95f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.30f, 0.30f, 0.30f, 0.50f);
    colors[ImGuiCol_SeparatorHovered]       = ACCENT_RED_HOVER;
    colors[ImGuiCol_SeparatorActive]        = ACCENT_RED_ACTIVE;
    colors[ImGuiCol_ResizeGrip]             = ACCENT_RED;
    colors[ImGuiCol_ResizeGripHovered]      = ACCENT_RED_HOVER;
    colors[ImGuiCol_ResizeGripActive]       = ACCENT_RED_ACTIVE;
    colors[ImGuiCol_Tab]                    = ImVec4(0.85f, 0.20f, 0.20f, 0.55f);
    colors[ImGuiCol_TabHovered]             = ACCENT_RED_HOVER;
    colors[ImGuiCol_TabActive]              = ACCENT_RED;
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.15f, 0.15f, 0.15f, 0.97f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ACCENT_RED;
    colors[ImGuiCol_PlotLinesHovered]       = ACCENT_RED_HOVER;
    colors[ImGuiCol_PlotHistogram]          = ACCENT_RED;
    colors[ImGuiCol_PlotHistogramHovered]   = ACCENT_RED_HOVER;
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.85f, 0.20f, 0.20f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = ACCENT_RED;
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.00f, 0.00f, 0.00f, 0.70f);
}

inline void ModernCheckbox(const char* label, bool* v) { ImGui::Checkbox(label, v); }
inline void ModernSlider(const char* label, float* v, float min, float max, const char* format = "%.1f") { ImGui::SliderFloat(label, v, min, max, format); }
inline void ModernSliderInt(const char* label, int* v, int min, int max) { ImGui::SliderInt(label, v, min, max); }
inline void ModernRadioButton(const char* label, int* v, int button_value) { ImGui::RadioButton(label, v, button_value); }

namespace ImGui {
    inline void BeginGroupPanel(const char* name, const ImVec2& size = ImVec2(0.0f, 0.0f)) {
        ImGui::BeginChild(name, size, true);
        ImGui::TextColored(ACCENT_RED, "%s", name);
        ImGui::Separator();
        ImGui::Spacing();
    }
    inline void EndGroupPanel() {
        ImGui::EndChild();
    }
}

inline void MetricsHelpMarker(const char* desc)
{
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.8f, 1.0f));
    ImGui::TextDisabled("( ? )");
    ImGui::PopStyleColor();
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.05f, 0.05f, 0.08f, 0.98f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.8f, 0.2f, 0.2f, 0.8f));
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextColored(ImVec4(0.95f, 0.95f, 0.98f, 1.0f), "%s", desc);
        ImGui::PopTextWrapPos();
        ImGui::PopStyleColor(2);
        ImGui::EndTooltip();
    }
}

inline void MelzmodButton(const char* label, ImVec2 size) {
    ImGui::Button(label, size);
}
