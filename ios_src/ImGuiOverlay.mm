#import "ImGuiOverlay.h"
#import "imgui/imgui.h"
#import "imgui/backends/imgui_impl_metal.h"

// Include ESP
#include "esp/ui_menu.h"
#include "esp/esp_core.h"
#include "esp/esp_player.h"
#include "esp/esp_minimap.h"

static UIWindow *GetActiveWindow(void) {
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
    if (!window) {
        window = [UIApplication sharedApplication].windows.firstObject;
    }
    return window;
}

@interface ImGuiOverlay () <MTKViewDelegate>
@property (nonatomic, strong) MTKView *mtkView;
@property (nonatomic, strong) id <MTLDevice> device;
@property (nonatomic, strong) id <MTLCommandQueue> commandQueue;
@property (nonatomic, strong) UIButton *toggleButton;
@end

@implementation ImGuiOverlay

+ (instancetype)sharedOverlay {
    static ImGuiOverlay *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        UIWindow *window = GetActiveWindow();
        CGRect frame = window ? window.bounds : [UIScreen mainScreen].bounds;
        sharedInstance = [[ImGuiOverlay alloc] initWithFrame:frame];
        if (window) {
            [window addSubview:sharedInstance];
        } else {
            [[UIApplication sharedApplication].keyWindow addSubview:sharedInstance];
        }
    });
    return sharedInstance;
}

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        self.backgroundColor = [UIColor clearColor];
        self.userInteractionEnabled = YES; // Agar bisa terima touch
        self.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
        
        [self setupMetal];
        [self setupImGui];
        [self setupToggleButton];
        [self becomeFirstResponder];
    }
    return self;
}

- (BOOL)canBecomeFirstResponder {
    return YES;
}

- (void)motionEnded:(UIEventSubtype)motion withEvent:(UIEvent *)event {
    if (motion == UIEventSubtypeMotionShake) {
        bShowMenu = !bShowMenu;
        self.toggleButton.hidden = bShowMenu;
    }
}

- (void)dealloc {
    if (ImGui::GetCurrentContext()) {
        ImGui_ImplMetal_Shutdown();
        ImGui::DestroyContext();
    }
}

- (void)setupMetal {
    self.device = MTLCreateSystemDefaultDevice();
    if (!self.device) {
        NSLog(@"[Cheat] Metal device unavailable. Overlay disabled.");
        return;
    }

    self.commandQueue = [self.device newCommandQueue];
    if (!self.commandQueue) {
        NSLog(@"[Cheat] Could not create Metal command queue. Overlay disabled.");
        return;
    }

    self.mtkView = [[MTKView alloc] initWithFrame:self.bounds device:self.device];
    if (!self.mtkView) {
        NSLog(@"[Cheat] Failed to create MTKView.");
        return;
    }

    self.mtkView.backgroundColor = [UIColor clearColor];
    self.mtkView.delegate = self;
    self.mtkView.framebufferOnly = NO;
    self.mtkView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    self.mtkView.enableSetNeedsDisplay = YES;
    self.mtkView.paused = NO;
    
    // Agar background transparan (overlay)
    self.mtkView.clearColor = MTLClearColorMake(0, 0, 0, 0);
    [self addSubview:self.mtkView];
}

- (void)setupToggleButton {
    // Buat tombol native iOS agar 100% bisa diklik tanpa masalah hitTest
    self.toggleButton = [UIButton buttonWithType:UIButtonTypeCustom];

    self.toggleButton.frame = CGRectMake(20, self.bounds.size.height - 80, 50, 50);

    self.toggleButton.backgroundColor = [UIColor clearColor];
    self.toggleButton.layer.cornerRadius = 25;
    self.toggleButton.clipsToBounds = YES;

    UIImage *toggleImage = [UIImage imageNamed:@"togle.jpg"];
    if (!toggleImage) {
        toggleImage = [UIImage systemImageNamed:@"circle.fill"];
    }
    [self.toggleButton setImage:toggleImage forState:UIControlStateNormal];

    self.toggleButton.imageView.contentMode = UIViewContentModeScaleAspectFill;
    self.toggleButton.contentHorizontalAlignment = UIControlContentHorizontalAlignmentFill;
    self.toggleButton.contentVerticalAlignment = UIControlContentVerticalAlignmentFill;

    [self.toggleButton addTarget:self
                          action:@selector(onToggleClicked)
                forControlEvents:UIControlEventTouchUpInside];

    [self addSubview:self.toggleButton];
}

