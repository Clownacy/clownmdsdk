#ifndef STRING_H
#define STRING_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((visibility("default"))) void* memcpy(void *dest, const void *src, size_t count);
__attribute__((visibility("default"))) void* memmove(void *dest, const void *src, size_t count);
__attribute__((visibility("default"))) void* memset(void *dest, int ch, size_t count);

#ifdef __cplusplus
}
#endif

#endif /* STRING_H */
