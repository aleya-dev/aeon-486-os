#include "drivers/block/ata.h"
#include "drivers/display/display.h"
#include "drivers/display/textmode.h"
#include "multiboot.h"
#include "platform/i386/gdt.h"
#include "platform/i386/hal.h"
#include "platform/i386/idt.h"
#include "platform/i386/irq.h"
#include <lib/hexdump.h>
#include <memory/paging.h>

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
nosound ()
{
  kint8_t tmp = inportb (0x61) & 0xFC;
  outportb (0x61, tmp);
}

void
kernel_main (const kuint32_t magic, const kuint32_t addr)
{
  multiboot_info_t *mbi;
  multiboot_memory_map_t *mbmm;

  gdt_init ();

  display_t *display = textmode_init ();
  display->clear ();
  display_register (display);

  kprintf ("Now booting AEON\n");

  idt_init ();
  irq_init ();

  mbi = page_physical_address ((void *)addr, sizeof (multiboot_info_t));
  mbmm = page_physical_address ((void *)mbi->mmap_addr,
                                sizeof (multiboot_memory_map_t)
                                    * mbi->mmap_length);

  int i;
  for (i = 0; i < mbi->mmap_length; i += sizeof (multiboot_memory_map_t))
    {
      multiboot_memory_map_t *mmmt = &mbmm[i];

      // if (mmmt->type == MULTIBOOT_MEMORY_AVAILABLE)
      {
        kprintf ("Addr: %x | Len: %x | Size: %x | Type: %d\n", mmmt->addr,
                 mmmt->len, mmmt->size, mmmt->type);
      }
    }

  ata_init ();

  kprintf ("Done!\n");
}
