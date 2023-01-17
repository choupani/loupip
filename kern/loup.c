#include "./precom.h"
#include "./define.h"
#include "./stdinc.h"
#include "./ioctl.h"
#include "./inito.h"
#include "./dinito.h"
#include "./mem_defs.h"
#include "./mbq.h"
#include "./pktio.h"
#include "../com/ioctl_def.h"

MALLOC_DEFINE(M_LOUP_IOCTL, "loup_ioctl", DEV_NAME " ioctl");

static int IPv4_hooked  = 0;
static int IPv6_hooked  = 0;
static int ETH_hooked   = 0;

d_open_t  dev_open;
d_close_t dev_close;
d_read_t  dev_read;
d_write_t dev_write;
d_ioctl_t dev_ioctl;
d_poll_t  dev_poll;

struct	selinfo	dev_rsel;	/* read select */

static struct cdev *sdev;
static int proc_count = 0;       /* Device Busy flag */

u_int8_t z_hooked  = 0;
u_int8_t z_running = 0;

static int pkt_rcv(void*, struct mbuf**, struct ifnet*, struct ifnet*, int, int);

static struct cdevsw flwacct_cdevsw = {
  .d_version = D_VERSION,
  .d_open    = dev_open,
  .d_close   = dev_close,
  .d_read    = dev_read,
  .d_write   = dev_write,
  .d_ioctl   = dev_ioctl,
  .d_poll    = dev_poll,
  .d_name    = DEV_NAME,
  .d_flags   = D_TTY,
};

static int
layer2_hook(struct mbuf **m, struct ifnet *ifp, int dir)
{  
  return (FW_PASS);
}

static int
input_hook(void *arg, struct mbuf **m, struct ifnet *ifp, int dir, int flags, struct inpcb *inp)
{
  
  int action = FW_PASS;

  action = pkt_rcv(arg, m, ifp, ifp, dir, flags);

  return (action);
}

static int
output_hook(void *arg, struct mbuf **m, struct ifnet *ifp, int dir, int flags, struct inpcb *inp)
{
  int action = FW_PASS;

  action = pkt_rcv(arg, m, ifp, ifp, dir, flags);

  return (action);
}

/*
 * processing for ethernet packets (in and out).
 */
static int
ether_hook(void *arg, struct mbuf **m0, struct ifnet *ifp, int dir, int flags, struct inpcb *inp)
{
  int ret = 0;

  ret = layer2_hook(m0, ifp, dir);

  if (ret != 0) {
    if (*m0) {
      FREE_PKT(*m0);
    }
    *m0 = NULL;
  }
  
  return (ret);
}

static int
init_module(void)
{
  struct pfil_head *pfh_inet = NULL;
  struct pfil_head *pfh_inet6 = NULL;
  struct pfil_head *pfh_eth = NULL;

  if (z_hooked)
    return (0);

  init_objects();

  bzero(&dev_rsel, sizeof(dev_rsel));
  
  pfh_inet = pfil_head_get(PFIL_TYPE_AF, AF_INET);
  if (pfh_inet == NULL)
    return (ENOENT);
  pfil_add_hook_flags(input_hook, NULL, PFIL_IN | PFIL_WAITOK, pfh_inet);
  pfil_add_hook_flags(output_hook, NULL, PFIL_OUT | PFIL_WAITOK, pfh_inet);
  IPv4_hooked = 1;
  

  pfh_inet6 = pfil_head_get(PFIL_TYPE_AF, AF_INET6);
  if (pfh_inet6 == NULL)
    return (ENOENT);
  
  pfil_add_hook_flags(input_hook, NULL, PFIL_IN | PFIL_WAITOK, pfh_inet6);
  pfil_add_hook_flags(output_hook, NULL, PFIL_OUT | PFIL_WAITOK, pfh_inet6);
  IPv6_hooked = 1;
  
  pfh_eth = pfil_head_get(PFIL_TYPE_AF, AF_LINK);
  if (pfh_eth == NULL)
    return (ENOENT);
  pfil_add_hook_flags(ether_hook, NULL, PFIL_IN | PFIL_OUT | PFIL_WAITOK, pfh_eth);
  ETH_hooked = 1;

  z_hooked = 1;
  z_running = 1;
  
  sdev = make_dev(&flwacct_cdevsw,
		  MOD_MINOR,
		  UID_ROOT,
		  GID_WHEEL,
		  0600,
		  DEV_NAME);

  printf("loaded " DEV_NAME " kernel module.\n");
  
  return (0);
}

static int
deinit_module(void)
{
  struct pfil_head *pfh_inet;
  struct pfil_head *pfh_inet6;
  struct pfil_head *pfh_eth;

  if (!z_hooked)
    return (0);

  pfh_inet = pfil_head_get(PFIL_TYPE_AF, AF_INET);
  if (pfh_inet == NULL)
    return (ENOENT);
  if( IPv4_hooked ) {
    pfil_remove_hook_flags(input_hook, NULL, PFIL_IN | PFIL_WAITOK, pfh_inet);
    pfil_remove_hook_flags(output_hook, NULL, PFIL_OUT | PFIL_WAITOK, pfh_inet);
  }
  
  pfh_inet6 = pfil_head_get(PFIL_TYPE_AF, AF_INET6);
  if (pfh_inet6 == NULL)
    return (ENOENT);
  if( IPv6_hooked ) {
    pfil_remove_hook_flags(input_hook, NULL, PFIL_IN | PFIL_WAITOK, pfh_inet6);
    pfil_remove_hook_flags(output_hook, NULL, PFIL_OUT | PFIL_WAITOK, pfh_inet6);
  }

  pfh_eth = pfil_head_get(PFIL_TYPE_AF, AF_LINK);
  if (pfh_eth == NULL)
    return (ENOENT);
  if( ETH_hooked )
    pfil_remove_hook_flags(ether_hook, NULL, PFIL_IN | PFIL_OUT | PFIL_WAITOK, pfh_eth);

  IPv4_hooked = 0;
  IPv6_hooked = 0;
  ETH_hooked  = 0;

  z_hooked  = 0;
  z_running = 0;

  selwakeup(&dev_rsel);

  dinit_objects();
 
  destroy_dev(sdev);
  
  printf("unloaded " DEV_NAME " kernel module.\n");
  
  return (0);
}

