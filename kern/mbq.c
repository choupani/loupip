
#include "./mbq.h"
#include "./define.h"
#include "./stdinc.h"
#include "./mem_defs.h"

#define MAX_PACKET_LIST 512

struct pkt_head pktlst_head;

int num_pkts = 0;

struct mtx mbq_mtx;

MALLOC_DEFINE(M_LOUP_MBFQ, "loup_mbufq", DEV_NAME " mbufq");

void
init_mbq_mutex(void)
{
  mtx_init(&mbq_mtx, "mbq mtx", NULL, MTX_DEF);
}

void
destroy_mbq_mutex(void)
{
  mtx_destroy(&mbq_mtx);
}

void 
pktlst_init(void)
{
  TAILQ_INIT(&pktlst_head);  
}

void
pktlst_clear(void)
{
  struct pkt_list *cur;
  while ((cur = TAILQ_FIRST(&pktlst_head))) {
    m_freem(cur->m);
    TAILQ_REMOVE(&pktlst_head, cur, entries);
    MEM_FREE(cur, M_LOUP_MBFQ);
  }

  num_pkts = 0;
}

int
pktlst_size(void)
{
  int count = 0;

  MQ_LOCK();

  count = num_pkts;
  
  MQ_UNLOCK();

  return (count);
}

int
pkt_push(struct mbuf *m, struct sltphdr *sh)
{
  MQ_LOCK();
  if( num_pkts>MAX_PACKET_LIST ) {
    MQ_UNLOCK();
    return (FW_DROP);
  }
  MQ_UNLOCK();

  struct pkt_list *pkt = MEM_ALLOC(sizeof(struct pkt_list), M_LOUP_MBFQ, M_NOWAIT);

  if( pkt == NULL )
    return (FW_DROP);

  pkt->m   =  m;
  pkt->sh  = *sh;

  MQ_LOCK();
  
  TAILQ_INSERT_TAIL(&pktlst_head, pkt, entries);

  num_pkts++;

  MQ_UNLOCK();

  return (FW_QUEUE);
}

u_int8_t
pktlst_pop(char *buf, size_t *len)
{
  struct pkt_list *cur = NULL;
  
  const static int sltphdr_size = sizeof(struct sltphdr);

  int d = 0;

  MQ_LOCK();
  
  cur = TAILQ_FIRST(&pktlst_head);
  
  if( cur != NULL ) {
    memcpy(buf, &cur->sh, sltphdr_size);
    
    *len = cur->m->m_pkthdr.len;

    m_copydata(cur->m, 0, *len, buf+sltphdr_size);

    *len += sltphdr_size;

    m_freem(cur->m);

    TAILQ_REMOVE(&pktlst_head, cur, entries);

    --num_pkts;

    d = 1;

    MEM_FREE(cur, M_LOUP_MBFQ);
  }

  MQ_UNLOCK();
  
  return (d);
}