- (void)onToggleClicked {
    bShowMenu = !bShowMenu;
    self.toggleButton.hidden = bShowMenu; // Sembunyikan tombol native jika menu ImGui terbuka
}

- (void)layoutSubviews {
    [super layoutSubviews];
    self.mtkView.frame = self.bounds;
    self.toggleButton.frame = CGRectMake(20, self.bounds.size.height - 80, 50, 50);
}

- (void)setupImGui {
    if (!self.device) {
        NSLog(@"[Cheat] skip setupImGui: Metal device is nil.");
        return;
    }

    @try {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        
        ImGui::StyleColorsDark();
        
        // Kembalikan skala ke normal (1.0x) agar menu tidak 'gede banget'
        // DisplayFramebufferScale sudah cukup untuk membuat resolusinya tajam di Retina
        ImGui::GetStyle().ScaleAllSizes(1.0f);
        io.FontGlobalScale = 1.0f;
        
        if (!ImGui_ImplMetal_Init(self.device)) {
            NSLog(@"[Cheat] ImGui_ImplMetal_Init failed.");
        }
        
        // Set device untuk icon loader (agar pakai device yang sama, bukan MTLCreateSystemDefaultDevice)
        SetIconDevice(self.device);
        
        // Konfigurasi IO Display
        io.DisplaySize.x = self.bounds.size.width;
        io.DisplaySize.y = self.bounds.size.height;
    } @catch (NSException *ex) {
        NSLog(@"[Cheat] setupImGui exception: %@", ex);
    }
}

- (void)updateIOWithTouchEvent:(UIEvent *)event {
    if (!event.allTouches || event.allTouches.count == 0) {
        return;
    }

    UITouch *anyTouch = event.allTouches.anyObject;
    if (!anyTouch) {
        return;
    }

    CGPoint location = [anyTouch locationInView:self];
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(location.x, location.y);
    
    if (anyTouch.phase == UITouchPhaseBegan) {
        io.AddMouseButtonEvent(0, true);
    } else if (anyTouch.phase == UITouchPhaseEnded || anyTouch.phase == UITouchPhaseCancelled) {
        io.AddMouseButtonEvent(0, false);
    }
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [self updateIOWithTouchEvent:event];
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [self updateIOWithTouchEvent:event];
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [self updateIOWithTouchEvent:event];
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [self updateIOWithTouchEvent:event];
}

// Meneruskan touch ke game jika menu ImGui tidak disentuh
- (UIView *)hitTest:(CGPoint)point withEvent:(UIEvent *)event {
    if (bShowMenu) {
        // Jika menu terbuka, cek apakah touch mengenai window ImGui
        // ImGuiIO& io = ImGui::GetIO();
        // io.WantCaptureMouse akan true jika kita menyentuh window ImGui
        return [super hitTest:point withEvent:event];
    }
    
    // Jika menu tertutup, hanya tombol native yang bisa disentuh
    if (!self.toggleButton.hidden && CGRectContainsPoint(self.toggleButton.frame, point)) {
        return self.toggleButton;
    }
    
    return nil; // Sentuhan diteruskan ke game
}

#pragma mark - MTKViewDelegate

- (void)mtkView:(MTKView *)view drawableSizeWillChange:(CGSize)size {
}

