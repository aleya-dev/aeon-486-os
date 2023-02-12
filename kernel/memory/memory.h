#ifndef MEMORY_H
#define MEMORY_H

#include <klibc/types.h>

/* Get the total available size of memory from CMOS above the standard 1mb in
 * bytes
 */
kuint32_t mem_get_bytes (void);

/* Print information about the detected memory size to the kernel log.
 * Uses mem_get_bytes().
 */
void mem_print_info (void);

/* Initialize physical memory allocator
 */
void mem_initialize (void);

/* Allocate memory for the kernel. This also pages in the memory in kernel
 * address space. Memory will always be page aligned (4k) and always have a
 * size that is a multiple of 4k.
 *
 * TODO: This same mechanism should be able to allocate for user space. Maybe
 * just pass flags?
 */
void *kmalloc (const ksize_t size);

/* Free up memory allocated by kmalloc. Giving a pointer to this that was not
 * allocated or paged by kmalloc will lead to undefined behavior as kfree uses
 * special bits in the page table to work out the size of the allocated
 * physical memory.
 *
 * \return The amount of 4k blocks that were freed.
 */
kuint32_t kfree (void *ptr);

kuint32_t align (const kuint32_t ptr, const kuint32_t align);

kuint32_t page_align (const kuint32_t ptr);

#endif /* MEMORY_H */
