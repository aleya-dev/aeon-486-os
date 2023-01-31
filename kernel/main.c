#include "drivers/display/display.h"
#include "drivers/display/textmode.h"

void
kernel_main (void)
{
  display_t *display = textmode_init ();
  display->clear ();
  display_register (display);

  kprintf ("Hello! %i\n", 1337);
  kprintf ("This is a kernel!");
}
