#pragma once
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

static inline bool StreamDetector_IsScreenRecording() {
    #if __IPHONE_OS_VERSION_MAX_ALLOWED >= 110000
    if (@available(iOS 11.0, *)) {
        return UIScreen.mainScreen.isCaptured;
    }
    #endif
    return false;
}

static inline bool StreamDetector_IsAirPlayActive() {
    #if __IPHONE_OS_VERSION_MAX_ALLOWED >= 50000
    if (@available(iOS 5.0, *)) {
        for (UIScreen *screen in UIScreen.screens) {
            if (screen != UIScreen.mainScreen && screen.mirroredScreen == UIScreen.mainScreen) {
                return true;
            }
        }
    }
    #endif
    return false;
}

static inline bool StreamDetector_IsStreaming() {
    return StreamDetector_IsScreenRecording() || StreamDetector_IsAirPlayActive();
}

namespace StreamDetector {
    inline bool IsScreenRecording() {
        return StreamDetector_IsScreenRecording();
    }
    
    inline bool IsAirPlayActive() {
        return StreamDetector_IsAirPlayActive();
    }
    
    inline bool IsStreaming() {
        return StreamDetector_IsStreaming();
    }
}
