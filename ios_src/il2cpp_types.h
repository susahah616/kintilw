#pragma once
#include <cstdint>
#include <vector>
#include <string>

// ============================================================
// IL2CPP Runtime Types - Ported from Code Breaker (Il2Cpp.h)
// Untuk ARM64 iOS internal cheat (dylib injection)
// ============================================================

// IL2CPP String (managed System.String)
struct Il2CppString {
    void *klass;
    void *monitor;
    int length;
    char chars[1]; // UTF-16 chars, variable length

    std::string toString() {
        if (length <= 0 || length > 512) return "";
        std::string result;
        result.reserve(length);
        char16_t* wchars = reinterpret_cast<char16_t*>(chars);
        for (int i = 0; i < length; i++) {
            char16_t ch = wchars[i];
            result += (ch < 128) ? (char)ch : '?';
        }
        return result;
    }
};

// IL2CPP Array<T> (managed T[])
template <typename T>
struct Il2CppArray {
    void* klass;
    void* monitor;
    void* bounds;
    int   max_length;
    T     vector[0]; // C-style flexible array member

    int getLength() { return max_length; }
    T* getPointer() { return vector; }

    template<typename V = T>
    std::vector<V> toCPPlist() {
        std::vector<V> ret;
        for (int i = 0; i < max_length; i++)
            ret.push_back(vector[i]);
        return ret;
    }
};

// IL2CPP List<T> (managed System.Collections.Generic.List<T>)
template <typename T>
struct Il2CppList {
    void *klass;
    void *monitor;
    Il2CppArray<T> *items;
    int size;
    int version;

    T* getItems() { return items->getPointer(); }
    int getSize() { return size; }
};

// IL2CPP Dictionary<TKey, TValue> (managed System.Collections.Generic.Dictionary<TKey,TValue>)
template<typename TKey, typename TValue>
struct Il2CppDictionary {
    struct Entry {
        int hashCode, next;
        TKey key;
        TValue value;
    };

    void *klass;
    void *monitor;
    Il2CppArray<int> *buckets;
    Il2CppArray<Entry> *entries;
    int count;
    int version;
    int freeList;
    int freeCount;
    void *comparer;
    Il2CppArray<TKey> *keys;
    Il2CppArray<TValue> *values;
    void *syncRoot;

    int getNumKeys() { return count; }
};
