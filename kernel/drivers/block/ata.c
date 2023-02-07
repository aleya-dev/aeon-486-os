#include "ata.h"
#include <aeon/types.h>
#include <drivers/display/display.h>
#include <lib/hexdump.h>
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

typedef struct
{
  kuint8_t drive;
} ide_private_data;

kuint8_t ata_pm = 0; /* Primary master exists? */
kuint8_t ata_ps = 0; /* Primary Slave exists? */
kuint8_t ata_sm = 0; /* Secondary master exists? */
kuint8_t ata_ss = 0; /* Secondary slave exists? */

kuint8_t ide_buf[512];

void
ide_select_drive (kuint8_t bus, kuint8_t i)
{
  if (bus == ATA_PRIMARY)
    if (i == ATA_MASTER)
      outportb (ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xA0);
    else
      outportb (ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xB0);
  else if (i == ATA_MASTER)
    outportb (ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xA0);
  else
    outportb (ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xB0);
}

kuint8_t
ide_identify (kuint8_t bus, kuint8_t drive)
{
  kuint16_t io = 0;
  ide_select_drive (bus, drive);
  if (bus == ATA_PRIMARY)
    io = ATA_PRIMARY_IO;
  else
    io = ATA_SECONDARY_IO;
  /* ATA specs say these values must be zero before sending IDENTIFY */
  outportb (io + ATA_REG_SECCOUNT0, 0);
  outportb (io + ATA_REG_LBA0, 0);
  outportb (io + ATA_REG_LBA1, 0);
  outportb (io + ATA_REG_LBA2, 0);
  /* Now, send IDENTIFY */
  outportb (io + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
  // kprintf ("Sent IDENTIFY\n");
  /* Now, read status port */
  kuint8_t status = inportb (io + ATA_REG_STATUS);
  if (status)
    {
      /* Now, poll untill BSY is clear. */
      while (inportb (io + ATA_REG_STATUS) & ATA_SR_BSY != 0)
        ;
    pm_stat_read:
      status = inportb (io + ATA_REG_STATUS);
      if (status & ATA_SR_ERR)
        {
          kprintf ("%s%s has ERR set. Disabled.\n",
                   bus == ATA_PRIMARY ? "Primary" : "Secondary",
                   drive == ATA_PRIMARY ? " master" : " slave");
          return 0;
        }
      while (!(status & ATA_SR_DRQ))
        goto pm_stat_read;
      kprintf ("%s%s is online.\n",
               bus == ATA_PRIMARY ? "Primary" : "Secondary",
               drive == ATA_PRIMARY ? " master" : " slave");

      for (int i = 0; i < 256; i++)
        {
          *(kuint16_t *)(ide_buf + i * 2) = inportw (io + ATA_REG_DATA);
        }
    }
}

void
ata_probe (void)
{
  /* First check the primary bus,
   * and inside the master drive.
   */

  ide_identify (ATA_PRIMARY, ATA_MASTER);
  {
    ata_pm = 1;
    // device_t *dev = (device_t *)malloc(sizeof(device_t));
    // ide_private_data *priv = (ide_private_data
    // *)malloc(sizeof(ide_private_data));
    /* Now, process the IDENTIFY data */
    /* Model goes from W#27 to W#46 */
    // char *str = (char *)malloc(40);
    char str[40];
    for (int i = 0; i < 40; i += 2)
      {
        str[i] = ide_buf[ATA_IDENT_MODEL + i + 1];
        str[i + 1] = ide_buf[ATA_IDENT_MODEL + i];
      }
    // dev->name = str;
    // dev->unique_id = 32;
    // dev->dev_type = DEVICE_BLOCK;
    // priv->drive = (ATA_PRIMARY << 1) | ATA_MASTER;
    // dev->priv = priv;
    // dev->read = ata_read;
    // device_add(dev);
    kprintf ("Device: %s\n", str);

    khexdump (ide_buf, sizeof (ide_buf));
  }
  ide_identify (ATA_PRIMARY, ATA_SLAVE);
  /*ide_identify(ATA_SECONDARY, ATA_MASTER);
  ide_identify(ATA_SECONDARY, ATA_SLAVE);*/
}

void
ata_init (void)
{
  kprintf ("Checking for ATA drives\n");
  // ide_buf = (kuint16_t *)malloc(512);
  // set_int(ATA_PRIMARY_IRQ, ide_primary_irq);
  // set_int(ATA_SECONDARY_IRQ, ide_secondary_irq);
  ata_probe ();
  //_kill();
}
