#ifndef DISPLAY_H
#define DISPLAY_H

#include <aeon/console.h>
#include <stdarg.h>

typedef struct display_t
{
  kuint16_t width;
  kuint16_t height;
  console_t console;

  void (*puts) (const char *);
  void (*putc) (const char);
  void (*clear) (void);
} display_t;

void display_register (display_t *display);

void kprintf (const char *fmt, ...);
void kvprintf (const char *fmt, va_list ap);

#define panic(...)                                                            \
  do                                                                          \
    {                                                                         \
      kprintf ("***KERNEL PANIC*** in %s at line %d in function: %s\n",       \
               __FILE__, __LINE__, __func__);                                 \
      kprintf (__VA_ARGS__);                                                  \
      for (;;)                                                                \
        ;                                                                     \
    }                                                                         \
  while (0)

#endif /* DISPLAY_H */
