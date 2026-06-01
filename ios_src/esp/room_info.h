#pragma once
#include <string>
#include "stealth.h"
#include "../Il2CppResolver.h"
#include "../memory_internal.h"

struct RoomInfo {
    bool valid = false;
    float winRate = 0.0f;
    int bestHeroId = 0;
    std::string bestHeroName;
    std::string phase;
};

inline bool TryReadStringField(void* klass, uintptr_t instance, const char* image, const char* className, const char* fieldName, std::string& outValue) {
    size_t offset = Il2CppGetFieldOffset(image, "", className, fieldName);
    if (offset == 0) return false;
    uintptr_t strPtr = InternalMemory::Read<uintptr_t>(instance + offset);
    if (!strPtr) return false;
    outValue = InternalMemory::ReadIL2CppString(strPtr);
    return !outValue.empty();
}

inline bool TryReadFloatField(void* klass, uintptr_t instance, const char* image, const char* className, const char* fieldName, float& outValue) {
    size_t offset = Il2CppGetFieldOffset(image, "", className, fieldName);
    if (offset == 0) return false;
    outValue = InternalMemory::Read<float>(instance + offset);
    return true;
}

inline bool TryReadIntField(void* klass, uintptr_t instance, const char* image, const char* className, const char* fieldName, int& outValue) {
    size_t offset = Il2CppGetFieldOffset(image, "", className, fieldName);
    if (offset == 0) return false;
    outValue = InternalMemory::Read<int>(instance + offset);
    return true;
}

inline RoomInfo ReadRoomInfoFromGame() {
    RoomInfo info;
    const char* images[] = { "Assembly-CSharp.dll", "Assembly-CSharp-firstpass.dll", nullptr };
    const char* candidates[] = {
        "Um9vbU1hbmFnZXI=", "RHJhZnRNYW5hZ2Vy", "TWF0Y2hSb29t", "RHJhZnRSb29t", "Um9vbUluZm8=",
        "R2FtZVJvb20=", "TWF0Y2hSb29tSW5mbw==", "TG9iYnlSb29t", nullptr
    };
    const char* fieldsInstance[] = { "SW5zdGFuY2U=", "aW5zdGFuY2U=", "c19JbnN0YW5jZQ==", nullptr };
    const char* winRateFields[] = { "V2luUmF0ZQ==", "d2luUmF0ZQ==", "d2luX3JhdGU=", "UmF0ZVdpbg==", "TWF0Y2hXaW5SYXRl", nullptr };
    const char* bestHeroNameFields[] = {
        "QmVzdEhlcm9OYW1l", "YmVzdEhlcm9OYW1l", "YmVzdF9oZXJvX25hbWU=", "QmVzdEhlcm8=", "YmVzdEhlcm8=", "VG9wSGVyb05hbWU=", nullptr
    };
    const char* bestHeroIdFields[] = {
        "QmVzdEhlcm9JZA==", "YmVzdEhlcm9JZA==", "YmVzdF9oZXJvX2lk", "SGVyb0lk", "VG9wSGVyb0lk", nullptr
    };
    const char* phaseFields[] = {
        "UGhhc2U=", "cGhhc2U=", "RHJhZnRQaGFzZQ==", "Um9vbVBocmFzZQ==", "Q3VycmVudFBoYXNl", "Y3VycmVudFBoYXNl", nullptr
    };

    for (int i = 0; images[i]; ++i) {
        const char* image = images[i];
        for (int j = 0; candidates[j]; ++j) {
            std::string className = S(candidates[j]);
            void* klass = Il2CppGetClassType(image, "", className.c_str());
            if (!klass) continue;

            for (int k = 0; fieldsInstance[k]; ++k) {
                std::string staticField = S(fieldsInstance[k]);
                void* instance = nullptr;
                Il2CppGetStaticFieldValue(image, "", className.c_str(), staticField.c_str(), &instance);
                if (!instance) continue;
                uintptr_t ptr = (uintptr_t)instance;

                info.valid = true;
                info.bestHeroName = "Unknown";
                info.phase = "Unknown";
                for (int f = 0; winRateFields[f]; ++f) {
                    if (TryReadFloatField(klass, ptr, image, className.c_str(), S(winRateFields[f]).c_str(), info.winRate)) {
                        break;
                    }
                }
                for (int f = 0; bestHeroNameFields[f]; ++f) {
                    if (TryReadStringField(klass, ptr, image, className.c_str(), S(bestHeroNameFields[f]).c_str(), info.bestHeroName)) {
                        break;
                    }
                }
                for (int f = 0; bestHeroIdFields[f]; ++f) {
                    if (TryReadIntField(klass, ptr, image, className.c_str(), S(bestHeroIdFields[f]).c_str(), info.bestHeroId)) {
                        break;
                    }
                }
                for (int f = 0; phaseFields[f]; ++f) {
                    if (TryReadStringField(klass, ptr, image, className.c_str(), S(phaseFields[f]).c_str(), info.phase)) {
                        break;
                    }
                }

                if (info.bestHeroName.empty() && info.bestHeroId != 0) {
                    info.bestHeroName = "HeroId_" + std::to_string(info.bestHeroId);
                }

                return info;
            }
        }
    }

    return info;
}
