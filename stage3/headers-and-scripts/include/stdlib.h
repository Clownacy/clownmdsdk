#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>

int abs(int i);

void* memcpy(void *dest, const void *src, size_t count);
void* memset(void *dest, int ch, size_t count);

#endif /* STDLIB_H */
