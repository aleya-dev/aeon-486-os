#include "drivers/display/display.h"
#include "drivers/display/textmode.h"
#include "platform/i386/gdt.h"
#include "platform/i386/idt.h"
#include "platform/i386/irq.h"

void
kernel_main (void)
{
  gdt_init ();

  display_t *display = textmode_init ();
  display->clear ();
  display_register (display);

  kprintf ("Now booting AEON\n");

  idt_init ();
  irq_init ();

  kprintf ("Done!\n");
}
