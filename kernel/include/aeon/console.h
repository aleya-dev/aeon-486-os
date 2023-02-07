#ifndef AEON_KERNEL_CONSOLE_H
#define AEON_KERNEL_CONSOLE_H

#include <klibc/types.h>

typedef struct console_t
{
    kuint16_t cx;
    kuint16_t cy;
    kuint8_t bgcol;
    kuint8_t fgcol;
} console_t;

#endif /* AEON_CONSOLE_H */
