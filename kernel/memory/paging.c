#include "paging.h"

#include <drivers/display/display.h>
#include <klibc/math.h>
#include <klibc/stddef.h>
#include <klibc/types.h>
#include <memory/memory.h>

#define PTF_PRESENT (1 << 0)

/* When unpaging a pointer, it will keep unpaging until an end of block is
 * found
 * TODO: Should this continue into the next page table?
 */
#define PTF_AEON_END_OF_BLOCK (1 << 10)

/* The page should never be unpaged. It belongs to the kernel */
#define PTF_AEON_LOCKED (1 << 11)

/* Page directory flags */
#define PDF_READ_WRITE (1 << 1)
#define PDF_PRESENT (1 << 0)

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
#define KERNEL_FIRST_PAGE_TABLE_OFFSET 768

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
      /* Skip pages that are present or locked */
      if (page_table[i] & PTF_PRESENT || page_table[i] & PTF_AEON_LOCKED)
        continue;

      /* Did we reach the end of the table? */
      if (i + requested_count >= page_table_size)
        return OUT_OF_FREE_BLOCKS; /* out of page memory... */

      /* Now check if we can allocate the requested tables in a row */
      found = 1;
      for (j = 0; j < requested_count; ++j)
        {
          /* Is this page in use? .... */
          if (page_table[i + j] & PTF_PRESENT
              || page_table[i + j] & PTF_AEON_LOCKED)
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

kuint32_t
compute_highest_free_address (void)
{
  kuint32_t highest_address = 0;
  kuint32_t i;

  for (i = 0; i < KERNEL_PAGE_TABLES_LEN; ++i)
    {
      highest_address
          = MAX (highest_address, (g_page_table_C0000000[i] & ~0x7ff));
    }

  /* The highest address in a page table is still a page being used.
   * The first available free address is 4kb further.
   */
  return highest_address + PAGE_SIZE;
}

void *
page (const kuint32_t physical_address, const ksize_t size,
      const kuint32_t flags)
{
  kuint32_t aligned_physical_address;
  kuint32_t required_page_count;
  kuint32_t page_offset;
  kuint32_t i;
  kuint32_t *page_table;
  kuint32_t page_table_length;

  /* Are we mapping to kernel or user space? */
  if (flags & PTF_USER)
    {
      /* TODO: Add mechanism for user space tables */
      page_table = NULL;
      page_table_length = 0;
    }
  else
    {
      page_table = g_page_table_C0000000;
      page_table_length = KERNEL_PAGE_TABLES_LEN;
    }

  required_page_count = (size / PAGE_SIZE) + 1;
  page_offset = find_free_blocks_offset (page_table, page_table_length,
                                         required_page_count);

  if (page_offset == OUT_OF_FREE_BLOCKS)
    panic ("Out of page space.");

  /* Create the pages */
  aligned_physical_address = physical_address;
  for (i = page_offset; i < page_offset + required_page_count; ++i)
    {
      page_table[i] = aligned_physical_address | flags | PTF_PRESENT;
      aligned_physical_address += PAGE_SIZE;
    }

  /* Mark the last block as end_of_block so the entire chain can be unpaged */
  page_table[i - 1] |= PTF_AEON_END_OF_BLOCK;

  pagetable_refresh ();

  /* Return the paged address adjusted with the offset into the page */
  if (flags & PTF_USER)
    {
      /* TODO: Add mechanism for user space tables */
      return NULL;
    }
  else
    {
      return (void *)(KERNEL_FIRST_PAGE_ADDRESS + (page_offset * PAGE_SIZE));
    }
}

void *
page_unaligned (const kuint32_t physical_address, const ksize_t size,
                const kuint32_t flags)
{
  kuint32_t aligned_physical_address;
  kuint32_t offset;
  void *virtual_address;

  aligned_physical_address = page_align (physical_address);
  offset = (kuint32_t)physical_address - aligned_physical_address;

  virtual_address = page (aligned_physical_address, size, flags);

  /* Return the paged address adjusted with the offset into the page */
  return (void *)(((kuint8_t *)virtual_address) + offset);
}

kuint32_t
unpage (void *virt_address)
{
  kuint32_t page_count;
  kuint32_t page_entry;
  kuint32_t *page_table_address;

  /* Find the correct page directory and table index
   * AAAAAAAAAA         BBBBBBBBBB        CCCCCCCCCCCC
   * directory index    page table index  offset into page
   */
  const kuint32_t page_directory_index
      = (((kuint32_t)virt_address) >> 22) & 0x3ff;
  const kuint32_t page_table_index = (((kuint32_t)virt_address) >> 12) & 0x3ff;

  /* Is the page table within kernel space? */
  if (page_directory_index >= KERNEL_FIRST_PAGE_TABLE_OFFSET)
    {
      page_table_address
          = &g_page_table_C0000000[(page_directory_index
                                    - KERNEL_FIRST_PAGE_TABLE_OFFSET)
                                       * PAGE_TABLE_LEN
                                   + page_table_index];
    }
  else
    {
      /* TODO: Unpage user space stuff */
      page_table_address = NULL;
    }

  page_count = 0;
  do
    {
      if (*page_table_address & PTF_AEON_LOCKED)
        panic ("Kernel tried to unpage a locked page.");

      page_entry = *page_table_address;
      *page_table_address = 0;
      ++page_table_address;
      ++page_count;
    }
  while (!(page_entry & PTF_AEON_END_OF_BLOCK));

  pagetable_refresh ();

  return page_count;
}

kuint32_t
get_physical_address (void *virt_address)
{
  /* Find the correct page directory and table index
   * AAAAAAAAAA         BBBBBBBBBB        CCCCCCCCCCCC
   * directory index    page table index  offset into page
   */
  const kuint32_t page_directory_index
      = (((kuint32_t)virt_address) >> 22) & 0x3ff;
  const kuint32_t page_table_index = (((kuint32_t)virt_address) >> 12) & 0x3ff;

  /* Is the page table within kernel space? */
  if (page_directory_index >= KERNEL_FIRST_PAGE_TABLE_OFFSET)
    {
      return g_page_table_C0000000[(page_directory_index
                                    - KERNEL_FIRST_PAGE_TABLE_OFFSET)
                                       * PAGE_TABLE_LEN
                                   + page_table_index]
             & ~0xfff;
    }
  else
    {
      /* TODO: user space stuff */
      return 0;
    }
}
