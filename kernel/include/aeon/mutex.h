#ifndef AEON_KERNEL_MUTEX_H
#define AEON_KERNEL_MUTEX_H

#include <aeon/types.h>

typedef struct mutex_t
{
  kuint8_t locked;
} mutex_t;

#define DEFINE_MUTEX(name) static mutex_t name = { .locked = 0 }

void mutex_lock (mutex_t *m);
void mutex_unlock (mutex_t *m);

#endif
