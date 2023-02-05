#include "drivers/display/display.h"
#include "platform/i386/hal.h"
#include <aeon/memory.h>
#include <aeon/types.h>

typedef struct idt_entry_t
{
  kuint16_t entry_point_address_lo;
  kuint16_t segment_selector; /* Our kernel segment goes here! */
  kuint8_t always0;           /* This will ALWAYS be set to 0! */
  kuint8_t flags;             /* Set using the above table! */
  kuint16_t entry_point_address_hi;
} __attribute__ ((packed)) idt_entry_t;

typedef struct idt_ptr_t
{
  kuint16_t limit;
  kuint32_t base;
} __attribute__ ((packed)) idt_ptr_t;

static idt_entry_t g_idt[256];
static idt_ptr_t g_idtp;

static void
idt_update_lidt (void)
{
  __asm__ volatile ("lidt %0" ::"m"(g_idtp));
}

void
idt_set_gate (const kuint8_t index, const void *base,
              const kuint16_t segment_selector, const kuint8_t flags)
{
  g_idt[index].entry_point_address_lo = (((const kuint32_t)base) & 0xFFFF);
  g_idt[index].entry_point_address_hi
      = (((const kuint32_t)base) >> 16) & 0xFFFF;
  g_idt[index].segment_selector = segment_selector;
  g_idt[index].always0 = 0;
  g_idt[index].flags = flags;
}

static kuint8_t test_success = 0;
static kuint32_t test_timeout = 0x1000;

void
__idt_test_handler ()
{
  INT_START;
  test_success = 1;
  INT_END;
}

void
idt_init (void)
{
  /* Sets the special IDT pointer up, just like in 'gdt.c' */
  g_idtp.limit = sizeof (g_idt) - 1;
  g_idtp.base = (kuint32_t)&g_idt;

  /* Clear out the entire IDT, initializing it to zeros */
  memset (&g_idt, 0, sizeof (idt_entry_t) * 256);

  /* Add any new ISRs to the IDT here using idt_set_gate */
  idt_set_gate (0x2f, &__idt_test_handler, 0x08, 0x8e); // 0 |
  // IDT_32BIT_INTERRUPT_GATE | IDT_PRESENT;);

  /* Points the processor's internal register to the new IDT */
  idt_update_lidt ();

  kprintf ("Testing IDT...");

  //__asm__ volatile ("int $0x2f");
  while (test_timeout-- != 0)
    {
      if (test_success != 0)
        {
          kprintf ("SUCCESS\n");
          break;
        }
    }
  if (!test_success)
    panic ("FAILED (timeout)");
}
