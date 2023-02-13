#include "ata.h"

#include <drivers/display/display.h>
#include <klibc/memory.h>
#include <klibc/types.h>
#include <lib/hexdump.h>
#include <memory/memory.h>
#include <platform/i386/hal.h>

#define ATA_PRIMARY_IO 0x1F0
#define ATA_SECONDARY_IO 0x170

#define ATA_PRIMARY_DCR_AS 0x3F6
#define ATA_SECONDARY_DCR_AS 0x376

#define ATA_PRIMARY_IRQ 14
#define ATA_SECONDARY_IRQ 15

#define ATA_SR_BSY 0x80
#define ATA_SR_DRDY 0x40
#define ATA_SR_DF 0x20
#define ATA_SR_DSC 0x10
#define ATA_SR_DRQ 0x08
#define ATA_SR_CORR 0x04
#define ATA_SR_IDX 0x02
#define ATA_SR_ERR 0x01

#define ATA_ER_BBK 0x80
#define ATA_ER_UNC 0x40
#define ATA_ER_MC 0x20
#define ATA_ER_IDNF 0x10
#define ATA_ER_MCR 0x08
#define ATA_ER_ABRT 0x04
#define ATA_ER_TK0NF 0x02
#define ATA_ER_AMNF 0x01

#define ATA_CMD_READ_PIO 0x20
#define ATA_CMD_READ_PIO_EXT 0x24
#define ATA_CMD_READ_DMA 0xC8
#define ATA_CMD_READ_DMA_EXT 0x25
#define ATA_CMD_WRITE_PIO 0x30
#define ATA_CMD_WRITE_PIO_EXT 0x34
#define ATA_CMD_WRITE_DMA 0xCA
#define ATA_CMD_WRITE_DMA_EXT 0x35
#define ATA_CMD_CACHE_FLUSH 0xE7
#define ATA_CMD_CACHE_FLUSH_EXT 0xEA
#define ATA_CMD_PACKET 0xA0
#define ATA_CMD_IDENTIFY_PACKET 0xA1
#define ATA_CMD_IDENTIFY 0xEC

#define ATAPI_CMD_READ 0xA8
#define ATAPI_CMD_EJECT 0x1B

#define ATA_IDENT_DEVICETYPE 0
#define ATA_IDENT_CYLINDERS 2
#define ATA_IDENT_HEADS 6
#define ATA_IDENT_SECTORS 12
#define ATA_IDENT_SERIAL 20
#define ATA_IDENT_MODEL 54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID 106
#define ATA_IDENT_MAX_LBA 120
#define ATA_IDENT_COMMANDSETS 164
#define ATA_IDENT_MAX_LBA_EXT 200

#define IDE_ATA 0x00
#define IDE_ATAPI 0x01

#define ATA_MASTER 0x00
#define ATA_SLAVE 0x01

#define ATA_REG_DATA 0x00
#define ATA_REG_ERROR 0x01
#define ATA_REG_FEATURES 0x01
#define ATA_REG_SECCOUNT0 0x02
#define ATA_REG_LBA0 0x03
#define ATA_REG_LBA1 0x04
#define ATA_REG_LBA2 0x05
#define ATA_REG_HDDEVSEL 0x06
#define ATA_REG_COMMAND 0x07
#define ATA_REG_STATUS 0x07
#define ATA_REG_SECCOUNT1 0x08
#define ATA_REG_LBA3 0x09
#define ATA_REG_LBA4 0x0A
#define ATA_REG_LBA5 0x0B
#define ATA_REG_CONTROL 0x0C
#define ATA_REG_ALTSTATUS 0x0C
#define ATA_REG_DEVADDRESS 0x0D

// Channels:
#define ATA_PRIMARY 0x00
#define ATA_SECONDARY 0x01

// Directions:
#define ATA_READ 0x00
#define ATA_WRITE 0x013

typedef struct ata_drive_info_t
{
  kuint8_t available;
  char model[41];
  kuint16_t cylinders;
  kuint16_t heads;
  kuint16_t sectors_per_track;
  kuint32_t total_sectors;
  kuint8_t buffer[512];
} ata_drive_info_t;

typedef struct ata_info_t
{
  /* ATA_PRIMARY/ATA_SECONDARY and ATA_MASTER/ATA_SLAVE */
  ata_drive_info_t drive[2][2];
} ata_info_t;

static ata_info_t *g_ata_info;

/* Select the ata drive based on the bus and bus index
 * \param bus The bus (ATA_MASTER or ATA_SLAVE)
 * \param drive
 * \return The base io port for the given bus
 */
static kuint16_t
ata_select_drive (const kuint8_t bus, const kuint8_t drive)
{
  if (bus == ATA_PRIMARY)
    {
      if (drive == ATA_MASTER)
        outportb (ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xA0);
      else
        outportb (ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xB0);

      return ATA_PRIMARY_IO;
    }

  if (drive == ATA_MASTER)
    outportb (ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xA0);
  else
    outportb (ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xB0);

  return ATA_SECONDARY_IO;
}

static void
ata_poll_status_busy (const kuint16_t reg)
{
  while ((inportb (reg + ATA_REG_STATUS) & ATA_SR_BSY) != 0)
    ;
}

