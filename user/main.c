
#include "./precom.h"
#include "../kern/define.h"
#include "../kern/nt_sltp.h"
#include "../com/ioctl_def.h"

u_int8_t terminated = 0;
const static int SLTP_HDR_SIZE = sizeof(struct sltphdr);

#pragma pack(push, 1)

struct v46_addr {
  union {
    struct in_addr      v4;
    struct in6_addr     v6;
    u_int32_t           addr32[4];
  } sxa;
#define v4      sxa.v4
#define v6      sxa.v6
#define addr32  sxa.addr32
};

#pragma pack(pop)

/* usr/src/sys/netinet6/in6.c */
/*
 * Convert IP6 address to printable (loggable) representation. Caller
 * has to make sure that ip6buf is at least INET6_ADDRSTRLEN long.
 */
static char digits[] = "0123456789abcdef";
char *
ip6_sprintf(char *ip6buf, const struct in6_addr *addr)
{
  int i, cnt = 0, maxcnt = 0, idx = 0, index = 0;
  char *cp;
  const u_int16_t *a = (const u_int16_t *)addr;
  const u_int8_t *d;
  int dcolon = 0, zero = 0;

  cp = ip6buf;

  for (i = 0; i < 8; i++) {
    if (*(a + i) == 0) {
      cnt++;
      if (cnt == 1)
	idx = i;
    }
    else if (maxcnt < cnt) {
      maxcnt = cnt;
      index = idx;
      cnt = 0;
    }
  }
  if (maxcnt < cnt) {
    maxcnt = cnt;
    index = idx;
  }

  for (i = 0; i < 8; i++) {
    if (dcolon == 1) {
      if (*a == 0) {
	if (i == 7)
	  *cp++ = ':';
	a++;
	continue;
      } else
	dcolon = 2;
    }
    if (*a == 0) {
      if (dcolon == 0 && *(a + 1) == 0 && i == index) {
	if (i == 0)
	  *cp++ = ':';
	*cp++ = ':';
	dcolon = 1;
      } else {
	*cp++ = '0';
	*cp++ = ':';
      }
      a++;
      continue;
    }
    d = (const u_char *)a;
    /* Try to eliminate leading zeros in printout like in :0001. */
    zero = 1;
    *cp = digits[*d >> 4];
    if (*cp != '0') {
      zero = 0;
      cp++;
    }
    *cp = digits[*d++ & 0xf];
    if (zero == 0 || (*cp != '0')) {
      zero = 0;
      cp++;
    }
    *cp = digits[*d >> 4];
    if (zero == 0 || (*cp != '0')) {
      zero = 0;
      cp++;
    }
    *cp++ = digits[*d & 0xf];
    *cp++ = ':';
    a++;
  }
  *--cp = '\0';
  return (ip6buf);
}

void
sx_sprint_addr(struct v46_addr *addr, sa_family_t af, char *buf, int lbuf, u_int8_t braket)
{
  buf[0] = 0;
  int l = 0;

  switch (af) {
  case AF_INET: {
    u_int32_t a = ntohl(addr->addr32[0]);
    if( braket ) 
      l = sprintf(buf, "[%u.%u.%u.%u]", (a>>24)&255, (a>>16)&255, (a>>8)&255, a&255);
    else
      l = sprintf(buf, "%u.%u.%u.%u", (a>>24)&255, (a>>16)&255, (a>>8)&255, a&255);
    break;
  }
  case AF_INET6: {
    if( braket ) {
      char ip6buf[INET6_ADDRSTRLEN];
      l = snprintf(buf, lbuf, "[%s]",
	       ip6_sprintf(ip6buf, &addr->v6));
    }
    else
      ip6_sprintf(buf, &addr->v6);
    break;
  }
  }

  if( l<0 || l>=lbuf ) 
    buf[0] = 0;
}

void
print_info(u_char *packet, int len)
{
  struct ip *ip4 = NULL;		// pointer to IP header
  ip4 = (struct ip*)packet;
  struct v46_addr *saddr, *daddr;
  sa_family_t af = 0;
  int  hlen = 0;
  int  tot_len = 0;
  u_int8_t proto = 0;
  
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

    saddr = (struct v46_addr *)&h->ip_src;
    daddr = (struct v46_addr *)&h->ip_dst;
    proto = h->ip_p;
    af = AF_INET;
    
  } else if (ip4->ip_v == 6) {
    struct ip6_hdr *h = (struct ip6_hdr*)ip4;

    if( len < sizeof(struct ip6_hdr) ) {
      printf("Small packet: len:%d\n", len);
      goto FAIL;
    }

    tot_len = ntohs(h->ip6_plen) + sizeof (struct ip6_hdr);

    if( tot_len != len ) {
      printf("Malformed packet: len:%d, tot_len:%d\n", len, tot_len);
      goto FAIL;
    }

    saddr = (struct v46_addr *)&h->ip6_src;
    daddr = (struct v46_addr *)&h->ip6_dst;
    proto = h->ip6_nxt;
    af = AF_INET6;
  }

  if( af==AF_INET || af==AF_INET6 ) {
    char src[INET6_ADDRSTRLEN+3];
    char dst[INET6_ADDRSTRLEN+3];

    sx_sprint_addr(saddr, af, src, sizeof(src)-1, 1);
    sx_sprint_addr(daddr, af, dst, sizeof(dst)-1, 1);

    printf("%s -> %s \t proto:(%d)\n", src, dst, proto);
  }

  return;

 FAIL:
  return;
}

void
process(int dev, u_char *data, int len)
{
  struct sltphdr *sh = NULL;
  u_char *packet = NULL;

  sh = (struct sltphdr*)data;
  packet = data + SLTP_HDR_SIZE;

  print_info(packet, len-SLTP_HDR_SIZE);
  //printf("Read packet ifname:[%s], len:%d, dir:%d\n", sh->ifname, len - SLTP_HDR_SIZE, sh->dir);
  
  write(dev, data, len);
}

void* 
drv_io(int dev)
{
  int n = 0;

  static u_char packet[CB_MAXLEN_DEV+SLTP_HDR_SIZE];
  struct pollfd pfd;
  int ret = 0;

  while (!terminated) {
    bzero(&pfd, sizeof(struct pollfd));

    pfd.fd = dev;
    pfd.events = (POLLIN|POLLPRI);

    ret = poll(&pfd, 1, 10000);

    if (ret <= 0) {
      continue;
    }

    if( pfd.revents & (POLLIN) ) {
      n = read(dev, packet, sizeof(packet));
      if( n > SLTP_HDR_SIZE )
	process(dev, packet, n);
      else
	printf("size is too small: %d\n", n);
    }
  }
  
  return (NULL);
}

int
main(int argc, char *argv[])
{  
  int dev = 0; 
    
  dev = open("/dev/" DEV_NAME, O_RDWR);
  
  if (dev == -1) {
    printf("unable open /dev/%s device.\n", DEV_NAME);
    exit(EXIT_FAILURE);
  }

  drv_io(dev);

  close(dev);
  
  return (0);
}
