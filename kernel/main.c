#include "drivers/display/display.h"
#include "drivers/display/textmode.h"
//#include "platform/i386/idt.h"

void
kernel_main (void)
{
  display_t *display = textmode_init ();
  display->clear ();
  display_register (display);

  kprintf ("Now booting AEON\n");

  //idt_init ();
}
