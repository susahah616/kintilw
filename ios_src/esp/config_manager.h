#pragma once
#include "config.h"
#include <fstream>
#include <string>

inline bool SaveConfig() {
    std::ofstream file("/private/var/mobile/mlbb_esp_config.ini");
    if (!file.is_open()) return false;

    file << "AutoLoadSettings=" << AutoLoadSettings << "\n";
    file << "ESPLine=" << Feature.ESPLine << "\n";
    file << "ESPBox=" << Feature.ESPBox << "\n";
    file << "ESPName=" << Feature.ESPName << "\n";
    file << "ESPHero=" << Feature.ESPHero << "\n";
    file << "ESPRound=" << Feature.ESPRound << "\n";
    file << "ESPHealth=" << Feature.ESPHealth << "\n";
    file << "ESPAlertIcon=" << Feature.ESPAlertIcon << "\n";
    file << "ESPAlert=" << Feature.ESPAlert << "\n";
    file << "ESPMinion=" << Feature.ESPMinion << "\n";
    file << "ESPMAlert=" << Feature.ESPMAlert << "\n";
    file << "ESPMRound=" << Feature.ESPMRound << "\n";
    file << "ESPMHealth=" << Feature.ESPMHealth << "\n";
    file << "ESPMBox=" << Feature.ESPMBox << "\n";
    file << "ESPMName=" << Feature.ESPMName << "\n";
    file << "ESPSkillCD=" << Feature.ESPSkillCD << "\n";
    file << "ESPSpellCD=" << Feature.ESPSpellCD << "\n";
    file << "ESPScale=" << Feature.ESPScale << "\n";

    file << "MinimapESP=" << g_ESPCfg.MinimapESP << "\n";
    file << "MinimapHideBackground=" << g_ESPCfg.MinimapHideBackground << "\n";
    file << "MinimapSize=" << g_ESPCfg.MinimapSize << "\n";
    file << "MinimapIconSize=" << g_ESPCfg.MinimapIconSize << "\n";
    file << "MinimapPosX=" << g_ESPCfg.MinimapPosX << "\n";
    file << "MinimapPosY=" << g_ESPCfg.MinimapPosY << "\n";

    file << "AutoRetriEnabled=" << AutoRetriEnabled << "\n";
    file << "RetriLord=" << RetriLord << "\n";
    file << "RetriTurtle=" << RetriTurtle << "\n";
    file << "RetriBuff=" << RetriBuff << "\n";
    file << "RetriCrab=" << RetriCrab << "\n";
    file << "RetriLitho=" << RetriLitho << "\n";
    file << "RetriPosX=" << RetriPos.x << "\n";
    file << "RetriPosY=" << RetriPos.y << "\n";
    file << "window_scale=" << window_scale << "\n";

    file.close();
    return true;
}

inline bool LoadConfig() {
    std::ifstream file("/private/var/mobile/mlbb_esp_config.ini");
    if (!file.is_open()) return false;

    std::string line;

    while (std::getline(file, line)) {
        size_t delim = line.find('=');
        if (delim == std::string::npos) continue;

        std::string key = line.substr(0, delim);
        std::string val = line.substr(delim + 1);

        int v = atoi(val.c_str());
        float f = atof(val.c_str());

        if (key == "AutoLoadSettings") AutoLoadSettings = v;
        else if (key == "ESPLine") Feature.ESPLine = v;
        else if (key == "ESPBox") Feature.ESPBox = v;
        else if (key == "ESPName") Feature.ESPName = v;
        else if (key == "ESPHero") Feature.ESPHero = v;
        else if (key == "ESPRound") Feature.ESPRound = v;
        else if (key == "ESPHealth") Feature.ESPHealth = v;
        else if (key == "ESPAlertIcon") Feature.ESPAlertIcon = v;
        else if (key == "ESPAlert") Feature.ESPAlert = v;
        else if (key == "ESPMinion") Feature.ESPMinion = v;
        else if (key == "ESPMAlert") Feature.ESPMAlert = v;
        else if (key == "ESPMRound") Feature.ESPMRound = v;
        else if (key == "ESPMHealth") Feature.ESPMHealth = v;
        else if (key == "ESPMBox") Feature.ESPMBox = v;
        else if (key == "ESPMName") Feature.ESPMName = v;
        else if (key == "ESPSkillCD") Feature.ESPSkillCD = v;
        else if (key == "ESPSpellCD") Feature.ESPSpellCD = v;
        else if (key == "ESPScale") Feature.ESPScale = f;

        else if (key == "MinimapESP") g_ESPCfg.MinimapESP = v;
        else if (key == "MinimapHideBackground") g_ESPCfg.MinimapHideBackground = v;
        else if (key == "MinimapSize") g_ESPCfg.MinimapSize = v;
        else if (key == "MinimapIconSize") g_ESPCfg.MinimapIconSize = v;
        else if (key == "MinimapPosX") g_ESPCfg.MinimapPosX = f;
        else if (key == "MinimapPosY") g_ESPCfg.MinimapPosY = f;

        else if (key == "AutoRetriEnabled") AutoRetriEnabled = v;
        else if (key == "RetriLord") RetriLord = v;
        else if (key == "RetriTurtle") RetriTurtle = v;
        else if (key == "RetriBuff") RetriBuff = v;
        else if (key == "RetriCrab") RetriCrab = v;
        else if (key == "RetriLitho") RetriLitho = v;
        else if (key == "RetriPosX") RetriPos.x = f;
        else if (key == "RetriPosY") RetriPos.y = f;
        else if (key == "window_scale") window_scale = (f > 0.4f && f < 2.0f) ? f : 1.0f;
    }

    file.close();
    return true;
}