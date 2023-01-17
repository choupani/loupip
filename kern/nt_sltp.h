
#ifndef _NT_SLTP_H
#define _NT_SLTP_H

#ifdef _KERNEL_
#include "./precom.h"
#endif

#define CB_MAXLEN_DEV 0xFFFF

/*
* SLTP header.
*/
#pragma pack(push, 1)
struct sltphdr {
  char           ifname[IFNAMSIZ+1];
  int            dir;
  uint64_t	 cksum_flags;	/* checksum and offload features */
  u_int32_t      cksum_data;
};
#pragma pack(pop)

#endif /* _NT_SLTP_H */
