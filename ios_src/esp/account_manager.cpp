#include "account_manager.h"
#include "stealth.h"
#include "../Il2CppResolver.h"
#include <ctime>
#include <iostream>

namespace {
    static LoginErrorCode MapLoginErrorCode(int errorCode) {
        switch (errorCode) {
            case 1:
                return LoginErrorCode::NetworkError;
            case 2:
                return LoginErrorCode::InvalidCredentials;
            case 3:
                return LoginErrorCode::ServerError;
            case 4:
                return LoginErrorCode::Timeout;
            case 5:
                return LoginErrorCode::BannedAccount;
            default:
                return LoginErrorCode::UnknownError;
        }
    }
}

namespace AccountManager {
    AccountData g_AccountData;
    
    void Initialize() {
        STEALTH_LOG(@"[Account] AccountManager initialized");
        g_AccountData.state = AccountState::Guest;
        g_AccountData.isGuest = true;
        g_AccountData.isLoggedIn = false;
        g_AccountData.autoResetEnabled = true;
        g_AccountData.maxLoginAttempts = 3;
        g_AccountData.resetDelaySeconds = 5;
        g_AccountData.autoResetToGuest = true;
        g_AccountData.consecutiveFailures = 0;
        g_AccountData.loginAttempts = 0;
    }
    
    bool CheckLoginFailure(LoginErrorCode errorCode, const std::string& errorMessage) {
        if (!g_AccountData.autoResetEnabled) {
            return false;
        }
        
        // Record login failure
        g_AccountData.lastErrorCode = errorCode;
        g_AccountData.lastErrorMessage = errorMessage;
        g_AccountData.lastFailureTime = std::time(nullptr);
        g_AccountData.consecutiveFailures++;
        
        STEALTH_LOG(@"[Account] Login failure detected: %d (consecutive: %d)", 
              (int)errorCode, g_AccountData.consecutiveFailures);
        
        // Check if auto reset should trigger
        if (ShouldAutoReset()) {
            STEALTH_LOG(@"[Account] Triggering auto-reset...");
            AutoResetAccount();
            return true;
        }
        
        return false;
    }
    
    bool ShouldAutoReset() {
        // Check conditions for auto reset
        if (!g_AccountData.autoResetEnabled) {
            return false;
        }
        
        // Trigger if consecutive failures exceed max attempts
        if (g_AccountData.consecutiveFailures >= g_AccountData.maxLoginAttempts) {
            return true;
        }
        
        // Trigger if login errors indicate permanent failure (ban, etc.)
        if (g_AccountData.lastErrorCode == LoginErrorCode::BannedAccount) {
            return true;
        }
        
        return false;
    }
    
    void AutoResetAccount(bool forceGuest) {
        if (g_AccountData.resetInProgress) {
            return;
        }
        
        g_AccountData.resetInProgress = true;
        g_AccountData.needsReset = true;
        g_AccountData.state = AccountState::ResetPending;
        
        STEALTH_LOG(@"[Account] Starting auto-reset process...");
        
        bool targetGuest = forceGuest || g_AccountData.autoResetToGuest;

        // Reset account data
        g_AccountData.accountId = 0;
        g_AccountData.accountName = "";
        g_AccountData.playerId = "";
        g_AccountData.isLoggedIn = false;
        g_AccountData.isGuest = targetGuest;
        g_AccountData.loginAttempts = 0;
        g_AccountData.consecutiveFailures = 0;
        g_AccountData.lastErrorCode = LoginErrorCode::NoError;
        g_AccountData.lastErrorMessage = "";
        
        // Persist reset to game memory
        WriteAccountDataToGame();

        // Update state after write
        g_AccountData.state = AccountState::Guest;
        g_AccountData.lastResetTime = std::time(nullptr);
        
        if (targetGuest) {
            STEALTH_LOG(@"[Account] Account reset to guest successfully");
        } else {
            STEALTH_LOG(@"[Account] Account logged out successfully");
        }
        
        // Clear UI login state if needed (implement based on your UI framework)
        // ClearUILoginState();
        
        g_AccountData.resetInProgress = false;
        g_AccountData.needsReset = false;
    }
    
