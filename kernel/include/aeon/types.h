#ifndef AEON_TYPES_FILE_H
#define AEON_TYPES_FILE_H

/* Prepended with k to not clash with standard C types */
typedef char kint8_t;
typedef unsigned char kuint8_t;
typedef short kint16_t;
typedef unsigned short kuint16_t;
typedef long kint32_t;
typedef unsigned long kuint32_t;

typedef kuint16_t uid_t;
typedef kuint16_t gid_t;

/* Like unix epoch, but unsigned to avoid year 2038 nonsense. */
typedef kuint32_t epoch_t;

/* Offset in a file/device */
typedef kint32_t offset_t;

/* Generic size type */
typedef kuint32_t ksize_t;

/* Dummy macros used for api documentation */

/* A given pointer to a function is written to by the function */
#define OUT

/* A given pointer to a function is only read by the function */
#define IN

/* A given pointer to a function is read from and then written into */
#define INOUT

#endif
