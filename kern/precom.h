#ifndef _GEN_PRECOM_H
#define _GEN_PRECOM_H

#pragma clang diagnostic ignored "-Waddress-of-packed-member"

#include <sys/cdefs.h>
#include <sys/ctype.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/module.h>
#include <sys/systm.h>
#include <sys/errno.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/conf.h>
#include <sys/uio.h>
#include <sys/malloc.h>
#include <sys/ioccom.h>
#include <sys/mbuf.h>
#include <sys/sysent.h>
#include <sys/sysproto.h>
#include <sys/proc.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/queue.h>
#include <sys/tree.h>
#include <sys/random.h>
#include <sys/fcntl.h>
#include <sys/kthread.h>
#include <sys/lock.h>
#include <sys/rmlock.h>
#include <sys/mutex.h>
#include <sys/sx.h>
#include <sys/hash.h>
#include <sys/md5.h>
#include <sys/alq.h>
#include <sys/module.h>
#include <sys/pcpu.h>
#include <sys/sbuf.h>
#include <sys/smp.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/unistd.h>
#include <sys/sbuf.h>
#include <sys/syslog.h>
#include <sys/param.h>
#include <sys/bus.h>
#include <sys/interrupt.h>
#include <sys/limits.h>
#include <sys/zlib.h>
#include <sys/syscallsubr.h>
#include <sys/taskqueue.h>
#include <sys/poll.h>

#include <crypto/des/des_locl.h>
#include <crypto/des/des.h>

#include <opencrypto/cryptodev.h>
#include <opencrypto/deflate.h>

#include <vm/uma.h>

#include <net/if.h>
#include <net/if_var.h>
#include <net/if_types.h>
#include <net/pfil.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <net/route.h>
#include <net/netisr.h>
#include <net/if_dl.h>
#include <net/if_llc.h>
#include <net/bpf.h>
#include <net/if_vlan_var.h>
#include <net/if_llatbl.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/in_var.h>
#include <netinet/ip.h>
#include <netinet/ip_var.h>
#include <netinet/tcp.h>
#include <netinet/tcp_var.h>
#include <netinet/tcp_fsm.h>
#include <netinet/tcp_seq.h>
#include <netinet/sctp_crc32.h>
#include <netinet/sctp.h>
#include <netinet/udp.h>
#include <netinet/udp_var.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip_fw.h>
#include <netinet/in_pcb.h>

#include <netinet/ip6.h>
#include <netinet6/ip6_var.h>
#include <netinet/icmp6.h>
#include <netinet6/nd6.h>
#include <netinet6/scope6_var.h>
#include <netinet6/in6_pcb.h>

#include <net/if_gre.h>

#include <netipsec/keydb.h>

#include <machine/iodev.h>
#include <machine/in_cksum.h>
#include <machine/endian.h>
#include <machine/md_var.h>
#include <machine/stdarg.h>

#include <netpfil/ipfw/ip_fw_private.h>

#endif
