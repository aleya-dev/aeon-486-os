#include "memory.h"

kuint32_t
align (const kuint32_t ptr, const kuint32_t align)
{
  kuint32_t addr = ptr;
  addr &= -align;
  return (void *)addr;
}

kuint32_t
page_align (const kuint32_t ptr)
{
  return align (ptr, 0x1000);
}
