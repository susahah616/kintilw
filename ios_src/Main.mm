#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#include <mach-o/dyld.h>
#include <pthread.h>
#include <dlfcn.h>
#include <unistd.h>
#include "memory_internal.h"
#include "esp/entity.h"
#include "esp/account_manager.h"
#include "esp/config_manager.h"
#include "esp/stealth.h"

#import "ImGuiOverlay.h"

#include "Il2CppResolver.h"

// Thread pembaca memori (Scanner)
static int g_UISetupAttempts = 0;
static const int kMaxUISetupAttempts = 30;

void* MemoryThread(void* arg) {
    STEALTH_LOG(@"[Cheat] Memory Thread Started!");
    
    // Tunggu sampai Il2Cpp berhasil di-attach
    while (!Il2CppAttach()) {
        sleep(2);
    }
    
    STEALTH_LOG(@"[Cheat] Il2Cpp Attached Successfully!");
    
    // Initialize Account Manager
    AccountManager::Initialize();
    STEALTH_LOG(@"[Account] Account Manager initialized");

    if (LoadAutoLoadFlag()) {
        if (LoadConfig()) {
            AccountManager::g_AccountData.autoResetEnabled = AutoResetAccountEnabled;
            AccountManager::g_AccountData.maxLoginAttempts = AutoResetMaxAttempts;
            AccountManager::g_AccountData.resetDelaySeconds = AutoResetDelaySeconds;
            AccountManager::g_AccountData.autoResetToGuest = AutoResetToGuest;
            STEALTH_LOG(@"[Config] Auto-loaded settings from config file.");
        } else {
            STEALTH_LOG(@"[Config] Auto-load failed: unable to open config file.");
        }
    }
    
    // SANGAT PENTING: Tunggu game sampai benar-benar masuk ke menu/loading screen
    // Jika kita memanggil Il2CppGetStaticFieldValue terlalu cepat, Unity akan mencoba
    // menginisialisasi BattleManager sebelum engine siap, yang menyebabkan EXC_BAD_ACCESS (Crash).
    STEALTH_LOG(@"[Cheat] Waiting 15 seconds for Unity engine to boot...");
    sleep(15);
    
    while(true) {
        void* bmInst = nullptr;
        void* logicBmInst = nullptr;
        
        // Dapatkan static field Instance dari BattleManager (Bukan GameLogic, persis Code Breaker)
        Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "BattleManager", "Instance", &bmInst);
        
        // Dapatkan static field Instance dari LogicBattleManager
        Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "LogicBattleManager", "Instance", &logicBmInst);
        
        // SANGAT AMAN: Hapus total pemanggilan method `GetBattleState()`
        // Karena memanggil method Il2Cpp dari background thread di iOS sering memicu `abort()` dari Unity Engine!
        // Sebagai gantinya, kita cukup cek apakah `bmInst` dan `logicBmInst` tidak null.
        // Jika tidak null, berarti kita sedang berada di dalam match/room.
        
        g_Battle.dbg_bmInst = (uintptr_t)bmInst;
        g_Battle.dbg_logicBmInst = (uintptr_t)logicBmInst;
        g_Battle.dbg_battleState = 6; // Dummy state for debug overlay

        if (bmInst && logicBmInst) {
            g_Battle.Update((uintptr_t)bmInst, (uintptr_t)logicBmInst);
        } else {
            g_Battle.isValid = false; // Disable ESP di Lobby
        }
        
        usleep(30000); // 30ms sleep (~33 fps ESP update rate)
    }
    
    return NULL;
}

static UIWindow *FindActiveWindow(void) {
    UIWindow *window = nil;

    #if __IPHONE_OS_VERSION_MAX_ALLOWED >= 150000
    for (UIScene *scene in [UIApplication sharedApplication].connectedScenes) {
        if ([scene isKindOfClass:[UIWindowScene class]] &&
            ((UIWindowScene *)scene).activationState == UISceneActivationStateForegroundActive) {
            for (UIWindow *w in ((UIWindowScene *)scene).windows) {
                if (w.isKeyWindow) { window = w; break; }
            }
            if (!window) window = ((UIWindowScene *)scene).windows.firstObject;
            if (window) break;
        }
    }
    #endif

    if (!window) {
        window = [UIApplication sharedApplication].keyWindow;
    }

    if (!window && [UIApplication sharedApplication].delegate &&
        [[UIApplication sharedApplication].delegate respondsToSelector:@selector(window)]) {
        window = [[UIApplication sharedApplication].delegate window];
    }

    if (!window) {
        window = [UIApplication sharedApplication].windows.firstObject;
    }

    return window;
}

void SetupUI() {
    if (g_UISetupAttempts++ >= kMaxUISetupAttempts) {
        STEALTH_LOG(@"[Cheat] SetupUI failed: maximum retries reached.");
        return;
    }

    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        UIWindow *window = FindActiveWindow();
        if (window) {
            STEALTH_LOG(@"[Cheat] UIWindow found. Initializing UI Overlay...");
            [ImGuiOverlay sharedOverlay];
        } else {
            STEALTH_LOG(@"[Cheat] UIWindow not ready yet. Retrying... (%d)", g_UISetupAttempts);
            SetupUI();
        }
    });
}

// Atribut constructor menjamin fungsi ini dipanggil otomatis 
// sesaat setelah libmlbb_cheat.dylib berhasil di-load oleh iOS
__attribute__((constructor))
void InitCheat() {
    STEALTH_LOG(@"[Cheat] Dylib Injected Successfully!");
    
    // Setup UI di Main Thread dengan retry mechanism
    SetupUI();

    // Attempt account singleton lookup on main thread to support new patch names
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(2.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        AccountManager::ReadAccountDataFromGame();
    });

    // Jalankan Memory Scanner di background thread
    pthread_t ptid;
    if (pthread_create(&ptid, NULL, MemoryThread, NULL) == 0) {
        pthread_detach(ptid);
    }
}
