#include "Il2CppResolver.h"
#include <dlfcn.h>
#include <string.h>
#include <map>
#include <string>

extern "C" {
    const void *(*il2cpp_assembly_get_image)(const void *assembly) = nullptr;
    void *(*il2cpp_domain_get)() = nullptr;
    void **(*il2cpp_domain_get_assemblies)(const void *domain, size_t *size) = nullptr;
    const char *(*il2cpp_image_get_name)(void *image) = nullptr;
    void *(*il2cpp_class_from_name)(const void *image, const char *namespaze, const char *name) = nullptr;
    void *(*il2cpp_class_get_field_from_name)(void *klass, const char *name) = nullptr;
    void *(*il2cpp_class_get_method_from_name)(void *klass, const char *name, int argsCount) = nullptr;
    void *(*il2cpp_class_get_parent)(void *klass) = nullptr;
    void *(*il2cpp_class_get_fields)(void *klass, void* *iter) = nullptr;
    const char *(*il2cpp_field_get_name)(void *field) = nullptr;
    size_t (*il2cpp_field_get_offset)(void *field) = nullptr;
    void (*il2cpp_field_static_get_value)(void *field, void *value) = nullptr;
    void (*il2cpp_field_static_set_value)(void *field, void *value) = nullptr;
    void *(*il2cpp_thread_attach)(void *domain) = nullptr;
}

bool Il2CppAttach() {
    // Di iOS, UnityFramework memuat libil2cpp.
    // Kita coba buka handle global dulu.
    void* handle = dlopen(NULL, RTLD_LAZY);
    if (!handle) return false;

    // Coba resolve salah satu fungsi untuk memastikan kita di context yang benar
    il2cpp_domain_get = (void*(*)()) dlsym(handle, "il2cpp_domain_get");
    
    // Jika gagal di namespace global, coba memuat UnityFramework secara eksplisit
    if (!il2cpp_domain_get) {
        handle = dlopen("UnityFramework.framework/UnityFramework", RTLD_LAZY);
        if (!handle) return false;
        il2cpp_domain_get = (void*(*)()) dlsym(handle, "il2cpp_domain_get");
    }

    if (!il2cpp_domain_get) return false; // Gagal attach

    il2cpp_assembly_get_image = (const void *(*)(const void *)) dlsym(handle, "il2cpp_assembly_get_image");
    il2cpp_domain_get_assemblies = (void **(*)(const void *, size_t *)) dlsym(handle, "il2cpp_domain_get_assemblies");
    il2cpp_image_get_name = (const char *(*)(void *)) dlsym(handle, "il2cpp_image_get_name");
    il2cpp_class_from_name = (void *(*)(const void *, const char *, const char *)) dlsym(handle, "il2cpp_class_from_name");
    il2cpp_class_get_field_from_name = (void *(*)(void *, const char *)) dlsym(handle, "il2cpp_class_get_field_from_name");
    il2cpp_class_get_method_from_name = (void *(*)(void *, const char *, int)) dlsym(handle, "il2cpp_class_get_method_from_name");
    il2cpp_class_get_parent = (void *(*)(void *)) dlsym(handle, "il2cpp_class_get_parent");
    il2cpp_class_get_fields = (void *(*)(void *, void **)) dlsym(handle, "il2cpp_class_get_fields");
    il2cpp_field_get_name = (const char *(*)(void *)) dlsym(handle, "il2cpp_field_get_name");
    il2cpp_field_get_offset = (size_t (*)(void *)) dlsym(handle, "il2cpp_field_get_offset");
    il2cpp_field_static_get_value = (void (*)(void *, void *)) dlsym(handle, "il2cpp_field_static_get_value");
    il2cpp_field_static_set_value = (void (*)(void *, void *)) dlsym(handle, "il2cpp_field_static_set_value");

    return true;
}

void *Il2CppGetImageByName(const char *image) {
    if (!il2cpp_domain_get || !il2cpp_domain_get_assemblies || !il2cpp_assembly_get_image || !il2cpp_image_get_name) return nullptr;
    
    void* domain = il2cpp_domain_get();
    if (!domain) return nullptr; // Unity belum selesai inisialisasi
    
    size_t size = 0;
    void **assemblies = il2cpp_domain_get_assemblies(domain, &size);
    if (!assemblies) return nullptr;
    
    for(size_t i = 0; i < size; ++i) {
        void *img = (void *)il2cpp_assembly_get_image(assemblies[i]);
        if (!img) continue;
        const char *img_name = il2cpp_image_get_name(img);
        if (img_name && strcmp(img_name, image) == 0) {
            return img;
        }
    }
    return nullptr;
}

void *Il2CppGetClassType(const char *image, const char *namespaze, const char *clazz) {
    static std::map<std::string, void *> cache;
    std::string s = std::string(image) + std::string(namespaze) + std::string(clazz);
    
    if (cache.count(s) > 0) return cache[s];
    
    void *img = Il2CppGetImageByName(image);
    if (!img) return nullptr;
    
    void *klass = il2cpp_class_from_name(img, namespaze, clazz);
    if (!klass) return nullptr;
    
    cache[s] = klass;
    return klass;
}

void Il2CppGetStaticFieldValue(const char *image, const char *namespaze, const char *clazz, const char *name, void *output) {
    void *klass = Il2CppGetClassType(image, namespaze, clazz);
    if (!klass) return;
    
    void *field = il2cpp_class_get_field_from_name(klass, name);
    if (!field) return;
    
    if (il2cpp_field_static_get_value) {
        il2cpp_field_static_get_value(field, output);
    }
}

size_t Il2CppGetFieldOffset(const char *image, const char *namespaze, const char *clazz, const char *name) {
    void *klass = Il2CppGetClassType(image, namespaze, clazz);
    if (!klass) return 0;
    
    void *field = nullptr;
    while (klass != nullptr) {
        field = il2cpp_class_get_field_from_name(klass, name);
        if (field) break;
        if (il2cpp_class_get_parent) {
            klass = il2cpp_class_get_parent(klass);
        } else {
            break;
        }
    }
    
    if (!field) return 0;
    
    if (il2cpp_field_get_offset) {
        return il2cpp_field_get_offset(field);
    }
    return 0;
}

size_t FindFieldContaining(const char *image, const char *namespaze, const char *clazz, const char *searchStr) {
    if (!il2cpp_class_get_fields || !il2cpp_field_get_name || !il2cpp_field_get_offset) return 0;
    
    void *klass = Il2CppGetClassType(image, namespaze, clazz);
    if (!klass) return 0;
    
    while (klass != nullptr) {
        void* iter = nullptr;
        void* field = il2cpp_class_get_fields(klass, &iter);
        while (field != nullptr) {
            const char* name = il2cpp_field_get_name(field);
            if (name && strcasestr(name, searchStr)) {
                return il2cpp_field_get_offset(field);
            }
            field = il2cpp_class_get_fields(klass, &iter);
        }
        if (il2cpp_class_get_parent) {
            klass = il2cpp_class_get_parent(klass);
        } else {
            break;
        }
    }
    return 0;
}

void *Il2CppGetMethodOffset(const char *image, const char *namespaze, const char *clazz, const char *name, int argsCount) {
    void *klass = Il2CppGetClassType(image, namespaze, clazz);
    if (!klass) return nullptr;
    
    void **method = (void **)il2cpp_class_get_method_from_name(klass, name, argsCount);
    if (!method) return nullptr;
    
    return *method;
}
