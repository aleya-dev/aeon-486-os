#ifndef KLIBC_STDLIB_H
#define KLIBC_STDLIB_H

#include <klibc/types.h>

char *itoa (kuint32_t value, char *str, const int base);
char *itoa_s (kuint32_t value, char *str, const int base);

#define OFFSETOF(TYPE, ELEMENT) ((ksize_t) & (((TYPE *)0)->ELEMENT))

#endif /* KLIBC_STDLIB_H */
