
#ifndef _NET_MBQ_H
#define _NET_MBQ_H

#include "./precom.h"
#include "./nt_sltp.h"

struct pkt_list {
  struct sltphdr sh;
  struct mbuf    *m;

  TAILQ_ENTRY(pkt_list) entries;
};
TAILQ_HEAD(pkt_head, pkt_list) ;

extern void init_mbq_mutex(void);
extern void destroy_mbq_mutex(void);

extern void pktlst_init(void);
extern void pktlst_clear(void);
extern int pktlst_size(void);
extern int pkt_push(struct mbuf*, struct sltphdr*);
extern u_int8_t pktlst_pop(char*, size_t*);

#endif /* _NET_MBQ_H */
