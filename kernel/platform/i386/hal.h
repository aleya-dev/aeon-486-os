#ifndef HAL_H
#define HAL_H

#include <aeon/types.h>

#define INT_START asm volatile ("pusha");
#define INT_END                                                               \
  asm volatile ("popa");                                                      \
  asm volatile ("iret");

kuint8_t inportb (const kuint16_t port);

kuint16_t inportw (const kuint16_t port);

kuint32_t inportl (kuint16_t port);

void outportb (const kuint16_t port, const kuint8_t value);

void outportw (const kuint16_t port, const kuint16_t value);

void outportl (const kuint16_t port, const kuint32_t value);

#endif /* HAL_H */
