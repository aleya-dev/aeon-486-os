#ifndef PAGING_H
#define PAGING_H

#include <klibc/types.h>

/* Page table flags */
#define PTF_READ_WRITE (1 << 1)
#define PTF_USER (1 << 2)

/* Take a 4k aligned physical address create one or more pages to access it
 * The pointer returned points to the given object in virtual memory with the
 * correct offset; so it can be used directly.
 *
 * When completed, the object must be unpaged with unpage().
 *
 * \param physical_address The physical address to be paged; must be 4k aligned
 * \param size The total size of the memory block to be paged in bytes
 * \param flags Controls the flags that will be set for each of the page table
 *              entries. Certain flags like PRESENT will be automatically set.
 *              If PTF_USER is set, the page will be mapped to user space. If
 *              it is not set, the page will be mapped upper half kernel
 *              memory (>= 0xC0000000).
 *
 * \return Virtual paged address to the given object.
 */
void *page (const kuint32_t physical_address, const ksize_t size,
            const kuint32_t flags);

/* Take an unaligned physical address create one or more pages to access it
 * The pointer returned points to the given object in virtual memory with the
 * correct offset; so it can be used directly.
 *
 * When completed, the object must be unpaged with unpage().
 *
 * \param physical_address The physical address to be paged; may be unaligned
 * \param size The total size of the memory block to be paged in bytes
 * \param flags Controls the flags that will be set for each of the page table
 *              entries. Certain flags like PRESENT will be automatically set.
 *              If PTF_USER is set, the page will be mapped to user space. If
 *              it is not set, the page will be mapped upper half kernel
 *              memory (>= 0xC0000000).
 *
 * \return Virtual paged address to the given object.
 */
void *page_unaligned (const kuint32_t physical_address, const ksize_t size,
                      const kuint32_t flags);

/* Unpage all pages that were paged for the given object
 *
 * \param address The address to unpage. Must be a pointer returned by either
 *                page() or page_unaligned(). Any other pointer will cause
 *                undefined behavior.
 */
void unpage (void *address);

#endif /* PAGING_H */