- (void)drawInMTKView:(MTKView *)view {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = view.bounds.size.width;
    io.DisplaySize.y = view.bounds.size.height;
    
    // SANGAT PENTING: Set FramebufferScale agar sentuhan akurat dan menu tidak terpotong (clipping) di layar Retina
    io.DisplayFramebufferScale = ImVec2(view.contentScaleFactor, view.contentScaleFactor);
    
    id<MTLCommandBuffer> commandBuffer = [self.commandQueue commandBuffer];
    MTLRenderPassDescriptor *renderPassDescriptor = view.currentRenderPassDescriptor;
    id<CAMetalDrawable> drawable = view.currentDrawable;
    
    if (renderPassDescriptor != nil && commandBuffer != nil && drawable != nil) {
        id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
        
        ImGui_ImplMetal_NewFrame(renderPassDescriptor);
        ImGui::NewFrame();
        
        // Render UI
        ShowMenu();
        RenderRetriDot();
        
        // Sinkronisasi status tombol toggle native setiap frame
        // Panggil Menu UI
        // ShowMenu() sudah dipanggil di atas
        
        // Sinkronisasi data entitas dari thread pemindai memori
        g_Battle.SwapBuffers();
        
        // DEBUG TEXT DI LAYAR
       // DEBUG TEXT DI LAYAR DISABLED
char debugStr[1] = "";
char extraStr[1] = "";
char enemyStr[1] = "";

// std::string finalDebugStr = std::string(debugStr) + extraStr + enemyStr;
// ImGui::GetBackgroundDrawList()->AddText(ImVec2(50, 50), IM_COL32(0, 255, 0, 255), finalDebugStr.c_str());

ImDrawList* bgDraw = ImGui::GetBackgroundDrawList();
        
        // --- MINIMAP BORDER (snake glow) — aktif SELALU saat MinimapESP=true, tidak perlu dalam match ---
        DrawMinimapBorder(bgDraw);
        
        // Panggil ESP Core (hanya jika fitur aktif)
        if (g_Battle.isValid) {
            SyncFeatureToESP();
            
            // --- UPDATE SCALE FACTOR (otomatis dari sistem iOS) ---
            g_ContentScaleFactor = view.contentScaleFactor;
            
            // --- SAFE AREA INFO ---
            UIEdgeInsets safeArea = UIEdgeInsetsZero;
            UIWindow *mainWindow = self.window;
            
            #if __IPHONE_OS_VERSION_MAX_ALLOWED >= 150000
            if (!mainWindow) {
                for (UIWindowScene *scene in [UIApplication sharedApplication].connectedScenes) {
                    if ([scene isKindOfClass:[UIWindowScene class]] &&
                        scene.activationState == UISceneActivationStateForegroundActive) {
                        for (UIWindow *w in scene.windows) {
                            if (w.isKeyWindow) { mainWindow = w; break; }
                        }
                        if (!mainWindow) mainWindow = scene.windows.firstObject;
                        if (mainWindow) break;
                    }
                }
            }
            #endif
            if (!mainWindow) mainWindow = [UIApplication sharedApplication].keyWindow;
            if (mainWindow) safeArea = mainWindow.safeAreaInsets;
            
            g_SafeAreaTop = (float)safeArea.top;
            g_ESPCfg.ScreenOffsetX = 0.0f;
            g_ESPCfg.ScreenOffsetY = 0.0f;
            
            RenderESPCore();
        }
        
        // Sinkronisasi status tombol toggle native setiap frame
        self.toggleButton.hidden = bShowMenu;
        
        ImGui::Render();
        ImDrawData *draw_data = ImGui::GetDrawData();
        ImGui_ImplMetal_RenderDrawData(draw_data, commandBuffer, renderEncoder);
        
        [renderEncoder endEncoding];
        [commandBuffer presentDrawable:drawable];
    }
    if (commandBuffer != nil) {
        [commandBuffer commit];
    }
}

@end
