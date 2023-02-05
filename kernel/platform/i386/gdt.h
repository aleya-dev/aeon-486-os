#ifndef GDT_H
#define GDT_H

#include <aeon/types.h>

/* Access flags */

/* Best left clear (0), the CPU will set it when the segment is accessed. */
#define ACCESS_FLAG_ACCESSED 0x01

/* For code segments: Readable bit. If unset, read access for this segment
 * is not allowed. If set, read access is allowed. Write access is never
 * allowed for code segments.
 */
#define ACCESS_FLAG_CODE_SEGMENT_READABLE 0x02

/* For data segments: Writeable bit. If unset, write access for this segment
 * is not allowed. If set, write access is allowed. Read access is always
 * allowed for data segments.
 */
#define ACCESS_FLAG_DATA_SEGMENT_WRITABLE 0x02

/* If unset, code in this segment can only be executed from the ring set in
 * DPL. If set, code in this segment can be executed from an equal or lower
 * privilege level. For example, code in ring 3 can far-jump to conforming code
 * in a ring 2 segment. The DPL field represent the highest privilege level
 * that is allowed to execute the segment. For example, code in ring 0 cannot
 * far-jump to a conforming code segment where DPL is 2, while code in ring 2
 * and 3 can. Note that the privilege level remains the same, ie. a far-jump
 * from ring 3 to a segment with a DPL of 2 remains in ring 3 after the jump.
 */
#define ACCESS_FLAG_CODE_SEGMENT_CONFORMING 0x04

/* For data selectors: Direction bit. If unset, the segment grows up. If set
 * the segment grows down, ie. the Offset has to be greater than the Limit.
 */
#define ACCESS_FLAG_DATA_SEGMENT_GROWS_DOWN 0x04

/* If unset, the descriptor defines a data segment. If set, it defines a code
 * segment which can be executed from.
 */
#define ACCESS_FLAG_IS_EXECUTABLE 0x08

/* If unset, the descriptor defines a system segment (eg. a Task State
 * Segment). If set, it defines a code or data segment.
 */
#define ACCESS_FLAG_IS_CODE_OR_DATA 0x10

#define ACCESS_FLAG_RING_0 0x00
#define ACCESS_FLAG_RING_1 0x20
#define ACCESS_FLAG_RING_2 0x40
#define ACCESS_FLAG_RING_3 0x60

/* Must be set for any valid segment. */
#define ACCESS_FLAG_IS_PRESENT 0x80

/* Additional flags */

/* If unset, the descriptor defines a 16-bit protected mode segment. If set, it
 * defines a 32-bit protected mode segment. A GDT can have both 16-bit and
 * 32-bit selectors at once.
 */
#define GDT_FLAG_IS_32_BITS 0x04

/* If unset, the Limit is in 1 Byte blocks (byte granularity). If set, the
 * Limit is in 4 KiB blocks (page granularity).
 */
#define GDT_FLAG_4KB_GRANULARITY 0x08

typedef struct gdt_entry_t
{
  /* low 16 bits of limit */
  kuint16_t limit_low;

  /* low 16 bits of base */
  kuint16_t base_low;

  /* middle 8 bits of base */
  kuint8_t base_middle;

  /* Access flags, determine what ring this segment can be used in. */
  kuint8_t access_flags;

  /* High 4 bits of limit */
  kuint8_t limit_high : 4;

  /* Additional flags; granularity, size flag, long mode */
  kuint8_t flags : 4;

  /* High 8 bits of base */
  kuint8_t base_high;
} __attribute__ ((packed)) gdt_entry_t;

typedef struct gdt_ptr_t
{
  kuint16_t limit; /* Size of the GDT */
  kuint32_t base;  /* Start of the GDT */
} __attribute__ ((packed)) gdt_ptr_t;

void gdt_init (void);

#endif /* GDT_H */
