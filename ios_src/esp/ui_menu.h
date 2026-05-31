#pragma once
#include "imgui.h"
#include "config.h"
#include "ui_core.h"
#include "config_manager.h"
#include "entity.h"

inline void SyncFeatureToESP() {
    g_ESPCfg.ESPLine      = Feature.ESPLine;
    g_ESPCfg.ESPBox       = Feature.ESPBox;
    g_ESPCfg.ESPName      = Feature.ESPName;
    g_ESPCfg.ESPHealth    = Feature.ESPHealth;
    g_ESPCfg.ESPHero      = Feature.ESPHero;
    g_ESPCfg.ESPRound     = Feature.ESPRound;
    g_ESPCfg.ESPAlert     = Feature.ESPAlert;
    g_ESPCfg.ESPAlertIcon = Feature.ESPAlertIcon;
    g_ESPCfg.ESPMinion    = Feature.ESPMinion;
    g_ESPCfg.ESPMAlert    = Feature.ESPMAlert;
    g_ESPCfg.ESPMRound    = Feature.ESPMRound;
    g_ESPCfg.ESPMHealth   = Feature.ESPMHealth;
    g_ESPCfg.ESPMBox      = Feature.ESPMBox;
    g_ESPCfg.ESPMName     = Feature.ESPMName;
    g_ESPCfg.ESPSkillCD   = Feature.ESPSkillCD;
    g_ESPCfg.ESPSpellCD   = Feature.ESPSpellCD;
    g_ESPCfg.ScreenScale   = Feature.ESPScale;
    // g_ESPCfg.MinimapESP dikontrol LANGSUNG dari menu tab Minimap
    // JANGAN override di sini — akan bikin toggle tidak bisa diubah!
}

