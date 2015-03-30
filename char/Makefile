obj-m += char.o

KDIR = /usr/src/linux-headers-$(shell uname -r)
DEBUG_CFLAGS +=-g -DDEBUG_OUTPUT

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

debug: 
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) EXTRA_CFLAGS="$(DEBUG_CFLAGS)" modules 

clean: 
	rm -rf *.o *.ko *.mod.* *.symvers *.order

