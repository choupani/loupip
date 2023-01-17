
#include "./pktio.h"
#include "./define.h"

void
mbuf_io(struct mbuf **mb, struct ifnet *ifp, int dir, sa_family_t af)
{
  struct mbuf *m = NULL;

  m = (*mb);
  m->m_nextpkt = 0;

  switch (dir) {

  case FW_OUT:
    switch(af) {
    case AF_INET:
      ip_output(m, NULL, NULL, IP_FORWARDING, NULL, NULL);
      break;
    case AF_INET6:
      ip6_output(m, NULL, NULL, IPV6_FORWARDING, NULL, NULL, NULL);
      break;	
    }
    break ;
    
  case FW_IN :
    switch(af) {
    case AF_INET:
      netisr_dispatch(NETISR_IP, m);
      break;
    case AF_INET6:
      netisr_dispatch(NETISR_IPV6, m);
      break;
      
    }
    break ;
    
  }
 
  return;
}

void
pkt_io(char *buf, int len, struct sltphdr *sh)
{
  struct mbuf *m = NULL;
  struct ip *ip4 = NULL;		// pointer to IP header
  struct ifnet *ifp = NULL;
  struct m_tag *mtag = NULL;
  struct loup_tag *tag = NULL;
  sa_family_t af = 0;
  int  hlen = 0;
  int  tot_len = 0;
  int dir = FW_IN;

  dir = sh->dir;
  ifp   = ifunit(sh->ifname); 
  ip4 = (struct ip*)buf;

  if (ip4->ip_v == 4) {

    struct ip *h = ip4;

    if( len < sizeof(struct ip) ) {
      printf("Small packet: len:%d\n", len);
      goto FAIL;
    }

    hlen = h->ip_hl << 2;
    tot_len = ntohs(h->ip_len);

    if( tot_len != len ||
	hlen < sizeof(struct ip) ||
	hlen > len ||
	hlen > tot_len )  {
      printf("Malformed packet: len:%d, tot_len:%d, hlen:%d\n", len, tot_len, hlen);
      goto FAIL;
    }

    af = AF_INET;
  } else if (ip4->ip_v == 6) {

    struct ip6_hdr *h = (struct ip6_hdr*)buf;

    if( len < sizeof(struct ip6_hdr) ) {
      printf("Small packet: len:%d\n", len);
      goto FAIL;
    }

    tot_len = ntohs(h->ip6_plen) + sizeof (struct ip6_hdr);

    if( tot_len != len ) {
      printf("Malformed packet: len:%d, tot_len:%d\n", len, tot_len);
      goto FAIL;
    }

    af = AF_INET6;
  }
  
  /* create outgoing mbuf */
  MGETHDR(m, M_NOWAIT, MT_HEADER);
  
  if (m == NULL) {
    goto FAIL;
  }
  
  m->m_data += max_linkhdr;
  m->m_pkthdr.len = m->m_len = 0;
  m->m_pkthdr.rcvif = NULL;

  if( dir==FW_IN ) {
    m->m_pkthdr.rcvif = ifp;
  } else if( dir==FW_OUT ) {
    m->m_pkthdr.csum_flags = sh->cksum_flags; //CSUM_DELAY_DATA;
    m->m_pkthdr.csum_data  = sh->cksum_data;  //0;
  }

  m_copyback(m, 0, len, buf);

  if(m == NULL) {
    goto FAIL;
  }

  if ((mtag = m_tag_get(LOUP_TAG, sizeof(struct loup_tag), M_NOWAIT)) == NULL) {
    printf("TAG allocation failed.\n");
    goto FAIL;
  }

  tag = (struct loup_tag *)(mtag + 1);
  tag->dir = dir;
  m_tag_prepend(m, mtag);

  mbuf_io(&m, ifp, dir, af);

  return;

 FAIL:
  if(m)
    m_freem(m);
  return;
}
