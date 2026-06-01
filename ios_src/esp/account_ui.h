#pragma once

// ============================================================
//  ACCOUNT RESET UI - ImGui Menu
//  Shows only guest reset controls and basic account status
// ============================================================

#include "account_manager.h"
#include "stealth.h"
#include "ui_core.h"

namespace AccountUI {
    void RenderAccountStatus() {
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "=== ACCOUNT RESET ===");

        std::string stateStr = AccountManager::GetStateString();
        ImVec4 stateColor;
        if (AccountManager::g_AccountData.state == AccountState::LoggedIn) {
            stateColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
        } else if (AccountManager::g_AccountData.state == AccountState::Guest) {
            stateColor = ImVec4(0.5f, 0.8f, 1.0f, 1.0f);
        } else if (AccountManager::g_AccountData.state == AccountState::LoginFailed) {
            stateColor = ImVec4(1.0f, 0.8f, 0.0f, 1.0f);
        } else {
            stateColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        }

        ImGui::TextColored(stateColor, "State: %s", stateStr.c_str());

        if (!AccountManager::g_AccountData.accountName.empty()) {
            ImGui::TextColored(TEXT_PRIMARY, "Account: %s", AccountManager::g_AccountData.accountName.c_str());
        } else {
            ImGui::TextColored(TEXT_SECONDARY, "Account: (guest)");
        }

        ImGui::TextColored(TEXT_SECONDARY, "Debug: accountPtr=%p, error=%s",
            (void*)AccountManager::g_AccountData.memoryPtr,
            AccountManager::GetErrorString().c_str());

        if (AccountManager::g_AccountData.resetInProgress) {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "[*] RESET IN PROGRESS [*]");
        }
    }

    void RenderGuestReset() {
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::BeginGroupPanel("Guest Reset", ImVec2(ImGui::GetContentRegionAvail().x, 0));
        ImGui::TextWrapped("Use this panel only to reset the current account state into guest mode.");
        ImGui::Spacing();

        if (ImGui::Button("Reset to Guest Now", ImVec2(ImGui::GetContentRegionAvail().x, 40))) {
            AccountManager::AutoResetAccount(true);
            STEALTH_LOG(@"[Account] Manual guest reset triggered from UI");
        }

        ImGui::EndGroupPanel();
    }

    void RenderAccountMenu() {
        ImGui::BeginChild("GuestResetAccount", ImVec2(0, 0), true);
        ImGui::BeginGroupPanel("Account Reset", ImVec2(ImGui::GetContentRegionAvail().x, 0));
        RenderAccountStatus();
        ImGui::EndGroupPanel();

        ImGui::Spacing();
        RenderGuestReset();
        ImGui::EndChild();
    }
}

// ============================================================
//  INTEGRATION POINT in ImGuiOverlay.mm
// ============================================================
/*
Add this to your ImGui rendering function (e.g., in ImGuiOverlay::Render()):

    if (ImGui::BeginTabItem("Settings")) {
        AccountUI::RenderAccountMenu();
        ImGui::EndTabItem();
    }
*/
