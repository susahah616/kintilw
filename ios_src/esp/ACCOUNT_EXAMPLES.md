// ============================================================
//  ACCOUNT MANAGER - INTEGRATION EXAMPLES
//  Example usage patterns and integration points
// ============================================================

#include "account_manager.h"

// ============================================================
//  SCENARIO 1: Handle Game's Login System
// ============================================================

/*
If your game has a LoginManager or AuthService singleton, hook it like this:

Step 1: Get the game's account/login manager singleton
*/
void InitializeGameAccountHook() {
    // Get AccountManager or LoginManager instance from game
    void* gameAccountMgr = nullptr;
    Il2CppGetStaticFieldValue(
        "Assembly-CSharp.dll", 
        "",                           // or the proper namespace
        "AccountManager",             // Game's account class name
        "Instance",                   // Static field name
        &gameAccountMgr
    );
    
    if (gameAccountMgr) {
        AccountManager::g_AccountData.memoryPtr = (uintptr_t)gameAccountMgr;
        NSLog(@"[Account] Game account manager located at 0x%llx", 
              (uintptr_t)gameAccountMgr);
    }
}

/*
Step 2: Hook the login result callback
Place this where the game's login completion is handled:
*/
void HookLoginResult_Example() {
    // When game's login succeeds:
    void OnGameLoginSuccess() {
        AccountManager::SetAccountState(AccountState::LoggedIn);
        NSLog(@"[Account] Game login successful - account state updated");
    }
    
    // When game's login fails:
    void OnGameLoginFailure(const char* errorMsg) {
        // Determine error type from error message or error code
        LoginErrorCode errorCode = LoginErrorCode::ServerError;
        
        if (strstr(errorMsg, "network") || strstr(errorMsg, "Network")) {
            errorCode = LoginErrorCode::NetworkError;
        } else if (strstr(errorMsg, "credential") || strstr(errorMsg, "password")) {
            errorCode = LoginErrorCode::InvalidCredentials;
        } else if (strstr(errorMsg, "timeout") || strstr(errorMsg, "Timeout")) {
            errorCode = LoginErrorCode::Timeout;
        } else if (strstr(errorMsg, "ban") || strstr(errorMsg, "Ban")) {
            errorCode = LoginErrorCode::BannedAccount;
        }
        
        AccountManager::CheckLoginFailure(errorCode, errorMsg);
    }
}

// ============================================================
//  SCENARIO 2: Use in Battle/Game State Monitoring
// ============================================================

/*
Call this periodically in your memory scanning thread to check 
if auto-reset is needed:
*/
void UpdateAccountStateInGameLoop() {
    // Check if we're in lobby (not in battle)
    void* bmInst = nullptr;
    Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "BattleManager", "Instance", &bmInst);
    
    if (!bmInst) {
        // We're in lobby - safe to perform auto-reset if needed
        if (AccountManager::ShouldAutoReset()) {
            NSLog(@"[Account] In lobby, performing auto-reset");
            AccountManager::AutoResetAccount();
        }
    }
}

// ============================================================
//  SCENARIO 3: UI Integration
// ============================================================

/*
Add to your ImGui menu rendering:
*/
void RenderAccountMenuExample() {
    if (ImGui::CollapsingHeader("Account Settings")) {
        // Show current account state
        ImGui::Text("Status: %s", AccountManager::GetStateString().c_str());
        ImGui::Text("Failed Attempts: %d/%d",
            AccountManager::g_AccountData.consecutiveFailures,
            AccountManager::g_AccountData.maxLoginAttempts
        );
        
        // Auto-reset toggle
        bool enabled = AccountManager::g_AccountData.autoResetEnabled;
        if (ImGui::Checkbox("Auto-Reset on Login Fail", &enabled)) {
            AccountManager::g_AccountData.autoResetEnabled = enabled;
        }
        
        // Max attempts slider
        int maxAttempts = AccountManager::g_AccountData.maxLoginAttempts;
        if (ImGui::SliderInt("Max Login Attempts", &maxAttempts, 1, 10)) {
            AccountManager::g_AccountData.maxLoginAttempts = maxAttempts;
        }
        
        // Manual buttons
        if (ImGui::Button("Manual Reset to Guest")) {
            AccountManager::AutoResetAccount();
        }
        
        if (ImGui::Button("Clear Failures")) {
            AccountManager::ClearLoginFailures();
        }
    }
}

// ============================================================
//  SCENARIO 4: Persistent Storage (Save/Load Settings)
// ============================================================