static int
mod_evhandler(struct module *m, int what, void *arg)
{
  int err = 0;
    
  switch(what) {
  case MOD_LOAD:
    err = init_module();
  break;
  case MOD_QUIESCE:
  case MOD_SHUTDOWN:
    err = deinit_module();
    break;
  case MOD_UNLOAD:
  break;
  default:
    err = EINVAL;
  break;
  }
  return (err);
}

int
dev_open(struct cdev *dev, int oflags, int devtype, struct thread *td)
{
  int err = 0;
  proc_count++;
  return (err);
}

int
dev_close(struct cdev *dev, int fflag, int devtype, struct thread *td)
{
  int err = 0;
  if( proc_count>0 )
    proc_count--;
  return (err);
}

int
dev_read(struct cdev *dev, struct uio *uio, int ioflag)
{
  static char	buf[CB_MAXLEN_DEV+sizeof(struct sltphdr)]; 
  size_t       	len;
  int		nbytes = 0;  // No data is available

  if (!z_hooked)
    return (0);

  if( pktlst_pop(buf, &len) ) {
    nbytes = uiomove(buf, MIN(uio->uio_resid, len), uio);
  }
  
  return (nbytes);
}

int
dev_write(struct cdev *dev, struct uio *uio, int ioflag)
{  
  /* Copy the buffer in from user memory to kernel memory */

  static char	buf[CB_MAXLEN_DEV+sizeof(struct sltphdr)];
  size_t       	len;
  int		error;
  static const int hdr_size = sizeof(struct sltphdr);

  if (!z_hooked)
    return (0);
  
  if ((len = uio->uio_iov->iov_len) >= CB_MAXLEN_DEV)
    return (EINVAL);

  if( len < sizeof(struct sltphdr) )
    return (EINVAL);

  error = copyin(uio->uio_iov->iov_base, buf, len);

  if(!error) {
    struct sltphdr *sh = (struct sltphdr*)buf;
    pkt_io(buf + hdr_size, len - hdr_size, sh);
  }
  
  return (error);
}

int
dev_poll(struct cdev *dev, int events, struct thread *td)
{  
  int revents = 0;

  if( !z_hooked )
    return (0);

  if ((events & (POLLIN | POLLRDNORM)) && pktlst_size()>0)
    revents |= events & (POLLIN | POLLRDNORM);

  if ((revents == 0) && ((events & (POLLIN|POLLRDNORM)) != 0))
    selrecord(td, &dev_rsel);
  
  return (revents);

}

static int
pkt_rcv(void *arg, struct mbuf **m, struct ifnet *iifp, struct ifnet *oifp, int dir, int flag)
{
  u_int16_t action = FW_PASS;
  struct sltphdr sh;
  struct m_tag	 *mtag = NULL;

  if(!z_running || !proc_count)
    goto PASS;

  if ( (mtag = m_tag_find(*m, LOUP_TAG, NULL)) != NULL)
    goto PASS;

  sh.ifname[0] = 0;
  sh.dir = dir;
  sh.cksum_flags = (*m)->m_pkthdr.csum_flags;
  sh.cksum_data  = (*m)->m_pkthdr.csum_data;
  if( iifp )
    strlcpy(sh.ifname, iifp->if_xname, IFNAMSIZ);
  
  action = pkt_push(*m, &sh);

  if( action==FW_QUEUE )
    selwakeup(&dev_rsel);
  
  switch ( action ) {
  case FW_DROP:
    if( m )
      m_freem(*m);
    *m = NULL;
    break;
  case FW_QUEUE:
    action = FW_PASS;
    *m = NULL;
  }
  
 PASS:
  return (action);
}

int
dev_ioctl(struct cdev *dev, u_long cmd, caddr_t addr, int flags, struct thread *td)
{
  int error = NO_ERROR;
  struct ioctlbuffer *ub = NULL;
  caddr_t *kb = NULL;
  struct ioctlST iost;
  u_int8_t copy_out = 0;
  
  if( !z_hooked )
    return (EACCES);

  ub = (struct ioctlbuffer *)addr;
  if (ub == NULL)
    return (ERROR_INVALID);
  kb = MEM_ALLOC(ub->size, M_LOUP_IOCTL, M_NOWAIT);
  if (kb == NULL) {
    return (ERROR_MALLOC);
  }
  if (copyin(ub->buffer, kb, ub->size)) {
    MEM_FREE(kb, M_LOUP_IOCTL);
    return (ERROR_INVALID);
  }

  iost.ub = ub;
  iost.kb = kb;

  error = parsecmd(&iost, cmd, &copy_out);

  if (kb != NULL) {
    if( copy_out )
      if (copyout(kb, ub->buffer, ub->size))
	error = ERROR_INVALID;
    MEM_FREE(kb, M_LOUP_IOCTL);
  }

  return (error);
}

DEV_MODULE(loupip, mod_evhandler, NULL);
MODULE_VERSION(loupip, 1);
