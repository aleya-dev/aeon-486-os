#ifndef MEMORY_H
#define MEMORY_H

#include <klibc/types.h>

kuint32_t align (const kuint32_t ptr, const kuint32_t align);

kuint32_t page_align (const kuint32_t ptr);

#endif /* MEMORY_H */
