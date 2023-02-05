#ifndef AEON_KERNEL_MEMORY_H
#define AEON_KERNEL_MEMORY_H

#include <aeon/types.h>

void *memcpy (const void *dest, const void *src, ksize_t count);

void *memset16 (void *ptr, const kuint16_t value, ksize_t num);

void *memset (void *ptr, const int value, ksize_t num);

#endif
