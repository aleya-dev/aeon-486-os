#include "memory.h"

#include <drivers/display/display.h>
#include <klibc/memory.h>
#include <klibc/stdbool.h>
#include <kmodule.h>
#include <lib/hexdump.h>
#include <memory/paging.h>
#include <platform/i386/hal.h>

kmodule (memory);

typedef struct memory_allocator_t
{
  kuint32_t min_address;
  kuint32_t max_address;
  kuint32_t block_count;
  kuint32_t mem_map[0];
} memory_allocator_t;

memory_allocator_t *g_memory_allocator;

#define MEM_BLOCK_SIZE 4096
#define OUT_OF_MEMORY 0xffffffff

static void
mem_memory_map_set (const kuint32_t bit)
{
  g_memory_allocator->mem_map[bit / 32] |= (1 << (bit % 32));
}

static void
mem_memory_map_unset (const kuint32_t bit)
{
  g_memory_allocator->mem_map[bit / 32] &= ~(1 << (bit % 32));
}

static bool
mem_memory_map_test (const kuint32_t bit)
{
  return g_memory_allocator->mem_map[bit / 32] & (1 << (bit % 32));
}

kuint32_t
mem_get_bytes (void)
{
  kuint16_t total;
  kuint8_t lowmem;
  kuint8_t highmem;

  outportb (0x70, 0x30);
  lowmem = inportb (0x71);
  outportb (0x70, 0x31);
  highmem = inportb (0x71);

  total = lowmem | highmem << 8;
  return total * 1024;
}

void
mem_print_info (void)
{
  const kuint32_t bytes = mem_get_bytes ();
  const kuint32_t min_addr = compute_highest_free_address ();

  kprintf ("Memory size: %i bytes (%i KiB)\n", bytes, bytes / 1024);
  kprintf ("First available address: 0x%x\n", min_addr);
}

void
mem_initialize (void)
{
  dbg ("Initializing memory\n");

  const kuint32_t min_address = compute_highest_free_address ();
  const kuint32_t max_address
      = mem_get_bytes (); /* TODO: Check for memory holes */

  dbg (" - Min: 0x%x Max: 0x%x\n", min_address, max_address);

  /* Allocate the first free page for allocator bookkeeping.
   * TODO: If the system has too much memory, the bookkeeping won't
   * fit in 1 page. Maybe the kernel should just panic or not use
   * this memory?
   */
  g_memory_allocator = page (min_address, 4096, PTF_READ_WRITE);
  memset (g_memory_allocator, 0, 4096);

  dbg (" - g_memory_allocator = 0x%x\n", g_memory_allocator);

  g_memory_allocator->min_address = min_address + MEM_BLOCK_SIZE;
  g_memory_allocator->max_address = max_address;
  g_memory_allocator->block_count
      = (max_address - min_address) / MEM_BLOCK_SIZE;

  dbg (" - Block Count = %i\n", g_memory_allocator->block_count);
}

static kuint32_t
mem_find_first_free_block (void)
{
  kuint32_t i;
  kuint32_t j;

  for (i = 0; i < g_memory_allocator->block_count / 32; ++i)
    {
      /* If the entire block is used up, don't bother looking through it. */
      if (g_memory_allocator->mem_map[i] == 0xffffffff)
        continue;

      for (j = 0; j < 32; j++)
        {
          const kuint32_t bit = 1 << j;
          if (!(g_memory_allocator->mem_map[i] & bit))
            return i * 32 + j;
        }
    }

  return OUT_OF_MEMORY;
}

static kuint32_t
mem_find_first_free_block_range (const ksize_t size)
{
  kuint32_t i;
  kuint32_t j;

  if (size == 1)
    return mem_find_first_free_block ();

  for (i = 0; i < g_memory_allocator->block_count / 32; i++)
    {
      /* If the entire block is used up, don't bother looking through it. */
      if (g_memory_allocator->mem_map[i] == 0xffffffff)
        continue;

      for (j = 0; j < 32; j++)
        {
          const kuint32_t bit = 1 << j;
          if (!(g_memory_allocator->mem_map[i] & bit))
            {
              kuint32_t free = 0;
              kuint32_t start_bit = i * 32;
              start_bit += bit;

              for (kuint32_t count = 0; count <= size; count++)
                {
                  if (!mem_memory_map_test (start_bit + count))
                    free++;

                  if (free == size)
                    return i * 32 + j;
                }
            }
        }
    }

  return OUT_OF_MEMORY;
}

void *
kmalloc (const ksize_t size)
{
  dbg ("kmalloc: %i bytes\n", size);

  kuint32_t i;
  kuint32_t physical_address;
  void *virtual_address;
  const kuint32_t required_block_count = (size / MEM_BLOCK_SIZE) + 1;
  const kuint32_t free_block
      = mem_find_first_free_block_range (required_block_count);

  if (free_block == OUT_OF_MEMORY)
    panic ("Out of physical memory.");

  dbg (" - Block count: %i\n", required_block_count);
  dbg (" - First free block: %i\n", free_block);

  for (i = 0; i < required_block_count; ++i)
    mem_memory_map_set (free_block + i);

  physical_address
      = g_memory_allocator->min_address + free_block * MEM_BLOCK_SIZE;

  dbg (" - Physical Address: 0x%x\n", physical_address);

  virtual_address = page (physical_address, size, PTF_READ_WRITE);

  dbg (" - Virtual Address: 0x%x\n", virtual_address);

  return virtual_address;
}

kuint32_t
kfree (void *ptr)
{
  dbg ("kfree: 0x%x\n", ptr);

  const kuint32_t physical_address = get_physical_address (ptr);

  dbg ("Physical address: 0x%x\n", physical_address);

  const kuint32_t block_count = unpage (ptr);

  dbg (" - Unpaged block count: %i\n", block_count);

  const kuint32_t frame
      = (g_memory_allocator->min_address - physical_address) / MEM_BLOCK_SIZE;

  dbg (" - Start frame: %i\n", frame);

  for (kuint32_t i = 0; i < block_count; ++i)
    mem_memory_map_unset (frame + i);
}

kuint32_t
align (const kuint32_t ptr, const kuint32_t align)
{
  kuint32_t addr = ptr;
  addr &= -align;
  return addr;
}

kuint32_t
page_align (const kuint32_t ptr)
{
  return align (ptr, 0x1000);
}
