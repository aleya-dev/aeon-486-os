#ifndef MEMORY_H
#define MEMORY_H

#include <klibc/types.h>

/* Get the total available size of memory from CMOS above the standard 1mb in bytes
 */
kuint32_t mem_get_bytes(void);

kuint32_t align (const kuint32_t ptr, const kuint32_t align);

kuint32_t page_align (const kuint32_t ptr);

#endif /* MEMORY_H */
