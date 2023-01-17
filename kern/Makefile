SRCS = loup.c ioctl.c str.c inito.c dinito.c mbq.c pktio.c

KMOD = loupip

CFLAGS += -D_KERNEL_
DEBUG_FLAGS += -g

install:
	cp loupip.ko ../out/krn/
	cp loupip.ko.debug ../out/krn/
rclean:
	@make clean
	rm -f *~
.include  <bsd.kmod.mk>
