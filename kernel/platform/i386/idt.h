#ifndef IDT_H
#define IDT_H

void idt_init (void);

void idt_set_gate (const kuint8_t index, const void *base,
                   const kuint16_t segment_selector, const kuint8_t flags);

#endif /* IDT_H */
