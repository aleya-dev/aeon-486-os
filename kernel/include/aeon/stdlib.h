#ifndef AEON_KERNEL_STDLIB_H
#define AEON_KERNEL_STDLIB_H

#include <aeon/types.h>

char *itoa (int value, char *str, const int base);
char *itoa_s (int value, char *str, const int base);

#define OFFSETOF(TYPE, ELEMENT) ((ksize_t) & (((TYPE *)0)->ELEMENT))

#endif /* AEON_KERNEL_STDLIB_H */
