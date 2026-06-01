# Auto Account Reset System
Auto-resets game account to guest on login failure

## Architecture

### Data Structures

**AccountData** (`esp/account_manager.h`)
- Account identity (accountId, accountName, playerId)
- Account state (LoggedIn, Guest, LoginFailed, ResetPending)
- Login tracking (attempts, failures, error codes)
- Auto-reset settings (enabled, max attempts, delay, etc.)

**AccountState Enum**
- `Unknown` - Uninitialized state
- `LoggedIn` - User successfully logged in
- `Guest` - Playing as guest (no account)
- `LoginFailed` - Most recent login attempt failed
- `ResetPending` - Scheduled for auto-reset

**LoginErrorCode Enum**
- `NoError` - No error
- `NetworkError` - Network connectivity issue
- `InvalidCredentials` - Wrong username/password
- `ServerError` - Server-side error
- `Timeout` - Connection timeout
- `BannedAccount` - Account is banned (triggers auto-reset)
- `UnknownError` - Generic error

### Core Functions

```cpp
// Initialize the account manager (called automatically in Main.mm)
AccountManager::Initialize();

// Record login failure and check for auto-reset trigger
bool success = AccountManager::CheckLoginFailure(
    LoginErrorCode::InvalidCredentials, 
    "Invalid username or password"
);

// Determine if auto-reset should trigger
bool shouldReset = AccountManager::ShouldAutoReset();

// Perform account reset to guest
AccountManager::AutoResetAccount();

// Update account state
AccountManager::SetAccountState(AccountState::LoggedIn);
AccountManager::SetLoginAttempt(true); // or false

// Get current state for UI display
std::string state = AccountManager::GetStateString();
std::string error = AccountManager::GetErrorString();
```

## Configuration

Edit `esp/config.h` to customize auto-reset behavior:

```cpp
extern bool AutoResetAccountEnabled;      // Enable/disable auto-reset (default: true)
extern int AutoResetMaxAttempts;          // Max failed attempts before reset (default: 3)
extern int AutoResetDelaySeconds;         // Delay before reset (default: 5 seconds)
extern bool AutoResetToGuest;             // Reset to guest or logout (default: true)
```

## Integration

### Step 1: Hook Login Result Callback
Find the game's login result handler and add:

```cpp
extern "C" void OnLoginResult(bool success, LoginErrorCode errorCode) {
    if (success) {
        AccountManager::SetAccountState(AccountState::LoggedIn);
    } else {
        AccountManager::CheckLoginFailure(errorCode, "Login failed from game");
    }
}
```

### Step 2: Hook Game's Account Manager
Find the game's `AccountManager` or `LoginManager` class and get its pointer:

```cpp
void* accountMgr = nullptr;
Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "AccountManager", "Instance", &accountMgr);
AccountManager::g_AccountData.memoryPtr = (uintptr_t)accountMgr;
```

### Step 3: Implement Memory Read/Write (Optional)
For persistent account reset, implement in `account_manager.cpp`:

```cpp
void AccountManager::ReadAccountDataFromGame() {
    uintptr_t ptr = GetAccountPointerFromGame();
    if (ptr) {
        using namespace InternalMemory;
        g_AccountData.accountId = Read<uint64_t>(ptr + 0x10);
        g_AccountData.isLoggedIn = ReadBool(ptr + 0x20);
    }
}

void AccountManager::WriteAccountDataToGame() {
    uintptr_t ptr = GetAccountPointerFromGame();
    if (ptr) {
        using namespace InternalMemory;
        Write<uint64_t>(ptr + 0x10, 0);            // Clear account ID
        WriteBool(ptr + 0x20, false);              // Set logged in = false
        WriteBool(ptr + 0x21, true);               // Set guest mode = true
    }
}
```

## Auto-Reset Triggers

Auto-reset is triggered when:
1. **Max consecutive failures exceeded**: More than `maxLoginAttempts` failed login attempts
2. **Account banned**: Receives `LoginErrorCode::BannedAccount`

## Usage Example

### Initialize at app start:
```cpp
void AppDidLaunch() {
    AccountManager::Initialize();
    // ...
}
```

### Check login in UI layer:
```cpp
bool HandleLoginClick() {
    bool result = PerformGameLogin(username, password);
    
    if (!result) {
        // Game login failed
        AccountManager::CheckLoginFailure(
            LoginErrorCode::ServerError,
            "Failed to connect to login server"
        );
    } else {
        AccountManager::SetAccountState(AccountState::LoggedIn);
    }
    
    return result;
}
```

### Display account status in UI (ImGui):
```cpp
ImGui::Text("Account: %s", AccountManager::GetStateString().c_str());
ImGui::Text("Error: %s", AccountManager::GetErrorString().c_str());
ImGui::Text("Failed Attempts: %d", 
    AccountManager::g_AccountData.consecutiveFailures);

if (AccountManager::g_AccountData.needsReset) {
    ImGui::TextColored(ImVec4(1,0,0,1), "AUTO-RESET PENDING");
}
```

## Offset Reference
(Requires reverse engineering of the game binary)

Game account structure offsets (example):
```cpp
#define OFF_ACCOUNT_ID       0x10
#define OFF_ACCOUNT_LOGGED   0x20
#define OFF_ACCOUNT_GUEST    0x21
#define OFF_ACCOUNT_NAME     0x30
#define OFF_ACCOUNT_ERROR    0x40
```

## Thread Safety
- `AccountManager::g_AccountData` is accessed from main memory thread
- All operations are performed atomically within the member functions
- Consider adding mutex if multi-threaded access is needed:

```cpp
#include <mutex>
namespace AccountManager {
    static std::mutex g_AccountMutex;
    // Lock access: std::lock_guard<std::mutex> lock(g_AccountMutex);
}
```

## Logging
All account operations log to NSLog:
```
[Account] AccountManager initialized
[Account] Login failure detected: 3 (consecutive: 1)
[Account] Triggering auto-reset...
[Account] Account reset to guest successfully
```

## Files Modified
- `esp/account_manager.h` - Header with structures and interface
- `esp/account_manager.cpp` - Implementation with auto-reset logic
- `esp/config.h` - Configuration variables
- `esp/config.cpp` - Configuration initialization
- `Main.mm` - Integration with memory thread
