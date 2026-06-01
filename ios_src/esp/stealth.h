#pragma once
#include <string>
#include <vector>
#include <cstdio>

#ifdef __OBJC__
#import <Foundation/Foundation.h>
#endif

inline std::string Base64Decode(const char* input) {
    static const unsigned char d[] = {
        64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
        64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
        64,64,64,64,64,64,64,64,64,64,64,62,64,64,64,63,
        52,53,54,55,56,57,58,59,60,61,64,64,64, 0,64,64,
        64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
        15,16,17,18,19,20,21,22,23,24,25,64,64,64,64,64,
        64,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
        41,42,43,44,45,46,47,48,49,50,51,64,64,64,64,64
    };

    std::string output;
    int val = 0, valb = -8;
    for (const char* ptr = input; *ptr; ++ptr) {
        unsigned char c = static_cast<unsigned char>(*ptr);
        if (c >= sizeof(d)) break;
        if (d[c] == 64) break;
        val = (val << 6) + d[c];
        valb += 6;
        if (valb >= 0) {
            output.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return output;
}

inline std::string S(const char* b64) {
    return Base64Decode(b64);
}

#ifdef __OBJC__
#define STEALTH_LOG(fmt, ...) do { \
    NSString *message = [NSString stringWithFormat:(fmt), ##__VA_ARGS__]; \
    fprintf(stderr, "%s\n", message.UTF8String); \
    fflush(stderr); \
} while (0)
#else
#define STEALTH_LOG(fmt, ...) do { \
    fprintf(stderr, (fmt)"\n", ##__VA_ARGS__); \
    fflush(stderr); \
} while (0)
#endif
