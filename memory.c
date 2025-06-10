#include "minall.h"

char memory_pool[MEMORY_POOL_SIZE];
size_t memory_offset = 0;

void* minall_malloc(size_t size) {
    // Align to 8-byte boundary for better performance
    size = (size + 7) & ~7;
    
    if (memory_offset + size > MEMORY_POOL_SIZE) {
        fprintf(stderr, "Memory pool exhausted!\n");
        return NULL;
    }
    
    void* ptr = &memory_pool[memory_offset];
    memory_offset += size;
    return ptr;
}

void minall_reset() {
    memory_offset = 0;
}
