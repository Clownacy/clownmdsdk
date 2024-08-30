#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int abs(int i);

void* memcpy(void *dest, const void *src, size_t count);
void* memset(void *dest, int ch, size_t count);

#ifdef __cplusplus
}
#endif

#endif /* STDLIB_H */
