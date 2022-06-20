obj-m = maze.o 

KDIR := /work/achroimx_kernel
PWD := $(shell pwd)

driver : 
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

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
	adb push stopwatch.ko /data/local/tmp