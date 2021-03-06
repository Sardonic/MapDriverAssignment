
CC=gcc
DEBUG=-g -D_DEBUG
DEFINE=-DMODULE -D__KERNEL__ -DLINUX
WARNINGS=-Wall -Wmissing-prototypes -Wmissing-declarations
#ISO=-ansi -pedantic
CC_OPTIONS=-O1 $(WARNINGS) $(ISO) $(DEBUG) $(DEFINE)
SHELL=/bin/bash

# Where to look for header files
INC=-I. -I/usr/include -I/usr/src/kernels/`uname -r`/include -D_DEBUG

TFO_C=testForkExec.c
DRIVER=asciimap.o
MODULE=asciimap.ko

obj-m += $(DRIVER)

all: 
	$(CC) $(CC_OPTIONS) $(TFO_C) -o testForkExec
	DIR=$(PWD)
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	cd $(DIR)
	@echo "Woo! You compiled a driver!!"

build:
	make all
	make register
	eval `dmesg | grep mknod | tail -1`

clean:
	rm -f $(DRIVER)
	DIR=$(PWD)
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	cd $(DIR)

rebuild:
	make clean
	sudo rmmod asciimap
	sudo rm /dev/asciimap
	make all
	make register
	eval `dmesg | grep mknod | tail -1`
	
test:
	gcc -g test.c -o test
	./test

retest:
	make rebuild
	gcc -g test.c -o test
	./test

register: $(DRIVER)
	insmod ./$(MODULE)
	modinfo $(MODULE)
	lsmod | grep asciimap

clean-all:
	make clean
	rmmod asciimap
	lsmod