inline void RenderRetriDot() {
    if (!AutoRetriEnabled) return;

    ImGui::SetNextWindowPos(RetriPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(80, 80), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.0f); 

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                             ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;
    
    if (!bShowMenu) {
        flags |= ImGuiWindowFlags_NoInputs;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    if (ImGui::Begin("RetriDot", NULL, flags)) {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        
        draw_list->AddCircleFilled(ImVec2(p.x + 40, p.y + 40), 30.0f, IM_COL32(255, 50, 50, 150));
        draw_list->AddCircle(ImVec2(p.x + 40, p.y + 40), 30.0f, IM_COL32(255, 255, 255, 255), 0, 2.0f);
        
        const char* text = "R";
        ImVec2 textSize = ImGui::CalcTextSize(text);
        draw_list->AddText(ImVec2(p.x + 40 - textSize.x/2, p.y + 40 - textSize.y/2), IM_COL32(255, 255, 255, 255), text);
        
        if (bShowMenu) {
            RetriPos = ImGui::GetWindowPos();
            draw_list->AddText(ImVec2(p.x, p.y - 20), IM_COL32(255, 255, 0, 255), "Drag to Retri Button");
        }
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

// Texture logo menu — void* = (__bridge void*)id<MTLTexture> Metal texture
static void* g_MenuLogoTex = nullptr;
static bool g_MenuLogoLoaded = false;

static ImVec2 menuPos = ImVec2(20, 20);
static ImVec2 menuSize = ImVec2(520, 360);
static ImVec2 minSize  = ImVec2(440, 300);
static ImVec2 maxSize  = ImVec2(900, 700);

inline void ShowMenu()
{
    bFullChecked = true;
    ImGuiIO& io = ImGui::GetIO();
    
    // Set FontGlobalScale hanya saat pertama atau berubah (bukan setiap frame)
    static float lastWindowScale = -1.0f;
    if (lastWindowScale != window_scale) {
        io.FontGlobalScale = window_scale;
        lastWindowScale = window_scale;
    }
    
    ApplyDarkAMOLEDTheme();

    if (!bShowMenu) {
        // Tombol toggle sekarang ditangani oleh native UIButton di ImGuiOverlay.mm
        // Jadi kita tidak perlu render apapun via ImGui jika menu tertutup
        return;
    }

    ImGui::SetNextWindowPos(menuPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(menuSize, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSizeConstraints(minSize, maxSize);

    std::string strTittle = "@gigitoogood" + tittle;
    if (ImGui::Begin(strTittle.c_str(), &bShowMenu, ImGuiWindowFlags_NoCollapse)) {
        
        menuPos = ImGui::GetWindowPos();
        menuSize = ImGui::GetWindowSize();

        const float SIDEBAR_WIDTH = 140.0f;  // Lebih kecil agar content area lebih luas
        ImGui::BeginChild("sidebar", ImVec2(SIDEBAR_WIDTH, 0), true, ImGuiWindowFlags_NoScrollbar);
        
        static int selectedTab = 0;
        const char* tabs[] = {"Home", "Visual", "Auto Retri", "Minimap", "Settings"};
        for (int i = 0; i < 5; ++i) {
            if (ImGui::Button(tabs[i], ImVec2(ImGui::GetContentRegionAvail().x, 45))) {
                selectedTab = i;
            }
            ImGui::Spacing();
        }
        
        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::BeginChild("content", ImVec2(0, 0), true);

        if (selectedTab == 0) {
            ImGui::BeginGroupPanel(" INFO MOD", ImVec2(ImGui::GetContentRegionAvail().x, 0));
            ImGui::TextColored(TEXT_PRIMARY, "Author   : gigi");
            ImGui::TextColored(TEXT_PRIMARY, "Version  : 3.0");
            ImGui::TextColored(TEXT_PRIMARY, "Status   : -");
            ImGui::Spacing();
            if (ImGui::Button("CODE BREAKER GROUP", ImVec2(ImGui::GetContentRegionAvail().x, 48))) {
                // Telegram
            }
            ImGui::EndGroupPanel();

            ImGui::Spacing();
            ImGui::BeginGroupPanel("Menu Setting", ImVec2(ImGui::GetContentRegionAvail().x, 0));
            ModernCheckbox("Auto Resize", &bFlagAutoResize);
            ModernSlider("UI Scale", &window_scale, 0.6f, 1.4f, "%.1f");
            window_scale = clampf(window_scale, 0.6f, 1.4f);

            if (ImGui::Button("Clear Cache", ImVec2(ImGui::GetContentRegionAvail().x, 48))) {
                // ClearGameCache
            }

            ImGui::Spacing();
            ImGui::TextWrapped("To show menu again, tap the CODE BREAKER button at bottom-left.");
            ImGui::Separator();
            ImGui::EndGroupPanel();
        }
        else if (selectedTab == 1) {
            float columnWidth = (ImGui::GetContentRegionAvail().x - 8) / 2;
            
            ImGui::BeginChild("VisualLeft", ImVec2(columnWidth, 0), true);
            ImGui::BeginGroupPanel("Player", ImVec2(0, 0));
            ModernCheckbox("ESPLine", &Feature.ESPLine);
            ModernCheckbox("ESPBox", &Feature.ESPBox);
            ModernCheckbox("ESPName", &Feature.ESPName);
            ModernCheckbox("ESPHero", &Feature.ESPHero);
            ModernCheckbox("ESPRound", &Feature.ESPRound);
            ModernCheckbox("ESPHealth", &Feature.ESPHealth);
            ModernCheckbox("ESPAlertIcon", &Feature.ESPAlertIcon);
            ModernCheckbox("Show Hero Alert", &Feature.ESPAlert);
            ImGui::EndGroupPanel();
            
            ImGui::BeginGroupPanel("Drone View", ImVec2(0, 0));
            ModernSlider("##DroneView", &SetFieldOfView, 0, 60, "%.1f");
            ImGui::EndGroupPanel();
            ImGui::EndChild();

            ImGui::SameLine();
            ImGui::BeginChild("VisualRight", ImVec2(0, 0), true);
            ImGui::BeginGroupPanel("Monster", ImVec2(0, 0));
            ModernCheckbox("Minion Dot", &Feature.ESPMinion);
            ModernCheckbox("Monster Alert", &Feature.ESPMAlert);
            ModernCheckbox("Monster Round", &Feature.ESPMRound);
            ModernCheckbox("Monster Health", &Feature.ESPMHealth);
            ModernCheckbox("Monster Box", &Feature.ESPMBox);
            ModernCheckbox("Monster Name", &Feature.ESPMName);
            ImGui::EndGroupPanel();

            ImGui::BeginGroupPanel("Cooldown", ImVec2(0, 0));
            ModernCheckbox("Cooldown Skill", &Feature.ESPSkillCD);
            ModernCheckbox("Cooldown Spell", &Feature.ESPSpellCD);
            ImGui::EndGroupPanel();
            ImGui::EndChild();
        }
        else if (selectedTab == 2) {
            float columnWidth = (ImGui::GetContentRegionAvail().x - 8) / 2;
            ImGui::BeginChild("RetriLeft", ImVec2(columnWidth, 0), true);
            ImGui::BeginGroupPanel("Auto Retri Master", ImVec2(0, 0));
            ModernCheckbox("Enable Auto Retri", &AutoRetriEnabled);
            ImGui::SameLine(); MetricsHelpMarker("Automatically casts Retribution on selected targets when their HP falls below your Retribution damage.");
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ModernCheckbox("Target Lord", &RetriLord);
            ModernCheckbox("Target Turtle", &RetriTurtle);
            ModernCheckbox("Target Buff (Purple/Red)", &RetriBuff);
            ModernCheckbox("Target Crab (Gold)", &RetriCrab);
            ModernCheckbox("Target Lithowanderer", &RetriLitho);
            ImGui::EndGroupPanel();
            ImGui::EndChild();

            ImGui::SameLine();
            ImGui::BeginChild("RetriRight", ImVec2(0, 0), true);
            ImGui::BeginGroupPanel("Info", ImVec2(0, 0));
            ImGui::TextWrapped("Aktifkan Auto Retri, lalu geser titik merah 'R' ke atas tombol Retribution Anda di layar.");
            ImGui::EndGroupPanel();
            ImGui::EndChild();
        }
        else if (selectedTab == 3) {
            ImGui::BeginChild("KontenMinimap", ImVec2(0, 0), true);
            ImGui::BeginGroupPanel("Radar Minimap", ImVec2(0, 0));
            ModernCheckbox("Enable Minimap ESP", &g_ESPCfg.MinimapESP);
            ModernCheckbox("Sembunyikan Garis Latar", &g_ESPCfg.MinimapHideBackground);
            
            if (g_ESPCfg.MinimapESP) {
                ImGui::Spacing();
                ImGui::Text("Konfigurasi Peta");
                ModernSliderInt("Minimap Size", &g_ESPCfg.MinimapSize, 100, 600);
                ModernSliderInt("Icon Size", &g_ESPCfg.MinimapIconSize, 16, 64);
                ModernSlider("Pos X", &g_ESPCfg.MinimapPosX, -50.0f, 600.0f, "%.1f");
                ModernSlider("Pos Y", &g_ESPCfg.MinimapPosY, -50.0f, 600.0f, "%.1f");
                g_ESPCfg.MinimapSize = clampi(g_ESPCfg.MinimapSize, 100, 600);
                g_ESPCfg.MinimapIconSize = clampi(g_ESPCfg.MinimapIconSize, 16, 64);
                g_ESPCfg.MinimapPosX = clampf(g_ESPCfg.MinimapPosX, -50.0f, 600.0f);
                g_ESPCfg.MinimapPosY = clampf(g_ESPCfg.MinimapPosY, -50.0f, 600.0f);
            }
            ImGui::EndGroupPanel();
            ImGui::EndChild();
        }
        else if (selectedTab == 4) {
            ImGui::BeginChild("KontenSettings", ImVec2(0, 0), true);
            ImGui::BeginGroupPanel("Global Settings", ImVec2(0, 0));
            
            ModernCheckbox("Auto Load Settings", &AutoLoadSettings);
            ImGui::SameLine(); MetricsHelpMarker("If enabled, settings will automatically load when ESP starts.");
            
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::TextColored(ACCENT_RED, "ESP Screen Alignment");
            ModernSlider("Screen Scale", &Feature.ESPScale, 0.5f, 3.0f, "%.2f");
            Feature.ESPScale = clampf(Feature.ESPScale, 0.5f, 3.0f);
            ImGui::TextWrapped("Screen offset otomatis terdeteksi dari safe area iOS (notch/Dynamic Island).");
            
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            if (ImGui::Button("Save Settings", ImVec2(ImGui::GetContentRegionAvail().x, 40))) {
    toastsaveload = SaveConfig() ? "Settings Saved!" : "Save Failed!";
}

ImGui::Spacing();

if (ImGui::Button("Load Settings", ImVec2(ImGui::GetContentRegionAvail().x, 40))) {
    toastsaveload = LoadConfig() ? "Settings Loaded!" : "Load Failed!";
}

if (!toastsaveload.empty()) {
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "%s", toastsaveload.c_str());
}

ImGui::EndGroupPanel();
ImGui::EndChild();
}

ImGui::EndChild();
}

ImGui::End();
}
