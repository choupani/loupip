
#ifndef KRN_IOCTL_H
#define KRN_IOCTL_H

#include "./precom.h"

struct ioctlST {
  struct ioctlbuffer *ub;
  caddr_t            *kb;
};

extern int parsecmd(struct ioctlST*, u_long, u_int8_t*);

#endif /* KRN_IOCTL_H */