static void
ata_identify (const kuint8_t bus, const kuint8_t drive)
{
  kuint8_t status;
  identify_device_data_t *device_data;
  ata_drive_info_t *current_drive = &g_ata_info->drive[bus][drive];
  kuint16_t io = ata_select_drive (bus, drive);

  /* ATA specs say these values must be zero before sending IDENTIFY */
  outportb (io + ATA_REG_SECCOUNT0, 0);
  outportb (io + ATA_REG_LBA0, 0);
  outportb (io + ATA_REG_LBA1, 0);
  outportb (io + ATA_REG_LBA2, 0);
  outportb (io + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

  status = inportb (io + ATA_REG_STATUS);

  if (!status)
    return;

  ata_poll_status_busy (io);

  do
    {
      status = inportb (io + ATA_REG_STATUS);
      if (status & ATA_SR_ERR)
        {
          kprintf ("%s %s has ERR set. Disabled.\n",
                   bus == ATA_PRIMARY ? "Primary" : "Secondary",
                   drive == ATA_PRIMARY ? "master" : "slave");
          return;
        }
    }
  while (!(status & ATA_SR_DRQ));

  kprintf ("%s %s is online.\n", bus == ATA_PRIMARY ? "Primary" : "Secondary",
           drive == ATA_PRIMARY ? "master" : "slave");

  current_drive->available = 1;

  for (int i = 0; i < 256; i++)
    {
      *(kuint16_t *)(current_drive->buffer + i * 2)
          = inportw (io + ATA_REG_DATA);
    }

  device_data = (identify_device_data_t *)current_drive->buffer;

  /* For some reason the model name's character is screwed up. */
  for (int i = 0; i < 40; i += 2)
    {
      current_drive->model[i] = device_data->model_number[i + 1];
      current_drive->model[i + 1] = device_data->model_number[i];
    }

  current_drive->model[40] = '\0';
  current_drive->cylinders = device_data->num_cylinders;
  current_drive->heads = device_data->num_heads;
  current_drive->sectors_per_track = device_data->current_sectors_per_track;
  current_drive->total_sectors = device_data->current_sector_capacity;
}

void
ata_probe (void)
{
  ata_identify (ATA_PRIMARY, ATA_MASTER);
  ata_identify (ATA_PRIMARY, ATA_SLAVE);
  ata_identify (ATA_SECONDARY, ATA_MASTER);
  ata_identify (ATA_SECONDARY, ATA_SLAVE);
}

void
ata_init (void)
{
  kprintf ("Initializing ATA drives\n");

  g_ata_info = kmalloc (sizeof (ata_info_t));
  memset (g_ata_info, 0, sizeof (ata_info_t));

  ata_probe ();
}

static void
ata_print_single_info (const kuint8_t bus, const kuint8_t drive)
{
  if (g_ata_info->drive[bus][drive].available)
    {
      kprintf ("ATA Drive %i:%i: %s\n", bus, drive,
               g_ata_info->drive[bus][drive].model);
    }
}

static void
ata_poll_status (const kuint16_t base_io)
{
  kuint8_t status;

  for (int i = 0; i < 4; i++)
    inportb (base_io + ATA_REG_ALTSTATUS);

  do
    {
      status = inportb (base_io + ATA_REG_STATUS);
    }
  while (status & ATA_SR_BSY);

  do
    {
      status = inportb (base_io + ATA_REG_STATUS);

      if (status & ATA_SR_ERR)
        panic ("ATA device failure.\n");
    }
  while (!(status & ATA_SR_DRQ));
}

static void
ata_delay (const kuint16_t base_io)
{
  kuint32_t i;
  for (i = 0; i < 4; i++)
    inportb (base_io + ATA_REG_ALTSTATUS);
}

void
ata_print_info (void)
{
  ata_print_single_info (ATA_PRIMARY, ATA_MASTER);
  ata_print_single_info (ATA_PRIMARY, ATA_SLAVE);
  ata_print_single_info (ATA_SECONDARY, ATA_MASTER);
  ata_print_single_info (ATA_SECONDARY, ATA_SLAVE);
}

void
ata_read_sector (const kuint8_t bus, const kuint8_t drive, const kuint32_t lba,
                 kuint8_t *dst)
{
  kuint32_t i;
  kuint16_t base_io;
  const kuint8_t device_select = (drive == ATA_MASTER ? 0xE0 : 0xF0);

  if (bus == ATA_PRIMARY)
    base_io = ATA_PRIMARY_IO;
  else
    base_io = ATA_SECONDARY_IO;

  outportb (base_io + ATA_REG_HDDEVSEL,
            (device_select | (kuint8_t)((lba >> 24 & 0x0F))));
  outportb (base_io + 1, 0x00);
  outportb (base_io + ATA_REG_SECCOUNT0, 1);
  outportb (base_io + ATA_REG_LBA0, (kuint8_t)((lba)));
  outportb (base_io + ATA_REG_LBA1, (kuint8_t)((lba) >> 8));
  outportb (base_io + ATA_REG_LBA2, (kuint8_t)((lba) >> 16));
  outportb (base_io + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

  ata_poll_status (base_io);

  for (i = 0; i < 256; i++)
    {
      *(kuint16_t *)(dst + i * 2) = inportw (base_io + ATA_REG_DATA);
    }

  ata_delay (base_io);
}
