#include "gdt.h"
#include <aeon/stdlib.h>

static gdt_ptr_t g_gdt_ptr;
static gdt_entry_t g_gdt_entries[5];

static void
gdt_flush (void)
{
  __asm__ volatile (
      "lgdt %0\n\t"
      "mov $0x10, %%ax \n\t" /* 0x10 is the offset in the GDT to
                              * our data segment (sizeof(gdt_entry_t) * 2)
                              */
      "mov %%ax, %%ds\n\t"   /* Load all data segment selectors */
      "mov %%ax, %%es\n\t"
      "mov %%ax, %%fs\n\t"
      "mov %%ax, %%gs\n\t"
      "mov %%ax, %%ss\n\t"
      "jmp $0x08,$.done\n\t" /* 0x08 is the offset to our code
                              * segment (sizeof(gdt_entry_t)). far jump to it
                              * to set cs
                              */
      ".done:\n\t" ::"m"(g_gdt_ptr));
}

static void
gdt_set_gate (const ksize_t num, const kuint32_t base, const kuint32_t limit,
              const kuint32_t access, const kuint32_t flags)
{
  g_gdt_entries[num].base_low = (base & 0xFFFF);
  g_gdt_entries[num].base_middle = (base >> 16) & 0xFF;
  g_gdt_entries[num].base_high = (base >> 24) & 0xFF;

  g_gdt_entries[num].limit_low = (limit & 0xFFFF);
  g_gdt_entries[num].limit_high = (limit >> 16) & 0x0F;

  g_gdt_entries[num].access_flags = access;
  g_gdt_entries[num].flags = flags;
}

void
gdt_init (void)
{
  g_gdt_ptr.limit = sizeof (g_gdt_entries) - 1;
  g_gdt_ptr.base = (kuint32_t)&g_gdt_entries;

  gdt_set_gate (0, 0, 0, 0, 0); // Null segment
  gdt_set_gate (1, 0, 0xFFFFFFFF,
                ACCESS_FLAG_IS_EXECUTABLE | ACCESS_FLAG_CODE_SEGMENT_READABLE
                    | ACCESS_FLAG_IS_CODE_OR_DATA | ACCESS_FLAG_IS_PRESENT,
                GDT_FLAG_IS_32_BITS
                    | GDT_FLAG_4KB_GRANULARITY); // Code segment

  gdt_set_gate (2, 0, 0xFFFFFFFF,
                ACCESS_FLAG_DATA_SEGMENT_WRITABLE | ACCESS_FLAG_IS_CODE_OR_DATA
                    | ACCESS_FLAG_IS_PRESENT,
                GDT_FLAG_IS_32_BITS
                    | GDT_FLAG_4KB_GRANULARITY); // Data segment

  gdt_set_gate (3, 0, 0xFFFFFFFF,
                ACCESS_FLAG_IS_EXECUTABLE | ACCESS_FLAG_CODE_SEGMENT_READABLE
                    | ACCESS_FLAG_IS_CODE_OR_DATA | ACCESS_FLAG_RING_3
                    | ACCESS_FLAG_IS_PRESENT,
                GDT_FLAG_IS_32_BITS
                    | GDT_FLAG_4KB_GRANULARITY); // User mode code segment

  gdt_set_gate (4, 0, 0xFFFFFFFF,
                ACCESS_FLAG_DATA_SEGMENT_WRITABLE | ACCESS_FLAG_IS_CODE_OR_DATA
                    | ACCESS_FLAG_RING_3 | ACCESS_FLAG_IS_PRESENT,
                GDT_FLAG_IS_32_BITS
                    | GDT_FLAG_4KB_GRANULARITY); // User mode data segment

  gdt_flush ();
}
