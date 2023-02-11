#include "drivers/block/ata.h"
#include "drivers/display/display.h"
#include "drivers/display/textmode.h"
#include "memory/memory.h"
#include "memory/paging.h"
#include "multiboot.h"
#include "platform/i386/gdt.h"
#include "platform/i386/hal.h"
#include "platform/i386/idt.h"
#include "platform/i386/irq.h"

static void
play_sound (kuint32_t nFrequence)
{
  kuint32_t Div;
  kuint8_t tmp;

  // Set the PIT to the desired frequency
  Div = 1193180 / nFrequence;
  outportb (0x43, 0xb6);
  outportb (0x42, (kuint8_t)(Div));
  outportb (0x42, (kuint8_t)(Div >> 8));

  // And play the sound using the PC speaker
  tmp = inportb (0x61);
  if (tmp != (tmp | 3))
    {
      outportb (0x61, tmp | 3);
    }
}

static void
nosound (void)
{
  kint8_t tmp = inportb (0x61) & 0xFC;
  outportb (0x61, tmp);
}

void
kernel_main (const kuint32_t magic, const kuint32_t addr)
{
  gdt_init ();

  display_t *display = textmode_init ();
  display->clear ();
  display_register (display);

  if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    panic ("Multiboot magic invalid: %x", magic);

  kprintf ("Now booting AEON 0.0.1\n");

  idt_init ();
  irq_init ();

  kprintf ("Memory size (cmos): %i\n", mem_get_bytes ());

  ata_init ();

  kprintf ("Done!\n");
}
