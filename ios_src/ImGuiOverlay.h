#pragma once
#import <UIKit/UIKit.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

@interface ImGuiOverlay : UIView
+ (instancetype)sharedOverlay;
- (void)setupImGui;
@end
