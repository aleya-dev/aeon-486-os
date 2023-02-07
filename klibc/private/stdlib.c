#include <klibc/stdlib.h>
#include <klibc/stddef.h>

static char tbuf[32];
static const char bchars[] = { '0', '1', '2', '3', '4', '5', '6', '7',
                               '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

// void itoa(unsigned int i,unsigned int base,char* buf) {

char *
itoa (int value, char *str, const int base)
{
  int pos = 0;
  int opos;
  int top;

  if (value == 0 || base > 16)
    {
      str[0] = '0';
      str[1] = '\0';
      return str;
    }

  while (value != 0)
    {
      tbuf[pos] = bchars[value % base];
      pos++;
      value /= base;
    }

  top = pos--;

  for (opos = 0; opos < top; pos--, opos++)
    str[opos] = tbuf[pos];

  str[opos] = 0;

  return str;
}

char *
itoa_s (int value, char *str, const int base)
{
  if (base > 16)
    return NULL;

  if (value < 0)
    {
      *str++ = '-';
      value *= -1;
    }

  return itoa (value, str, base);
}
