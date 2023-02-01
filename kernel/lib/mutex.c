#include <aeon/mutex.h>

void
mutex_lock (mutex_t *m)
{
  while (m->locked)
    {
    } // TODO
  m->locked = 1;
}

void
mutex_unlock (mutex_t *m)
{
  m->locked = 0;
}
