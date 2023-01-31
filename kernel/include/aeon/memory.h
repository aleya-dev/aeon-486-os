#ifndef AEON_KERNEL_MEMORY_H
#define AEON_KERNEL_MEMORY_H

#include <aeon/types.h>

void *
memcpy (const void *dest, const void *src, ksize_t count)
{
  char *dst8 = (char *)dest;
  char *src8 = (char *)src;

  if (count & 1)
    {
      dst8[0] = src8[0];
      dst8 += 1;
      src8 += 1;
    }

  count /= 2;
  while (count--)
    {
      dst8[0] = src8[0];
      dst8[1] = src8[1];

      dst8 += 2;
      src8 += 2;
    }

  return (void *)dest;
}

void *
memset16 (void *ptr, const kuint16_t value, ksize_t num)
{
  kuint16_t *p = ptr;

  while (num--)
    *p++ = value;

  return ptr;
}

void *
memset (void *ptr, const int value, ksize_t num)
{
  unsigned char *p = ptr;

  while (num--)
    *p++ = (unsigned char)value;

  return ptr;
}

#endif
