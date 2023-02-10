#include "paging.h"

#include <drivers/display/display.h>
#include <klibc/types.h>
#include <memory/memory.h>

/* Page table flags */
#define PTF_PRESENT (1 << 0)
#define PTF_READ_WRITE (1 << 1)

/* When unpaging a pointer, it will keep unpaging until an end of block is
 * found
 * TODO: Should this continue into the next page table?
 */
#define PTF_AEON_END_OF_BLOCK (1 << 10)

/* The page should never be unpaged. It belongs to the kernel */
#define PTF_AEON_LOCKED (1 << 11)

/* Page directory flags */
#define PDF_PRESENT (1 << 0)
#define PDF_READ_WRITE (1 << 1)

#define PAGE_DIRECTORY_LEN 1024
#define PAGE_TABLE_LEN 1024
#define PAGE_SIZE 0x1000

extern kuint32_t g_page_directory[PAGE_DIRECTORY_LEN];

/* Identity map for 00000000 */
extern kuint32_t g_page_table_00000000[PAGE_TABLE_LEN];

/* Kernel pages */
#define NUM_KERNEL_PAGE_TABLES 3
#define KERNEL_PAGE_TABLES_LEN (PAGE_TABLE_LEN * NUM_KERNEL_PAGE_TABLES)
#define KERNEL_FIRST_PAGE_ADDRESS 0xC0000000

extern kuint32_t g_page_table_C0000000[PAGE_TABLE_LEN];
extern kuint32_t g_page_table_C0400000[PAGE_TABLE_LEN];
extern kuint32_t g_page_table_C0800000[PAGE_TABLE_LEN];

#define OUT_OF_FREE_BLOCKS 0xffffffff

/* Refresh the current page directory */
static void
pagetable_refresh (void)
{
  __asm__ volatile ("movl %cr3, %ecx");
  __asm__ volatile ("movl %ecx, %cr3");
}

/* Find given amount of blocks free in a given page table.
 * \param page_table Pointer to a page table
 * \param page_table_size Typically a page table is 1024 entries,
 *                        but if they are contiguous, this algorithm
 *                        could be used to spread the allocation across
 *                        multiple tables.
 * \param requested_count The amount of continuous pages requested
 * \return Offset, or OUT_OF_FREE_BLOCKS if no block was found.
 */
static ksize_t
find_free_blocks_offset (const kuint32_t *page_table,
                         const ksize_t page_table_size,
                         const ksize_t requested_count)
{
  ksize_t i;
  ksize_t j;
  kuint32_t found;

  for (i = 0; i < page_table_size; ++i)
    {
      /* Skip pages that are present */
      if (page_table[i] & PDF_PRESENT)
        continue;

      /* Did we reach the end of the table? */
      if (i + requested_count >= page_table_size)
        return OUT_OF_FREE_BLOCKS; /* out of page memory... */

      /* Now check if we can allocate the requested tables in a row */
      found = 1;
      for (j = 0; j < requested_count; ++j)
        {
          /* Is this page in use? .... */
          if (page_table[i + j] & PDF_PRESENT)
            {
              /* .... then we could not use this index */
              found = 0;
              break;
            }
        }

      if (found == 1)
        return i;
    }

  return OUT_OF_FREE_BLOCKS;
}

void *
page_physical_address (void *phys_address, const ksize_t size)
{
  kuint32_t aligned_physical_address;
  kuint32_t offset;
  kuint32_t required_page_count;
  kuint32_t page_offset;
  kuint32_t i;

  kprintf ("Requested paging of physical address %x (size: %x)\n",
           phys_address, size);

  aligned_physical_address = (kuint32_t)page_align ((void *)phys_address);
  offset = (kuint32_t)phys_address - aligned_physical_address;

  required_page_count = ((size + offset) / PAGE_SIZE) + 1;

  page_offset = find_free_blocks_offset (
      g_page_table_C0000000, KERNEL_PAGE_TABLES_LEN, required_page_count);

  if (page_offset == OUT_OF_FREE_BLOCKS)
    panic ("Kernel out of page space.");

  kprintf (" - Required pages: %i\n", required_page_count);
  kprintf (" - Using page offset (from %x): %i\n", KERNEL_FIRST_PAGE_ADDRESS,
           page_offset);

  /* Create the pages */
  for (i = page_offset; i < page_offset + required_page_count; ++i)
    {
      g_page_table_C0000000[i]
          = aligned_physical_address | PTF_READ_WRITE | PTF_PRESENT;
      aligned_physical_address += PAGE_SIZE;
    }

  /* Mark the last block as end_of_block so the entire chain can be unpaged */
  g_page_table_C0000000[i - 1] |= PTF_AEON_END_OF_BLOCK;

  pagetable_refresh ();

  kprintf (" - Returning virtual address: %x\n", (kuint32_t)
           (KERNEL_FIRST_PAGE_ADDRESS + (page_offset * PAGE_SIZE) + offset));

  /* Return the paged address adjusted with the offset into the page */
  return (void *)(KERNEL_FIRST_PAGE_ADDRESS + (page_offset * PAGE_SIZE)
                  + offset);
}

void
unpage_physical_address (void *virt_address)
{
}
