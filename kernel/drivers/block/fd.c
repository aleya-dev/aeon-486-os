/* Floppy drive controller driver
 * Most of this file is combined code from the Linux kernel, osdev and various
 * open source projects on github.
 * https://wiki.osdev.org/Floppy_Disk_Controller
 * https://forum.osdev.org/viewtopic.php?t=13538
 * https://github.com/levex/osdev
 */

#if 0

#include <aeon/types.h>
#include "platform/i386/hal.h"
#include "kprint.h"

typedef struct floppy_info_t
{
  unsigned int cylinders;
  unsigned int sectors_per_track;
  unsigned int num_heads;
  unsigned int num_tracks;
} floppy_info_t;

/* Values taken from linux kernel 1.0 */
static floppy_info_t floppy_info[] = {
  { 0, 0, 0, 0 },      /* no testing */
  { 720, 9, 2, 40 },   /* 360kB PC diskettes */
  { 2400, 15, 2, 80 }, /* 1.2 MB AT-diskettes */
  { 720, 9, 2, 40 },   /* 360kB in 720kB drive */
  { 1440, 9, 2, 80 },  /* 3.5" 720kB diskette */
  { 720, 9, 2, 40 },   /* 360kB in 1.2MB drive */
  { 1440, 9, 2, 80 },  /* 720kB in 1.2MB drive */
  { 2880, 18, 2, 80 }, /* 1.44MB diskette */
};

/* No auto detection yet. Assume 1.44mb floppy */
static floppy_info_t *current_type = &floppy_info[7];

static void
lba_to_chs (IN const floppy_info_t *info, const kuint32_t lba,
                    OUT kuint16_t *cyl, OUT kuint16_t *head,
                    OUT kuint16_t *sector)
{
  *cyl = lba / (2 * info->sectors_per_track);
  *head = ((lba % (2 * info->sectors_per_track)) / info->sectors_per_track);
  *sector
      = ((lba % (2 * info->sectors_per_track)) % info->sectors_per_track + 1);
}



void fd_init()
{
	//kprint("Looking for floppy devices\n");
	//outportb(0x70, 0x10);
	//kuint8_t cmos = inportb(0x71);
	//__parse_cmos((cmos&0xf0) >> 4, cmos&0x0f);
	//if(!primary_avail) goto exit;

	mprint("Registering FDC IRQ\n");
	set_int(38, (uint32_t)flpy_irq);
	mprint("Resetting controller.\n");
	fdc_reset();
	mprint("Floppy is now usable.\n");
exit:	_kill();
}

#endif
