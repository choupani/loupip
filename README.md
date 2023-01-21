# loupip
loupip is a kernel module in freebsd that helps user-space packet processing. loupip provides a mechanism for passing packets out from kernel-space in layer3 to userspace, then receiving these packets back or even new packets into the kernel. These packets may also be modified in userspace prior to reinjection back into the kernel. Every packet has a direction and ifname which indicates the packet direction and adapter name which packet has been recceived on.
loupip can help to develope any packet processing module such as firewall, traffic shaper, ... in layer3 in user-space. ‌Because loupip kernel-module is using context switching to send/recive packets to/from user-space, could effect to performance. It supports ip4/ip6 protocol stack and helps to devlopers process packets without having to write any low-level kernel code.
In kernel module has implemnted poll machanism to interrupt user-space when received a new packet.
loupip developed under freebsd11.4 and for other versions of freebsd should be tested and maybe needs to change.

