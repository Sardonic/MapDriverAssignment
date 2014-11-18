
CC=gcc
DEBUG=-g -D_DEBUG
DEFINE=-DMODULE -D__KERNEL__ -DLINUX
WARNINGS=-Wall -Wmissing-prototypes -Wmissing-declarations
#ISO=-ansi -pedantic
CC_OPTIONS=-O1 $(WARNINGS) $(ISO) $(DEBUG) $(DEFINE)

# Where to look for header files
INC=-I. -I/usr/include -I/usr/src/kernels/`uname -r`/include

DRIVER=map.o
MODULE=map.ko

obj-m += $(DRIVER)

all: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	@echo "Woo! You compiled a driver!!"

clean:
	rm -f $(DRIVER)
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

register: $(DRIVER)
	insmod ./$(MODULE)
	modinfo $(MODULE)
	lsmod | grep map

clean-all:
	make clean
	rmmod map
	lsmod
