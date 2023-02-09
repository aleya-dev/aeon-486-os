#include <lib/hexdump.h>

#include "drivers/block/ata.h"
#include "drivers/display/display.h"
#include "drivers/display/textmode.h"
#include "multiboot.h"
#include "platform/i386/gdt.h"
#include "platform/i386/idt.h"
#include "platform/i386/irq.h"

extern kuint32_t g_page_directory[1024];
extern kuint32_t g_page_table_00000000[1024];
extern kuint32_t g_page_table_C0000000[1024];

void
kernel_main (const kuint32_t magic, const kuint32_t addr)
{
  multiboot_info_t *mbi;

  if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
      /* TODO: Can't use textmode yet. Implement an early panic text message */
      return;
    }

  mbi = (multiboot_info_t *)addr;

  gdt_init ();

  display_t *display = textmode_init ();
  display->clear ();
  display_register (display);

  kprintf ("Now booting AEON\n");

  idt_init ();
  irq_init ();

  ata_init ();

  kprintf ("Done!\n");
}
