
#ifndef _NET_PKIO_H
#define _NET_PKIO_H

#include "./precom.h"
#include "./nt_sltp.h"

#define LOUP_TAG 0x40

struct loup_tag {
  int	dir;
};

void mbuf_io(struct mbuf**, struct ifnet*, int, sa_family_t);

void pkt_io(char*, int, struct sltphdr*);

#endif
