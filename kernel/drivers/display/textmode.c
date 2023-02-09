#include "textmode.h"
#include <klibc/memory.h>

#define VGA_MEMORY 0xB8000
#define TEXTMODE_WIDTH 80
#define TEXTMODE_HEIGHT 25

static display_t display = { 0 };

static kuint8_t
textmode_make_color (const kuint8_t fg, const kuint8_t bg)
{
  return fg | bg << 4;
}

static kuint16_t
textmode_create_entry (const char c, const kuint8_t color)
{
  kuint16_t _char = (kuint16_t)c;
  kuint16_t _color = (kuint16_t)color;
  return _char | _color << 8;
}

static inline void
textmode_scrollup ()
{
  for (int y = 0; y < TEXTMODE_HEIGHT; y++)
    {
      memcpy ((char *)(VGA_MEMORY + y * TEXTMODE_WIDTH * 2),
              (char *)(VGA_MEMORY + (y + 1) * TEXTMODE_WIDTH * 2),
              TEXTMODE_WIDTH * 2);
    }

  kuint8_t entity
      = textmode_make_color (display.console.fgcol, display.console.bgcol);
  memset16 ((char *)(VGA_MEMORY + TEXTMODE_HEIGHT * TEXTMODE_WIDTH * 2),
            textmode_create_entry (' ', entity), TEXTMODE_WIDTH * 2);
}

static void
textmode_clear (void)
{
  kuint8_t entity
      = textmode_make_color (display.console.fgcol, display.console.bgcol);

  for (ksize_t y = 0u; y < TEXTMODE_HEIGHT; y++)
    for (ksize_t x = 0u; x < TEXTMODE_WIDTH; x++)
      {
        const ksize_t index = y * 2 * TEXTMODE_WIDTH + x * 2;
        *(kuint16_t *)(VGA_MEMORY + index)
            = textmode_create_entry (' ', entity);
      }

  display.console.cx = 0;
  display.console.cy = 0;
}

void
textmode_putc (const char c)
{
  if (!c)
    return;

  if (display.console.cx >= TEXTMODE_WIDTH || c == '\n')
    {
      display.console.cx = 0;
      display.console.cy++;
    }
  if (display.console.cy >= TEXTMODE_HEIGHT - 1)
    {
      textmode_scrollup ();
      display.console.cy--;
    }

  if (c == '\n')
    return;

  const ksize_t index
      = display.console.cy * 2 * TEXTMODE_WIDTH + display.console.cx * 2;

  *(kuint16_t *)(VGA_MEMORY + index) = textmode_create_entry (
      c, textmode_make_color (display.console.fgcol, display.console.bgcol));

  display.console.cx++;
}

static void
textmode_puts (const char *s)
{
  while (*s != 0)
    textmode_putc (*s++);
}

display_t *
textmode_init (void)
{
  display.width = TEXTMODE_WIDTH;
  display.height = TEXTMODE_HEIGHT;
  display.console.cx = 0;
  display.console.cy = 0;
  display.console.fgcol = COLOR_WHITE;
  display.console.bgcol = COLOR_BLACK;
  display.puts = textmode_puts;
  display.putc = textmode_putc;
  display.clear = textmode_clear;
  return &display;
}
