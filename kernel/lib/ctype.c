#include <aeon/ctype.h>

int
isprint (const int ch)
{
  return ch >= 0x20 && ch <= 0x7e;
}
