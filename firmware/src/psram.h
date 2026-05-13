#include <esp_heap_caps.h>

template<typename T, typename... Args>
T* psram_new(Args&&... args) {
    void* mem = heap_caps_malloc(sizeof(T), MALLOC_CAP_SPIRAM);
    return mem ? new (mem) T(std::forward<Args>(args)...) : nullptr;
}

template<typename T>
void psram_delete(T* ptr) {
    if (!ptr) {
        return;
    }

    ptr->~T();
    heap_caps_free(ptr);
}