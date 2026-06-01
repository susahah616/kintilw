#pragma once
#include <cstdint>
#include <string>
#include <ctime>
#include "../memory_internal.h"

// ============================================================
//  ACCOUNT MANAGER - Auto Reset on Login Failure
//  Handles game account data and auto-reset logic
// ============================================================

enum class AccountState {
    Unknown = 0,
    LoggedIn = 1,
    Guest = 2,
    LoginFailed = 3,
    ResetPending = 4
};

enum class LoginErrorCode {
    NoError = 0,
    NetworkError = 1,
    InvalidCredentials = 2,
    ServerError = 3,
    Timeout = 4,
    BannedAccount = 5,
    UnknownError = 6
};

struct AccountData {
    // Account Identity
    uint64_t accountId = 0;
    std::string accountName;
    std::string playerId;
    
    // Account State
    AccountState state = AccountState::Guest;
    bool isGuest = true;
    bool isLoggedIn = false;
    
    // Login Tracking
    int loginAttempts = 0;
    int consecutiveFailures = 0;
    time_t lastLoginTime = 0;
    time_t lastFailureTime = 0;
    LoginErrorCode lastErrorCode = LoginErrorCode::NoError;
    std::string lastErrorMessage;
    
    // Auto Reset Settings
    bool autoResetEnabled = true;
    int maxLoginAttempts = 3;
    int resetDelaySeconds = 5;
    bool autoResetToGuest = true;
    
    // Status
    bool needsReset = false;
    bool resetInProgress = false;
    time_t lastResetTime = 0;
    
    // Memory pointer (if managed by game)
    uintptr_t memoryPtr = 0;
};

namespace AccountManager {
    extern AccountData g_AccountData;
    
    // Initialize account manager
    void Initialize();
    
    // Check if login failed and trigger auto-reset if needed
    bool CheckLoginFailure(LoginErrorCode errorCode, const std::string& errorMessage = "");
    
    // Auto reset account to guest
    void AutoResetAccount();
    
    // Update account state
    void SetAccountState(AccountState newState);
    void SetLoginAttempt(bool success);
    
    // Check if auto reset should trigger
    bool ShouldAutoReset();
    
    // Clear login failures
    void ClearLoginFailures();
    
    // Get account state string for UI
    std::string GetStateString();
    std::string GetErrorString();
    
    // Memory reading helpers
    uintptr_t GetAccountPointerFromGame();
    void ReadAccountDataFromGame();
    void WriteAccountDataToGame();
}

extern "C" bool OnGameLoginResult(bool success, int errorCode);
