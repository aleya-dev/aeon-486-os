#include <klibc/string.h>

ksize_t
strlen (const char *str)
{
  ksize_t i = 0;

  while (str[i] != 0)
    ++i;

  return i;
}
