#pragma once
#include <string>
#include "imgui.h"

// Variables Dummy
extern bool bShowMenu;
extern bool bFlagAutoResize;
extern float window_scale;
extern bool bFullChecked;
extern std::string tittle;
extern std::string toastsaveload;
extern int screenWidth;
extern int screenHeight;
extern float g_ContentScaleFactor; // view.contentScaleFactor (2.0 / 3.0 Retina)
extern float g_SafeAreaTop;        // safeArea.top dalam points (notch/Dynamic Island)

struct FeatureState {
    bool MinimapIcon = true;
    bool HideLine = false;
    bool ESPLine = false;
    bool ESPBox = true;
    bool ESPName = true;
    bool ESPHero = true;
    bool ESPRound = false;
    bool ESPHealth = true;
    bool ESPAlertIcon = true;
    bool ESPAlert = true;
    bool ESPMinion = false;
    bool ESPMAlert = false;
    bool ESPMRound = false;
    bool ESPMHealth = false;
    bool ESPMBox = false;
    bool ESPMName = false;
    bool ESPSkillCD = true;
    bool ESPSpellCD = true;
    // ESPOffsetX / ESPOffsetY dihapus — auto-detect dari safe area iOS
    float ESPScale = 1.0f;
};
extern FeatureState Feature;

struct EspConfig {
    bool ESPBox;
    bool ESPLine;
    bool ESPRound;
    bool ESPName;
    bool ESPHero;
    bool ESPHealth;
    bool ESPAlert;
    bool ESPAlertIcon;
    bool ESPHealthVertical;
    float RoundBox = 8.0f;
    int AlertSz = 64;
    int IconSZ = 64;
    
    bool ESPSkillCD;
    bool ESPSpellCD;
    
    bool ESPMRound;
    bool ESPMHealth;
    bool ESPMinion;
    bool ESPMName;
    bool ESPMBox;
    bool ESPMAlert;
    
    // Minimap Radar
    bool MinimapESP;
    bool MinimapHideBackground;
    float MinimapPosX = 86.25f;
    float MinimapPosY = 0.0f;
    int MinimapSize = 274;
    int MinimapIconSize = 32;
    
    // Screen Offsets — dikontrol penuh oleh auto-detect safe area iOS (notch/Dynamic Island)
    // Nilai di sini hanya SEMENTARA per-frame, tidak boleh disimpan/dimuat dari config
    float ScreenOffsetX = 0.0f;
    float ScreenOffsetY = 0.0f; // 0.0f baseline; safeArea.top ditambahkan tiap frame oleh ImGuiOverlay.mm
    float ScreenScale = 1.0f;
};
extern EspConfig g_ESPCfg;

// Variabel Auto Retri
extern bool AutoRetriEnabled;
extern bool RetriLord;
extern bool RetriTurtle;
extern bool RetriCrab;
extern bool RetriBuff;
extern bool RetriLitho;
extern ImVec2 RetriPos;

// UI & Menu Variables
extern float SetFieldOfView;
extern bool AutoLoadSettings;

// Posisi & Ukuran
struct Vector2D { float x, y; };
extern Vector2D StartPos;
extern int MapSize;
extern int ICSize;
extern int ICHealthThin;
extern bool isTeam;
extern bool isEnemy;
extern int selectedOption;
