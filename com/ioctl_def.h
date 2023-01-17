#ifndef _COM_IOCTL_H
#define _COM_IOCTL_H

/*
 * ioctl parameter structure
 */

struct ioctlbuffer {
  u_int32_t size;
  u_int32_t entries;
  void *buffer;
};

/*
 * ioctl operations
 */


#define IOCSTART          _IOWR('Z',  1,    struct ioctlbuffer)
#define IOCSTOP           _IOWR('Z',  2,    struct ioctlbuffer)


/*
 * ioctl errors
 */

enum ioctl_error {
  NO_ERROR=0,
  ERROR_INVALID=20,
  ERROR_MALLOC
};



#endif
