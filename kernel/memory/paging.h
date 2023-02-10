#ifndef PAGING_H
#define PAGING_H

#include <klibc/types.h>

/* Take a physical address and temporarily page it into kernel space
 * The pointer returned points to the given object in virtual memory.
 *
 * When completed, the object must be unpaged with unpage_physical_address.
 *
 * \return Virtual paged address to the given object
 */
void *page_physical_address (void *phys_address, const ksize_t size);

/* Unpage all pages that were paged for the given object
 */
void unpage_physical_address (void *virt_address);

#endif /* PAGING_H */
