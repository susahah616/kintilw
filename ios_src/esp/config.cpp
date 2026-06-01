#include "config.h"

// Variables Dummy
bool bShowMenu = false;
bool bFlagAutoResize = false;
float window_scale = 1.0f;
bool bFullChecked = false;
std::string tittle = "MLBB Internal iOS";
std::string toastsaveload = "";
int screenWidth = 0;
int screenHeight = 0;
float g_ContentScaleFactor = 2.0f; // default 2x Retina, di-update tiap frame dari ImGuiOverlay
float g_SafeAreaTop = 0.0f;        // di-update tiap frame dari ImGuiOverlay

FeatureState Feature;
EspConfig g_ESPCfg;

// Variabel Auto Retri
bool AutoRetriEnabled = false;
bool RetriLord = true;
bool RetriTurtle = true;
bool RetriCrab = false;
bool RetriBuff = false;
bool RetriLitho = false;
ImVec2 RetriPos = ImVec2(0,0);
// Auto Account Reset Settings
bool AutoResetAccountEnabled = true;
int AutoResetMaxAttempts = 3;
int AutoResetDelaySeconds = 5;
bool AutoResetToGuest = true;
// Stream Detection Settings
bool StreamSafeModeEnabled = true;
bool StreamDetected = false;
// UI & Menu Variables
float SetFieldOfView = 60.0f;
bool AutoLoadSettings = false;

// Posisi & Ukuran
Vector2D StartPos = {0, 0};
int MapSize = 100;
int ICSize = 30;
int ICHealthThin = 5;
bool isTeam = false;
bool isEnemy = true;
int selectedOption = 0;
