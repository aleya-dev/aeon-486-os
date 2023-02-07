#ifndef KLIBC_MEMORY_H
#define KLIBC_MEMORY_H

#include <klibc/types.h>

void *memcpy (const void *dest, const void *src, ksize_t count);

void *memset16 (void *ptr, const kuint16_t value, ksize_t num);

void *memset (void *ptr, const int value, ksize_t num);

#endif /* KLIBC_MEMORY_H */
