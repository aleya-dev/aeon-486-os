#include "drivers/display/display.h"
#include "platform/i386/hal.h"
#include <aeon/types.h>

void _set_idtr (void)
{
	//asm volatile ("push %ebp");
	//asm volatile ("movl %esp, %ebp");

	asm volatile ("lidt 0x10F0");
    
	//asm volatile ("movl %ebp, %esp");
	//asm volatile ("pop %ebp");
	//asm volatile ("ret");
}

void _idt_default_handler (void)
{
	asm volatile ("pushal");
	asm volatile ("mov $0x20, %al");
	asm volatile ("mov $0x20, %dx");
	asm volatile ("out %al, %dx");
	asm volatile ("popal");
	asm volatile ("iretl");
}

static kuint32_t idt_location = 0;
static kuint32_t idtr_location = 0;
static kuint16_t idt_size = 0x800;

static kuint8_t test_success = 0;
static kuint32_t test_timeout = 0x1000;

static kuint8_t __idt_setup = 0;

void
schedule (void)
{
  /* TODO: Implement scheduler */
  asm volatile ("iret");
}

void
__idt_test_handler (void)
{
  INT_START;
  test_success = 1;
  INT_END;
}

void
idt_register_interrupt (kuint8_t i, kuint32_t callback)
{
  if (!__idt_setup)
    panic ("Invalid IDT!");

  *(kuint16_t *)(idt_location + 8 * i + 0)
      = (kuint16_t)(callback & 0x0000ffff);
  *(kuint16_t *)(idt_location + 8 * i + 2) = (kuint16_t)0x8;
  *(kuint8_t *)(idt_location + 8 * i + 4) = 0x00;
  *(kuint8_t *)(idt_location + 8 * i + 5)
      = 0x8e; // 0 | IDT_32BIT_INTERRUPT_GATE | IDT_PRESENT;
  *(kuint16_t *)(idt_location + 8 * i + 6)
      = (kuint16_t)((callback & 0xffff0000) >> 16);

  if (test_success)
    kprintf ("Registered INT#%d\n", i);
}

void
idt_init (void)
{
  idt_location = 0x2000;
  kprintf ("Location: 0x%x\n", idt_location);

  idtr_location = 0x10F0;
  kprintf ("IDTR location: 0x%x\n", idtr_location);
  __idt_setup = 1;

  for (kuint8_t i = 0; i < 255; i++)
    {
      idt_register_interrupt (i, (kuint32_t)&_idt_default_handler);
    }
  idt_register_interrupt (0x2f, (kuint32_t)&__idt_test_handler);

    //idt_register_interrupt (0x2e, (kuint32_t)&schedule);

  kprintf ("Registered all interrupts to default handler.\n");

  /* create IDTR now */
  *(kuint16_t *)idtr_location = idt_size - 1;
  *(kuint32_t *)(idtr_location + 2) = idt_location;
  kprintf ("IDTR.size = 0x%x IDTR.offset = 0x%x\n",
           *(kuint16_t *)idtr_location, *(kuint32_t *)(idtr_location + 2));
  _set_idtr ();
  kprintf ("IDTR set, testing link.\n");

  asm volatile ("int $0x2f");
  while (test_timeout-- != 0)
    {
      if (test_success != 0)
        {
          kprintf ("Test succeeded, disabling INT#0x2F\n");
          idt_register_interrupt (0x2F, (kuint32_t)&_idt_default_handler);
          break;
        }
    }

  if (!test_success)
    panic ("IDT link is offline (timeout).");
}
