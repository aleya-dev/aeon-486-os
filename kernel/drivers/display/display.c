#include "display.h"
#include <aeon/mutex.h>
#include <klibc/stdlib.h>
#include <klibc/string.h>

#define MAX_DISPLAYS 8

static kuint32_t s_num_displays = 0;
static display_t *s_displays[MAX_DISPLAYS];

DEFINE_MUTEX (m_kprintf);

static void
display_puts (const char *str)
{
  kuint32_t i;

  for (i = 0; i < s_num_displays; ++i)
    s_displays[i]->puts (str);
}

static void
display_putc (const char c)
{
  kuint32_t i;

  for (i = 0; i < s_num_displays; ++i)
    s_displays[i]->putc (c);
}

void
display_register (display_t *display)
{
  s_displays[s_num_displays++] = display;
}

void
kprintf (const char *fmt, ...)
{
  va_list ap;
  va_start (ap, fmt);
  kvprintf (fmt, ap);
  va_end (ap);
}

void
kvprintf (const char *fmt, va_list ap)
{
  int num_width = 0;
  char *s = 0;

  mutex_lock (&m_kprintf);

  for (ksize_t i = 0u; i < strlen (fmt); ++i)
    {
      if (fmt[i] == '%')
        {
          switch (fmt[i + 1])
            {
            case '0':
              {
                num_width = fmt[i + 2] - '0';
                ++i;
                ++i;
                break;
              }
            }

          switch (fmt[i + 1])
            {
            case 's':
              {
                s = va_arg (ap, char *);
                display_puts (s);
                ++i;
                continue;
              }
            case 'i':
            case 'd':
              {
                int len;
                const int c = va_arg (ap, int);
                char str[32] = { 0 };
                itoa_s (c, str, 10);

                len = strlen (str);

                if (len < num_width)
                  {
                    int i;
                    for (i = 0; i < num_width - len; ++i)
                      display_putc ('0');
                  }

                display_puts (str);
                ++i;
                continue;
              }
            case 'x':
              {
                int len;
                const int c = va_arg (ap, int);
                char str[32] = { 0 };
                itoa (c, str, 16);

                len = strlen (str);

                if (len < num_width)
                  {
                    int i;
                    for (i = 0; i < num_width - len; ++i)
                      display_putc ('0');
                  }

                display_puts (str);
                ++i;
                continue;
              }
            /** character **/
            case 'c':
              {
                // char gets promoted to int for va_arg!
                // clean it.
                const char c = (char)(va_arg (ap, int) & ~0xFFFFFF00);
                display_putc (c);
                ++i;
                continue;
              }
            default:
              break;
            }
        }
      else
        {
          display_putc (fmt[i]);
        }
    }

  mutex_unlock (&m_kprintf);
}
