#ifndef MEMORY_H
#define MEMORY_H

#include <klibc/types.h>

void *
align (void *ptr, kuint32_t align);

void *
page_align (void *ptr);

#endif /* MEMORY_H */
