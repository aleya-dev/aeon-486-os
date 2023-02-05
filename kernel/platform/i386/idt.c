#include "drivers/display/display.h"
#include "platform/i386/hal.h"
#include <aeon/memory.h>
#include <aeon/types.h>

extern void isr_0 (void);
extern void isr_1 (void);
extern void isr_2 (void);
extern void isr_3 (void);
extern void isr_4 (void);
extern void isr_5 (void);
extern void isr_6 (void);
extern void isr_7 (void);
extern void isr_8 (void);
extern void isr_9 (void);
extern void isr_10 (void);
extern void isr_11 (void);
extern void isr_12 (void);
extern void isr_13 (void);
extern void isr_14 (void);
extern void isr_15 (void);
extern void isr_16 (void);
extern void isr_17 (void);
extern void isr_18 (void);
extern void isr_19 (void);
extern void isr_20 (void);
extern void isr_21 (void);
extern void isr_22 (void);
extern void isr_23 (void);
extern void isr_24 (void);
extern void isr_25 (void);
extern void isr_26 (void);
extern void isr_27 (void);
extern void isr_28 (void);
extern void isr_29 (void);
extern void isr_30 (void);
extern void isr_31 (void);

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

typedef struct registers_t
{
  kuint32_t ds;                                     // Data segment selector
  kuint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
  kuint32_t int_no,
      err_code; // Interrupt number and error code (if applicable)
  kuint32_t eip, cs, eflags, useresp,
      ss; // Pushed by the processor automatically.
} __attribute__ ((packed)) registers_t;

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
idt_init (void)
{
  /* Sets the special IDT pointer up, just like in 'gdt.c' */
  g_idtp.limit = sizeof (g_idt) - 1;
  g_idtp.base = (kuint32_t)&g_idt;

  /* Clear out the entire IDT, initializing it to zeros */
  memset (&g_idt, 0, sizeof (idt_entry_t) * 256);

  idt_set_gate (0, isr_0, 0x08, 0x8E); /* Division by zero */
  idt_set_gate (1, isr_1, 0x08,
                0x8E); /* Single-step interrupt (see trap flag) */
  idt_set_gate (2, isr_2, 0x08, 0x8E); /* NMI */
  idt_set_gate (3, isr_3, 0x08, 0x8E); /* Breakpoint (which benefits from the
                                          shorter 0xCC encoding of INT 3) */
  idt_set_gate (4, isr_4, 0x08, 0x8E); /* Overflow */
  idt_set_gate (5, isr_5, 0x08, 0x8E); /* Bound Range Exceeded */
  idt_set_gate (6, isr_6, 0x08, 0x8E); /* Invalid Opcode */
  idt_set_gate (7, isr_7, 0x08, 0x8E); /* Coprocessor not available */
  idt_set_gate (8, isr_8, 0x08, 0x8E); /* Double Fault */
  idt_set_gate (9, isr_9, 0x08,
                0x8E); /* Coprocessor Segment Overrun (386 or earlier only) */
  idt_set_gate (10, isr_10, 0x08, 0x8E); /* Invalid Task State Segment */
  idt_set_gate (11, isr_11, 0x08, 0x8E); /* Segment not present */
  idt_set_gate (12, isr_12, 0x08, 0x8E); /* Stack Segment Fault */
  idt_set_gate (13, isr_13, 0x08, 0x8E); /* General Protection Fault */
  idt_set_gate (14, isr_14, 0x08, 0x8E); /* Page Fault */
  idt_set_gate (15, isr_15, 0x08, 0x8E); /* reserved */
  idt_set_gate (16, isr_16, 0x08, 0x8E); /* x87 Floating Point Exception */
  idt_set_gate (17, isr_17, 0x08, 0x8E); /* Alignment Check */
  idt_set_gate (18, isr_18, 0x08, 0x8E); /* Machine Check */
  idt_set_gate (19, isr_19, 0x08, 0x8E); /* SIMD Floating-Point Exception */
  idt_set_gate (20, isr_20, 0x08, 0x8E); /* Virtualization Exception */
  idt_set_gate (
      21, isr_21, 0x08,
      0x8E); /* Control Protection Exception (only available with CET) */
  idt_set_gate (22, isr_22, 0x08, 0x8E);
  idt_set_gate (23, isr_23, 0x08, 0x8E);
  idt_set_gate (24, isr_24, 0x08, 0x8E);
  idt_set_gate (25, isr_25, 0x08, 0x8E);
  idt_set_gate (26, isr_26, 0x08, 0x8E);
  idt_set_gate (27, isr_27, 0x08, 0x8E);
  idt_set_gate (28, isr_28, 0x08, 0x8E);
  idt_set_gate (29, isr_29, 0x08, 0x8E);
  idt_set_gate (30, isr_30, 0x08, 0x8E);
  idt_set_gate (31, isr_31, 0x08, 0x8E);

  /* Add any new ISRs to the IDT here using idt_set_gate */
  // idt_set_gate (0x2f, __idt_test_handler, 0x08, 0x8e); // 0 |
  //  IDT_32BIT_INTERRUPT_GATE | IDT_PRESENT;);

  /* Points the processor's internal register to the new IDT */
  idt_update_lidt ();

  kprintf ("Testing IDT...");

  __asm__ volatile ("int $10");
  __asm__ volatile ("int $11");
  __asm__ volatile ("int $12");
}

void
isr_handler (registers_t regs)
{
  kprintf ("recieved interrupt: %i\n", regs.int_no);
}