    void SetAccountState(AccountState newState) {
        g_AccountData.state = newState;
        
        switch (newState) {
            case AccountState::LoggedIn:
                g_AccountData.isLoggedIn = true;
                g_AccountData.isGuest = false;
                g_AccountData.consecutiveFailures = 0; // Clear failures on successful login
                STEALTH_LOG(@"[Account] Account logged in");
                break;
                
            case AccountState::Guest:
                g_AccountData.isLoggedIn = false;
                g_AccountData.isGuest = true;
                STEALTH_LOG(@"[Account] Account set to guest");
                break;
                
            case AccountState::LoginFailed:
                g_AccountData.isLoggedIn = false;
                STEALTH_LOG(@"[Account] Login failed");
                break;
                
            default:
                break;
        }
    }
    
    void SetLoginAttempt(bool success) {
        g_AccountData.loginAttempts++;
        
        if (success) {
            STEALTH_LOG(@"[Account] Login attempt successful");
            SetAccountState(AccountState::LoggedIn);
            g_AccountData.consecutiveFailures = 0;
            g_AccountData.lastLoginTime = std::time(nullptr);
        } else {
            STEALTH_LOG(@"[Account] Login attempt failed");
            SetAccountState(AccountState::LoginFailed);
            CheckLoginFailure(LoginErrorCode::UnknownError, "Login attempt failed");
        }
    }
    
    void ClearLoginFailures() {
        g_AccountData.consecutiveFailures = 0;
        g_AccountData.loginAttempts = 0;
        g_AccountData.lastErrorCode = LoginErrorCode::NoError;
        g_AccountData.lastErrorMessage = "";
        g_AccountData.lastFailureTime = 0;
        
        STEALTH_LOG(@"[Account] Login failures cleared");
    }
    
    std::string GetStateString() {
        switch (g_AccountData.state) {
            case AccountState::LoggedIn:
                return "Logged In";
            case AccountState::Guest:
                return "Guest";
            case AccountState::LoginFailed:
                return "Login Failed";
            case AccountState::ResetPending:
                return "Reset Pending";
            default:
                return "Unknown";
        }
    }
    
    std::string GetErrorString() {
        switch (g_AccountData.lastErrorCode) {
            case LoginErrorCode::NoError:
                return "No Error";
            case LoginErrorCode::NetworkError:
                return "Network Error";
            case LoginErrorCode::InvalidCredentials:
                return "Invalid Credentials";
            case LoginErrorCode::ServerError:
                return "Server Error";
            case LoginErrorCode::Timeout:
                return "Connection Timeout";
            case LoginErrorCode::BannedAccount:
                return "Account Banned";
            default:
                return "Unknown Error";
        }
    }
    
    static bool ValidateAccountPointer(uintptr_t ptr) {
        if (!ptr || ptr < 0x100000000) {
            return false;
        }

        uintptr_t namePtr = InternalMemory::Read<uintptr_t>(ptr + 0x30);
        uintptr_t playerIdPtr = InternalMemory::Read<uintptr_t>(ptr + 0x48);

        if ((namePtr && namePtr < 0x100000000) || (playerIdPtr && playerIdPtr < 0x100000000)) {
            return false;
        }

        return true;
    }

