#ifndef HAL_H
#define HAL_H

#include <klibc/types.h>

#define INT_START __asm__ volatile ("pusha")
#define INT_END                                                               \
  __asm__ volatile ("popa");                                                  \
  __asm__ volatile ("iret")

kuint8_t inportb (const kuint16_t port);

kuint16_t inportw (const kuint16_t port);

kuint32_t inportl (kuint16_t port);

void outportb (const kuint16_t port, const kuint8_t value);

void outportw (const kuint16_t port, const kuint16_t value);

void outportl (const kuint16_t port, const kuint32_t value);

void io_wait(void);

#endif /* HAL_H */
