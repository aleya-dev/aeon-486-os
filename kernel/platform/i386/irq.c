#include "irq.h"
#include "hal.h"
#include <drivers/display/display.h>

#define PIC1 0x20 /* IO base address for master PIC */
#define PIC2 0xA0 /* IO base address for slave PIC */
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)

#define PIC_EOI 0x20 /* End-of-interrupt command code */

#define ICW1_ICW4 0x01      /* ICW4 (not) needed */
#define ICW1_SINGLE 0x02    /* Single (cascade) mode */
#define ICW1_INTERVAL4 0x04 /* Call address interval 4 (8) */
#define ICW1_LEVEL 0x08     /* Level triggered (edge) mode */
#define ICW1_INIT 0x10      /* Initialization - required! */

#define ICW4_8086 0x01       /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO 0x02       /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE 0x08  /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C /* Buffered mode/master */
#define ICW4_SFNM 0x10       /* Special fully nested (not) */

#define PIC_IRQ_OFFSET1 32
#define PIC_IRQ_OFFSET2 (PIC_IRQ_OFFSET1 + 8)

static void
pic_remap (void)
{
  kuint8_t a1;
  kuint8_t a2;

  a1 = inportb (PIC1_DATA); // save masks
  a2 = inportb (PIC2_DATA);

  /* starts the initialization sequence (in cascade mode) */
  outportb (PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
  io_wait ();
  outportb (PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
  io_wait ();
  outportb (PIC1_DATA, PIC_IRQ_OFFSET1); // ICW2: Master PIC vector offset
  io_wait ();
  outportb (PIC2_DATA, PIC_IRQ_OFFSET2); // ICW2: Slave PIC vector offset
  io_wait ();
  outportb (PIC1_DATA, 4); // ICW3: tell Master PIC that there is a slave PIC
                           // at IRQ2 (0000 0100)
  io_wait ();

  /* ICW3: tell Slave PIC its cascade identity (0000 0010) */
  outportb (PIC2_DATA, 2);
  io_wait ();

  outportb (PIC1_DATA, ICW4_8086);
  io_wait ();
  outportb (PIC2_DATA, ICW4_8086);
  io_wait ();

  outportb (PIC1_DATA, a1); // restore saved masks.
  outportb (PIC2_DATA, a2);
}

static void
pic_send_eoi (const kuint32_t irq)
{
  if (irq >= 8)
    outportb (PIC2_COMMAND, PIC_EOI);

  outportb (PIC1_COMMAND, PIC_EOI);
}

void
irq_init (void)
{
  kprintf ("Setting up IRQ...");
  pic_remap ();
  kprintf ("Done.\n");
}

void
irq_handler (const kuint32_t irq)
{
  kprintf ("received irq: %i\n", irq);

  pic_send_eoi (irq);
}
