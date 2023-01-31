#include "hal.h"

kuint8_t
inportb (const kuint16_t port)
{
  kuint8_t ret;
  asm volatile ("inb %%dx, %%al" : "=a"(ret) : "d"(port));
  return ret;
}

kuint16_t
inportw (const kuint16_t port)
{
  kuint16_t ret;
  asm volatile ("inw %%dx, %%ax" : "=a"(ret) : "d"(port));
  return ret;
}

kuint32_t
inportl (kuint16_t port)
{
  kuint32_t ret;
  asm volatile ("inl %%dx, %%eax" : "=a"(ret) : "d"(port));
  return ret;
}

void
outportb (const kuint16_t port, const kuint8_t value)
{
  asm volatile ("outb %%al, %%dx" : : "d"(port), "a"(value));
}

void
outportw (const kuint16_t port, const kuint16_t value)
{
  asm volatile ("outw %%ax, %%dx" : : "d"(port), "a"(value));
}

void
outportl (const kuint16_t port, const kuint32_t value)
{
  asm volatile ("outl %%eax, %%dx" : : "d"(port), "a"(value));
}