/*
Save auto-reset settings to disk:
*/
void SaveAutoResetSettings(const std::string& configPath) {
    // Using your config system (pseudo-code)
    ConfigFile cfg(configPath);
    
    cfg.Set("account.autoResetEnabled", 
            AccountManager::g_AccountData.autoResetEnabled);
    cfg.Set("account.maxLoginAttempts", 
            AccountManager::g_AccountData.maxLoginAttempts);
    cfg.Set("account.resetDelaySeconds", 
            AccountManager::g_AccountData.resetDelaySeconds);
    cfg.Set("account.autoResetToGuest", 
            AccountManager::g_AccountData.autoResetToGuest);
    
    cfg.Save();
    NSLog(@"[Account] Settings saved");
}

/*
Load auto-reset settings from disk:
*/
void LoadAutoResetSettings(const std::string& configPath) {
    ConfigFile cfg(configPath);
    
    AccountManager::g_AccountData.autoResetEnabled = 
        cfg.Get("account.autoResetEnabled", true);
    AccountManager::g_AccountData.maxLoginAttempts = 
        cfg.Get("account.maxLoginAttempts", 3);
    AccountManager::g_AccountData.resetDelaySeconds = 
        cfg.Get("account.resetDelaySeconds", 5);
    AccountManager::g_AccountData.autoResetToGuest = 
        cfg.Get("account.autoResetToGuest", true);
    
    NSLog(@"[Account] Settings loaded");
}

// ============================================================
//  SCENARIO 5: Advanced - Custom Error Handling
// ============================================================

/*
Handle specific game error codes:
*/
void HandleGameLoginError(int gameErrorCode) {
    LoginErrorCode errorCode = LoginErrorCode::UnknownError;
    std::string message = "Unknown error";
    
    // Map game error codes to our enum
    switch (gameErrorCode) {
        case 1:  // Game: Network error
            errorCode = LoginErrorCode::NetworkError;
            message = "Network connection failed";
            break;
        case 2:  // Game: Invalid account
            errorCode = LoginErrorCode::InvalidCredentials;
            message = "Invalid account or password";
            break;
        case 3:  // Game: Server error
            errorCode = LoginErrorCode::ServerError;
            message = "Server error - please try again later";
            break;
        case 4:  // Game: Timeout
            errorCode = LoginErrorCode::Timeout;
            message = "Connection timeout - please try again";
            break;
        case 5:  // Game: Account banned
            errorCode = LoginErrorCode::BannedAccount;
            message = "Account is banned";
            break;
        default:
            errorCode = LoginErrorCode::UnknownError;
            message = "An error occurred during login";
    }
    
    AccountManager::CheckLoginFailure(errorCode, message);
}

// ============================================================
//  SCENARIO 6: Memory-based Account Reset (if game allows)
// ============================================================

/*
If you can identify the game's account data structure in memory,
implement these functions for persistent reset:
*/

// Example: Offsets for Mobile Legends account structure
#define OFF_ACCOUNT_ID          0x10
#define OFF_ACCOUNT_LOGGED_IN   0x20
#define OFF_ACCOUNT_IS_GUEST    0x21
#define OFF_ACCOUNT_NAME        0x30

void ResetAccountInMemory() {
    uintptr_t ptr = AccountManager::g_AccountData.memoryPtr;
    if (!ptr) return;
    
    using namespace InternalMemory;
    
    // Clear account ID
    Write<uint64_t>(ptr + OFF_ACCOUNT_ID, 0);
    
    // Set logged in = false
    WriteBool(ptr + OFF_ACCOUNT_LOGGED_IN, false);
    
    // Set guest mode = true
    WriteBool(ptr + OFF_ACCOUNT_IS_GUEST, true);
    
    NSLog(@"[Account] Account data reset in game memory");
}

// ============================================================
//  USAGE CHECKLIST
// ============================================================

/*
Integration Steps:

1. [ ] Include account_manager.h in Main.mm
2. [ ] Call AccountManager::Initialize() in memory thread after Il2CppAttach
3. [ ] Hook game's login success callback to SetAccountState(LoggedIn)
4. [ ] Hook game's login failure callback to CheckLoginFailure()
5. [ ] Add account status display to ImGui menu (optional)
6. [ ] Add auto-reset settings to ImGui menu (optional)
7. [ ] Test with intentional login failures
8. [ ] Verify auto-reset triggers after max attempts
9. [ ] Test banned account detection (if applicable)
10.[ ] Save/load auto-reset settings (if desired)

Quick Test:
- Simulate login failure 3+ times
- Verify account resets to guest automatically
- Check NSLog for [Account] messages
- Verify UI shows correct state/error messages
*/
