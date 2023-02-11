#include "memory.h"
#include <drivers/display/display.h>
#include <platform/i386/hal.h>

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
  kprintf ("Memory size: %i bytes (%i KiB)\n", bytes, bytes / 1024);
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
