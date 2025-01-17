#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((visibility("default"))) int abs(int i);

__attribute__((visibility("default"))) void* memcpy(void *dest, const void *src, size_t count);
__attribute__((visibility("default"))) void* memmove(void *dest, const void *src, size_t count);
__attribute__((visibility("default"))) void* memset(void *dest, int ch, size_t count);

#ifdef __cplusplus
}
#endif

#endif /* STDLIB_H */
