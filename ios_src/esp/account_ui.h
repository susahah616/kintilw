#pragma once

// ============================================================
//  ACCOUNT UI INTEGRATION - ImGui Menu
//  Renders account status, settings, and reset controls
// ============================================================

#include "account_manager.h"
#include "stealth.h"
#include "config.h"
#include "ui_core.h"

namespace AccountUI {
    
    void RenderAccountStatus() {
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "=== ACCOUNT STATUS ===");
        
        // Current state
        std::string stateStr = AccountManager::GetStateString();
        ImVec4 stateColor;
        if (AccountManager::g_AccountData.state == AccountState::LoggedIn) {
            stateColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
        } else if (AccountManager::g_AccountData.state == AccountState::Guest) {
            stateColor = ImVec4(0.5f, 0.8f, 1.0f, 1.0f); // Blue
        } else if (AccountManager::g_AccountData.state == AccountState::LoginFailed) {
            stateColor = ImVec4(1.0f, 0.8f, 0.0f, 1.0f); // Orange/Yellow
        } else {
            stateColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
        }
        
        ImGui::TextColored(stateColor, "State: %s", stateStr.c_str());
        
        // Account info
        if (!AccountManager::g_AccountData.accountName.empty()) {
            ImGui::TextColored(TEXT_PRIMARY, "Account: %s", AccountManager::g_AccountData.accountName.c_str());
        } else {
            ImGui::TextColored(TEXT_SECONDARY, "Account: (guest)");
        }
        
        // Login attempts
        ImGui::Text("Login Attempts: %d", AccountManager::g_AccountData.loginAttempts);
        
        // Consecutive failures
        if (AccountManager::g_AccountData.consecutiveFailures > 0) {
            ImVec4 failColor = AccountManager::g_AccountData.consecutiveFailures >= 
                AccountManager::g_AccountData.maxLoginAttempts ? 
                ACCENT_RED : ImVec4(1.0f, 0.8f, 0.0f, 1.0f);
            ImGui::TextColored(failColor, "Failed Attempts: %d/%d", 
                AccountManager::g_AccountData.consecutiveFailures,
                AccountManager::g_AccountData.maxLoginAttempts);
        }
        
        // Error message
        if (AccountManager::g_AccountData.lastErrorCode != LoginErrorCode::NoError) {
            ImGui::TextColored(ACCENT_RED, 
                "Error: %s", AccountManager::GetErrorString().c_str());
            if (!AccountManager::g_AccountData.lastErrorMessage.empty()) {
                ImGui::TextWrapped("Details: %s", AccountManager::g_AccountData.lastErrorMessage.c_str());
            }
        }
        
        // Reset pending indicator
        if (AccountManager::g_AccountData.needsReset) {
            ImGui::Spacing();
            ImGui::TextColored(ACCENT_RED, "[!] AUTO-RESET PENDING [!]");
        }
        
        if (AccountManager::g_AccountData.resetInProgress) {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "[*] RESET IN PROGRESS [*]");
        }
    }
    
    void RenderAccountSettings() {
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::BeginGroupPanel("Auto-Reset Settings", ImVec2(ImGui::GetContentRegionAvail().x, 0));
        
        // Enable/disable - sync with AccountManager
        ModernCheckbox("Enable Auto-Reset on Login Fail", &AutoResetAccountEnabled);
        AccountManager::g_AccountData.autoResetEnabled = AutoResetAccountEnabled;
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Automatically resets account to guest after failed login attempts");
        }
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        if (AutoResetAccountEnabled) {
            // Max attempts slider - sync with AccountManager
            ModernSliderInt("Max Login Attempts", &AutoResetMaxAttempts, 1, 10);
            AccountManager::g_AccountData.maxLoginAttempts = AutoResetMaxAttempts;
            
            // Reset delay slider - sync with AccountManager
            ModernSliderInt("Reset Delay (seconds)", &AutoResetDelaySeconds, 0, 30);
            AccountManager::g_AccountData.resetDelaySeconds = AutoResetDelaySeconds;
            
            // Reset to guest checkbox - sync with AccountManager
            ModernCheckbox("Reset to Guest Mode", &AutoResetToGuest);
            AccountManager::g_AccountData.autoResetToGuest = AutoResetToGuest;
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("When true: reset to guest. When false: logout completely");
            }
        } else {
            ImGui::TextColored(TEXT_SECONDARY, "Auto-reset is currently disabled");
        }
        
        ImGui::EndGroupPanel();
        
        ImGui::Spacing();
        ImGui::BeginGroupPanel("Quick Actions", ImVec2(ImGui::GetContentRegionAvail().x, 0));
        
        // Manual reset button
        if (ImGui::Button("Reset to Guest Now", ImVec2(ImGui::GetContentRegionAvail().x, 40))) {
            AccountManager::AutoResetAccount();
            STEALTH_LOG(@"[Account] Manual reset triggered from UI");
        }
        
        // Clear failures button
        ImGui::Spacing();
        if (ImGui::Button("Clear Login Failures", ImVec2(ImGui::GetContentRegionAvail().x, 40))) {
            AccountManager::ClearLoginFailures();
            STEALTH_LOG(@"[Account] Login failures cleared from UI");
        }
        
        ImGui::EndGroupPanel();
    }
    
    void RenderAccountMenu() {
        ImGui::BeginChild("KontenAccount", ImVec2(0, 0), true);
        
        ImGui::BeginGroupPanel("Account Status", ImVec2(ImGui::GetContentRegionAvail().x, 0));
        RenderAccountStatus();
        ImGui::EndGroupPanel();
        
        ImGui::Spacing();
        RenderAccountSettings();
        
        ImGui::EndChild();
    }
}

// ============================================================
//  INTEGRATION POINT in ImGuiOverlay.mm
// ============================================================
/*
Add this to your ImGui rendering function (e.g., in ImGuiOverlay::Render()):

    // Example in a menu section
    if (ImGui::BeginTabItem("Settings")) {
        // ... existing settings ...
        
        AccountUI::RenderAccountMenu();
        
        ImGui::EndTabItem();
    }

Or standalone:
    
    AccountUI::RenderAccountStatus();   // Just show status
    AccountUI::RenderAccountSettings(); // Just show settings
    AccountUI::RenderAccountMenu();     // Full menu with collapsing header
*/
