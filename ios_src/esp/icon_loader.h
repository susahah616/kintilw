#pragma once
// ============================================================
//  ICON LOADER — iOS Metal Version (In-Memory, No Disk I/O)
//  Base64 decode → stb_image_from_memory → MTLTexture
//  ImGui Metal backend: ImTextureID = (__bridge_retained void*)id<MTLTexture>
// ============================================================

#ifdef __OBJC__

#import <Metal/Metal.h>
#import <Foundation/Foundation.h>
#include <string>
#include <vector>

// stb_image — decode PNG/JPEG dari memory buffer
#ifndef ICON_LOADER_STB_DONE
#define ICON_LOADER_STB_DONE
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "stb_image.h"

// base64 decoder
#include "base64.hpp"

// Icon data + struct
#include "IconList.h"

// ==============================================================
//  Global device pointer (diset dari ImGuiOverlay saat setupMetal)
// ==============================================================
static id<MTLDevice> g_IconDevice = nil;

inline void SetIconDevice(id<MTLDevice> device) {
    g_IconDevice = device;
}

// ==============================================================
//  Buat MTLTexture dari raw PNG bytes di memory (NO disk I/O)
// ==============================================================
static void* CreateMetalTextureFromMemory(const std::string& pngDecoded) {
    if (pngDecoded.empty()) return nullptr;
    
    id<MTLDevice> dev = g_IconDevice ? g_IconDevice : MTLCreateSystemDefaultDevice();
    if (!dev) return nullptr;
    
    int w = 0, h = 0;
    const unsigned char* rawBytes = reinterpret_cast<const unsigned char*>(pngDecoded.data());
    unsigned char* img = stbi_load_from_memory(rawBytes, (int)pngDecoded.size(), &w, &h, nullptr, 4);
    if (!img || w <= 0 || h <= 0) return nullptr;
    
    MTLTextureDescriptor* desc = [MTLTextureDescriptor
        texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                     width:(NSUInteger)w
                                    height:(NSUInteger)h
                                 mipmapped:NO];
    desc.usage        = MTLTextureUsageShaderRead;
    desc.storageMode  = MTLStorageModeShared;
    
    id<MTLTexture> tex = [dev newTextureWithDescriptor:desc];
    if (!tex) { stbi_image_free(img); return nullptr; }
    
    [tex replaceRegion:MTLRegionMake2D(0, 0, (NSUInteger)w, (NSUInteger)h)
           mipmapLevel:0
             withBytes:img
           bytesPerRow:(NSUInteger)(4 * w)];
    stbi_image_free(img);
    
    // __bridge_retained: ARC tidak akan free selama kita hold raw pointer ini
    return (__bridge_retained void*)tex;
}

// ==============================================================
//  Load hero icons dari iconHeroList[] (ICON.h)
//  Dipanggil SEKALI dari thread Metal (saat frame pertama render)
// ==============================================================
static bool s_HeroIconLoaded = false;
static bool s_MonsterIconLoaded = false;

static void AttachHeroIcons() {
    if (s_HeroIconLoaded) return;
    s_HeroIconLoaded = true;
    
    int size = (int)(sizeof(iconHeroList) / sizeof(iconHeroList[0]));
    HeroIcon.resize(size);
    
    for (int i = 0; i < size; i++) {
        std::string decoded = base64::from_base64(iconHeroList[i]);
        Icon ic;
        ic.texture = CreateMetalTextureFromMemory(decoded);
        ic.IsValid  = (ic.texture != nullptr);
        HeroIcon[i] = ic;
    }
}

// ==============================================================
//  Load monster icons dari MonsterList[] (MonsterIcon.h)
// ==============================================================
static void AttachMonsterIcons() {
    if (s_MonsterIconLoaded) return;
    s_MonsterIconLoaded = true;
    
    int size = (int)(sizeof(MonsterList) / sizeof(MonsterList[0]));
    MonsterIcon.resize(size);
    
    for (int i = 0; i < size; i++) {
        std::string decoded = base64::from_base64(MonsterList[i]);
        Icon ic;
        ic.texture = CreateMetalTextureFromMemory(decoded);
        ic.IsValid  = (ic.texture != nullptr);
        MonsterIcon[i] = ic;
    }
}

// ==============================================================
//  Public init — panggil dari thread Metal setelah device siap
// ==============================================================
inline void InitAllIcons() {
    static bool done = false;
    if (done) return;
    done = true;
    AttachHeroIcons();
    AttachMonsterIcons();
}

#else
// Non-ObjC fallback stubs
inline void SetIconDevice(void*) {}
inline void InitAllIcons() {}
#endif // __OBJC__
