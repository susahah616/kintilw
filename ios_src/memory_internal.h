#pragma once
#include <stdint.h>
#include <string.h>
#include <string>

// Dalam arsitektur internal (iOS dylib), kita memodifikasi kelas Memory
// agar tidak membaca file /proc/pid/mem, melainkan menggunakan pointer langsung.
// Hal ini membuat performa baca memori menjadi instan (0 ms delay).

extern uintptr_t g_il2cppBase;

namespace InternalMemory {
    template<typename T>
    inline T Read(uintptr_t address) {
        if (address < 0x100000000) return T{}; 
        return *(T*)address;
    }
    
    inline bool ReadRaw(uintptr_t address, void* buffer, size_t size) {
        if (address < 0x100000000) return false;
        memcpy(buffer, (void*)address, size);
        return true;
    }

    template<typename T>
    inline bool Write(uintptr_t address, T value) {
        if (address < 0x100000000) return false;
        *(T*)address = value;
        return true;
    }

    // Helper wrappers
    inline bool ReadBool(uintptr_t address) { return Read<bool>(address); }
    inline int32_t ReadInt32(uintptr_t address) { return Read<int32_t>(address); }
    inline float ReadFloat(uintptr_t address) { return Read<float>(address); }
    inline uintptr_t ReadPtr(uintptr_t address) { return Read<uintptr_t>(address); }
    
    inline std::string ReadIL2CppString(uintptr_t strPtr) {
        if (!strPtr) return "";
        int32_t len = ReadInt32(strPtr + 0x10);
        if (len <= 0 || len > 256) return "";
        char buf[512] = {};
        for (int i = 0; i < len && i < 128; i++) {
            uint16_t ch = Read<uint16_t>(strPtr + 0x14 + i * 2);
            buf[i] = (ch < 128) ? (char)ch : '?';
        }
        return std::string(buf);
    }
}
