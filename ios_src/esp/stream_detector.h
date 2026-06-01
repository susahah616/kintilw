#pragma once
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

namespace StreamDetector {
    
    inline bool IsScreenRecording() {
        #if __IPHONE_OS_VERSION_MAX_ALLOWED >= 110000
        if (@available(iOS 11.0, *)) {
            return UIScreen.mainScreen.isCaptured;
        }
        #endif
        return false;
    }
    
    inline bool IsAirPlayActive() {
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
    
    inline bool IsStreaming() {
        return IsScreenRecording() || IsAirPlayActive();
    }
}
