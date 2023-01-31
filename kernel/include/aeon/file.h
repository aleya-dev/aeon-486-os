#ifndef AEON_KERNEL_FILE_H
#define AEON_KERNEL_FILE_H

#include <aeon/types.h>

typedef struct file_operations_t
{
  int (*read) (struct inode *, struct file *, char *, int);
  int (*write) (struct inode *, struct file *, char *, int);
  int (*seek) (struct inode *, struct file *, off_t, int);
  int (*readdir) (struct inode *, struct file *, struct dirent *, int);

  int (*open) (struct inode *, struct file *);
  void (*close) (struct inode *, struct file *);
  int (*flush) (struct inode *, struct file *);
} file_operations_t;

typedef struct file_info_t
{
  uid_t uid;
  gid_t gid;
};

typedef struct file_t
{
  offset_t offset;
  file_info_t *info;
  file_operations_t *operations;
} file_t;

#endif
