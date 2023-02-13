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
      kprintf ("\n\n***KERNEL PANIC***\n\n %s:%d in %s.\n", __FILE__,         \
               __LINE__, __func__);                                           \
      kprintf (__VA_ARGS__);                                                  \
      for (;;)                                                                \
        ;                                                                     \
    }                                                                         \
  while (0)

#ifdef DEBUG
#define dbg(...)                                                              \
  do                                                                          \
    {                                                                         \
      kprintf ("[%s]: ", __s_module_name__);                                  \
      kprintf (__VA_ARGS__);                                                  \
    }                                                                         \
  while (0)
#else
#define dbg(...)                                                              \
  do                                                                          \
    {                                                                         \
    }                                                                         \
  while (0)
#endif

#endif /* DISPLAY_H */
