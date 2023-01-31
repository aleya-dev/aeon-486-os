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

void display_register(display_t *display);

void kprintf (const char *fmt, ...);
void kvprintf(const char *fmt, va_list ap);

#endif /* DISPLAY_H */