    // Memory reading helpers - implement based on your game's structure
    uintptr_t GetAccountPointerFromGame() {
        if (g_AccountData.memoryPtr && ValidateAccountPointer(g_AccountData.memoryPtr)) {
            return g_AccountData.memoryPtr;
        }

        g_AccountData.memoryPtr = 0;

        const char* images[] = { "Assembly-CSharp.dll", "Assembly-CSharp-firstpass.dll", nullptr };
        const char* namespaces[] = { "", "Battle", "Account", "Game", nullptr };
        const char* classes[] = { "AccountManager", "LoginManager", "AuthManager", "SecurityManager", "AuthService", nullptr };
        const char* fields[] = { "Instance", "instance", "s_Instance", nullptr };

        for (int i = 0; images[i]; ++i) {
            for (int j = 0; namespaces[j]; ++j) {
                for (int k = 0; classes[k]; ++k) {
                    for (int l = 0; fields[l]; ++l) {
                        void* instance = nullptr;
                        Il2CppGetStaticFieldValue(images[i], namespaces[j], classes[k], fields[l], &instance);
                        if (instance) {
                            uintptr_t candidate = (uintptr_t)instance;
                            if (ValidateAccountPointer(candidate)) {
                                g_AccountData.memoryPtr = candidate;
                                STEALTH_LOG(@"[Account] Found account singleton: %s.%s.%s in %s (ptr=%p)", namespaces[j], classes[k], fields[l], images[i], (void*)candidate);
                                return g_AccountData.memoryPtr;
                            }
                            STEALTH_LOG(@"[Account] Invalid account singleton candidate: %s.%s.%s in %s (ptr=%p)", namespaces[j], classes[k], fields[l], images[i], (void*)candidate);
                        }
                    }
                }
            }
        }

        return 0;
    }
    
    void ReadAccountDataFromGame() {
        uintptr_t ptr = GetAccountPointerFromGame();
        if (!ptr) {
            STEALTH_LOG(@"[Account] ReadAccountDataFromGame failed: no account pointer");
            return;
        }

        // Example offsets. Adjust after reverse engineering the new patch.
        g_AccountData.accountId = InternalMemory::Read<uint64_t>(ptr + 0x10);
        g_AccountData.isLoggedIn = InternalMemory::ReadBool(ptr + 0x20);
        g_AccountData.isGuest = InternalMemory::ReadBool(ptr + 0x21);

        uintptr_t namePtr = InternalMemory::Read<uintptr_t>(ptr + 0x30);
        g_AccountData.accountName = InternalMemory::ReadIL2CppString(namePtr);

        uintptr_t playerIdPtr = InternalMemory::Read<uintptr_t>(ptr + 0x48);
        g_AccountData.playerId = InternalMemory::ReadIL2CppString(playerIdPtr);
        STEALTH_LOG(@"[Account] Read account data from game memory (id: %llu, loggedIn: %d, guest: %d)", g_AccountData.accountId, g_AccountData.isLoggedIn, g_AccountData.isGuest);
    }
    
    void WriteAccountDataToGame() {
        uintptr_t ptr = GetAccountPointerFromGame();
        if (!ptr) {
            STEALTH_LOG(@"[Account] WriteAccountDataToGame failed: no account pointer");
            return;
        }

        bool success = true;
        success &= InternalMemory::Write<uint64_t>(ptr + 0x10, 0); // clear account id
        success &= InternalMemory::Write<bool>(ptr + 0x20, false); // set logged in false
        success &= InternalMemory::Write<bool>(ptr + 0x21, g_AccountData.isGuest);  // set guest mode based on current target state
        success &= InternalMemory::Write<uintptr_t>(ptr + 0x30, 0); // clear account name pointer
        success &= InternalMemory::Write<uintptr_t>(ptr + 0x48, 0); // clear player id pointer

        if (!success) {
            STEALTH_LOG(@"[Account] WriteAccountDataToGame failed: one or more memory writes did not succeed");
        } else {
            STEALTH_LOG(@"[Account] Wrote account reset data back to game memory");
        }

        g_AccountData.isLoggedIn = false;
        g_AccountData.state = AccountState::Guest;
    }
}

extern "C" bool OnGameLoginResult(bool success, int errorCode) {
    if (success) {
        AccountManager::SetAccountState(AccountState::LoggedIn);
        return true;
    }

    LoginErrorCode mappedCode = MapLoginErrorCode(errorCode);
    AccountManager::CheckLoginFailure(mappedCode, "OnGameLoginResult failure");
    return false;
}