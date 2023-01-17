#ifndef _KRN_DEFINE_H
#define _KRN_DEFINE_H

#define  MOD_VERSION   "1.0"
#define  DEV_NAME      "loupip"

#define  MOD_MINOR     15

#define MAX_URL_RW 104

enum  {FW_IN=1, FW_OUT};

enum  {FW_PASS, FW_DROP, FW_QUEUE};

#define M_SKIP_LOUPIP       M_PROTO12        /* packet processed by loup */

#define MQ_ASSERT(h) mtx_assert(&mbq_mtx, (h))

#define MQ_LOCK()       do {			\
    MQ_ASSERT(MA_NOTOWNED);			\
    mtx_lock(&mbq_mtx);				\
  } while(0)

#define MQ_UNLOCK()     do {			\
    MQ_ASSERT(MA_OWNED);			\
    mtx_unlock(&mbq_mtx);			\
  } while(0)


#define PULLUP_TO(len, p, T)			\
  do {						\
    int x = (len) + (T);			\
    if (pkt->m->m_len < x)			\
      pkt->m = m_pullup(pkt->m, x);		\
    p = mtod(pkt->m, char *) + (len);		\
  } while (0)

extern struct mtx mbq_mtx;

#endif

