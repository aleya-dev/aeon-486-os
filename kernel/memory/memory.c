#include "memory.h"

void *
align (void *ptr, kuint32_t align)
{
  kuint32_t addr = (kuint32_t)ptr;
  addr &= -align;
  return (void *)addr;
}

void *
page_align (void *ptr)
{
  return align (ptr, 0x1000);
}
