obj-m = maze.o 

KDIR := /work/achroimx_kernel
PWD := $(shell pwd)

driver : 
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD)

.PHONY: clean
clean : 
	rm -rf *.o
	rm -rf *.ko
	rm -rf *.mod.c
	rm -rf *.order
	rm -rf *.symvers

.PHONY: push
push:
	make
	adb push maze.ko /data/local/tmp