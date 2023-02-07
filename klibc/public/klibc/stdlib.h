#ifndef KLIBC_STDLIB_H
#define KLIBC_STDLIB_H

#include <klibc/types.h>

char *itoa (int value, char *str, const int base);
char *itoa_s (int value, char *str, const int base);

#define OFFSETOF(TYPE, ELEMENT) ((ksize_t) & (((TYPE *)0)->ELEMENT))

#endif /* KLIBC_STDLIB_H */
