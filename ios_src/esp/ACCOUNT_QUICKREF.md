# Auto Account Reset System - Quick Reference

## Files Created

| File | Purpose |
|------|---------|
| `account_manager.h` | Core data structures and API |
| `account_manager.cpp` | Implementation and logic |
| `account_ui.h` | ImGui menu integration |
| `AUTO_RESET_GUIDE.md` | Full documentation |
| `ACCOUNT_EXAMPLES.md` | Code integration examples |

## Key Data Structures

### AccountState
```
LoggedIn (1)      - Successful login
Guest (2)         - Playing as guest
LoginFailed (3)   - Most recent login failed
ResetPending (4)  - Scheduled for reset
```

### LoginErrorCode
```
NetworkError (1)      - Connection issue
InvalidCredentials (2) - Wrong password
ServerError (3)       - Server issue
Timeout (4)           - Connection timeout
BannedAccount (5)     - Account banned (triggers auto-reset)
```

## Core API

```cpp
// Initialize (auto-called in Main.mm)
AccountManager::Initialize();

// Check if login failed - returns true if auto-reset triggered
bool needsReset = AccountManager::CheckLoginFailure(
    LoginErrorCode::InvalidCredentials,
    "Invalid password"
);

// Manual reset to guest
AccountManager::AutoResetAccount();

// Update state
AccountManager::SetAccountState(AccountState::LoggedIn);
AccountManager::SetLoginAttempt(true/false);

// Check conditions
bool shouldReset = AccountManager::ShouldAutoReset();

// Get strings for UI
std::string state = AccountManager::GetStateString();
std::string error = AccountManager::GetErrorString();

// Clear failures
AccountManager::ClearLoginFailures();
```

## Global Account Data

```cpp
AccountManager::g_AccountData

Fields:
- accountId (uint64_t)
- accountName (string)
- state (AccountState)
- isLoggedIn (bool)
- isGuest (bool)
- consecutiveFailures (int)
- lastErrorCode (LoginErrorCode)
- lastErrorMessage (string)
- autoResetEnabled (bool) - default: true
- maxLoginAttempts (int) - default: 3
- resetDelaySeconds (int) - default: 5
- autoResetToGuest (bool) - default: true
- needsReset (bool)
- resetInProgress (bool)
```

## Configuration Variables

```cpp
// In config.h / config.cpp
extern bool AutoResetAccountEnabled;    // Enable/disable feature
extern int AutoResetMaxAttempts;        // Max failed attempts
extern int AutoResetDelaySeconds;       // Reset delay
extern bool AutoResetToGuest;           // Reset mode
```

## Auto-Reset Triggers

Resets automatically when:
1. Consecutive failed attempts ≥ maxLoginAttempts (default: 3)
2. Receives BannedAccount error code

## Integration Points

### 1. Include Header
```cpp
#include "esp/account_manager.h"
```

### 2. Initialize (in Main.mm)
```cpp
// After Il2CppAttach() succeeds
AccountManager::Initialize();
```

### 3. Hook Login Success
```cpp
void OnGameLoginSuccess() {
    AccountManager::SetAccountState(AccountState::LoggedIn);
}
```

### 4. Hook Login Failure
```cpp
void OnGameLoginFailure(int errorCode) {
    AccountManager::CheckLoginFailure(
        (LoginErrorCode)errorCode,
        "Login failed"
    );
}
```

### 5. Add UI Menu (optional)
```cpp
#include "esp/account_ui.h"

// In ImGui rendering:
AccountUI::RenderAccountMenu();
```

## Usage Examples

### Simple Login Handler
```cpp
bool Login(const char* username, const char* password) {
    bool result = GameAPI::Login(username, password);
    
    if (result) {
        AccountManager::SetAccountState(AccountState::LoggedIn);
    } else {
        AccountManager::CheckLoginFailure(
            LoginErrorCode::InvalidCredentials,
            "Login failed"
        );
    }
    
    return result;
}
```

### Check for Reset in Game Loop
```cpp
void GameLoop() {
    // ... game update ...
    
    // In lobby (safe to reset)
    if (!InBattle() && AccountManager::ShouldAutoReset()) {
        AccountManager::AutoResetAccount();
    }
}
```

### Display Status
```cpp
ImGui::Text("State: %s", 
    AccountManager::GetStateString().c_str());
ImGui::Text("Errors: %d", 
    AccountManager::g_AccountData.consecutiveFailures);
```

## Configuration (at runtime)

```cpp
// Change max attempts
AccountManager::g_AccountData.maxLoginAttempts = 5;

// Disable auto-reset
AccountManager::g_AccountData.autoResetEnabled = false;

// Manually trigger reset
AccountManager::AutoResetAccount();

// Clear failed attempts
AccountManager::ClearLoginFailures();
```

## Logging

All operations log to NSLog with `[Account]` prefix:
```
[Account] AccountManager initialized
[Account] Login failure detected: 2 (consecutive: 1)
[Account] Triggering auto-reset...
[Account] Account reset to guest successfully
```

## Memory Offsets

For game account structure (requires reverse engineering):

```cpp
#define OFF_ACCOUNT_ID       0x10
#define OFF_ACCOUNT_LOGGED   0x20
#define OFF_ACCOUNT_GUEST    0x21
#define OFF_ACCOUNT_NAME     0x30
#define OFF_ACCOUNT_ERROR    0x40

// Then implement in account_manager.cpp:
void ReadAccountDataFromGame();
void WriteAccountDataToGame();
```

## Quick Start Checklist

```
[ ] 1. Include account_manager.h in Main.mm
[ ] 2. Call AccountManager::Initialize() after Il2CppAttach()
[ ] 3. Add CheckLoginFailure() to game's login failure handler
[ ] 4. Add SetAccountState() to game's login success handler
[ ] 5. (Optional) Add AccountUI::RenderAccountMenu() to ImGui
[ ] 6. Test by failing login 3+ times
[ ] 7. Verify auto-reset triggers
[ ] 8. Check NSLog for [Account] messages
```

## Files Modified Summary

✓ `Main.mm` - Added includes and Initialize() call
✓ `config.h` - Added auto-reset configuration variables
✓ `config.cpp` - Initialized auto-reset configuration

## New Files Created

✓ `account_manager.h` - Core API and structures
✓ `account_manager.cpp` - Implementation
✓ `account_ui.h` - ImGui integration
✓ `AUTO_RESET_GUIDE.md` - Full documentation
✓ `ACCOUNT_EXAMPLES.md` - Integration examples
✓ `ACCOUNT_QUICKREF.md` - This file

## Support

For detailed integration:
- See `AUTO_RESET_GUIDE.md` for full architecture
- See `ACCOUNT_EXAMPLES.md` for code samples
- See `account_ui.h` for ImGui menu integration
